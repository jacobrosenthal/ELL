////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MakeTrainer.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TrainerArguments.h"

// trainers
#include "SortingTreeTrainer.h"
#include "StochasticGradientDescentTrainer.h"

// stl
#include <memory>

namespace common
{
    /// <summary> Makes a stochastic gradient descent trainer. </summary>
    ///
    /// <param name="parameters"> Trainer parameters. </param>
    /// <param name="trainerArguments"> Trainer command line arguments. </param>
    ///
    /// <returns> A unique_ptr to a stochastic gradient descent trainer. </returns>
    std::unique_ptr<trainers::IStochasticGradientDescentTrainer> MakeStochasticGradientDescentTrainer(uint64_t dim, const trainers::IStochasticGradientDescentTrainer::Parameters& parameters, const LossArguments& lossArguments);

    /// <summary> Makes a sorting tree trainer. </summary>
    ///
    /// <param name="parameters"> Trainer parameters. </param>
    /// <param name="trainerArguments"> Trainer command line arguments. </param>
    ///
    /// <returns> A unique_ptr to a sorting tree trainer. </returns>
    std::unique_ptr<trainers::ISortingTreeTrainer> MakeSortingTreeTrainer(const trainers::ISortingTreeTrainer::Parameters& parameters, const LossArguments& lossArguments);
}