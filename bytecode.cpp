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
const OpCode OpAdd{1};
const OpCode OpPop{2};
const OpCode OpMul{3};
const OpCode OpSub{4};
const OpCode OpDiv{5};
const OpCode OpTrue{6};
const OpCode OpFalse{7};
const OpCode OpEq{8};
const OpCode OpNeq{9};
const OpCode OpGt{10};
const OpCode OpMinus{11};
const OpCode OpSurprise{12};
const OpCode OpJumpIfFalse{13};
const OpCode OpJump{14};
const OpCode OpNull{15};
const OpCode OpSetGlobal{16};
const OpCode OpGetGlobal{17};
const OpCode OpArray{18};
const OpCode OpHash{19};
const OpCode OpIndex{20};
const OpCode OpCall{21};
const OpCode OpRetVal{22};
const OpCode OpRet{23};


// add definitions for debug purpose
struct Definition {
    string name;
    vector<int> operandWidths;
};
map<OpCode, Definition> defs = {
    {OpConstant, {"OpConstant", vector<int>{4}}},
    {OpAdd, {"OpAdd", vector<int>{}}}, // no operands
    {OpMul, {"OpMul", vector<int>{}}},
    {OpSub, {"OpSub", vector<int>{}}},
    {OpDiv, {"OpDiv", vector<int>{}}},
    {OpPop, {"OpPop", vector<int>{}}},
    {OpTrue, {"OpTrue", vector<int>{}}},
    {OpFalse, {"OpFalse", vector<int>{}}},
    {OpEq, {"OpEq", vector<int>{}}},
    {OpNeq, {"OpNeq", vector<int>{}}},
    {OpGt, {"OpGt", vector<int>{}}},
    {OpMinus, {"OpMinus", vector<int>{}}},
    {OpSurprise, {"OpSurprise", vector<int>{}}},
    {OpJumpIfFalse, {"OpJumpIfFalse", vector<int>{4}}},
    {OpJump, {"OpJump", vector<int>{4}}},
    {OpNull, {"OpNull", vector<int>{}}},
    {OpGetGlobal, {"OpGetGlobal", vector<int>{4}}},
    {OpSetGlobal, {"OpSetGlobal", vector<int>{4}}},
    {OpArray, {"OpArray", vector<int>{4}}},
    {OpHash, {"OpHash", vector<int>{4}}},
    {OpIndex, {"OpIndex", vector<int>{}}},
    {OpCall, {"OpCall", vector<int>{}}},
    {OpRetVal, {"OpRetVal", vector<int>{}}},
    {OpRet, {"OpRet", vector<int>{}}}
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
        if (lookup(instruction.at(byteCount))) {
            cout << "lookup: no instruction found" <<endl;
            return "";
        }
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

