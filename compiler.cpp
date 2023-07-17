#include"bytecode.cpp"
#include"object.cpp"
#include"parser.cpp"

struct ByteCode {
    Instruction instructions;
    vector<unique_ptr<Object>> constants;
};

class Compiler {
    private:
    Instruction instructions;
    vector<unique_ptr<Object>> constants;

    public:
    Compiler() = default;

    template<typename T> int compile(unique_ptr<T> node) {
        string type = node.get()->getType();
        // cout << type <<endl;
        if (type == ntypes.ExpressionStatement) {
            ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(node.get());
            if (compile(move(stmt->expression))) return 1;
        }
        else if (type == ntypes.InfixExpression) {
            InfixExpression* exp = dynamic_cast<InfixExpression*>(node.get());
            if (compile(move(exp->left))) return 1;
            if (compile(move(exp->right))) return 1;
            if (exp->Operator == "+") {
                emit(OpAdd, vector<int>{});
            }
        }
        else if (type == ntypes.IntLiteral) {
            IntLiteral* lit = dynamic_cast<IntLiteral*>(node.get());
            // unique_ptr<Object> integer = make_unique<Integer>(lit->value);
            emit(OpConstant, vector<int>{addConstant(make_unique<Integer>(lit->value))});
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
        return 0;
    }

    int addInstruction(Instruction instruction) {
        int pos = instruction.size();
        instructions.insert(instructions.end(), instruction.begin(), instruction.end());
        return pos;
    }
};