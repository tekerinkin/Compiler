#include <iostream>
#include <cassert>
#include "Parser/include/Tokenizer.hpp"
#include "Parser/include/Parser.h"
#include "Interpreter/include/Interpreter.h"
#include "Interpreter/include/Instruction.h"

using namespace std;
using namespace simpleparser;
using namespace interpreter;

struct Parameter {
    string _name;
    size_t _index;
};

struct CompiledFunction {
    size_t _instructionOffset;
    size_t _numArguments;
    bool _returnSmth;
};

void generateCodeForStatement(const Statement& currStatement,
                              const map<string, int16_t>& variableOffset,
                              map<string, Parameter> parameters,
                              vector<int16_t>& returnCmdJmpInstructions,
                              vector<Instruction>& compiledCode,
                              map<string, CompiledFunction>& functionToInstruction) {
    switch (currStatement._kind) {
        case StatementKind::VARIABLE_DECLARATION:
            switch (currStatement._type._type) {
                case simpleparser::INT32: {
                    if (!currStatement._parameters.empty()) {
                        const auto &initialValueParsed = currStatement._parameters[0];

                        if (initialValueParsed._kind != StatementKind::LITERAL) {
                            auto foundVar = variableOffset.find(currStatement._name);
                            if (foundVar == variableOffset.end())
                                throw runtime_error(string("Unknown variable \"") + currStatement._name + "\"");

                            generateCodeForStatement(initialValueParsed, variableOffset,
                                                     parameters, returnCmdJmpInstructions,
                                                     compiledCode, functionToInstruction);
                            compiledCode.push_back(Instruction{interpreter::STORE_INT_BASEPOINTER_RELATIVE, 0,
                                                               foundVar->second});
                        }
                    }
                    break;
                }
            }
            break;

        case StatementKind::FUNCTION_CALL:
            if (currStatement._name == "return") {
                if (currStatement._parameters.size() != 1)
                    throw runtime_error("Function \"return\" expects a single parameter");
                generateCodeForStatement(currStatement._parameters[0], variableOffset,
                                         parameters, returnCmdJmpInstructions,
                                         compiledCode, functionToInstruction);
                compiledCode.push_back(Instruction{interpreter::STORE_INT_BASEPOINTER_RELATIVE,
                                                   0, int16_t(-2 - parameters.size())});
                returnCmdJmpInstructions.push_back(compiledCode.size());
                compiledCode.push_back(Instruction{interpreter::JUMP_BY, 0, 0});
            } else if (currStatement._name == "printNum") {
                if (currStatement._parameters.size() != 1)
                    throw runtime_error("Function \"printNum\" expects a single parameter");
                generateCodeForStatement(currStatement._parameters[0], variableOffset,
                                         parameters, returnCmdJmpInstructions,
                                         compiledCode, functionToInstruction);
                compiledCode.push_back(Instruction{interpreter::PRINT_INT, 0, 0});
            } else {
                auto foundFunction = functionToInstruction.find(currStatement._name);
                if (foundFunction == functionToInstruction.end())
                    throw runtime_error(string("Unknown function \"") + currStatement._name + "\" called");

                if (foundFunction->second._returnSmth) {
                    compiledCode.push_back(Instruction{interpreter::PUSH_INT, 0, 0});
                }

                if (foundFunction->second._numArguments != currStatement._parameters.size())
                    throw runtime_error(string("Function ") + currStatement._name + " requires "
                                        + to_string(foundFunction->second._numArguments) + " arguments, but received "
                                        + to_string(currStatement._parameters.size()));

                for (auto &currParam: currStatement._parameters) {
                    generateCodeForStatement(currParam, variableOffset, parameters, returnCmdJmpInstructions,
                                             compiledCode, functionToInstruction);
                }

                size_t relativeJumpAddress = foundFunction->second._instructionOffset - compiledCode.size();
                compiledCode.push_back(Instruction{interpreter::CALL, 0, int16_t(relativeJumpAddress)});
                for (size_t x = currStatement._parameters.size(); x > 0; --x) {
                    compiledCode.push_back(Instruction{interpreter::POP_INT, 0, 0});
                }

            }
            break;
        case StatementKind::LITERAL:
            switch (currStatement._type._type) {
                case VOID:
                    break;
                case INT8:
                    break;
                case UINT8:
                    break;
                case INT32: {
                    int32_t initialValue = stoi(currStatement._name);
                    compiledCode.push_back(Instruction{interpreter::PUSH_INT, 0,
                                                       int16_t(initialValue)});
                    break;
                }
                case UINT32:
                    break;
                case DOUBLE:
                    break;
                case STRUCT:
                    break;
            }
            break;
        case StatementKind::OPERATOR_CALL:
            if (currStatement._parameters.size() != 2)
                throw runtime_error(string("Wrong number of parameters passed to operator \"")
                                    + currStatement._name + "\"");
            if (currStatement._name == "+" || currStatement._name == "<") {
                Opcode op = interpreter::ADD_INT;
                if (currStatement._name == "<")
                    op = interpreter::COMP_INT_LT;

                for (auto &currParam: currStatement._parameters)
                    generateCodeForStatement(currParam, variableOffset,
                                             parameters, returnCmdJmpInstructions,
                                             compiledCode, functionToInstruction);
                compiledCode.push_back(Instruction{op, 0, 0});
            } else if (currStatement._name == "=") {
                auto foundVar = variableOffset.find(currStatement._parameters[0]._name);
                if(foundVar == variableOffset.end())
                    throw runtime_error(string("Unknown variable \"") + currStatement._parameters[0]._name + "\"");
                generateCodeForStatement(currStatement._parameters[1], variableOffset,
                                         parameters, returnCmdJmpInstructions,
                                         compiledCode, functionToInstruction);
                compiledCode.push_back(Instruction{interpreter::STORE_INT_BASEPOINTER_RELATIVE,
                                                   0, foundVar->second});
            }
            break;

            case StatementKind::VARIABLE_NAME: {
            auto foundVar = variableOffset.find(currStatement._name);
            if(foundVar != variableOffset.end()) {
                compiledCode.push_back(Instruction{interpreter::LOAD_INT_BASEPOINTER_RELATIVE,
                                                   0, foundVar->second});
                break;
            }

            auto foundParam = parameters.find(currStatement._name);
            if(foundParam != parameters.end()) {
                compiledCode.push_back(Instruction{interpreter::LOAD_INT_BASEPOINTER_RELATIVE,
                                                   0, int16_t(-1 - parameters.size() + foundParam->second._index)});
                break;
            }
            throw runtime_error(string("Unknown variable \"") + currStatement._parameters[0]._name + "\"");
        }

        case StatementKind::WHILE_LOOP: {
            size_t conditionOffset = compiledCode.size();
            generateCodeForStatement(currStatement._parameters[0], variableOffset,
                                     parameters, returnCmdJmpInstructions,
                                     compiledCode, functionToInstruction);
            size_t conditionFalseJumpInstructionOffset = compiledCode.size();
            compiledCode.push_back(Instruction{interpreter::JUMP_BY_IF_ZERO, 0, 0});

            for(auto stmt = currStatement._parameters.begin() + 1; stmt != currStatement._parameters.end(); ++stmt) {
                compiledCode.push_back(Instruction{interpreter::JUMP_BY, 0,
                                                   int16_t(conditionOffset - compiledCode.size())});
                compiledCode[conditionFalseJumpInstructionOffset].p2 =
                        int16_t(compiledCode.size()- conditionFalseJumpInstructionOffset);
                break;
            }
        }
    }
}

