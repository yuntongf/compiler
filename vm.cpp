#include"compiler.cpp"
#include<iostream>

using namespace std;

const int stackSize = 2048;

class VM {
    public:
    vector<unique_ptr<Object>> constants;
    Instruction instructions;

    vector<unique_ptr<Object>> stack;
    int sp; // always points to the next free slot in stack

    VM(ByteCode bytecode) {
        instructions = bytecode.instructions;
        constants = move(bytecode.constants);
        stack = vector<unique_ptr<Object>>(stackSize);
        sp = 0;
    }

    unique_ptr<Object> peek() {
        if (sp == 0) return nullptr;
        return move(stack.at(sp - 1));
    }

    int push(unique_ptr<Object> constant) {
        if (sp >= stackSize) return 1; // stack overflow
        stack.at(sp++) = move(constant);
        return 0;
    }

    int run() {
        for (int ip = 0; ip < instructions.size(); ip++) {
            // fetch
            auto opcode = OpCode(instructions.at(ip));
            switch (opcode) {
                case OpConstant:
                    int constIndex = 0;
                    for (int i = 0; i < 4; i++) {
                        constIndex = (constIndex << 8) | (int) instructions.at(++ip);
                    }
                    if (push(move(constants.at(constIndex)))) return 1;
            }
        }
        return 0;
    }
};