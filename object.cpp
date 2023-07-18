#include<iostream>
#include<functional>

using namespace std;

struct ObjTypes {
    string INTEGER_OBJ = "INTEGER";
    string BOOLEAN_OBJ = "BOOLEAN";
    string NULL_OBJ = "NULL";
    string STRING_OBJ = "STRING";
    string ARRAY_OBJ = "ARRAY";
    string HASH_OBJ = "HASH_PAIR";
    string HASH_TABLE = "HASH_TABLE";
} objs;

class Object {
    public:
    string type;
    virtual ~Object() = default;
    virtual string serialize() const = 0;
    virtual string getType() const = 0;
    virtual bool hashable() const = 0;
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

    bool hashable() const override {
        return true;
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

    bool hashable() const override {
        return true;
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
    bool hashable() const override {
        return false;
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
    bool hashable() const override {
        return true;
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
    bool hashable() const override {
        return false;
    }
};

typedef size_t HashKey;

class HashPair: public Object {
    public:
    string type = objs.HASH_OBJ;
    unique_ptr<Object> key;
    unique_ptr<Object> value;

    HashPair(unique_ptr<Object>& key, unique_ptr<Object>& val) : key(move(key)), value(move(val)) {};
    string serialize() const override {
        string res = key.get()->serialize();
        res += ": ";
        res += value.get()->serialize();
        return res;
    }
    string getType() const override {
        return type;
    }
    bool hashable() const override {
        return false;
    }
};

class HashTable : public Object {
    public:
    string type = objs.HASH_TABLE;
    map<HashKey, unique_ptr<HashPair>> table;

    HashTable(map<HashKey, unique_ptr<HashPair>> table) : table(move(table)) {};

    string serialize() const override {
        string res = "{";
        int i = 0;
        for (auto& entry : table) {
            res += entry.second.get()->serialize();
            if (i++ < table.size() - 1) res += ", ";
        }
        res += "}";
        return res;
    }
    string getType() const override {return type;}
    bool hashable() const override {
        return false;
    }
};

HashKey hashKey(unique_ptr<Object>& obj) {
    string type = obj.get()->getType();
    if (type == objs.INTEGER_OBJ) {
        Integer* lit = dynamic_cast<Integer*>(obj.get());
        return hash<int>{}(lit->value);
    } else if (type == objs.BOOLEAN_OBJ) {
        Boolean* lit = dynamic_cast<Boolean*>(obj.get());
        return hash<bool>{}(lit->value);
    } else if (type == objs.STRING_OBJ) {
        String* lit = dynamic_cast<String*>(obj.get());
        return hash<string>{}(lit->value);
    } else {
        return 0;
    }
}