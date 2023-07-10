#include<iostream>
#include"lexer.cpp"
#include<iostream>
#include<memory>
#include<vector>

using namespace std;

class Node {
    public:
    virtual string serialize() const = 0;
};

// turn all fields into pointers

using namespace std;



/************************* Expressions *********************/
class Expression : public Node {
    public:
    Expression();
    virtual ~Expression();
    virtual string serialize() const;
};
class Identifier : public Expression {
    public:
    Token token;
    string value;
    string serialize() const final override;
};
/************************* Statements ************************/
class Statement : public Node {
    public:
    Statement();
    virtual ~Statement();
    virtual string serialize() const;
    
};
class LetStatement: public Statement {
    public:
    Token token;
    Identifier identifier;
    unique_ptr<Expression> value;
    LetStatement();
    LetStatement(Token tok, Identifier ident, unique_ptr<Expression>& val);
    
    string serialize() const final override;
};
class ReturnStatement: public Statement {
    public:
    Token token;
    unique_ptr<Expression> value;
    ReturnStatement();
    ReturnStatement(Token tok, unique_ptr<Expression>& val);
    string serialize() const final override;
};

/* To allow for a single line expression like "x + 5;"*/
class ExpressionStatement: public Statement {
    public:
    Token token;
    unique_ptr<Expression> expression;
    ExpressionStatement();
    ExpressionStatement(Token tok, unique_ptr<Expression>& express);

    string serialize() const final override;
};
class BlockStatement : public Statement {
    public:
    Token token;
    vector<unique_ptr<Statement>> statements;
    BlockStatement(Token tok, vector<unique_ptr<Statement>>&& stmts);
    string serialize();
};
// Expressions
class IntLiteral : public Expression {
    public:
    Token token;
    int value;
    IntLiteral(Token tok, int val);
    string serialize() const final override;
};
class BoolLiteral : public Expression {
    public:
    Token token;
    bool value;
    BoolLiteral(Token tok, bool val);
    string serialize() const final override;
};
class FnLiteral : public Expression {
    public:
    Token token;
    vector<unique_ptr<Expression>> params;
    unique_ptr<BlockStatement> body;
    FnLiteral(Token tok, vector<unique_ptr<Expression>>&& params, unique_ptr<BlockStatement>& body);
    string serialize() const override;

};
class PrefixExpression : public Expression {
    public:
    Token token;
    string Operator;
    unique_ptr<Expression> right;
    PrefixExpression();
    PrefixExpression(Token tok, string Operator, unique_ptr<Expression>& right);
    string serialize() const override;
};
class InfixExpression : public Expression {
    public:
    Token token;
    string Operator;
    unique_ptr<Expression> left;
    unique_ptr<Expression> right;
    InfixExpression();
    InfixExpression(Token tok, string Operator, unique_ptr<Expression>& left, unique_ptr<Expression>& right);
    string serialize() const override;
};
class IfExpression : public Expression {
    public:
    Token token;
    unique_ptr<Expression> condition;
    unique_ptr<BlockStatement> consequence;
    unique_ptr<BlockStatement> alternative;

    IfExpression(Token tok, 
    unique_ptr<Expression>& cond, 
    unique_ptr<BlockStatement>& cons, 
    unique_ptr<BlockStatement>& alt);
    
    string serialize() const override;
};
class CallExpression : public Expression {
    public:
    Token token;
    unique_ptr<Expression> function;
    vector<unique_ptr<Expression>> args;

    CallExpression(Token tok, unique_ptr<Expression>& function, vector<unique_ptr<Expression>>&& args);
    string serialize() const override;
};

/*********************** Program (root node) ********************/ 
class Program : public Node {
    public:
    vector<unique_ptr<Statement>> statements;
    string serialize() const final override;
};