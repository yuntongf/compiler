#include"ast.cpp"
#include<iostream>
#include<vector>

using namespace std;

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
    void parseLetStatement(LetStatement* statement);
    void parseReturnStatement(ReturnStatement* statement);
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

void Parser::parseStatement(Program* program) {
    if (currTok.type == types.LET) {
        LetStatement statement = LetStatement();
        parseLetStatement(&statement);
        if (&statement != nullptr) {
            program->statements.push_back(make_unique<LetStatement>(statement));
        }
    } else if (currTok.type == types.RETURN) {
        ReturnStatement statement = ReturnStatement();
        parseReturnStatement(&statement);
        if (&statement != nullptr) {
            program->statements.push_back(make_unique<ReturnStatement>(statement));
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


