#include<iostream>
#include<gtest/gtest.h>
#include"parser.cpp"

TEST(ParserTest, LetStatementTest) {
    string input = 
    "let x = 5;"
    "let y = 10;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    
    Program program = Program();
    int error = p.parseProgram(&program);
    if (error) {
        FAIL() << "test failed due to errors in parseProgram..." << endl;
    }
    if (&program == nullptr) {
        cout << "program pointer is null";
    }
    if (program.statements.size() != 2) {
        cout << "does not contain two statements; " 
            << "instead contains" << program.statements.size() << endl;
    }
    string identifiers[2] = {"x", "y"};
    string values[2] = {"5", "10"};
    for (int i = 0; i < program.statements.size(); i++) {
        Statement* temp = program.statements.at(i).get();
        LetStatement statement = dynamic_cast<LetStatement&>(*temp);
        ASSERT_EQ(statement.token.type, types.LET);
        ASSERT_EQ(statement.identifier.value, identifiers[i]);
        // ASSERT_EQ(statement.value.getLiteral(), values[i]);
    }
}

TEST(ParserTest, ErrorTest) {
    string input = 
    "let = 4;"
    "let x 5;";

    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (!error) FAIL() << "test failed due to no errors..." << endl;
    ASSERT_EQ(p.getErrors().size(), 2);
}

TEST(ParserTest, ReturnTest) {
    string input = 
    "return 509;"
    "return a == b;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    for (int i = 0; i < program.statements.size(); i++) {
        Statement* temp = program.statements.at(i).get();
        ReturnStatement statement = dynamic_cast<ReturnStatement&>(*temp);
        ASSERT_EQ(statement.token.type, types.RETURN);
        ASSERT_EQ(statement.token.literal, "return");
    }
}

TEST(ParserTest, SerializeTest) {
    string input = "let a = b;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    cout << program.serialize();
}

TEST(ParserTest, SingleIdentifierTest) {
    string input = "foo;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    Statement* temp = program.statements.at(0).get();
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(temp);
    ASSERT_EQ(stmt->token.literal, "foo");
}

TEST(ParserTest, IntLiteralTest) {
    string input = "5;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    Statement* temp = program.statements.at(0).get();
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(temp);
    ASSERT_EQ(stmt->token.literal, "5");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}