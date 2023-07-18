#include<iostream>
#include<map>
#include<memory>

using namespace std;

typedef string SymbolScope;

const SymbolScope GlobalScope = "GLOBAL";

class Symbol {
    public:
    string name;
    SymbolScope scope;
    int index;

    Symbol(string name, SymbolScope scope, int index) : name(name), scope(scope), index(index) {};
};

class SymbolTable {
    public:
    map<string, unique_ptr<Symbol>> store;
    int numDefs;

    SymbolTable() {
        numDefs = 0;
    }

    unique_ptr<Symbol>& define(string name) {
        if (store.count(name) > 0) {
            int index = store[name].get()->index;
            store[name] = make_unique<Symbol>(name, GlobalScope, index);
        } else {
            store[name] = make_unique<Symbol>(name, GlobalScope, numDefs);
            numDefs++;
        }
        return store[name];
    }

    unique_ptr<Symbol>& resolve(string name) {
        return store[name];
    }
};