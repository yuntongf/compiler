#include"parser.cpp"
#include<iostream>

using namespace std;

#define MAX_INPUT_LENGTH 200

void repl() {
    while (1) {
        cout << "> ";
        char input[MAX_INPUT_LENGTH];
        cin.getline(input, MAX_INPUT_LENGTH, '\n');

        Lexer l = Lexer(input);
        Parser p = Parser(l);
        auto program = Program();
        int error = p.parseProgram(&program);
        if (error == 0) {
            for (int i = 0; i < program.statements.size(); i++) {
                cout << program.statements.at(i).get()->serialize() << endl;
            }
        }
        // cin.clear();
    }
};