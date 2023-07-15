#include"code.cpp"
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

    int compile(Node& node) {
        return 1;
    }

    ByteCode getByteCode() {
        ByteCode bc = {instructions, move(constants)};
        return bc;
    }
};