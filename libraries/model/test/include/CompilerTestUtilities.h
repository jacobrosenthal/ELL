////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTestUtilities.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// model
#include "DynamicMap.h"
#include "IRCompiledMap.h"
#include "Port.h"

// testing
#include "testing.h"

// stl
#include <ostream>
#include <string>
#include <vector>

namespace ell
{
template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
    out << "[";
    for (size_t index = 0; index < v.size(); ++index)
    {
        if (index != 0)
            out << ", ";
        out << v[index];
    }
    out << "]";
    return out;
}

template <typename InputType, typename OutputType>
void PrintCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    bool ok = true;
    // compare output
    for (const auto& input : signal)
    {
        auto computedResult = map.Compute<OutputType>(input);
        auto compiledResult = compiledMap.Compute<OutputType>(input);
        ok = ok && testing::IsEqual(computedResult, compiledResult);
        std::cout << computedResult << " \t" << compiledResult << std::endl;
    }
}

template <typename InputType>
void PrintCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    switch (map.GetOutput(0).GetPortType())
    {
        case model::Port::PortType::boolean:
            PrintCompiledOutput<InputType, bool>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::integer:
            PrintCompiledOutput<InputType, int>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::real:
            PrintCompiledOutput<InputType, double>(map, compiledMap, signal, name);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
    }
}

template <typename InputType, typename OutputType>
void VerifyCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    bool ok = true;
    // compare output
    for (const auto& input : signal)
    {
        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<OutputType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<OutputType>(0);
        ok = ok && testing::IsEqual(computedResult, compiledResult);

        std::cout << computedResult << " \t" << compiledResult << std::endl;
    }
    testing::ProcessTest(std::string("Testing Compiled" + name + " compute"), ok);
}

template <typename InputType>
void VerifyCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    switch (map.GetOutput(0).GetPortType())
    {
        case model::Port::PortType::boolean:
            VerifyCompiledOutput<InputType, bool>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::integer:
            VerifyCompiledOutput<InputType, int>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::real:
            VerifyCompiledOutput<InputType, double>(map, compiledMap, signal, name);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
    }
}
}
