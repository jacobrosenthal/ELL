////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLoopEmitter.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;

    ///<summary> Class that simplifies for loop creation. </summary>
    class IRForLoopEmitter
    {
    public:
        /// <summary> Constructs an instance of IRForLoopEmitter. </summary>
        ///
        /// <param name="functionEmitter"> The function emitter. </param>
        IRForLoopEmitter(IRFunctionEmitter& functionEmitter);

        /// <summary> Gets the block containing the body of the for loop. </summary>
        ///
        /// <returns> Pointer to an llvm::BasicBlock that represents the body of the for loop. </returns>
        llvm::BasicBlock* GetBodyBlock() { return _pBodyBlock; }

        /// <summary> Gets the iteration count variable, the "i" of the for loop. </summary>
        ///
        /// <returns> Pointer to an llvm::Value that represents the iteration variable. </returns>
        llvm::Value* GetIterationVariable() { return _pIterationVariable; }

        /// <summary> Emits a load instruction for the iteration variable. </summary>
        ///
        /// <returns> Pointer to an llvm::Value that represents the iteration variable. </returns>
        llvm::Value* LoadIterationVariable();

        /// <summary> Emits the beginning of a for loop that repeats a given number of times. </summary>
        ///
        /// <param name="repeatCount"> Number of repetitions. </param>
        ///
        /// <returns> Pointer to the llvm::BasicBlock that represents the for loop. </returns>
        llvm::BasicBlock* Begin(const int repeatCount);

        /// <summary> Emits the beginning of a for loop from iStartAt to iMaxValue, with a given step size. </summary>
        ///
        /// <param name="iStartAt"> The start interation value. </param>
        /// <param name="iMaxValue"> The max iteration value. </param>
        /// <param name="stepSize"> Size of the step. </param>
        ///
        /// <returns> Pointer to the llvm::BasicBlock that represents the for loop. </returns>
        llvm::BasicBlock* Begin(const int iStartAt, const int iMaxValue, const int stepSize);

        /// <summary> Emits the beginning of a for loop that repeats the given number of times. </summary>
        ///
        /// <param name="pRepeatCount"> Pointer to an llvm::Value that contains the number of repetitions. </param>
        ///
        /// <returns> Pointer to the llvm::BasicBlock that represents the for loop. </returns>
        llvm::BasicBlock* Begin(llvm::Value* pRepeatCount);

        /// <summary> Emit the end of this for loop. </summary>
        void End();

        /// <summary> Resets this emitter, to start a new for loop. </summary>
        void Clear();

    private:
        void CreateBlocks();
        void EmitIterationVariable(VariableType type, llvm::Value* pStartValue);
        void EmitCondition(TypedComparison type, llvm::Value* pTestValue);
        void EmitIncrement(VariableType type, llvm::Value* pIncrementValue);
        llvm::BasicBlock* PrepareBody();

        IRFunctionEmitter& _functionEmitter; // Loop written into this function
        llvm::BasicBlock* _pInitializationBlock = nullptr; // The for loop is set up in this block - such as initializing iteration variables
        llvm::BasicBlock* _pConditionBlock = nullptr; // Here we do the loop termination check
        llvm::BasicBlock* _pBodyBlock = nullptr; // The body of the for loop
        llvm::BasicBlock* _pIncrementBlock = nullptr; // Here we increment the iteration variable
        llvm::BasicBlock* _pAfterBlock = nullptr; // When the loop is done, we branch to this block
        llvm::Value* _pIterationVariable = nullptr;
    };
}
}
