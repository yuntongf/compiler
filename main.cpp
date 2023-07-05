// #include "repl.cpp"
#include"parser.cpp"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    // cout << "Welcome to the July programming language" << endl;
    // repl();
    string input = "5;";
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    auto program = Program();
    int error = p.parseProgram(&program);
    if (error) cout << "test failed due to error in parser..." << endl;
    Statement* temp = program.statements.at(0).get();
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(temp);
    cout << stmt->expression << endl;
    IntLiteral* intLit = dynamic_cast<IntLiteral*>(stmt->expression);
    cout << intLit <<endl;
    return 0;
}