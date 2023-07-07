#include<iostream>
#include<gtest/gtest.h>
#include"parser.cpp"
#include<map>

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
    int values[2] = {5, 10};
    cout << "serialze" << program.serialize() << endl;
    for (int i = 0; i < program.statements.size(); i++) {
        Statement* temp = program.statements.at(i).get();
        LetStatement* statement = dynamic_cast<LetStatement*>(temp);
        ASSERT_EQ(statement->token.type, types.LET);
        ASSERT_EQ(statement->identifier.value, identifiers[i]);
        IntLiteral* intLit = dynamic_cast<IntLiteral*>(statement->value.get());
        ASSERT_EQ(intLit->value, values[i]);
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
        ReturnStatement* statement = dynamic_cast<ReturnStatement*>(temp);
        ASSERT_EQ(statement->token.type, types.RETURN);
        ASSERT_EQ(statement->token.literal, "return");
    }
}

TEST(ParserTest, SerializeTest) {
    string input = "let a = b;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    ASSERT_EQ(input, program.serialize());
}

TEST(ParserTest, LetInfixExpressionTest) {
    string input = "let a = 2 + 3 * 5;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    ASSERT_EQ("let a = (2 + (3 * 5));", program.serialize());
}

TEST(ParserTest, ReturnInfixExpressionTest) {
    string input = "return 2 - 3 / 5;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    ASSERT_EQ("return (2 - (3 / 5));", program.serialize());
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
    Identifier* ident = dynamic_cast<Identifier*>(stmt->expression.get());
    ASSERT_EQ(stmt->token.literal, "foo");
    ASSERT_EQ(ident->value, "foo");
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
    IntLiteral* intLit = dynamic_cast<IntLiteral*>(stmt->expression.get());
    ASSERT_EQ(intLit->value, 5);
}

TEST(ParserTest, BoolLiteralTest) {
    string input = "false;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    Statement* temp = program.statements.at(0).get();
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(temp);
    ASSERT_EQ(stmt->token.literal, "false");
    BoolLiteral* boolLit = dynamic_cast<BoolLiteral*>(stmt->expression.get());
    ASSERT_EQ(boolLit->value, false);
}

TEST(ParserTest, PrefixOperatorTest) {
    string input = "!5; -10;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    ExpressionStatement* stmt1 = dynamic_cast<ExpressionStatement*>(program.statements.at(0).get());
    ExpressionStatement* stmt2 = dynamic_cast<ExpressionStatement*>(program.statements.at(1).get());
    PrefixExpression* exp1 = dynamic_cast<PrefixExpression*>(stmt1->expression.get());
    PrefixExpression* exp2 = dynamic_cast<PrefixExpression*>(stmt2->expression.get());
    ASSERT_EQ(exp1->Operator, "!");
    ASSERT_EQ(exp2->Operator, "-");
    IntLiteral* intLit1 = dynamic_cast<IntLiteral*>(exp1->right.get());
    IntLiteral* intLit2 = dynamic_cast<IntLiteral*>(exp2->right.get());
    ASSERT_EQ(intLit1->value, 5);
    ASSERT_EQ(intLit2->value, 10);
}

TEST(ParserTest, InfixOperatorSimpleTest) {
    string input = "a + b; 1 * 2;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    ExpressionStatement* stmt1 = dynamic_cast<ExpressionStatement*>(program.statements.at(0).get());
    ExpressionStatement* stmt2 = dynamic_cast<ExpressionStatement*>(program.statements.at(1).get());
    InfixExpression* exp1 = dynamic_cast<InfixExpression*>(stmt1->expression.get());
    InfixExpression* exp2 = dynamic_cast<InfixExpression*>(stmt2->expression.get());
    ASSERT_EQ(exp1->Operator, "+");
    ASSERT_EQ(exp2->Operator, "*");
    Identifier* ident1 = dynamic_cast<Identifier*>(exp1->left.get());
    Identifier* ident2 = dynamic_cast<Identifier*>(exp1->right.get());
    IntLiteral* intLit1 = dynamic_cast<IntLiteral*>(exp2->left.get());
    IntLiteral* intLit2 = dynamic_cast<IntLiteral*>(exp2->right.get());
    ASSERT_EQ(ident1->value, "a");
    ASSERT_EQ(ident2->value, "b");
    ASSERT_EQ(intLit1->value, 1);
    ASSERT_EQ(intLit2->value, 2);
}

TEST(ParserTest, InfixSerializeTest) {
    string input = "a + b * c";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    ASSERT_EQ("(a + (b * c))", program.serialize());
}

TEST(ParserTest, InfixLongerSerializeTest) {
    string input = "a + b * c - d / c";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
    ASSERT_EQ("(a + ((b * c) - (d / c)))", program.serialize());
}

TEST(ParserTest, GroupedPrecedenceTest) {
    map<string, string> tests = {
        {"(1 + 2) + 3", "((1 + 2) + 3)"}
    };
    for (auto test : tests) {
        Lexer l = Lexer(test.first);
        Parser p = Parser(l);
        auto program = Program();
        int error = p.parseProgram(&program);
        if (error) FAIL() << "test failed due to error in parser..." << endl;
        ASSERT_EQ(program.serialize(), test.second);
    }
}

TEST(ParserTest, LongGroupedPrecedenceTest) {
    map<string, string> tests = {
        {"(--1 + 2) * 3", "((--1 + 2) * 3)"}
    };
    for (auto test : tests) {
        Lexer l = Lexer(test.first);
        Parser p = Parser(l);
        auto program = Program();
        int error = p.parseProgram(&program);
        if (error) FAIL() << "test failed due to error in parser..." << endl;
        ASSERT_EQ(program.serialize(), test.second);
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}