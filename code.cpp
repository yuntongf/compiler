#include<iostream>
#include<numeric>
#include<algorithm>
#include<memory>
#include<vector>
#include<map>
#include<initializer_list>

using namespace std;

typedef vector<byte> Instructions;
typedef byte OpCode;

// class Iota {
//     private:
//     // byte counter;
//     unsigned char counter;
//     byte* id;

//     public:
//     Iota() {
//         counter = 0;
//     }
//     byte next() {
//         byte temp{counter++};
//         *id = temp;
//         return *id;
//     }
// };
// unique_ptr<Iota> incrementor = make_unique<Iota>();
const OpCode OpConstant{0};

// add definitions for debug purpose
struct Definition {
    string name;
    vector<int> operandWidth;
};
map<OpCode, Definition> defs = {
    {OpConstant, {"OpConstant", vector<int>{4}}} // four bytes wide, assume no more than 65536 constants
};
Definition lookup(byte opcode) {
    return defs[opcode];
}

vector<byte> make(OpCode opcode, vector<int> operands) {
    auto def = defs.find(opcode);
    if (def == defs.end()) return vector<byte>(); // return empty vector when opcode not found, potential risk

    // Calculate instruction length
    int instructLen = 1;
    for (int w : def->second.operandWidth) {
        instructLen += w;
    }
    vector<byte> instruction = {opcode};
    int idx = 0;
    for (auto operand : operands) {
        int width = def->second.operandWidth.at(idx++);
        if (width == 4) { // integer
            for (int i = 3; i >= 0; i--) {
                byte b = (byte) ((operand >> (8 * i)) & 0xFF);
                cout << "bit " << 4 - i << "of OpConst is: " << (int) b << endl;
                instruction.push_back(b);
            }
            // cout << (unsigned int) bytes[0] << endl;
        }
    }
    return instruction;
}