#pragma once

#include "Instruction.h"
#include <cstdint>
#include <vector>
#include <optional>

namespace interpreter {
    using namespace std;

    struct InterpreterRegisters {
        vector<int16_t> _stack;
        vector<Instruction*> _returnAdressStack;
        Instruction* _currInstruction;
        size_t _baseIdx;
    };

    typedef void (*InstructionFunc)(InterpreterRegisters& registers);

    void ExitInstruction(InterpreterRegisters& registers);
    void AddIntInstruction(InterpreterRegisters& registers);
    void PushIntInstruction(InterpreterRegisters& registers);
    void PopIntInstruction(InterpreterRegisters& registers);
    void PrintIntInstruction(InterpreterRegisters& registers);
    void CompareIntLessInstruction(InterpreterRegisters& registers);
    void LoadIntInstruction(InterpreterRegisters& registers);
    void StoreIntInstruction(InterpreterRegisters& registers);
    void JumpByIfZeroInstruction(InterpreterRegisters& registers);
    void JumpByInstruction(InterpreterRegisters& registers);
    void LoadIntBasePointerRelativeInstruction(InterpreterRegisters& registers);
    void StoreIntBasePointerRelativeInstruction(InterpreterRegisters& registers);
    void CallInstruction(InterpreterRegisters& registers);
    void ReturnInstruction(InterpreterRegisters& registers);

    extern InstructionFunc gInstructionFunctions[NUM_INSTRUCTIONS];

    class Interpreter {
    public:
        static void Run(Instruction* code, vector<int16_t> args, int16_t* result = nullptr);
    };
}