#include"lexer.cpp"
#include<iostream>

using namespace std;

#define MAX_INPUT_LENGTH 100

void repl() {
    while (1) {
        cout << "> ";
        char input[MAX_INPUT_LENGTH];
        cin.getline(input, MAX_INPUT_LENGTH, '\n');

        Lexer l = Lexer(input);
        for (Token tok = l.nextToken(); tok.type != types.EoF; tok = l.nextToken()) {
            cout << "{Type: " << tok.type
                 << " Literal: " << tok.literal << "}" << endl;
        }
        // cin.clear();
    }
};