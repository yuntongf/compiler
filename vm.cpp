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

    unique_ptr<Object>& getLastPopped() {
        return stack.at(sp);
    }

    int push(unique_ptr<Object> constant) {
        if (sp >= stackSize) return 1; // stack overflow
        stack.at(sp++) = move(constant);
        return 0;
    }

    unique_ptr<Object>& pop() {
        return stack.at(--sp);
    }

    int run() {
        for (int ip = 0; ip < instructions.size(); ip++) {
            // fetch
            auto opcode = OpCode(instructions.at(ip));
            switch (opcode) {
                case OpConstant: 
                    {   
                        int constIndex = 0;
                        for (int i = 0; i < 4; i++) {
                            constIndex = (constIndex << 8) | (int) instructions.at(++ip);
                        }
                        if (push(move(constants.at(constIndex)))) return 1;
                    }
                    break;
                case OpAdd: case OpMul: case OpDiv: case OpSub:
                    // pop top two elements and add them
                    {   
                        unique_ptr<Object>& right = pop();
                        unique_ptr<Object>& left = pop();
                        if (left.get()->getType() != objs.INTEGER_OBJ || right.get()->getType() != objs.INTEGER_OBJ) {
                            return 1; // wrong 
                        }
                        Integer* leftInt = dynamic_cast<Integer*>(left.get());
                        Integer* rightInt = dynamic_cast<Integer*>(right.get());
                        int res = 0;
                        switch (opcode) {
                            case OpAdd: res = leftInt->value + rightInt->value; break;
                            case OpSub: res = leftInt->value - rightInt->value; break;
                            case OpMul: res = leftInt->value * rightInt->value; break;
                            case OpDiv: res = leftInt->value / rightInt->value; break;
                            default:
                                return 1; // unrecognized operation
                        }
                        unique_ptr<Object> o = make_unique<Integer>(res);
                        push(move(o));
                    }
                    break;
                case OpPop:
                    pop();
                    break;
                default:
                    break;
            }
        }
        return 0;
    }
};