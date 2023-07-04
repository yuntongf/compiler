#include<iostream>
#include<map>

using namespace std;

struct TokenType {
    const string ILLEGAL = "ILLEGAL";
    const string EoF = "EOF";

    // Identifiers + literals
    const string IDENT = "IDENTIFIER";
    const string INT = "INT";

    // Operators
    const string ASSIGN = "ASSIGN";
    const string PLUS = "PLUS";
    const string MINUS = "MINUS";
    const string SURPRISE = "SURPRISE";
    const string ASTERISK = "ASTERISK";
    const string SLASH = "SLASH";
    const string EQ = "EQ";
    const string NOT_EQ = "NOT_EQ";

    const string LESS = "LESS_THAN";
    const string GREATER = "GREATER_THAN";

    // Delimiters
    const string COMMA = "COMMA";
    const string SEMICOLON = "SEMICOLON";

    const string LPAREN = "LEFT_PARENTHESIS";
    const string RPAREN = "RIGHT_PARENTHESIS";
    const string LBRACE = "LEFT_BRACE";
    const string RBRACE = "RIGHT_BRACE";

    // Keywords
    const string FUNCTION = "FUNCTION";
    const string LET = "LET";
    const string TRUE = "TRUE";
    const string FALSE = "FALSE";
    const string IF = "IF";
    const string ELSE = "ELSE";
    const string RETURN = "RETURN";

} types;

struct Token {
    string type;
    string literal;
};

Token NewToken(string type, char c) {
    string literal = {c};
    if (c == 0) literal = "";
    Token tok = {type: type, literal: literal};
    return tok;
};

map<string, string> literalToType = {
    {"let", types.LET},
    {"fn", types.FUNCTION},
    {"true", types.TRUE},
    {"false", types.FALSE},
    {"if", types.IF},
    {"else", types.ELSE},
    {"return", types.RETURN}
};
string getType(string literal) {
    if (literalToType.count(literal)) {
        return literalToType.find(literal)->second;
    } else {
        return types.IDENT;
    }
}