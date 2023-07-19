#include "../lexer.cpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(TokenTest, Test0) {
    const string input = "=+(){},;";
    auto l = Lexer(input);

    Token tests[] = {
        {types.ASSIGN, "="},
        {types.PLUS, "+"},
        {types.LPAREN, "("},
        {types.RPAREN, ")"},
        {types.LBRACE, "{"},
        {types.RBRACE, "}"},
        {types.COMMA, ","},
        {types.SEMICOLON, ";"},
        {types.EoF, ""}
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, Test1) {
    const string input = 
    "let five = 5;"
    "let ten = 10;"
    "let add = fn(x, y) {"
        "x + y;"
    "};"
    "let result = add(five, ten);";

    auto l = Lexer(input);

    Token tests[] = {
        {types.LET, "let"},
        {types.IDENT, "five"},
        {types.ASSIGN, "="},
        {types.INT, "5"},
        {types.SEMICOLON, ";"},
        {types.LET, "let"},
        {types.IDENT, "ten"},
        {types.ASSIGN, "="},
        {types.INT, "10"},
        {types.SEMICOLON, ";"},
        {types.LET, "let"},
        {types.IDENT, "add"},
        {types.ASSIGN, "="},
        {types.FUNCTION, "fn"},
        {types.LPAREN, "("},
        {types.IDENT, "x"},
        {types.COMMA, ","},
        {types.IDENT, "y"},
        {types.RPAREN, ")"},
        {types.LBRACE, "{"},
        {types.IDENT, "x"},
        {types.PLUS, "+"},
        {types.IDENT, "y"},
        {types.SEMICOLON, ";"},
        {types.RBRACE, "}"},
        {types.SEMICOLON, ";"},
        {types.LET, "let"},
        {types.IDENT, "result"},
        {types.ASSIGN, "="},
        {types.IDENT, "add"},
        {types.LPAREN, "("},
        {types.IDENT, "five"},
        {types.COMMA, ","},
        {types.IDENT, "ten"},
        {types.RPAREN, ")"},
        {types.SEMICOLON, ";"},
        {types.EoF, ""}
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, Test2) {
    const string input = 
    "let five = 5;"
    "let ten = 10;"
    "let add = fn(x, y) {"
        "x + y;"
    "};"
    "let result = add(five, ten);"
    "!-/*5;"
    "5 < 10 > 5;";

    auto l = Lexer(input);

    Token tests[] = {
        {types.LET, "let"},
        {types.IDENT, "five"},
        {types.ASSIGN, "="},
        {types.INT, "5"},
        {types.SEMICOLON, ";"},
        {types.LET, "let"},
        {types.IDENT, "ten"},
        {types.ASSIGN, "="},
        {types.INT, "10"},
        {types.SEMICOLON, ";"},
        {types.LET, "let"},
        {types.IDENT, "add"},
        {types.ASSIGN, "="},
        {types.FUNCTION, "fn"},
        {types.LPAREN, "("},
        {types.IDENT, "x"},
        {types.COMMA, ","},
        {types.IDENT, "y"},
        {types.RPAREN, ")"},
        {types.LBRACE, "{"},
        {types.IDENT, "x"},
        {types.PLUS, "+"},
        {types.IDENT, "y"},
        {types.SEMICOLON, ";"},
        {types.RBRACE, "}"},
        {types.SEMICOLON, ";"},
        {types.LET, "let"},
        {types.IDENT, "result"},
        {types.ASSIGN, "="},
        {types.IDENT, "add"},
        {types.LPAREN, "("},
        {types.IDENT, "five"},
        {types.COMMA, ","},
        {types.IDENT, "ten"},
        {types.RPAREN, ")"},
        {types.SEMICOLON, ";"},
        {types.SURPRISE, "!"},
        {types.MINUS, "-"},
        {types.SLASH, "/"},
        {types.ASTERISK, "*"},
        {types.INT, "5"},
        {types.SEMICOLON, ";"},
        {types.INT, "5"},
        {types.LESS, "<"},
        {types.INT, "10"},
        {types.GREATER, ">"},
        {types.INT, "5"},
        {types.SEMICOLON, ";"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, TrueFalseIfElseReturnTest) {
    const string input = 
    "let a = true;"
    "let b = false;"
    "if else return;";

    auto l = Lexer(input);

    Token tests[] = {
        {types.LET, "let"},
        {types.IDENT, "a"},
        {types.ASSIGN, "="},
        {types.TRUE, "true"},
        {types.SEMICOLON, ";"},
        {types.LET, "let"},
        {types.IDENT, "b"},
        {types.ASSIGN, "="},
        {types.FALSE, "false"},
        {types.SEMICOLON, ";"},
        {types.IF, "if"},
        {types.ELSE, "else"},
        {types.RETURN, "return"},
        {types.SEMICOLON, ";"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, EqualNotEqualTest) {
    const string input = 
    "a == b;"
    "b != c;";

    auto l = Lexer(input);

    Token tests[] = {
        {types.IDENT, "a"},
        {types.EQ, "=="},
        {types.IDENT, "b"},
        {types.SEMICOLON, ";"},
        {types.IDENT, "b"},
        {types.NOT_EQ, "!="},
        {types.IDENT, "c"},
        {types.SEMICOLON, ";"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, EqualTest) {
    const string input = 
    "what = b;";

    auto l = Lexer(input);

    Token tests[] = {
        {types.IDENT, "what"},
        {types.ASSIGN, "="},
        {types.IDENT, "b"},
        {types.SEMICOLON, ";"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, StringTest) {
    const string input = 
    "\"he llo\"; \"world\"";

    auto l = Lexer(input);

    Token tests[] = {
        {types.STRING, "he llo"},
        {types.SEMICOLON, ";"},
        {types.STRING, "world"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, ArrayTest) {
    const string input = 
    "[1, 2, a]";

    auto l = Lexer(input);

    Token tests[] = {
        {types.LBRACKET, "["},
        {types.INT, "1"},
        {types.COMMA, ","},
        {types.INT, "2"},
        {types.COMMA, ","},
        {types.IDENT, "a"},
        {types.RBRACKET, "]"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, HashTest) {
    const string input = 
    "{a : 1}";

    auto l = Lexer(input);

    Token tests[] = {
        {types.LBRACE, "{"},
        {types.IDENT, "a"},
        {types.COLON, ":"},
        {types.INT, "1"},
        {types.RBRACE, "}"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

TEST(TokenTest, HashIndexTest) {
    const string input = 
    "{a : 1}[2]";

    auto l = Lexer(input);

    Token tests[] = {
        {types.LBRACE, "{"},
        {types.IDENT, "a"},
        {types.COLON, ":"},
        {types.INT, "1"},
        {types.RBRACE, "}"},
        {types.LBRACKET, "["},
        {types.INT, "2"},
        {types.RBRACKET, "]"},
        {types.EoF, ""},
    };

    for (Token test : tests) {
        Token tok = l.nextToken();
        ASSERT_EQ(test.type, tok.type);
        ASSERT_EQ(test.literal, tok.literal);
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}