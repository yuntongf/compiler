#include"vm.cpp"
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
        {"-2 - 2", -4}
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