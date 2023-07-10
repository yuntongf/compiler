#include"ast.cpp"
#include<iostream>
#include<vector>
#include<map>

using namespace std;
enum { // determines the "right-binding power" of an operation
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
    void parseExpressionStatement(ExpressionStatement* statement);

    unique_ptr<BlockStatement> parseBlockStatement();

    /* Expressions */
    unique_ptr<Expression> parseIdentifier();
    unique_ptr<Expression> parseIntLiteral();
    unique_ptr<Expression> parseBoolLiteral();
    unique_ptr<Expression> parseFnLiteral();

    unique_ptr<Expression> parseExpression(int precedence);
    unique_ptr<Expression> parsePrefixExpression();
    unique_ptr<Expression> parseInfixExpression(unique_ptr<Expression>& leftExpression);
    unique_ptr<Expression> parseGroupedExpression();
    unique_ptr<Expression> parseIfExpression();
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
        LetStatement stmt = LetStatement();
        parseLetStatement(&stmt);
        if (&stmt != nullptr) {
            program->statements.push_back(make_unique<LetStatement>(stmt.token, stmt.identifier, stmt.value));
        }
    } 
    // Return statements
    else if (currTok.type == types.RETURN) {
        ReturnStatement statement = ReturnStatement();
        parseReturnStatement(&statement);
        if (&statement != nullptr) {
            program->statements.push_back(make_unique<ReturnStatement>(statement.token, statement.value));
        }
    } 
    // Expression statements
    else {
        ExpressionStatement statement = ExpressionStatement();
        parseExpressionStatement(&statement);
        if (&statement != nullptr) {
            program->statements.push_back(make_unique<ExpressionStatement>(statement.token, statement.expression));
        }
    }
}

void Parser::parseLetStatement(LetStatement* statement) {
    statement->token = currTok;
    if (nextTok.type != types.IDENT) {
        errors.push_back("No identifiers after 'let'");
        statement = nullptr;
        return;
    } else {
        readToken();
        Identifier identifier;
        identifier.token = currTok; identifier.value = currTok.literal;
        statement->identifier = identifier;

        if (nextTok.type != types.ASSIGN) {
            errors.push_back("Expected '=', but got "+nextTok.literal);
            statement = nullptr;
            return;
        } else {
            readToken(); // currTok is '='
            readToken();
            statement->value = move(parseExpression(LOWEST));
            
            if (nextTok.type == types.SEMICOLON) readToken();
            else {
                errors.push_back("Expected ';', but got"+currTok.literal);
                statement == nullptr;
                return;
            }
        }
    }
}
void Parser::parseReturnStatement(ReturnStatement* statement) {
    statement->token = currTok; // current token is 'return'
    readToken();
    // parse expression
    statement->value = move(parseExpression(LOWEST));
    if (nextTok.type == types.SEMICOLON) readToken();
    else {
        errors.push_back("Expected ';', but got"+currTok.literal);
        statement == nullptr;
        return;
    }
}
void Parser::parseExpressionStatement(ExpressionStatement* stmt) {
    stmt->token = currTok;
    stmt->expression = move(parseExpression(LOWEST));
    /* Optional semicolon */
    if (nextTok.type == types.SEMICOLON) {
        readToken();
    }
}
unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    Token tok = currTok;
    readToken(); // skip '{'

    Program program = Program();
    while (currTok.type != types.RBRACE) {
        if (currTok.type == types.EoF) {
            errors.push_back("Expected '{' for block statement");
            return nullptr;
        }
        parseStatement(&program);
        readToken(); // skip ';'
    }
    return make_unique<BlockStatement>(tok, move(program.statements));
}


/************************** Expressions ****************************/
using pPrefixParser = unique_ptr<Expression> (Parser::*) ();
using pInfixParser = unique_ptr<Expression> (Parser::*) (unique_ptr<Expression>& leftExpression);

