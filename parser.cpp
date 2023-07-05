#include"ast.cpp"
#include<iostream>
#include<vector>
#include<map>

using namespace std;
/* pPrefixParser f = map[]; (*f)(params);*/
enum {
    _ = 0,
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL
};

class Parser {
    Lexer l;
    Token currTok;
    Token nextTok;
    vector<string> errors = {};

    public:
    Parser(Lexer& lexer) {
        l = lexer;
        readToken();
        readToken();
    };

    void readToken();
    vector<string> getErrors();
    int parseProgram(Program* program);
    void parseStatement(Program* program);

    /* Statement*/
    void parseLetStatement(LetStatement* statement);
    void parseReturnStatement(ReturnStatement* statement);
    void parserExpressionStatement(ExpressionStatement* statement);

    /* Expressions */
    unique_ptr<Expression> parseIdentifier();
    unique_ptr<Expression> parseIntLiteral();
    unique_ptr<Expression> parseExpression(int precedence);
    unique_ptr<Expression> parsePrefixExpression();
    unique_ptr<Expression> parseInfixExpression(unique_ptr<Expression>& leftExpression);
    // void prefixParser(Expression* expression);
    // void infixParser(Expression* expression); // argument is the left side of the infix operator
};


void Parser::readToken() {
    currTok = nextTok;
    nextTok = l.nextToken();
}

vector<string> Parser::getErrors() {
    return errors;
}

int Parser::parseProgram(Program* program) {
    while (currTok.type != types.EoF) {
        // cout << "start parsing statements";
        parseStatement(program);
        readToken();
    }
    if (errors.size() > 0) {
        cout << "parser has " << errors.size() << " errors:" << endl;
        for (string err : errors) {
            cout << "parser error: " 
                 << err << endl;
        }
        return 1;
    }
    return 0;
}


/********************** Statements **************************/
void Parser::parseStatement(Program* program) {
    // Let statements
    if (currTok.type == types.LET) {
        LetStatement statement = LetStatement();
        parseLetStatement(&statement);
        if (&statement != nullptr) {
            program->statements.push_back(make_unique<LetStatement>(statement));
        }
    } 
    // Return statements
    else if (currTok.type == types.RETURN) {
        ReturnStatement statement = ReturnStatement();
        parseReturnStatement(&statement);
        if (&statement != nullptr) {
            program->statements.push_back(make_unique<ReturnStatement>(statement));
        }
    } 
    // Expression statements
    else {
        ExpressionStatement statement = ExpressionStatement();
        parserExpressionStatement(&statement);
        if (&statement != nullptr) {
            program->statements.push_back(make_unique<ExpressionStatement>(statement.token, statement.expression));
        }
    }
}

void Parser::parseLetStatement(LetStatement* statement) {
    statement->token = currTok;
    if (nextTok.type != types.IDENT) {
        // cout << "No identifier" << endl;
        errors.push_back("No identifiers after 'let'");
        statement = nullptr;
        return;
    } else {
        readToken();
        Identifier identifier;
        identifier.token = currTok; identifier.value = currTok.literal;
        statement->identifier = identifier;

        if (nextTok.type != types.ASSIGN) {
            // cout << "No assign '='";
            errors.push_back("Expected '=', but got "+nextTok.literal);
            statement = nullptr;
            return;
        } else {
            readToken();
            // TODO: parse expression
            while (currTok.type != types.SEMICOLON) {
                readToken();
                if (currTok.type == types.IDENT) {
                    Identifier ident;
                    ident.token = currTok;
                    ident.value = currTok.literal;
                    statement->value = &ident;
                }
            }
        }
    }
}
void Parser::parseReturnStatement(ReturnStatement* statement) {
    statement->token = currTok;
    // TODO: parse expression
    while (currTok.type != types.SEMICOLON) {
        readToken();
    }
}
void Parser::parserExpressionStatement(ExpressionStatement* stmt) {
    stmt->token = currTok;
    stmt->expression = parseExpression(LOWEST);
    /* Optional semicolon */
    if (nextTok.type == types.SEMICOLON) {
        readToken();
    }
}


/************************** Expressions ****************************/
using pPrefixParser = unique_ptr<Expression> (Parser::*) ();
using pInfixParser = unique_ptr<Expression> (Parser::*) (unique_ptr<Expression>& leftExpression);
map<string, pPrefixParser> prefixParsers = {
    {types.IDENT, &Parser::parseIdentifier},
    {types.INT, &Parser::parseIntLiteral},
    {types.SURPRISE, &Parser::parsePrefixExpression},
    {types.MINUS, &Parser::parsePrefixExpression}
};
map<string, pInfixParser> infixParsers = {
    {types.EQ, &Parser::parseInfixExpression},
    {types.NOT_EQ, &Parser::parseInfixExpression},
    {types.LESS, &Parser::parseInfixExpression},
    {types.GREATER, &Parser::parseInfixExpression},
    {types.PLUS, &Parser::parseInfixExpression},
    {types.MINUS, &Parser::parseInfixExpression},
    {types.SLASH, &Parser::parseInfixExpression},
    {types.ASTERISK, &Parser::parseInfixExpression}
};
map<string, int> precedences = {
    {types.EQ, EQUALS},
    {types.NOT_EQ, EQUALS},
    {types.LESS, LESSGREATER},
    {types.GREATER, LESSGREATER},
    {types.PLUS, SUM},
    {types.MINUS, SUM},
    {types.SLASH, PRODUCT},
    {types.ASTERISK, PRODUCT}

};

unique_ptr<Expression> Parser::parseExpression(int precedence) {
    pPrefixParser prefixParser = prefixParsers[currTok.type];
    if (prefixParser == nullptr) return nullptr;
    unique_ptr<Expression> leftExpression = (this->*prefixParser)();

    int nextPrecedence = precedences[nextTok.type];
    while (nextTok.type != types.SEMICOLON && precedence < nextPrecedence) {
        pInfixParser infixParser = infixParsers[nextTok.type];
        if (infixParser == nullptr) return leftExpression;
        readToken();
        leftExpression = (this->*infixParser)(leftExpression);
    }
    return leftExpression;
}

unique_ptr<Expression> Parser::parseIdentifier() {
    // create an identifier on heap and return it's address
    Identifier ident = Identifier();
    ident.token = currTok;
    ident.value = currTok.literal;
    unique_ptr<Identifier> temp = make_unique<Identifier>(ident);
    return temp;
}

unique_ptr<Expression> Parser::parseIntLiteral() {
    int value = stoi(currTok.literal);
    unique_ptr<Expression> res = make_unique<IntLiteral>(currTok, value);
    return res;
}

unique_ptr<Expression> Parser::parsePrefixExpression() {
    Token tok = currTok;
    string Operator = currTok.literal;
    readToken();
    unique_ptr<Expression> right = parseExpression(PREFIX);
    return make_unique<PrefixExpression>(tok, Operator, right);
}

unique_ptr<Expression> Parser::parseInfixExpression(unique_ptr<Expression>& leftExpression) {
    Token tok = currTok;
    int precedence = precedences[currTok.type];
    readToken();
    unique_ptr<Expression> right = parseExpression(precedence);
    return make_unique<InfixExpression>(tok, tok.literal, leftExpression, right);
}

