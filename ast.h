#include<iostream>
#include"lexer.cpp"
#include<iostream>
#include<memory>
#include<vector>

using namespace std;

class Node {
    public:
    string type;
    virtual string serialize() const = 0;
    virtual string getType() const = 0;
};

// turn all fields into pointers

using namespace std;

struct NodeType {
    const string Program = "PROGRAM";
    const string Expression = "EXPRESSION";
    const string Statement = "STATEMENT";
    const string LetStatement = "LET_STATEMENT";
    const string ReturnStatement = "RETURN_STATEMENT";
    const string BlockStatement = "BLOCK_STATEMENT";
    const string IntLiteral = "INT_LITERAL";
    const string BoolLiteral = "BOOL_LITERAL";
    const string FnLiteral = "FN_LITERAL";
    const string CallExpression = "CALL_EXPRESSION";
    const string Identifier = "IDENTIFIER";
    const string ExpressionStatement = "EXPRESSION_STATEMENT";
    const string IfExpression = "IF_EXPRESSION";
    const string PrefixExpression = "PREFIX_EXPRESSION";
    const string InfixExpression = "INFIX_EXPRESSION";  
} ntypes;


/************************* Expressions *********************/
class Expression : public Node {
    public:
    string type = ntypes.Expression;
    Expression();
    virtual ~Expression();
    virtual string serialize() const;
    virtual string getType() const;
};
class Identifier : public Expression {
    public:
    Token token;
    string type = ntypes.Identifier;
    string value;
    string serialize() const final override;
    string getType() const final override;
};
/************************* Statements ************************/
class Statement : public Node {
    public:
    string type = ntypes.Statement;
    Statement();
    virtual ~Statement();
    virtual string serialize() const;
    virtual string getType() const;
};
class LetStatement: public Statement {
    public:
    Token token;
    string type = ntypes.LetStatement;
    Identifier identifier;
    unique_ptr<Expression> value;
    LetStatement();
    LetStatement(Token tok, Identifier ident, unique_ptr<Expression>& val);
    
    string serialize() const final override;
    string getType() const final override;
};
class ReturnStatement: public Statement {
    public:
    Token token;
    string type = ntypes.ReturnStatement;
    unique_ptr<Expression> value;
    ReturnStatement();
    ReturnStatement(Token tok, unique_ptr<Expression>& val);
    string serialize() const final override;
    string getType() const final override;
};

/* To allow for a single line expression like "x + 5;"*/
class ExpressionStatement: public Statement {
    public:
    Token token;
    string type = ntypes.ExpressionStatement;
    unique_ptr<Expression> expression;
    ExpressionStatement();
    ExpressionStatement(Token tok, unique_ptr<Expression>& express);

    string serialize() const final override;
    string getType() const final override;
};
class BlockStatement : public Statement {
    public:
    Token token;
    string type = ntypes.BlockStatement;
    vector<unique_ptr<Statement>> statements;
    BlockStatement(Token tok, vector<unique_ptr<Statement>>&& stmts);
    string serialize();
    string getType() const final override;
};
// Expressions
class IntLiteral : public Expression {
    public:
    Token token;
    string type = ntypes.IntLiteral;
    int value;
    IntLiteral(Token tok, int val);
    string serialize() const final override;
    string getType() const final override;
};
class BoolLiteral : public Expression {
    public:
    Token token;
    string type = ntypes.BoolLiteral;
    bool value;
    BoolLiteral(Token tok, bool val);
    string serialize() const final override;
    string getType() const final override;
};
class FnLiteral : public Expression {
    public:
    Token token;
    string type = ntypes.FnLiteral;
    vector<unique_ptr<Expression>> params;
    unique_ptr<BlockStatement> body;
    FnLiteral(Token tok, vector<unique_ptr<Expression>>&& params, unique_ptr<BlockStatement>& body);
    string serialize() const override;
    string getType() const override;
};
class PrefixExpression : public Expression {
    public:
    Token token;
    string type = ntypes.PrefixExpression;
    string Operator;
    unique_ptr<Expression> right;
    PrefixExpression();
    PrefixExpression(Token tok, string Operator, unique_ptr<Expression>& right);
    string serialize() const override;
    string getType() const override;
};
class InfixExpression : public Expression {
    public:
    Token token;
    string Operator;
    string type = ntypes.InfixExpression;
    unique_ptr<Expression> left;
    unique_ptr<Expression> right;
    InfixExpression();
    InfixExpression(Token tok, string Operator, unique_ptr<Expression>& left, unique_ptr<Expression>& right);
    string serialize() const override;
    string getType() const override;
};
class IfExpression : public Expression {
    public:
    Token token;
    string type = ntypes.IfExpression;
    unique_ptr<Expression> condition;
    unique_ptr<BlockStatement> consequence;
    unique_ptr<BlockStatement> alternative;

    IfExpression(Token tok, 
    unique_ptr<Expression>& cond, 
    unique_ptr<BlockStatement>& cons, 
    unique_ptr<BlockStatement>& alt);
    
    string serialize() const override;
    string getType() const override;
};
class CallExpression : public Expression {
    public:
    Token token;
    string type = ntypes.CallExpression;
    unique_ptr<Expression> function;
    vector<unique_ptr<Expression>> args;

    CallExpression(Token tok, unique_ptr<Expression>& function, vector<unique_ptr<Expression>>&& args);
    string serialize() const override;
    string getType() const override;
};

/*********************** Program (root node) ********************/ 
class Program : public Node {
    public:
    string type = ntypes.Program;
    vector<unique_ptr<Statement>> statements;
    Program() = default;
    // Program(vector<unique_ptr<Statement>>& statements) : statements(statements) {};
    string serialize() const final override;
    string getType() const override;
};