map<string, pPrefixParser> prefixParsers = {
    {types.IDENT, &Parser::parseIdentifier},
    {types.INT, &Parser::parseIntLiteral},
    {types.TRUE, &Parser::parseBoolLiteral},
    {types.FALSE, &Parser::parseBoolLiteral},
    {types.SURPRISE, &Parser::parsePrefixExpression},
    {types.MINUS, &Parser::parsePrefixExpression},
    {types.LPAREN, &Parser::parseGroupedExpression},
    {types.IF, &Parser::parseIfExpression},
    {types.FUNCTION, &Parser::parseFnLiteral}
};
map<string, pInfixParser> infixParsers = {
    {types.EQ, &Parser::parseInfixExpression},
    {types.NOT_EQ, &Parser::parseInfixExpression},
    {types.LESS, &Parser::parseInfixExpression},
    {types.GREATER, &Parser::parseInfixExpression},
    {types.PLUS, &Parser::parseInfixExpression},
    {types.MINUS, &Parser::parseInfixExpression},
    {types.SLASH, &Parser::parseInfixExpression},
    {types.ASTERISK, &Parser::parseInfixExpression},
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

    int nextPrecedence;
    if (precedences.find(nextTok.type) == precedences.end()) {
        nextPrecedence = LOWEST;
    } else {
        nextPrecedence = precedences[nextTok.type];
    }
    while (nextTok.type != types.SEMICOLON && nextTok.type != types.EoF && precedence < nextPrecedence) {
        // if (nextTok.type == types.RPAREN) readToken();
        pInfixParser infixParser = infixParsers[nextTok.type];
        if (infixParser == nullptr) {
            return leftExpression;
        }
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

unique_ptr<Expression> Parser::parseBoolLiteral() {
    bool boolean;
    if (currTok.type == types.TRUE) boolean = true;
    else boolean = false;
    return make_unique<BoolLiteral>(currTok, boolean);
}

unique_ptr<Expression> Parser::parseFnLiteral() {
    Token tok = currTok;
    readToken(); // skip 'fn'
    vector<unique_ptr<Expression>> params = {};
    // parse params
    if (currTok.type != types.LPAREN) {
        errors.push_back("Expected (, but instead got" + nextTok.literal);
        return nullptr;
    } else {
        readToken(); // skip '('
        while (currTok.type != types.RPAREN) {
            unique_ptr<Expression> param = parseIdentifier();
            if (param == nullptr) {
                errors.push_back("Failed to parse parameters of fn");
                return nullptr;
            }
            params.push_back(move(param));
            readToken(); // skip current identifier
            if (currTok.type == types.COMMA) readToken(); // skip 
            else if (currTok.type != types.RPAREN) {
                errors.push_back("Expect ',' or ')', but instead got " + currTok.literal);
                return nullptr;
            }
        }
        readToken(); // skip ')'
        unique_ptr<BlockStatement> body = parseBlockStatement();
        if (nextTok.type == types.SEMICOLON) readToken(); // skip to ';'
        return make_unique<FnLiteral>(tok, move(params), body);
    }
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

unique_ptr<Expression> Parser::parseGroupedExpression() {
    readToken(); // skip '('
    unique_ptr<Expression> exp = parseExpression(LOWEST);
    // test for back parenthesis
    if (nextTok.type != types.RPAREN) return nullptr;
    else readToken();
    
    return exp;
}

unique_ptr<Expression> Parser::parseIfExpression() {
    Token tok = currTok;
    readToken(); // skip 'if'
    if (currTok.type != types.LPAREN) {
        errors.push_back("Expected '(', but instead got '" + currTok.literal + "'");
        return nullptr;
    } 

    unique_ptr<Expression> condition = parseGroupedExpression();
    if (condition == nullptr) {
        errors.push_back("Cannot parse condition of if statement");
        return nullptr;
    } else readToken(); // skip ')'

    if (currTok.type != types.LBRACE) {
        errors.push_back("No block statement after if condition; Expected '{', but instead got '" + currTok.literal + "'");
        return nullptr;
    }
    unique_ptr<BlockStatement> consequence = parseBlockStatement();
    readToken(); // skip '}'

    unique_ptr<BlockStatement> alternative = nullptr;
    if (currTok.type == types.ELSE) {
        readToken(); // skip 'else'
        alternative = parseBlockStatement();
    } 
    if (nextTok.type == types.SEMICOLON) readToken(); // skip to ';'
    return make_unique<IfExpression>(tok, condition, consequence, alternative);
}

