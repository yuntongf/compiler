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

struct CompilationScope {
    Instruction instructions;
    EmittedInstruction last;
    EmittedInstruction prevLast;
};

class Compiler {
    private:
    Instruction instructions;
    vector<unique_ptr<Object>> constants;
    SymbolTable symbolTable;

    public:
    vector<unique_ptr<CompilationScope>> scopes;
    int scopeIndex;

    Compiler() {
        symbolTable = SymbolTable();
        auto scope = CompilationScope{Instruction{}, EmittedInstruction{}, EmittedInstruction{}};
        scopes.push_back(make_unique<CompilationScope>(scope));
        scopeIndex = 0;
    };

    // Compiler(SymbolTable* st, vector<unique_ptr<Object>>&& constants) : symbolTable(*st), constants(move(constants)) {};

    CompilationScope* getCurrScope() {
        return scopes.at(scopeIndex).get();
    }

    Instruction getCurrInstructions() {
        return scopes.at(scopeIndex).get()->instructions;
    }

    
    void setLastInstruction(OpCode opcode, int ip) {
        auto scope = getCurrScope();
        auto prevLast = scope->last;
        auto last = EmittedInstruction{opcode, ip};

        scope->prevLast = prevLast;
        scope->last = last;
    }

    int removeIfLastIs(OpCode opcode) {
        CompilationScope* scope = getCurrScope();
        if (scope == nullptr) return 1; // invalid scope
        if (scope->last.opcode == opcode) {
            auto prevLast = scope->prevLast;
            scope->instructions.pop_back();
            scope->last = prevLast;
        }
        return 0;
    }

    int replaceIfLastIs(OpCode opcode, OpCode update) {
        CompilationScope* scope = getCurrScope();
        if (scope == nullptr) return 1; // invalid scope
        if (scope->last.opcode == opcode) {
            scope->instructions.at(scope->last.ip) = update;
            scope->last = EmittedInstruction{update, scope->last.ip};
        }
        return 0;
    }

    int addIfLastIsNot(OpCode opcode, OpCode update) {
        CompilationScope* scope = getCurrScope();
        if (scope == nullptr) return 1; // invalid scope
        if (scope->last.opcode != opcode) {
            emit(update, vector<int>{});
        }
        return 0;
    }

    void changeOperand(int ip, vector<int> operand) {
        auto scope = getCurrScope();
        auto opcode = OpCode(scope->instructions.at(ip));
        auto newInstruction = constructByteCode(opcode, operand);
        // replace instruction
        for (int i = 0; i < newInstruction.size(); i++) {
            scope->instructions.at(ip + i) = newInstruction.at(i);
        }
    }

    template<typename T> int compile(unique_ptr<T> node) {
        string type = node.get()->getType();
        if (type == ntypes.Identifier) {
            Identifier* ident = dynamic_cast<Identifier*>(node.get());
            emit(OpGetGlobal, vector<int>{symbolTable.resolve(ident->value).get()->index});
        }
        else if (type == ntypes.LetStatement) {
            LetStatement* stmt = dynamic_cast<LetStatement*>(node.get());
            if (compile(move(stmt->value))) return 1; // failed to compile let statement expression
            // store to symbol table
            emit(OpSetGlobal, vector<int>{symbolTable.define(stmt->identifier.value).get()->index});
        }
        else if (type == ntypes.FnLiteral) {
            FnLiteral* fn = dynamic_cast<FnLiteral*>(node.get());
            enterScope();
            if (compile(move(fn->body))) return 1; // failed to compile func body
            if (replaceIfLastIs(OpPop, OpRetVal)) return 1; // failed to replace pop instruction with return instruction
            if (addIfLastIsNot(OpRetVal, OpRet)); // handle empty function
            auto instructions = leaveScope();
            auto compiledFn = CompiledFunction(instructions);
            int constIdx = addConstant(make_unique<CompiledFunction>(compiledFn));
            emit(OpConstant, vector<int>{constIdx});
        }
        else if (type == ntypes.ReturnStatement) {
            ReturnStatement* stmt = dynamic_cast<ReturnStatement*>(node.get());
            if (compile(move(stmt->value))) return 1; // failed to compile return statement
            emit(OpRetVal, vector<int>{});
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
            if (removeIfLastIs(OpPop)) return 1; // do not pop the result of consequence off stack

            int posJump = emit(OpJump, vector<int>{-1});
            changeOperand(posJumpIfFalse, vector<int>{(int) getCurrScope()->instructions.size()});

            if (exp->alternative == nullptr) {
                emit(OpNull, vector<int>{});
            } else {
                if (compile(move(exp->alternative))) return 1; // failed to compile alternative of if statement
                if (removeIfLastIs(OpPop)) return 1;
            }
            changeOperand(posJump, vector<int>{(int) getCurrScope()->instructions.size()});
        }
        else if (type == ntypes.BlockStatement) {
            BlockStatement* stmt = dynamic_cast<BlockStatement*>(node.get());
            for (int i = 0; i < stmt->statements.size(); i++) {
                if (compile(move(stmt->statements.at(i)))) return 1;
            }
        }
        else if (type == ntypes.StringLiteral) {
            StringLiteral* lit = dynamic_cast<StringLiteral*>(node.get());
            emit(OpConstant, vector<int>{addConstant(make_unique<String>(lit->value))});
        }
        else if (type == ntypes.ArrayLiteral) {
            ArrayLiteral* arr = dynamic_cast<ArrayLiteral*>(node.get());
            for (auto& exp : arr->elements) {
                if (compile(move(exp))) return 1; // error compiling element in array
            }
            emit(OpArray, vector<int>{(int) arr->elements.size()});
        }
        else if (type == ntypes.HashLiteral) {
            HashLiteral* hash = dynamic_cast<HashLiteral*>(node.get());
            for (auto& pair : hash->pairs) {
                if (compile(move(pair.first))) return 1;
                if (compile(move(pair.second))) return 1;
            }
            emit(OpHash, vector<int>{(int) hash->pairs.size() * 2});
        }
        else if (type == ntypes.IndexExpression) {
            IndexExpression* index = dynamic_cast<IndexExpression*>(node.get());
            if (compile(move(index->entity))) return 1; // failed to compile entity
            if (compile(move(index->index))) return 1; // failed to compile index
            emit(OpIndex, vector<int>{});
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
        ByteCode bc = {getCurrScope()->instructions, move(constants)};
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
        auto scope = getCurrScope();
        scope->prevLast = scope->last;
        scope->last = EmittedInstruction{opcode, pos};
        return pos;
    }

    int addInstruction(Instruction instruction) {
        Instruction temp = getCurrInstructions();
        int pos = temp.size();
        temp.insert(temp.end(), instruction.begin(), instruction.end());
        scopes.at(scopeIndex).get()->instructions = temp; // suspect
        return pos;
    }

    void enterScope() {
        auto scope = CompilationScope{
            Instruction{}, 
            EmittedInstruction{}, 
            EmittedInstruction{}
        };
        scopes.push_back(make_unique<CompilationScope>(scope));
        scopeIndex++;
    }

    Instruction leaveScope() {
        auto instructions = getCurrInstructions();
        scopes.pop_back();
        scopeIndex--;
        return instructions;
    }
};