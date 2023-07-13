#include<iostream>
#include<gtest/gtest.h>
#include"code.cpp"

using namespace std;

TEST(CompilerTest, ValueTest) {
    struct Test {
        OpCode opcode;
        vector<int> operands;
        vector<byte> expected;
    };
    vector<Test> tests = {
        {OpConstant, vector<int>{65534}, vector<byte>{OpConstant, (byte) 0, (byte) 0, (byte) 255, (byte)254}}
    };
    for (Test test : tests) {
        auto instruction = make(OpConstant, test.operands);
        ASSERT_EQ(instruction.size(), test.expected.size());
        for (int i = 0; i < test.expected.size(); i++) {
            ASSERT_EQ(test.expected.at(i), instruction.at(i));
        }
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}