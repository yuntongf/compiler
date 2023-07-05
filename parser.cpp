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
    Expression* parseIdentifier();
    Expression* parseIntLiteral();
    Expression* parseExpression(int precedence);
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
            program->statements.push_back(make_unique<ExpressionStatement>(statement));
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
                    statement->value = ident;
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
    Expression* expression = parseExpression(LOWEST);
    stmt->expression = *expression;
    /* Optional semicolon */
    if (nextTok.type == types.SEMICOLON) {
        readToken();
    }
}


/************************** Expressions ****************************/
using pfunc = Expression* (Parser::*) ();

map<string, pfunc> prefixParsers = {
    {types.IDENT, &Parser::parseIdentifier},
    {types.INT, &Parser::parseIntLiteral}
};

Expression* Parser::parseExpression(int precedence) {
    pfunc prefixParser = prefixParsers[currTok.type];
    if (prefixParser == nullptr) return nullptr;
    return (this->*prefixParser)();
}

Expression* Parser::parseIdentifier() {
    // create an identifier on heap and return it's address
    Identifier ident = Identifier();
    ident.token = currTok;
    ident.value = currTok.literal;
    return make_unique<Identifier>(ident).get();
}

Expression* Parser::parseIntLiteral() {
    IntLiteral intLit = IntLiteral();
    intLit.token = currTok;
    intLit.value = stoi(currTok.literal);
    return make_unique<IntLiteral>(intLit).get();
}


