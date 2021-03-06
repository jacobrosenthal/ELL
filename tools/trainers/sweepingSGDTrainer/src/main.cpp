////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (sweepingSGDTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "RandomEngines.h"

// data
#include "Example.h"

// common
#include "AppendNodeToModel.h"
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "EvaluatorArguments.h"
#include "LoadModel.h"
#include "MakeEvaluator.h"
#include "MakeTrainer.h"
#include "MapLoadArguments.h"
#include "MapSaveArguments.h"
#include "ModelLoadArguments.h"
#include "ModelSaveArguments.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "ParametersEnumerator.h"
#include "TrainerArguments.h"

// trainers
#include "EvaluatingIncrementalTrainer.h"
#include "SGDLinearTrainer.h"
#include "SweepingIncrementalTrainer.h"

// evaluators
#include "BinaryErrorAggregator.h"
#include "Evaluator.h"
#include "LossAggregator.h"

// lossFunctions
#include "HingeLoss.h"
#include "LogLoss.h"

// model
#include "InputNode.h"
#include "Model.h"

// nodes
#include "LinearPredictorNode.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <tuple>

using namespace ell;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedTrainerArguments trainerArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;
        common::ParsedMultiEpochIncrementalTrainerArguments multiEpochTrainerArguments;

        commandLineParser.AddOptionSet(trainerArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);
        commandLineParser.AddOptionSet(multiEpochTrainerArguments);

        // parse command line
        commandLineParser.Parse();

        // manually define regularization parameters to sweep over
        std::vector<double> regularization{ 1.0e-0, 1.0e-1, 1.0e-2, 1.0e-3, 1.0e-4, 1.0e-5, 1.0e-6 };

        if (trainerArguments.verbose)
        {
            std::cout << "Sweeping Stochastic Gradient Descent Trainer" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load map
        mapLoadArguments.defaultInputSize = dataLoadArguments.parsedDataDimension;
        model::DynamicMap map = common::LoadMap(mapLoadArguments);

        // load dataset
        if (trainerArguments.verbose) std::cout << "Loading data ..." << std::endl;
        auto mappedDataset = common::GetMappedDataset(dataLoadArguments, map);
        auto mappedDatasetDimension = map.GetOutput(0).Size();

        // get predictor type
        using PredictorType = predictors::LinearPredictor;

        // set up evaluators to only evaluate on the last update of the multi-epoch trainer
        evaluators::EvaluatorParameters evaluatorParameters{ multiEpochTrainerArguments.numEpochs, false };

        // create trainers
        auto generator = common::MakeParametersEnumerator<trainers::SGDLinearTrainerParameters>(regularization);
        std::vector<trainers::EvaluatingIncrementalTrainer<PredictorType>> evaluatingTrainers;
        std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> evaluators;
        for (size_t i = 0; i < regularization.size(); ++i)
        {
            auto SGDLinearTrainer = common::MakeSGDLinearTrainer(trainerArguments.lossArguments, generator.GenerateParameters(i));
            evaluators.push_back(common::MakeEvaluator<PredictorType>(mappedDataset.GetAnyDataset(), evaluatorParameters, trainerArguments.lossArguments));
            evaluatingTrainers.push_back(trainers::MakeEvaluatingIncrementalTrainer(std::move(SGDLinearTrainer), evaluators.back()));
        }

        // create meta trainer
        auto trainer = trainers::MakeSweepingIncrementalTrainer(std::move(evaluatingTrainers), multiEpochTrainerArguments);

        // train
        if (trainerArguments.verbose) std::cout << "Training ..." << std::endl;
        trainer->Update(mappedDataset.GetAnyDataset());
        predictors::LinearPredictor predictor(trainer->GetPredictor());
        predictor.Resize(mappedDatasetDimension);

        // print loss and errors
        if (trainerArguments.verbose)
        {
            std::cout << "Finished training.\n";

            // print evaluation
            for (size_t i = 0; i < regularization.size(); ++i)
            {
                std::cout << "Trainer " << i << ":\n";
                evaluators[i]->Print(std::cout);
                std::cout << std::endl;
            }
        }

        // save predictor model
        if (modelSaveArguments.outputModelFilename != "")
        {
            // Create a model
            auto model = common::AppendNodeToModel<nodes::LinearPredictorNode, PredictorType>(map, predictor);
            common::SaveModel(model, modelSaveArguments.outputModelFilename);
        }
    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cout << exception.GetHelpText() << std::endl;
        return 0;
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 1;
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "exception: " << exception.GetMessage() << std::endl;
        return 1;
    }
    return 0;
}
