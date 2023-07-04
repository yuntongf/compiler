#include "token.cpp"
#include <iostream>
#include <string.h>
using namespace std;

/* Allow lower and upper case letters, underscore */
bool isLetter(char c) {
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_';
}

bool isDigit(char c) {
    return '0' <= c && c <= '9';
}

class Lexer {
    string input;
    int currPos = 0; // position of current character
    int nextPos = 0; // after current char
    char currChar; // current character

    public:
    Lexer() = default;

    Lexer(const string input) {
        this->input = input;
        readChar();
    }

    /* Read the next character in input, if reach the 
    end, set current character to NUL */
    void readChar() {
        if (nextPos >= input.length()) {
            currChar = 0;
        } else {
            currChar = input[nextPos];
            currPos = nextPos;
            nextPos++;
        }
    };

    char peekChar() {
        if (nextPos >= input.length()) {
            return 0;
        } else {
            return input[nextPos];
        }
    }

    string readIdentifier() {
        int startPos = currPos;
        while (isLetter(currChar)) {
            readChar();
        }
        return input.substr(startPos, currPos - startPos);
    }

    string readNumber() {
        int startPos = currPos;
        while (isDigit(currChar)) {
            readChar();
        }
        return input.substr(startPos, currPos - startPos);
    }

    void skipWhitespace() {
        while (currChar == ' ' || currChar == '\t' || currChar == '\n' || currChar == 
        '\r') {
            readChar();
        }
    }

    /* Get Next Token */
    Token nextToken() {
        Token token;
        skipWhitespace();
        switch (currChar) {
            case '=':
                if (peekChar() == '=') {
                    token.type = types.EQ;
                    token.literal = "==";
                    readChar();
                } else {
                    token = NewToken(types.ASSIGN, currChar);
                }
                break;
            case ';':
                token = NewToken(types.SEMICOLON, currChar);
                break;
            case '(':
                token = NewToken(types.LPAREN, currChar);
                break;
            case ')':
                token = NewToken(types.RPAREN, currChar);
                break;
            case ',':
                token = NewToken(types.COMMA, currChar);
                break;
            case '+':
                token = NewToken(types.PLUS, currChar);
                break;
            case '-':
                token = NewToken(types.MINUS, currChar);
                break;
            case '/':
                token = NewToken(types.SLASH, currChar);
                break;
            case '*':
                token = NewToken(types.ASTERISK, currChar);
                break;
            case '!':
                if (peekChar() == '=') {
                    token.type = types.NOT_EQ;
                    token.literal = "!=";
                    readChar();
                } else {
                    token = NewToken(types.SURPRISE, currChar);
                }
                break;
            case '<':
                token = NewToken(types.LESS, currChar);
                break;
            case '>':
                token = NewToken(types.GREATER, currChar);
                break;
            case '{':
                token = NewToken(types.LBRACE, currChar);
                break;
            case '}':
                token = NewToken(types.RBRACE, currChar);
                break;
            case '\0':
                token = NewToken(types.EoF, currChar);
                break;
            default:
                if (isLetter(currChar)) {
                    token.literal = readIdentifier();
                    token.type = getType(token.literal);
                    return token; // already advanced to next char, so exit early
                } else if (isDigit(currChar)) {
                    token.type = types.INT;
                    token.literal = readNumber();
                    return token;
                } else {
                    token = NewToken(types.ILLEGAL, currChar);
                }
        }
        readChar();
        return token;
    }
};

