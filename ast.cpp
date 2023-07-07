#include"lexer.cpp"
#include<iostream>
#include<memory>
#include<vector>

// turn all fields into pointers

using namespace std;

class Node {
    public:
    virtual string serialize() const = 0;
};

/************************* Expressions *********************/
class Expression : public Node {
    public:
    Expression() = default;
    virtual ~Expression() = default;
    virtual string serialize() const {return "";};
};
class Identifier : public Expression {
    public:
    Token token;
    string value;
    string serialize() const final override {
        return value;
    }
};
class IntLiteral : public Expression {
    public:
    Token token;
    int value;

    IntLiteral(Token tok, int val) : token(tok), value(val) {};

    string serialize() const final override {
        return token.literal;
    }
};
class BoolLiteral : public Expression {
    public:
    Token token;
    bool value;

    BoolLiteral(Token tok, bool val) : token(tok), value(val){};

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

    string serialize() const override {
        return Operator + right.get()->serialize();
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

    string serialize() const override {
        // cout << "parsing infix expression" << endl;
        // cout << "left expression is" << left.get()->serialize() << endl;
        // cout << "right expression is" << right.get()->serialize() << endl;
        string res =  "(" + left.get()->serialize() + " " + Operator + " " + right.get()->serialize() + ")";
        // cout << res << endl;
        return res;
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
    unique_ptr<Expression> value;

    LetStatement() = default;
    LetStatement(Token tok, Identifier ident, unique_ptr<Expression>& val) : token(tok), identifier(ident), value(move(val)) {};
    
    string serialize() const final override {
        return token.literal 
                + " " 
                + identifier.serialize() 
                + " = " 
                + value.get()->serialize() + ";";
        
    };
};
class ReturnStatement: public Statement {
    public:
    Token token;
    unique_ptr<Expression> value;

    ReturnStatement() = default;
    ReturnStatement(Token tok, unique_ptr<Expression>& val) : token(tok), value(move(val)) {}
    
    string serialize() const final override {
        return token.literal + " " + value.get()->serialize() + ";";
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

    string serialize() const final override {
        cout << "parsing expression statement" << endl;
        return expression.get()->serialize();
        // return "";
    }
};

/*********************** Program (root node) ********************/ 
class Program : public Node {
    public:
    vector<unique_ptr<Statement>> statements;

    string serialize() const final override {
        string res = "";
        for (int i = 0; i < statements.size(); i++) {
            Statement* stmt = statements.at(i).get();
            res += stmt->serialize();
        }
        return res;
    }
};
