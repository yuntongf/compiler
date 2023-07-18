#include"bytecode.cpp"
#include"object.cpp"
#include"parser.cpp"
#include"symbol.cpp"

struct ByteCode {
    Instruction instructions;
    vector<unique_ptr<Object>> constants;
};

struct EmittedInstruction {
    OpCode opcode;
    int ip;
};

class Compiler {
    private:
    Instruction instructions;
    vector<unique_ptr<Object>> constants;
    EmittedInstruction last;
    EmittedInstruction prevLast;
    SymbolTable symbolTable;

    public:
    Compiler() {
        symbolTable = SymbolTable();
    };

    // Compiler(SymbolTable* st, vector<unique_ptr<Object>>&& constants) : symbolTable(*st), constants(move(constants)) {};

    void setLastInstruction(OpCode opcode, int ip) {
        prevLast = last;
        last = EmittedInstruction{opcode, ip};
    }

    void addConstants(vector<unique_ptr<Object>>&& constants) {
        this->constants = move(constants); 
    }

    void removeIfLastPop() {
        if (last.opcode == OpPop) {
            instructions.pop_back();
            last = prevLast;
        }
    }

    void changeOperand(int ip, vector<int> operand) {
        auto opcode = OpCode(instructions.at(ip));
        auto newInstruction = constructByteCode(opcode, operand);
        // replace instruction
        for (int i = 0; i < newInstruction.size(); i++) {
            instructions.at(ip + i) = newInstruction.at(i);
        }
    }

    template<typename T> int compile(unique_ptr<T> node) {
        string type = node.get()->getType();
        // cout << type <<endl;
        if (type == ntypes.Identifier) {
            Identifier* ident = dynamic_cast<Identifier*>(node.get());
            // auto symbol = symbolTable.resolve(ident->value);
            emit(OpGetGlobal, vector<int>{symbolTable.resolve(ident->value).get()->index});
        }
        else if (type == ntypes.LetStatement) {
            LetStatement* stmt = dynamic_cast<LetStatement*>(node.get());
            // if (compile(move(stmt->identifier))) return 1; // failed to compile identifier expression
            if (compile(move(stmt->value))) return 1; // failed to compile let statement expression
            // store to symbol table
            // auto symbol = symbolTable.define(stmt->identifier.value);
            emit(OpSetGlobal, vector<int>{symbolTable.define(stmt->identifier.value).get()->index});
        }
        else if (type == ntypes.ExpressionStatement) {
            ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(node.get());
            if (compile(move(stmt->expression))) return 1;
            emit(OpPop, vector<int>{});
        }
        else if (type == ntypes.PrefixExpression) {
            PrefixExpression* exp = dynamic_cast<PrefixExpression*>(node.get());
            if (compile(move(exp->right))) return 1; // expression invalid
            if (exp->Operator == "-") {
                emit(OpMinus, vector<int>{});
            } else if (exp->Operator == "!") {
                emit(OpSurprise, vector<int>{});
            }
        }
        else if (type == ntypes.InfixExpression) {
            InfixExpression* exp = dynamic_cast<InfixExpression*>(node.get());
            if (exp->Operator == "<") {
                if (compile(move(exp->right))) return 1;
                if (compile(move(exp->left))) return 1;
                emit(OpGt, vector<int>{});
                return 0;
            }
            if (compile(move(exp->left))) return 1;
            if (compile(move(exp->right))) return 1;

            if (exp->Operator == "+") {
                emit(OpAdd, vector<int>{});
            } else if (exp->Operator == "-") {
                emit(OpSub, vector<int>{});
            } else if (exp->Operator == "*") {
                emit(OpMul, vector<int>{});
            } else if (exp->Operator == "/") {
                emit(OpDiv, vector<int>{});
            } else if (exp->Operator == "==") {
                emit(OpEq, vector<int>{});
            } else if (exp->Operator == "!=") {
                emit(OpNeq, vector<int>{});
            } else if (exp->Operator == ">") {
                emit(OpGt, vector<int>{});
            } else {
                return 1; // unknown operator
            }
        }
        else if (type == ntypes.IntLiteral) {
            IntLiteral* lit = dynamic_cast<IntLiteral*>(node.get());
            // unique_ptr<Object> integer = make_unique<Integer>(lit->value);
            emit(OpConstant, vector<int>{addConstant(make_unique<Integer>(lit->value))});
        }
        else if (type == ntypes.BoolLiteral) {
            BoolLiteral* lit = dynamic_cast<BoolLiteral*>(node.get());
            if (lit->value) emit(OpTrue, vector<int>{});
            else emit(OpFalse, vector<int>{});
        }
        else if (type == ntypes.IfExpression) {
            IfExpression* exp = dynamic_cast<IfExpression*>(node.get());
            if (compile(move(exp->condition))) return 1; // failed to compile condition of if statement
            int posJumpIfFalse = emit(OpJumpIfFalse, vector<int>{-1}); // fix later
            if (compile(move(exp->consequence))) return 1; // failed to compile consequence
            removeIfLastPop(); // do not pop the result of consequence off stack

            int posJump = emit(OpJump, vector<int>{-1});
            changeOperand(posJumpIfFalse, vector<int>{(int) instructions.size()});

            if (exp->alternative == nullptr) {
                emit(OpNull, vector<int>{});
            } else {
                if (compile(move(exp->alternative))) return 1; // failed to compile alternative of if statement
                removeIfLastPop();
            }
            changeOperand(posJump, vector<int>{(int) instructions.size()});
        }
        else if (type == ntypes.BlockStatement) {
            BlockStatement* stmt = dynamic_cast<BlockStatement*>(node.get());
            for (int i = 0; i < stmt->statements.size(); i++) {
                if (compile(move(stmt->statements.at(i)))) return 1;
            }
        }
        return 0;
    }

    int compileProgram(Program* program) {
        for (int i = 0; i < program->statements.size(); i++) {
            if (compile(move(program->statements.at(i)))) return 1;
        }
        return 0;
    }

    ByteCode getByteCode() {
        ByteCode bc = {instructions, move(constants)};
        return bc;
    }

    int addConstant(unique_ptr<Object> obj) {
        constants.push_back(move(obj));
        return constants.size() - 1; // return index of obj in the constant list as the unique id
    }

    int emit(OpCode opcode, vector<int> operands) {
        auto instruction = constructByteCode(opcode, operands);
        // cout << serialize(instruction) << endl;
        int pos = addInstruction(instruction);
        prevLast = last;
        last = EmittedInstruction{opcode, pos};
        return pos;
    }

    int addInstruction(Instruction instruction) {
        int pos = instructions.size();
        instructions.insert(instructions.end(), instruction.begin(), instruction.end());
        return pos;
    }
};