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
    // else if (is_same<T, string>::value) {
    //     for (int i = 0; i < expected.size(); i++) {
    //         String* lit = dynamic_cast<String*>(actual.at(i).get());
    //         ASSERT_EQ(expected.at(i), lit->value);
    //     }
    // } 
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

TEST(CompilerTest, ConditionalNullTest) {
    string input = "if (false) {1}; 2;";
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
        constructByteCode(OpFalse, vector<int>{}), // 0000
        constructByteCode(OpJumpIfFalse, vector<int>{16}), // 0001
        constructByteCode(OpConstant, vector<int>{0}), // 0006
        constructByteCode(OpJump, vector<int>{17}), // 0011
        constructByteCode(OpNull, vector<int>{}), // 0016
        constructByteCode(OpPop, vector<int>{}), //0017
        constructByteCode(OpConstant, vector<int>{1}), //0018
        constructByteCode(OpPop, vector<int>{}) // 0023
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 2}, move(bytecode.constants));
}

TEST(CompilerTest, IfElseTest) {
    string input = "if (true) {1} else {2}; 3;";
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
        constructByteCode(OpTrue, vector<int>{}), // 0000
        constructByteCode(OpJumpIfFalse, vector<int>{16}), // 0001
        constructByteCode(OpConstant, vector<int>{0}), // 0006
        constructByteCode(OpJump, vector<int>{21}), // 0011
        constructByteCode(OpConstant, vector<int>{1}), // 0016
        constructByteCode(OpPop, vector<int>{}), //0021
        constructByteCode(OpConstant, vector<int>{2}), //0022
        constructByteCode(OpPop, vector<int>{}) // 0027
    };
    // cout << serialize(bytecode.instructions) << endl;
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 2, 3}, move(bytecode.constants));
}

TEST(CompilerTest, LetTest) {
    string input = "let one = 4; let two = 5; let two = one; one;";
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
        constructByteCode(OpSetGlobal, vector<int>{0}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpSetGlobal, vector<int>{1}),
        constructByteCode(OpGetGlobal, vector<int>{0}),
        constructByteCode(OpSetGlobal, vector<int>{1}),
        constructByteCode(OpGetGlobal, vector<int>{0}),
        constructByteCode(OpPop, vector<int>{}) 
    };
    // cout << serialize(bytecode.instructions) << endl;
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{4, 5}, move(bytecode.constants));
}

TEST(CompilerTest, StringTest) {
    string input = "\"hello\";";
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
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    // testConstants(vector<string>{"hello"}, move(bytecode.constants));
}
TEST(CompilerTest, StringConcatTest) {
    string input = "\"hello\" + \"world\";";
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
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    // testConstants(vector<string>{"hello"}, move(bytecode.constants));
}
TEST(CompilerTest, ArraySimpleTest) {
    string input = "[1, 2]";
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
        constructByteCode(OpArray, vector<int>{2}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 2}, move(bytecode.constants));
}
TEST(CompilerTest, ArrayExpressionTest) {
    string input = "[1, 2 * 5, 4 - 3]";
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
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpMul, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{3}),
        constructByteCode(OpConstant, vector<int>{4}),
        constructByteCode(OpSub, vector<int>{}),
        constructByteCode(OpArray, vector<int>{3}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 2, 5, 4, 3}, move(bytecode.constants));
}
TEST(CompilerTest, ArrayIndexTest) {
    string input = "[1, 2 * 5, 4 - 3][7 + 8]";
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
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpMul, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{3}),
        constructByteCode(OpConstant, vector<int>{4}),
        constructByteCode(OpSub, vector<int>{}),
        constructByteCode(OpArray, vector<int>{3}),
        constructByteCode(OpConstant, vector<int>{5}),
        constructByteCode(OpConstant, vector<int>{6}),
        constructByteCode(OpAdd, vector<int>{}),
        constructByteCode(OpIndex, vector<int>{}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 2, 5, 4, 3, 7, 8}, move(bytecode.constants));
}

TEST(CompilerTest, HashTest) {
    string input = "{1: 3, 2: 4 * 5, 3: 4 - 6}";
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
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpConstant, vector<int>{3}),
        constructByteCode(OpConstant, vector<int>{4}),
        constructByteCode(OpMul, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{5}),
        constructByteCode(OpConstant, vector<int>{6}),
        constructByteCode(OpConstant, vector<int>{7}),
        constructByteCode(OpSub, vector<int>{}),
        constructByteCode(OpHash, vector<int>{6}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    testConstants(vector<int>{1, 3, 2, 4, 5, 3, 4, 6}, move(bytecode.constants));
}

