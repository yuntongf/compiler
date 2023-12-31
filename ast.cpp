#include"ast.h"

// turn all fields into pointers

using namespace std;

/************************* Expressions *********************/
Expression::Expression() = default;
Expression::~Expression() = default;
string Expression::serialize() const {return "";}
string Expression::getType() const {return type;}

string Identifier::serialize() const {return value;}
string Identifier::getType() const {return type;}

IntLiteral::IntLiteral(Token tok, int val) : token(tok), value(val) {};
string IntLiteral::serialize() const {return token.literal;};
string IntLiteral::getType() const {return type;};

BoolLiteral::BoolLiteral(Token tok, bool val) : token(tok), value(val){};
string BoolLiteral::serialize() const {return token.literal;}
string BoolLiteral::getType() const {return type;};

StringLiteral::StringLiteral(Token tok, string val) : token(tok), value(val) {};
string StringLiteral::serialize() const {return "\"" + value + "\"";};
string StringLiteral::getType() const {return type;};

FnLiteral::FnLiteral(Token tok, vector<unique_ptr<Expression>>&& params, unique_ptr<BlockStatement>& body) : token(tok), params(move(params)), body(move(body)) {};
string FnLiteral::serialize() const {
    string paramStr = "(";
    for (int i = 0; i < params.size(); i++) {
        Expression* param = params.at(i).get();
        paramStr += param->serialize();
        if (i == params.size() - 1) {
            paramStr += ")";
        } else {
            paramStr += ", ";
        }
    }
    return "fn" + paramStr + body.get()->serialize();
}
string FnLiteral::getType() const {return type;};

ArrayLiteral::ArrayLiteral(Token tok, vector<unique_ptr<Expression>>&& elements) : token(tok), elements(move(elements)) {};
string ArrayLiteral::serialize() const {
    string res = "[";
    for (int i = 0; i < elements.size(); i++) {
        res += elements.at(i).get()->serialize();
        if (i < elements.size() - 1) res += ", ";
    }
    res += "]";
    return res;
}
string ArrayLiteral::getType() const {return type;};

HashLiteral::HashLiteral(Token tok, vector<pair<unique_ptr<Expression>, unique_ptr<Expression>>>& pairs) : token(tok), pairs(move(pairs)) {};
string HashLiteral::serialize() const {
    string res = "{";
    int i = 0;
    for (const auto& pair : pairs) {
        res += pair.first.get()->serialize();
        res += ": ";
        res += pair.second.get()->serialize();
        if (i++ < pairs.size() - 1) res += ", ";
    }
    res += "}";
    return res;
}
string HashLiteral::getType() const {return type;};

IndexExpression::IndexExpression(Token tok, unique_ptr<Expression>& entity, unique_ptr<Expression>& index) : token(tok), entity(move(entity)), index(move(index)) {};
string IndexExpression::serialize() const {
    string res = entity.get()->serialize();
    res += "[";
    res += index.get()->serialize();
    res += "]";
    return res;
}
string IndexExpression::getType() const {
    return type;
}

PrefixExpression::PrefixExpression() = default;
PrefixExpression::PrefixExpression(Token tok, string Operator, unique_ptr<Expression>& right) : token(tok), Operator(Operator), right(move(right)) {};
string PrefixExpression::serialize() const {
    return Operator + right.get()->serialize();
}
string PrefixExpression::getType() const {
    return type;
}

InfixExpression::InfixExpression() = default;
InfixExpression::InfixExpression(Token tok, string Operator, unique_ptr<Expression>& left, unique_ptr<Expression>& right) : token(tok), Operator(Operator), left(move(left)), right(move(right)) {};
string InfixExpression::serialize() const {
    string res =  "(" + left.get()->serialize() + " " + Operator + " " + right.get()->serialize() + ")";
    return res;
};
string InfixExpression::getType() const {
    return type;
}

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
string IfExpression::getType() const {return type;};

CallExpression::CallExpression(Token tok, unique_ptr<Expression>& function, vector<unique_ptr<Expression>>&& args) : token(tok), function(move(function)), args(move(args)) {};
string CallExpression::serialize() const {
    string paramStr = "(";
    for (int i = 0; i < args.size(); i++) {
        paramStr += args.at(i).get()->serialize();
        if (i == args.size() - 1) paramStr += ")";
        else paramStr += ", ";
    }
    return function.get()->serialize() + paramStr;
}
string CallExpression::getType() const {return type;};

/************************* Statements ************************/
Statement::Statement() = default;
Statement::~Statement() = default;
string Statement::serialize() const {return "";};
string Statement::getType() const {return type;};

LetStatement::LetStatement() = default;
LetStatement::LetStatement(Token tok, Identifier ident, unique_ptr<Expression>& val) : token(tok), identifier(ident), value(move(val)) {};
string LetStatement::serialize() const {
    return token.literal 
            + " " 
            + identifier.serialize() 
            + " = " 
            + value.get()->serialize() + ";";
    
};
string LetStatement::getType() const {return type;};

ReturnStatement::ReturnStatement() = default;
ReturnStatement::ReturnStatement(Token tok, unique_ptr<Expression>& val) : token(tok), value(move(val)) {}
string ReturnStatement::serialize() const {
    return token.literal + " " + value.get()->serialize() + ";";
}
string ReturnStatement::getType() const {return type;};

/* To allow for a single line expression like "x + 5;"*/
ExpressionStatement::ExpressionStatement() {
    expression = nullptr;
};
ExpressionStatement::ExpressionStatement(Token tok, unique_ptr<Expression>& express): token(tok), expression(move(express)) {};
string ExpressionStatement::serialize() const {
    return expression.get()->serialize();
}
string ExpressionStatement::getType() const {return type;};

BlockStatement::BlockStatement(Token tok, vector<unique_ptr<Statement>>&& stmts) : token(tok), statements(move(stmts)) {};
string BlockStatement::serialize() {
    string res = "{";
    for (int i = 0; i < statements.size(); i++) {
        res += statements.at(i).get()->serialize(); + ";";
    }
    return res + "}";
}
string BlockStatement::getType() const {return type;};

/*********************** Program (root node) ********************/ 
string Program::serialize() const {
    string res = "";
    for (int i = 0; i < statements.size(); i++) {
        Statement* stmt = statements.at(i).get();
        res += stmt->serialize();
    }
    return res;
}
string Program::getType() const {return type;};
