#include"compiler.cpp"
#include<iostream>

using namespace std;

const int stackSize = 2048;
const int globalsSize = 4096;

class VM {
    public:
    vector<unique_ptr<Object>> constants;
    vector<unique_ptr<Object>> globals;
    Instruction instructions;

    vector<unique_ptr<Object>> stack;
    int sp; // always points to the next free slot in stack

    VM(ByteCode bytecode) {
        instructions = bytecode.instructions;
        constants = move(bytecode.constants);
        globals = vector<unique_ptr<Object>>(globalsSize);
        stack = vector<unique_ptr<Object>>(stackSize);
        sp = 0;
    };

    unique_ptr<Object>& getLastPopped() {
        // cout << "sp is" << sp << endl;
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
    
    int isTrue(unique_ptr<Object>& obj) {
        string type = obj.get()->getType();
        if (type == objs.BOOLEAN_OBJ) {
            Boolean* boolean = dynamic_cast<Boolean*>(obj.get());
            return boolean->value;
        } else if (type == objs.NULL_OBJ) {
            return false;
        } else if (type == objs.INTEGER_OBJ) {
            Integer* integer = dynamic_cast<Integer*>(obj.get());
            return integer->value;
        } 
        // else if (type == objs.STRING_OBJ) {
        //     String* str = dynamic_cast<String*>(obj.get());
        //     return str->value
        // } 
        else {
            return -1; // cannot assign boolean value to object type 
        }
    }

    int run() {
        for (int ip = 0; ip < instructions.size(); ip++) { // ip at the end of the loop points the the last executed instruction
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
                {
                    unique_ptr<Object>& right = pop();
                    unique_ptr<Object>& left = pop();

                    // string concat
                    if (opcode == OpAdd && left.get()->getType() == objs.STRING_OBJ && right.get()->getType() == objs.STRING_OBJ) {
                        String* leftStr = dynamic_cast<String*>(left.get());
                        String* rightStr = dynamic_cast<String*>(right.get());
                        unique_ptr<Object> o = make_unique<String>(leftStr->value + rightStr->value);
                        if (push(move(o))) return 1; // failed to push str to stack
                        break;
                    }

                    // integer arithemtic
                    if (left.get()->getType() != objs.INTEGER_OBJ || right.get()->getType() != objs.INTEGER_OBJ) {
                        return 1; // wrong type
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
                    if (push(move(o))) return 1;
                }    
                break;
                case OpTrue: if (push(make_unique<Boolean>(true))) return 1; break;
                case OpFalse: if (push(make_unique<Boolean>(false))) return 1; break;
                case OpEq: case OpNeq: case OpGt:
                    {   
                        int right = isTrue(pop());
                        int left = isTrue(pop());
                        if (right == -1 || left == -1) return 1; // cannot assign boolean value to obj
                        bool res = false;
                        switch (opcode) {
                            case OpEq: res = left == right; break;
                            case OpNeq: res = left != right; break;
                            case OpGt: res = left > right; break;
                            default:
                                return 1; // unrecognized operation
                        }
                        unique_ptr<Object> o = make_unique<Boolean>(res);
                        if (push(move(o))) return 1;

                    }
                    break;
                case OpMinus:
                    {
                        unique_ptr<Object>& operand = pop();
                        if (operand.get()->getType() != objs.INTEGER_OBJ) return 1; // invalid operand for prefix operator '-'
                        Integer* integer = dynamic_cast<Integer*>(operand.get());
                        push(make_unique<Integer>(-integer->value));
                    }
                    break;
                case OpSurprise:
                    {
                        int boolean = isTrue(pop());
                        if (boolean == -1) return 1; // cannot assign boolean value to operand
                        push(make_unique<Boolean>(!boolean));
                    }
                    break;
                case OpPop:
                    pop();
                    break;
                case OpNull:
                    if (push(make_unique<Null>())) return 1; // failed to push null to stack
                    break;
                case OpJump:
                {
                    int jumpAddr = 0;
                    for (int i = 0; i < 4; i++) {
                        jumpAddr = (jumpAddr << 8) | (int) instructions.at(++ip);
                    }
                    ip = jumpAddr - 1;
                }
                break;
                case OpJumpIfFalse:
                {
                    int jumpAddr = 0;
                    for (int i = 0; i < 4; i++) {
                        jumpAddr = (jumpAddr << 8) | (int) instructions.at(++ip);
                    }

                    int condition = isTrue(pop());
                    if (!condition) {
                        ip = jumpAddr - 1;
                    }
                }
                break;
                case OpGetGlobal:
                {   
                    int index = 0;
                    for (int i = 0; i < 4; i++) {
                        index = (index << 8) | (int) instructions.at(++ip);
                    }
                    unique_ptr<Object> up = copyPtr(globals.at(index));
                    push(move(up));
                }
                break;
                case OpSetGlobal:
                {   
                    int index = 0;
                    for (int i = 0; i < 4; i++) {
                        index = (index << 8) | (int) instructions.at(++ip);
                    }
                    globals.at(index) = move(pop());
                }
                break;
                case OpArray:
                {
                    int numElements = 0;
                    for (int i = 0; i < 4; i++) {
                        numElements = (numElements << 8) | (int) instructions.at(++ip);
                    }

                    // build array
                    auto elements = vector<unique_ptr<Object>>(numElements);
                    for (int p = 0; p < numElements; p++) {
                        elements.at(p) = move(stack.at(sp-numElements + p));
                    }
                    push(make_unique<Array>(move(elements)));
                }
                break;
                default:
                    break;
            }
        }
        return 0;
    }

    template<typename T> unique_ptr<Object> copyPtr(unique_ptr<T>& up) {
        string type = up.get()->getType();
        if (type == objs.BOOLEAN_OBJ) {
            Boolean* boolean = dynamic_cast<Boolean*>(up.get());
            return make_unique<Boolean>(*boolean);
        } else if (type == objs.INTEGER_OBJ) {
            Integer* integer = dynamic_cast<Integer*>(up.get());
            return make_unique<Integer>(*integer);
        } else {
            return make_unique<Null>();
        }
        
    }

    unique_ptr<Object> buildArray(int start, int end) {
        auto elements = vector<unique_ptr<Object>>(end - start);
        for (int p = start; p < end; p++) {
            elements.at(p-start) = move(stack.at(p));
        }
        return make_unique<Array>(move(elements));
    }
};