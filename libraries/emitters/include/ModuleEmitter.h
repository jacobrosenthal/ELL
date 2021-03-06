////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModuleEmitter.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "Variable.h"

// stl
#include <string>

namespace ell
{
namespace emitters
{
    /// <summary> Code output formats </summary>
    enum class ModuleOutputFormat
    {
        ir,
        bitcode,
        assembly
    };

    /// <summary> Standard compiler switches. </summary>
    struct CompilerParameters
    {
        bool unrollLoops = false;
        bool inlineOperators = true;
        bool optimize = true;
        bool includeDiagnosticInfo = false;
    };

    /// <summary> Abstract base class for ELL compilers </summary>
    class ModuleEmitter
    {
    public:
        virtual ~ModuleEmitter() = default;

        /// <summary> Return the base compiler settings </summary>
        ///
        /// <returns> The settings for the compiler </returns>
        const CompilerParameters& GetCompilerParameters() { return _parameters; }

        /// <summary> Set the base compiler settings </summary>
        ///
        /// <param name="parameters"> The settings for the compiler to use </param>
        void SetCompilerParameters(const CompilerParameters& parameters);

        /// <summary> Set a function declaration </summary>
        ///
        /// <param name="functionName"> The name of the function to create </param>
        /// <param name="args"> The names and types of the arguments </param>
        virtual void BeginFunction(const std::string& functionName, NamedVariableTypeList& args) = 0;

        /// <summary> End the function </summary>
        virtual void EndFunction() = 0;

        /// <summary> Variable allocator </summary>
        ///
        /// <returns> The variable allocator </returns>
        VariableAllocator& Variables() { return _variables; }

        /// <summary> Allocate a *runtime* variable.. </summary>
        void AllocateVariable(Variable& var);

        /// <summary> Output the compiled model to the given file, using file extension to determine output format </summary>
        ///
        /// <param name="filePath"> The path of the file to write to </param>
        void WriteToFile(const std::string& filePath);

        /// <summary> Output the compiled model to the given file with the given format </summary>
        ///
        /// <param name="filePath"> The path of the file to write to </param>
        /// <param name="format"> The format of the output </param>
        void WriteToFile(const std::string& filePath, ModuleOutputFormat format);

        /// <summary> Output the compiled model to an output stream with the given format </summary>
        ///
        /// <param name="stream"> The stream to write to </param>
        /// <param name="format"> The format of the output </param>
        virtual void WriteToStream(std::ostream& stream, ModuleOutputFormat format) = 0;

        /// <summary> Return the appropriate output file type for the given filename extension </summary>
        ///
        /// <param name="extension"> The extension </param>
        /// <returns> The format of the output </returns>
        static ModuleOutputFormat GetFormatFromExtension(const std::string& extension);

    protected:
        void Reset();

        /// <summary> Free a *runtime* variable.. </summary>
        void FreeVar(Variable& var);

    private:
        CompilerParameters _parameters;

        EmittedVariableAllocator _inputVars; // Runtime input variable table
        EmittedVariableAllocator _outputVars; // Runtime output variable table

        EmittedVariableAllocator _globalVars; // Runtime global variable table
        EmittedVariableAllocator _literalVars; // Runtime literal variable table
        EmittedVariableAllocator _localVars; // Runtime local variable table (don't these belong to a function?)
        EmittedVariableAllocator _rValueVars; // Runtime RValues variable table (what scope?)

        VariableAllocator _variables; // variable object manager
    };
}
}
