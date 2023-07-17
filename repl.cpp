// #include"parser.cpp"
#include<iostream>
#include"vm.cpp"

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
        if (error) cout << "test failed due to error in parser..." << endl;

        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) cout << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) cout << "test failed due to error in vm..." << endl;

        cout << vm.getLastPopped().get()->serialize() << endl;
        // cin.clear();
    }
};