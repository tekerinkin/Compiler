#include <iostream>
#include <vector>
#include "../include/Instruction.h"
#include "../include/Interpreter.h"

using namespace std;
using namespace interpreter;

int main() {

    cout << "interpreter 1.0\n" << endl;

    Instruction code[] = {
            Instruction{PUSH_INT, 0, 0}, // x
            Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0, 0}, // load x
            Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0, -2}, // load parameter 1
            Instruction{COMP_INT_LT, 0, 0}, // x < 10
            Instruction{JUMP_BY_IF_ZERO, 0, 10}, // if x >= 10 bail!

            Instruction{PUSH_INT, 0, 4000},
            Instruction{PUSH_INT, 0, 1042},
            Instruction{ADD_INT, 0, 0},
            Instruction{PRINT_INT, 0, 0},

            Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0, 0}, // load x
            Instruction{PUSH_INT, 0, 1}, // load 1
            Instruction{ADD_INT, 0, 0}, // x + 1
            Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0, 0}, // x = (x + 1)
            Instruction{JUMP_BY, 0, -12}, // loop back to condition.

            Instruction{PUSH_INT, 0, 0}, // load 0.
            Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0, -2}, // load parameter 1.
            Instruction{COMP_INT_LT, 0, 0}, // 0 < parameter 1.
            Instruction{JUMP_BY_IF_ZERO, 0, 8}, // jump past recursive call.
            Instruction{PUSH_INT, 0, 0}, // reserve space for result.
            Instruction{LOAD_INT_BASEPOINTER_RELATIVE, 0, -2}, // load parameter 1.
            Instruction{PUSH_INT, 0, -1}, // load -1.
            Instruction{ADD_INT, 0, 0}, // parameter 1 - 1
            Instruction{CALL, 0, -22}, // call ourselves.
            Instruction{POP_INT, 0, 0}, // pop parameter.
            Instruction{POP_INT, 0, 0}, // pop result.

            Instruction{PUSH_INT, 0, 42}, // load 42
            Instruction{STORE_INT_BASEPOINTER_RELATIVE, 0, -3}, // returnValue = 42
            Instruction{JUMP_BY, 0, 1}, // jump to function's epilog + actually return.
            Instruction{POP_INT, 0, 0}, // delete 'x'.
            Instruction{RETURN, 0, 0}
    };

    int16_t result = 0;
    Interpreter::Run(code, {3}, &result);

    cout << "\nResult: " << result << "\ndone" << endl;

    return 0;
}
