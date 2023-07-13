#include<iostream>

using namespace std;

struct ObjTypes {
    string INTEGER_OBJ = "INTEGER";
} objs;

class Object {
    public:
    string type;
    virtual ~Object() = default;
    virtual const string serialize();
};

class Integer: public Object {
    public:
    int value;
    string type = objs.INTEGER_OBJ;
    Integer() = default;
    string serialize() const {
        return "" + value;
    }
};