void generateCodeForFunction(const FunctionDefinition& currFunc, vector<Instruction>& compileCode,
                             map<string, CompiledFunction>& functionToInstruction) {
    int numIntVariable = 0;
    vector<int16_t> returnCndJumpInstructions;
    map<string, int16_t> variableOffsets;
    map<string, Parameter> parameters;

    functionToInstruction[currFunc._name] = CompiledFunction{
        compileCode.size(),
        currFunc._parameters.size(),
        currFunc._returnsSmth
    };

    size_t paramIdx = 0;
    for(auto& currParamDef : currFunc._parameters) {
        parameters[currParamDef._parameterName] = Parameter{currParamDef._parameterName, paramIdx++};
    }

    for(const auto& currStatement : currFunc._statements) {
        switch(currStatement._kind) {
            case StatementKind::VARIABLE_DECLARATION:
                switch(currStatement._type._type) {
                    case VOID:
                        break;
                    case INT8:
                        break;
                    case UINT8:
                        break;
                    case INT32: {
                        int32_t initialValue = 0;
                        if(!currStatement._parameters.empty()) {
                            const auto& initialValueParsed = currStatement._parameters[0];
                            if(initialValueParsed._kind == StatementKind::LITERAL) {
                                assert(initialValueParsed._type._type == currStatement._type._type);
                                initialValue = stoi(initialValueParsed._name);
                            }
                        }
                        variableOffsets[currStatement._name] = numIntVariable;
                        ++numIntVariable;
                        compileCode.push_back(Instruction{interpreter::PUSH_INT,
                                                          0, int16_t(initialValue)});
                        break;
                    }
                    case UINT32:
                        break;
                    case DOUBLE:
                        break;
                    case STRUCT:
                        break;
                }
                break;
            default:
                break;
        }
    }

    for(const auto& currStmt : currFunc._statements) {
        generateCodeForStatement(currStmt, variableOffsets,
                                 parameters, returnCndJumpInstructions,
                                 compileCode, functionToInstruction);
    }

    size_t cleanupCodeOffset = compileCode.size();

    for(auto returnCmdJumpInstructionIdx : returnCndJumpInstructions) {
        compileCode[returnCmdJumpInstructionIdx].p2 = cleanupCodeOffset - returnCmdJumpInstructionIdx;
    }

    for(auto x = 0; x < numIntVariable; ++x)
        compileCode.push_back(Instruction{interpreter::POP_INT, 0, 0});

    compileCode.push_back(Instruction{interpreter::RETURN, 0, 0});
}

