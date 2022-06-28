#include "../include/Interpreter.h"
#include <iostream>

namespace interpreter {

    using namespace std;

    InstructionFunc gInstructionFunctions[NUM_INSTRUCTIONS] = {
            ExitInstruction,
            AddIntInstruction,
            PushIntInstruction,
            PopIntInstruction,
            PrintIntInstruction,
            CompareIntLessInstruction,
            LoadIntInstruction,
            StoreIntInstruction,
            JumpByIfZeroInstruction,
            JumpByInstruction,
            LoadIntBasePointerRelativeInstruction,
            StoreIntBasePointerRelativeInstruction,
            CallInstruction,
            ReturnInstruction,
    };

    void Interpreter::Run(Instruction *code, vector<int16_t> args, int16_t *result) {
        InterpreterRegisters registers{._currInstruction = code};

        if(result) {
            registers._stack.push_back(0);
        }
        registers._stack.insert(registers._stack.end(), args.begin(), args.end());

        registers._stack.push_back(0);
        registers._returnAdressStack.push_back(nullptr);
        registers._baseIdx = registers._stack.size();

        while(registers._currInstruction != nullptr) {
            gInstructionFunctions[registers._currInstruction->_opcode](registers);
        }

        size_t numArgs = args.size();

        while(numArgs--) {
            registers._stack.pop_back();
        }

        if(result)
            *result = registers._stack[0];
    }

    void ExitInstruction(InterpreterRegisters& registers) {
        registers._currInstruction = nullptr;
    }

    void AddIntInstruction(InterpreterRegisters& registers) {
        int16_t rightHandSide = registers._stack.back();
        registers._stack.pop_back();
        int16_t leftHandSide = registers._stack.back();
        registers._stack.pop_back();
        registers._stack.push_back(leftHandSide + rightHandSide);
        ++registers._currInstruction;
    }

    void PushIntInstruction(InterpreterRegisters& registers) {
        registers._stack.push_back(registers._currInstruction->p2);
        ++registers._currInstruction;
    }

    void PopIntInstruction(InterpreterRegisters& registers) {
        registers._stack.pop_back();
        ++registers._currInstruction;
    }

    void PrintIntInstruction(InterpreterRegisters& registers) {
        int16_t number = registers._stack.back();
        registers._stack.pop_back();
        cout << "Number printed: " << number << endl;
        ++registers._currInstruction;
    }

    void CompareIntLessInstruction(InterpreterRegisters& registers) {
        int16_t rightHandSide = registers._stack.back();
        registers._stack.pop_back();
        int16_t leftHandSide = registers._stack.back();
        registers._stack.pop_back();

        registers._stack.push_back(leftHandSide < rightHandSide);
        ++registers._currInstruction;
    }

    void LoadIntInstruction(InterpreterRegisters& registers) {
        registers._stack.push_back(registers._stack[registers._currInstruction->p2]);
        ++registers._currInstruction;
    }

    void StoreIntInstruction(InterpreterRegisters& registers) {
        registers._stack[registers._currInstruction->p2] = registers._stack.back();
        registers._stack.pop_back();
        ++registers._currInstruction;
    }

    void JumpByIfZeroInstruction(InterpreterRegisters& registers) {
        int16_t condition = registers._stack.back();
        registers._stack.pop_back();
        if(condition == 0)
            registers._currInstruction += registers._currInstruction->p2;
        else
            registers._currInstruction++;
    }

    void JumpByInstruction(InterpreterRegisters& registers) {
        registers._currInstruction += registers._currInstruction->p2;
    }

    void LoadIntBasePointerRelativeInstruction(InterpreterRegisters &registers) {
        registers._stack.push_back(registers._stack[registers._currInstruction->p2 + registers._baseIdx]);
        ++registers._currInstruction;
    }

    void StoreIntBasePointerRelativeInstruction(InterpreterRegisters &registers) {
        registers._stack[registers._currInstruction->p2 + registers._baseIdx] = registers._stack.back();
        registers._stack.pop_back();
        ++registers._currInstruction;
    }

    void CallInstruction(InterpreterRegisters& registers) {
        registers._stack.push_back((int16_t(registers._baseIdx)));
        registers._returnAdressStack.push_back(registers._currInstruction+1);
        registers._baseIdx = registers._stack.size();
        registers._currInstruction += registers._currInstruction->p2;
    }

    void ReturnInstruction(InterpreterRegisters& registers) {
        Instruction* returnAdress = registers._returnAdressStack.back();
        registers._returnAdressStack.pop_back();
        registers._baseIdx = registers._stack.back();
        registers._stack.pop_back();
        registers._currInstruction = returnAdress;
    }

}