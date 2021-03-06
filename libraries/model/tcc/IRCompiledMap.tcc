////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRCompiledMap.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename InputType>
    void IRCompiledMap::SetComputeFunctionForInputType()
    {
        auto outputSize = GetOutput(0).Size();
        auto functionPointer = _executionEngine->ResolveFunctionAddress(_functionName);
        ComputeFunction<InputType> computeFunction;
        switch (GetOutput(0).GetPortType()) // Switch on output type
        {
            case model::Port::PortType::boolean:
            {
                std::get<utilities::ConformingVector<bool>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void (*)(const InputType*, bool*)>(functionPointer);
                computeFunction = [this, functionPointer, fn](const InputType* input) {
                    fn(input, (bool*)std::get<utilities::ConformingVector<bool>>(_cachedOutput).data());
                };
            }
            break;

            case model::Port::PortType::integer:
            {
                std::get<utilities::ConformingVector<int>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void (*)(const InputType*, int*)>(functionPointer);
                computeFunction = [this, functionPointer, fn](const InputType* input) {
                    fn(input, std::get<utilities::ConformingVector<int>>(_cachedOutput).data());
                };
            }
            break;

            case model::Port::PortType::real:
            {
                std::get<utilities::ConformingVector<double>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void (*)(const InputType*, double*)>(functionPointer);
                computeFunction = [this, functionPointer, fn](const InputType* input) {
                    fn(input, std::get<utilities::ConformingVector<double>>(_cachedOutput).data());
                };
            }
            break;

            default:
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        std::get<ComputeFunction<InputType>>(_computeInputFunction) = computeFunction;
    }
}
}
