#include<iostream>

using namespace std;

struct ObjTypes {
    string INTEGER_OBJ = "INTEGER";
    string BOOLEAN_OBJ = "BOOLEAN";
    string NULL_OBJ = "NULL";
    string STRING_OBJ = "STRING";
    string ARRAY_OBJ = "ARRAY";
} objs;

class Object {
    public:
    string type;
    virtual ~Object() = default;
    virtual string serialize() const = 0;
    virtual string getType() const = 0;
};

class Integer: public Object {
    public:
    int value;
    string type = objs.INTEGER_OBJ;

    Integer(int val) : value(val) {};

    string serialize() const override {
        return to_string(value);
    }

    string getType() const override {
        return type;
    }
};

class Boolean: public Object {
    public:
    bool value;
    string type = objs.BOOLEAN_OBJ;

    Boolean(bool val) : value(val) {};

    string serialize() const override {
        return value ? "true" : "false";
    }

    string getType() const override {
        return type;
    }
};

class Null: public Object {
    public:
    string type = objs.NULL_OBJ;

    Null() = default;

    string serialize() const override {
        return "null";
    }
    string getType() const override {
        return type;
    }
};

class String: public Object {
    public:
    string type = objs.STRING_OBJ;
    string value;

    String() = default;
    String(string val) : value(val) {};

    string serialize() const override {
        return value;
    }
    string getType() const override {
        return type;
    }
};

class Array: public Object {
    public:
    string type = objs.ARRAY_OBJ;
    vector<unique_ptr<Object>> elements;

    Array(vector<unique_ptr<Object>>&& elements) : elements(move(elements)) {};

    string serialize() const override {
        string res = "[";
        int i = 0;
        for (auto& element : elements) {
            res += element.get()->serialize();
            if (i++ < elements.size() - 1) res += ", ";
        }
        res += "]";
        return res;
    }
    string getType() const override {
        return type;
    }
};