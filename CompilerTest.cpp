#include"compiler.cpp"
#include<iostream>
#include<gtest/gtest.h>

using namespace std;

TEST(CompilerTest, ValueTest) {
    struct Test {
        OpCode opcode;
        vector<int> operands;
        vector<byte> expected;
    };
    vector<Test> tests = {
        {OpConstant, vector<int>{65534}, vector<byte>{OpConstant, (byte) 0, (byte) 0, (byte) 255, (byte)254}},
        {OpAdd, vector<int>{}, vector<byte>{OpAdd}}
    };
    for (Test test : tests) {
        auto instruction = constructByteCode(test.opcode, test.operands);
        ASSERT_EQ(instruction.size(), test.expected.size());
        for (int i = 0; i < test.expected.size(); i++) {
            ASSERT_EQ(test.expected.at(i), instruction.at(i));
        }
    }
}

void testInstructions(Instruction expected, Instruction actual ) {
    ASSERT_EQ(expected.size(), actual.size());
    for (int i = 0; i < expected.size(); i++) {
        ASSERT_EQ(expected.at(i), actual.at(i));
    }
};

template<typename T> void testConstants(vector<T> expected, vector<unique_ptr<Object>> actual) {
    ASSERT_EQ(expected.size(), actual.size());
    if (is_same<T, int>::value) {
        for (int i = 0; i < expected.size(); i++) {
            Integer* lit = dynamic_cast<Integer*>(actual.at(i).get());
            ASSERT_EQ(expected.at(i), lit->value);
        }
    }
}

Instruction concatInstructions(vector<Instruction> instructions) {
    Instruction res = instructions.at(0);
    for (int i = 1; i < instructions.size(); i++) {
        Instruction second = instructions.at(i);
        res.insert(res.end(), second.begin(), second.end());
    }
    return res;
}

TEST(CompilerTest, ReadOperandsTest) {
    struct Test {
        OpCode opcode;
        vector<int> operands;
    };

    vector<Test> tests = {
        {OpConstant, vector<int>{65535}}
    };

    for (auto test : tests) {
        auto instruction = constructByteCode(test.opcode, test.operands);
        if (lookup(test.opcode)) FAIL() << "Opcode does not exist..." << endl;
        vector<int> operandsRead = destructOperandsByteCode(defs[test.opcode], instruction, 1).first;
        for (int i = 0; i < operandsRead.size(); i++) {
            ASSERT_EQ(operandsRead.at(i), test.operands.at(i));
        }
    }
}

TEST(CompilerTest, InstructionSerializeTest) {
    vector<Instruction> instructions = {
        constructByteCode(OpAdd, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpConstant, vector<int>{65534})
    };
    string expected = "0000 OpAdd\n0001 OpConstant 1\n0006 OpConstant 2\n0011 OpConstant 65534";
    Instruction concat = concatInstructions(instructions);

    ASSERT_EQ(serialize(concat), expected);
}

TEST(CompilerTest, ArithmeticTest) {
    string input = "1 + 2";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    
    auto compiler = Compiler();
    int err = compiler.compileProgram(&program);
    if (err) FAIL() << "test failed due to error in compiler..." << endl;

    auto bytecode = compiler.getByteCode();
    vector<Instruction> expected = {
        constructByteCode(OpConstant, vector<int>{0}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpAdd, vector<int>{}),
        constructByteCode(OpPop, vector<int>{})
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 2}, move(bytecode.constants));
}

TEST(CompilerTest, PopTest) {
    string input = "1; 2;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    
    auto compiler = Compiler();
    int err = compiler.compileProgram(&program);
    if (err) FAIL() << "test failed due to error in compiler..." << endl;

    auto bytecode = compiler.getByteCode();
    vector<Instruction> expected = {
        constructByteCode(OpConstant, vector<int>{0}),
        constructByteCode(OpPop, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpPop, vector<int>{})
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 2}, move(bytecode.constants));
}

TEST(CompilerTest, BooleanTest) {
    string input = "true; false;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    
    auto compiler = Compiler();
    int err = compiler.compileProgram(&program);
    if (err) FAIL() << "test failed due to error in compiler..." << endl;

    auto bytecode = compiler.getByteCode();
    vector<Instruction> expected = {
        constructByteCode(OpTrue, vector<int>{}),
        constructByteCode(OpPop, vector<int>{}),
        constructByteCode(OpFalse, vector<int>{}),
        constructByteCode(OpPop, vector<int>{})
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
}

TEST(CompilerTest, ComparisonTest) {
    string input = "1 == 1; 2 != 3; 2 < 3; 3 > 2;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    
    auto compiler = Compiler();
    int err = compiler.compileProgram(&program);
    if (err) FAIL() << "test failed due to error in compiler..." << endl;

    auto bytecode = compiler.getByteCode();
    vector<Instruction> expected = {
        constructByteCode(OpConstant, vector<int>{0}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpEq, vector<int>{}),
        constructByteCode(OpPop, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpConstant, vector<int>{3}),
        constructByteCode(OpNeq, vector<int>{}),
        constructByteCode(OpPop, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{4}),
        constructByteCode(OpConstant, vector<int>{5}),
        constructByteCode(OpGt, vector<int>{}),
        constructByteCode(OpPop, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{6}),
        constructByteCode(OpConstant, vector<int>{7}),
        constructByteCode(OpGt, vector<int>{}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 1, 2, 3, 3, 2, 3, 2}, move(bytecode.constants));
}

TEST(CompilerTest, PrefixTest) {
    string input = "!false; -1;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    
    auto compiler = Compiler();
    int err = compiler.compileProgram(&program);
    if (err) FAIL() << "test failed due to error in compiler..." << endl;

    auto bytecode = compiler.getByteCode();
    vector<Instruction> expected = {
        constructByteCode(OpFalse, vector<int>{}),
        constructByteCode(OpSurprise, vector<int>{}),
        constructByteCode(OpPop, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{0}),
        constructByteCode(OpMinus, vector<int>{}),
        constructByteCode(OpPop, vector<int>{})
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}