int main() {
    try {
        std::cout << "Compiler 01.\n" << endl;

        FILE* fh = fopen("/Users/dimashestakov/Desktop/Compiler/compiler.myc", "r");
        if(!fh)
            cerr << "Can't find file" << endl;

        fseek(fh, 0, SEEK_END);
        size_t fileSize = ftell(fh);
        fseek(fh, 0, SEEK_SET);
        string fileContents(fileSize, ' ');
        fread(fileContents.data(), 1, fileSize, fh);

        cout << fileContents << endl << endl;

        Tokenizer tokenizer;
        vector<Token> tokens = tokenizer.parse(fileContents);

        for(Token currToken : tokens)
            currToken.debugPrint();

        Parser parser;
        parser.parse(tokens);

        parser.debugPrint();

        vector<Instruction> compiledCode;

        map<string, FunctionDefinition> functions = parser.getFunctions();
        map<string, CompiledFunction> functionToInstruction;

        for(auto& [_, func] : functions)
            generateCodeForFunction(func, compiledCode, functionToInstruction);

        int16_t result = 0;
        size_t mainFunctionOffset = SIZE_MAX;
        auto foundFunction = functionToInstruction.find("main");
        if(foundFunction != functionToInstruction.end())
            throw runtime_error("Couldn't find main function");

        Interpreter::Run(compiledCode.data() + foundFunction->second._instructionOffset, {3}, &result);

        cout << "\nResult: " << result << "\ndone" << endl;
    } catch(exception& e) {
        cerr << "Error: " << e.what() << endl;
    } catch(...) {
        cerr << "Unknown error" << endl;
        return 1;
    }
    return 0;
}