#include"lexer.cpp"
#include<iostream>
#include<memory>
#include<vector>

using namespace std;

class Node {
    public:
    virtual string getLiteral() = 0;
};

/* Expressions */
class Expression : public Node {
    public:
    Expression() = default;
    virtual ~Expression() = default;
    virtual string getLiteral() {return"";};
};
class Identifier : public Expression {
    public:
    Token token;
    string value;
    string getLiteral() override {
        return token.literal;
    }
};

/* Statements */
class Statement : public Node {
    public:
    Statement() = default;
    virtual ~Statement() = default;
    virtual string getLiteral(){return"";};
    
};
class LetStatement: public Statement {
    public:
    Token token;
    Identifier identifier;
    Expression value;
    LetStatement() = default;
    string getLiteral() override {
        return token.literal;
    }
};
class ReturnStatement: public Statement {
    public:
    Token token;
    Expression value;

    ReturnStatement() = default;

    string getLiteral() override {
        return token.literal;
    }
};

/* Program (root node)*/ 
class Program : public Node {
    public:
    vector<unique_ptr<Statement>> statements;

    string getLiteral() {
        if (statements.size() > 0) {
            return statements[0]->getLiteral();
        } else {
            return "";
        }
    };
};
