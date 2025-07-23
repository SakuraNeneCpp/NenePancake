#include <NenePancake/NenePancake.hpp>

TEST(Math, Add){
    int a = 2+2;
    TT_EXPECT_EQ(a, 4);
    TT_ASSERT_TRUE(a == 4);
}

class MyFix : public pancake::Test {
public:
    void SetUp() override { buf = 42; }
    int buf;
};

TEST_F(MyFix, Value){
    TT_EXPECT_EQ(buf, 42);
}

int main(int argc, char** argv){
    return pancake::RunAllTests(argc, argv);
}