TEST(CompilerTest, FnTest) {
    string input = "fn(){return 2 + 3;}";
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
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    vector<Instruction> expectedConstants {
        constructByteCode(OpConstant, vector<int>{0}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpAdd, vector<int>{}),
        constructByteCode(OpRetVal, vector<int>{}),
    };
    vector<int> expectedInt = {2, 3};
    for (int i = 0; i < expectedInt.size(); i++) {
        Integer* lit = dynamic_cast<Integer*>(bytecode.constants.at(i).get());
        ASSERT_EQ(expectedInt.at(i), lit->value);
    }
    // third constant is a function turned into Instruction
    CompiledFunction* instruct = dynamic_cast<CompiledFunction*>(bytecode.constants.at(2).get());
    testInstructions(concatInstructions(expectedConstants), instruct->instructions);
}

TEST(CompilerTest, FnImplicitTest) {
    string input = "fn(){2 + 3;}";
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
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    vector<Instruction> expectedConstants {
        constructByteCode(OpConstant, vector<int>{0}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpAdd, vector<int>{}),
        constructByteCode(OpRetVal, vector<int>{}),
    };
    vector<int> expectedInt = {2, 3};
    for (int i = 0; i < expectedInt.size(); i++) {
        Integer* lit = dynamic_cast<Integer*>(bytecode.constants.at(i).get());
        ASSERT_EQ(expectedInt.at(i), lit->value);
    }
    // third constant is a function turned into Instruction
    CompiledFunction* instruct = dynamic_cast<CompiledFunction*>(bytecode.constants.at(2).get());
    testInstructions(concatInstructions(expectedConstants), instruct->instructions);
}

TEST(CompilerTest, FnTwoImplicitTest) {
    string input = "fn(){2; 3;}";
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
        constructByteCode(OpConstant, vector<int>{2}),
        constructByteCode(OpPop, vector<int>{}),
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    vector<Instruction> expectedConstants {
        constructByteCode(OpConstant, vector<int>{0}),
        constructByteCode(OpPop, vector<int>{}),
        constructByteCode(OpConstant, vector<int>{1}),
        constructByteCode(OpRetVal, vector<int>{}),
    };
    vector<int> expectedInt = {2, 3};
    for (int i = 0; i < expectedInt.size(); i++) {
        Integer* lit = dynamic_cast<Integer*>(bytecode.constants.at(i).get());
        ASSERT_EQ(expectedInt.at(i), lit->value);
    }
    // third constant is a function turned into Instruction
    CompiledFunction* instruct = dynamic_cast<CompiledFunction*>(bytecode.constants.at(2).get());
    testInstructions(concatInstructions(expectedConstants), instruct->instructions);
}

TEST(CompilerTest, FnEmptyTest) {
    string input = "fn(){ }";
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
    };
    testInstructions(concatInstructions(expected), bytecode.instructions);
    vector<Instruction> expectedConstants {
        constructByteCode(OpRet, vector<int>{}),
    };
    CompiledFunction* instruct = dynamic_cast<CompiledFunction*>(bytecode.constants.at(0).get());
    testInstructions(concatInstructions(expectedConstants), instruct->instructions);
}
TEST(CompilerTest, CompilerScopeTest) {
    auto compiler = Compiler();
    if (compiler.scopeIndex != 0) FAIL() << "wrong scope, should be 0, but got "<< compiler.scopeIndex << endl;

    compiler.emit(OpAdd, vector<int>{});
    compiler.enterScope();
    if (compiler.scopeIndex != 1) FAIL() << "wrong scope, should be 1, but got "<< compiler.scopeIndex << endl;

    compiler.emit(OpSub, vector<int>{});
    int size = compiler.scopes.at(compiler.scopeIndex).get()->instructions.size();
    if (size != 1)
        FAIL() << "wrong instruction length, expected 1 but got " << size << endl;
    auto last = compiler.scopes.at(compiler.scopeIndex).get()->last;
    if (last.opcode != OpSub)
        FAIL() << "wrong opcode, should be OpSub"<< endl;
    
    compiler.leaveScope();
    if (compiler.scopeIndex != 0) FAIL() << "wrong scope, should be 0, but got "<< compiler.scopeIndex << endl;
    
    compiler.emit(OpDiv, vector<int>{});
    size = compiler.scopes.at(compiler.scopeIndex).get()->instructions.size();
    if (size != 2)
        FAIL() << "wrong instruction length, expected 2 but got " << size << endl;

    last = compiler.scopes.at(compiler.scopeIndex).get()->last;
    if (last.opcode != OpDiv)
        FAIL() << "wrong opcode, should be OpDiv, but got " << endl;
    
    auto prevLast = compiler.scopes.at(compiler.scopeIndex).get()->prevLast;
    if (prevLast.opcode != OpAdd)
        FAIL() << "wrong opcode, should be OpAdd, but got " << endl;
    
};
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}