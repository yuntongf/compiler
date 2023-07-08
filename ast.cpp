#include"ast.h"

// turn all fields into pointers

using namespace std;

/************************* Expressions *********************/
Expression::Expression() = default;
Expression::~Expression() = default;
string Expression::serialize() const {return "";}

string Identifier::serialize() const {return value;}
IntLiteral::IntLiteral(Token tok, int val) : token(tok), value(val) {};
string IntLiteral::serialize() const {return token.literal;};

BoolLiteral::BoolLiteral(Token tok, bool val) : token(tok), value(val){};
string BoolLiteral::serialize() const {return token.literal;}

PrefixExpression::PrefixExpression() = default;
PrefixExpression::PrefixExpression(Token tok, string Operator, unique_ptr<Expression>& right) : token(tok), Operator(Operator), right(move(right)) {};
string PrefixExpression::serialize() const {
    return Operator + right.get()->serialize();
}

InfixExpression::InfixExpression() = default;
InfixExpression::InfixExpression(Token tok, string Operator, unique_ptr<Expression>& left, unique_ptr<Expression>& right) : token(tok), Operator(Operator), left(move(left)), right(move(right)) {};
string InfixExpression::serialize() const {
    string res =  "(" + left.get()->serialize() + " " + Operator + " " + right.get()->serialize() + ")";
    return res;
};

IfExpression::IfExpression(Token tok, 
unique_ptr<Expression>& cond, 
unique_ptr<BlockStatement>& cons, 
unique_ptr<BlockStatement>& alt) : token(tok), condition(move(cond)), consequence(move(cons)), alternative(move(alt)){};

string IfExpression::serialize() const {
    string alt;
    if (alternative != nullptr) alt = " else " + alternative.get()->serialize();
    return "if " + condition.get()->serialize() + " " +
            consequence.get()->serialize() + alt;
}

/************************* Statements ************************/
Statement::Statement() = default;
Statement::~Statement() = default;
string Statement::serialize() const {return "";};

LetStatement::LetStatement() = default;
LetStatement::LetStatement(Token tok, Identifier ident, unique_ptr<Expression>& val) : token(tok), identifier(ident), value(move(val)) {};
string LetStatement::serialize() const {
    return token.literal 
            + " " 
            + identifier.serialize() 
            + " = " 
            + value.get()->serialize() + ";";
    
};

ReturnStatement::ReturnStatement() = default;
ReturnStatement::ReturnStatement(Token tok, unique_ptr<Expression>& val) : token(tok), value(move(val)) {}
string ReturnStatement::serialize() const {
    return token.literal + " " + value.get()->serialize() + ";";
}

/* To allow for a single line expression like "x + 5;"*/
ExpressionStatement::ExpressionStatement() {
    expression = nullptr;
};
ExpressionStatement::ExpressionStatement(Token tok, unique_ptr<Expression>& express): token(tok), expression(move(express)) {};
string ExpressionStatement::serialize() const {
    return expression.get()->serialize();
}

BlockStatement::BlockStatement(Token tok, vector<unique_ptr<Statement>>&& stmts) : token(tok), statements(move(stmts)) {};
string BlockStatement::serialize() {
    string res = "{";
    for (int i = 0; i < statements.size(); i++) {
        res += statements.at(i).get()->serialize(); + ";";
    }
    return res + "}";
}

/*********************** Program (root node) ********************/ 
string Program::serialize() const {
    string res = "";
    for (int i = 0; i < statements.size(); i++) {
        Statement* stmt = statements.at(i).get();
        res += stmt->serialize();
    }
    return res;
}

