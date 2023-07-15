#include<iostream>
#include<numeric>
#include<algorithm>
#include<memory>
#include<vector>
#include<map>
#include<initializer_list>
#include<sstream>

using namespace std;

typedef vector<byte> Instruction;


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
    vector<int> operandWidths;
};
map<OpCode, Definition> defs = {
    {OpConstant, {"OpConstant", vector<int>{4}}} // four bytes wide, assume no more than 65536 constants
};
int lookup(byte opcode) {
    return defs.count(opcode) > 0 ? 0 : 1;
}


/******************** construct and destruct byte code *******************/
vector<byte> constructByteCode(OpCode opcode, vector<int> operands) {
    auto def = defs.find(opcode);
    if (def == defs.end()) return vector<byte>(); // return empty vector when opcode not found, potential risk

    // Calculate instruction length
    int instructLen = 1;
    for (int w : def->second.operandWidths) {
        instructLen += w;
    }
    vector<byte> instruction = {opcode};
    int idx = 0;
    for (auto operand : operands) {
        int width = def->second.operandWidths.at(idx++);
        if (width == 4) { // integer
            for (int i = 3; i >= 0; i--) {
                byte b = (byte) ((operand >> (8 * i)) & 0xFF);
                // cout << "bit " << 4 - i << "of OpConst is: " << (int) b << endl;
                instruction.push_back(b);
            }
            // cout << (unsigned int) bytes[0] << endl;
        }
    }
    return instruction;
}

pair<vector<int>, int> destructOperandsByteCode(Definition def, vector<byte> bytecode, int offset) {
    vector<int> operands = {};
    int byteCount = 0;
    for (int i = 0; i < def.operandWidths.size(); i++) {
        int width = def.operandWidths.at(i);
        if (width == 4) {
            int operand = 0;
            for (int i = 0; i < 4; i++) {
                operand = (operand << 8) | (int) bytecode.at(offset++);
            }
            operands.push_back(operand);
        }
        byteCount += width;
    }
    // assert(operands.size() == def.operandWidths.size());
    return pair<vector<int>, int>{move(operands), byteCount};
}

/******************** debug print statements *******************/
string formatInstruction(Definition definition, vector<int>& operands) {
    std::ostringstream buffer;
    buffer << definition.name;
    for (int operand : operands) {
        buffer << " " << operand;
    }
    return buffer.str();
}
string formatByteCount(int byteCount) {
    string str = to_string(byteCount);
    int length = str.length();
    for (int i = 0; i < 4 - length; i++) {
        str = "0" + str;
    }
    return str;
};

string serialize(Instruction instruction) {
    int byteCount = 0;
    Definition def;
    ostringstream buffer;
    while (byteCount < instruction.size()) {
        if (lookup(instruction.at(byteCount))) return "";
        else {
            def = defs[instruction.at(byteCount)];
            buffer << formatByteCount(byteCount) << " ";
            byteCount++;
        };
        auto pair = destructOperandsByteCode(def, instruction, byteCount);
        buffer << formatInstruction(def, pair.first);
        byteCount += pair.second;
        if (byteCount < instruction.size()) buffer << "\n";
    }
    return buffer.str();
}

