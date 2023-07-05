#include"lexer.cpp"
#include<iostream>
#include<memory>
#include<vector>

// turn all fields into pointers

using namespace std;

class Node {
    public:
    virtual string getLiteral() = 0;
    virtual string serialize() const = 0;
};

/************************* Expressions *********************/
class Expression : public Node {
    public:
    Expression() = default;
    virtual ~Expression() = default;
    virtual string getLiteral() {return"";};
    virtual string serialize() const {return "aha";};
};
class Identifier : public Expression {
    public:
    Token token;
    string value;
    string getLiteral() override {
        return token.literal;
    };
    string serialize() const final override {
        return value;
    }
};
class IntLiteral : public Expression {
    public:
    Token token;
    int value;

    IntLiteral(Token tok, int val) : token(tok), value(val) {};

    string getLiteral() override {
        return token.literal;
    }
    string serialize() const final override {
        return token.literal;
    }
};
class PrefixExpression : public Expression {
    public:
    Token token;
    string Operator;
    unique_ptr<Expression> right;
    
    PrefixExpression() = default;
    PrefixExpression(Token tok, string Operator, unique_ptr<Expression>& right) : token(tok), Operator(Operator), right(move(right)) {};

    string getLiteral() final override {
        return token.literal;
    };
    string serialize() const override {
        return token.literal;
    }
};
class InfixExpression : public Expression {
    public:
    Token token;
    string Operator;
    unique_ptr<Expression> left;
    unique_ptr<Expression> right;

    InfixExpression() = default;
    InfixExpression(Token tok, string Operator, unique_ptr<Expression>& left, unique_ptr<Expression>& right) : token(tok), Operator(Operator), left(move(left)), right(move(right)) {};

    string getLiteral() final override {
        return token.literal;
    };
    string serialize() const override {
        return token.literal;
    };
};
/************************* Statements ************************/
class Statement : public Node {
    public:
    Statement() = default;
    virtual ~Statement() = default;
    virtual string getLiteral(){return"";};
    virtual string serialize() const {return "";};
    
};
class LetStatement: public Statement {
    public:
    Token token;
    Identifier identifier;
    Expression* value;
    LetStatement() = default;
    string getLiteral() override {
        return token.literal;
    }
    string serialize() const final override {
        return token.literal 
                + " " 
                + identifier.serialize() 
                + " = " 
                + value->serialize() + ";";
        
    };
};
class ReturnStatement: public Statement {
    public:
    Token token;
    Expression* value;

    ReturnStatement() = default;

    string getLiteral() override {
        return token.literal;
    }
    
    string serialize() const final override {
        return token.literal + " " + value->serialize() + ";";
    }
};
/* To allow for a single line expression like "x + 5;"*/
class ExpressionStatement: public Statement {
    public:
    Token token;
    unique_ptr<Expression> expression;

    ExpressionStatement() {
        expression = nullptr;
    };

    ExpressionStatement(Token tok, unique_ptr<Expression>& express): token(tok), expression(move(express)) {};

    string getLiteral() {
        return token.literal;
    };

    string serialize() const final override {
        return "";
    }
};

/*********************** Program (root node) ********************/ 
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

    string serialize() const final override {
        string res = "";
        for (int i = 0; i < statements.size(); i++) {
            Statement* stmt = statements.at(i).get();
            res += stmt->serialize();
        }
        return res;
    }
};
