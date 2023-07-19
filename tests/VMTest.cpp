#include"../vm.cpp"
#include<iostream>
#include<gtest/gtest.h>

using namespace std;

void parse(string input, Program* program) {
    Lexer l = Lexer(input);
    Parser p = Parser(l);
    int error = p.parseProgram(program);
    if (error) FAIL() << "test failed due to error in parser..." << endl;
}

template <typename T>
struct VMTest {
    const string input;
    T expected;
};


const auto True = Boolean(true);
const auto False = Boolean(false);

TEST(VMTest, VMIntegerTest) {
    vector<VMTest<int>> tests = {
        {"1", 1},
        {"2", 2},
        {"-5", -5},
        {"1 + 2", 3},
        {"2 - 3", -1},
        {"4 * 3", 12},
        {"4 / 2", 2},
        {"-2 - 2", -4},
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Integer* integer = dynamic_cast<Integer*>(obj.get());
        ASSERT_EQ(integer->value, test.expected);

    }
}

TEST(VMTest, VMBooleanTest) {
    vector<VMTest<bool>> tests = {
        {"true", true},
        {"!true", false},
        {"false", false},
        {"1 < 2", true},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"true == true", true},
        {"!true != true", true},
        {"true != false", true},
        {"(5 < 6) == true", true},
        {"(6 > 5) == true", true},
        {"(-8 > 5) == true", false},
        {"!(if (false) {3};)", true}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Boolean* boolean = dynamic_cast<Boolean*>(obj.get());
        ASSERT_EQ(boolean->value, test.expected);
        // ASSERT_EQ(vm.sp, 0);
    }
}

TEST(VMTest, VMConditionalTest) {
    vector<VMTest<int>> tests = {
        {"if (true) {1};", 1},
        {"if (true) {2} else {3};", 2},
        {"if (false) {2} else {3};", 3},
        {"if (if (false) {1};) {3} else {4};", 4}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Integer* integer = dynamic_cast<Integer*>(obj.get());
        ASSERT_EQ(integer->value, test.expected);
    }
}

TEST(VMTest, VMConditionalNullTest) {
    auto null = Null();
    vector<VMTest<Null>> tests = {
        {"if (false) {1};", null},
        {"if (1 == 2) {3};", null}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        ASSERT_EQ(test.expected.getType(), obj.get()->getType());
    }
}

TEST(VMTest, VMLetTest) {
    vector<VMTest<int>> tests = {
        {"let one = 1; one;", 1},
        {"let one = 1; let two = 2; one + two;", 3},
        {"let one = 1; let two = one; one + two;", 2}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;
        // cout << serialize(compiler.getByteCode().instructions) << endl;
        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Integer* integer = dynamic_cast<Integer*>(obj.get());
        ASSERT_EQ(integer->value, test.expected);
    }
}

TEST(VMTest, StringTest) {
    vector<VMTest<string>> tests = {
        {"\"hello\"", "hello"},
        {"\"he\" + \"llo\"", "hello"}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;
        // cout << serialize(compiler.getByteCode().instructions) << endl;
        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        String* lit = dynamic_cast<String*>(obj.get());
        ASSERT_EQ(lit->value, test.expected);
    }
}

TEST(VMTest, ArrayTest) {
    vector<VMTest<vector<int>>> tests = {
        {"[]", vector<int>{}},
        {"[1]", vector<int>{1}},
        {"[1, 2 + 4]", vector<int>{1, 6}}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;
        // cout << serialize(compiler.getByteCode().instructions) << endl;
        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Array* lit = dynamic_cast<Array*>(obj.get());
        ASSERT_EQ(lit->elements.size(), test.expected.size());
        for (int i = 0; i < test.expected.size(); i++) {
            Integer* intLit = dynamic_cast<Integer*>(lit->elements.at(i).get());
            ASSERT_EQ(intLit->value, test.expected.at(i));
        }
    }
}

TEST(VMTest, HashTableTest) {
    vector<VMTest<vector<pair<int, int>>>> tests = {
        {"{}", vector<pair<int, int>>{}},
        {"{1: 2, 3: 4}", vector<pair<int, int>>{{1, 2}, {3, 4}}},
        {"{1: 2, 3: 2 + 4}", vector<pair<int, int>>{{1, 2}, {3, 6}}}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;
        // cout << serialize(compiler.getByteCode().instructions) << endl;
        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        HashTable* hash = dynamic_cast<HashTable*>(obj.get());
        ASSERT_EQ(hash->table.size(), test.expected.size());
        int i = 0;
        for (auto& entry : hash->table) {
            Integer* key = dynamic_cast<Integer*>(entry.second.get()->key.get());
            ASSERT_EQ(key->value, test.expected.at(i).first);
            Integer* key2 = dynamic_cast<Integer*>(entry.second.get()->value.get());
            ASSERT_EQ(key2->value, test.expected.at(i++).second);
        }
        // ASSERT_EQ(hash->serialize(), "{1: 2, 3: 4}");
    }
}

TEST(VMTest, ArrayIndexTest) {
    vector<VMTest<int>> tests = {
        {"[1, 2, 3][1]", 2},
        {"[1, 3, 5 * 4][4 - 2]", 20},
        {"{1: 2, 3: 4, 2: 1}[3]", 4},
        // {"{\"hello\": 2, \"world\": 6}[\"hello\"]", 2}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Integer* integer = dynamic_cast<Integer*>(obj.get());
        ASSERT_EQ(integer->value, test.expected);
    }
}

TEST(VMTest, ArrayIndexNullTest) {
    auto null = Null();
    vector<VMTest<Null>> tests = {
        {"[][9]", null},
        {"[1, 2, 3][999]", null},
        {"[][0]", null},
        {"{1: 2, 3: 4}[2]", null}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        ASSERT_EQ(test.expected.getType(), obj.get()->getType());
    }
}


TEST(VMTest, FnCallingTest) {
    vector<VMTest<int>> tests = {
        {"let f = fn(){return 2 + 3;}; f();", 5},
        {"let a = fn(){return 1;}; let b = fn(){return 2 + a();}; let c = fn(){b() + 3;}; c();", 6},
        {"let a = fn(){return 1; 2;}; a();", 1},
        {"let a = fn(){return 1; return 2;}; a();", 1}
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;
        // cout << serialize(compiler.getByteCode().instructions) << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Integer* integer = dynamic_cast<Integer*>(obj.get());
        ASSERT_EQ(integer->value, test.expected);

    }
}

TEST(VMTest, FnCallingNullTest) {
    auto null = Null();
    vector<VMTest<Null>> tests = {
        {"let a = fn(){}; a();", null},
        {"let a = fn(){}; let b = fn(){a();}; b();", null},
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        ASSERT_EQ(test.expected.getType(), obj.get()->getType());
    }
}

TEST(VMTest, FirstClassFnTest) {
    vector<VMTest<int>> tests = {
        {"let inside = fn(){2 + 3;}; let outside = fn(){inside}; outside()();", 5},
    };
    for (auto test : tests) {
        auto program = Program();
        parse(test.input, &program);
        auto compiler = Compiler();
        int err = compiler.compileProgram(&program);
        if (err) FAIL() << "test failed due to error in compiler..." << endl;

        auto vm = VM(compiler.getByteCode());
        if (vm.run()) FAIL() << "test failed due to error in vm..." << endl;

        unique_ptr<Object>& obj = vm.getLastPopped();
        Integer* integer = dynamic_cast<Integer*>(obj.get());
        ASSERT_EQ(integer->value, test.expected);

    }
}