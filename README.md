# NenePancake
C++のテストフレームワークです.  
環境によってはセキュリティソフトにウイルスとして検出される場合があります. その場合は適宜スキャンを無効化してください.

```cpp
#include <iostream>
#include <NeneMath/FiniteField.hpp>
#include <NenePancake/NenePancake.hpp>

//--------------------------------------
// int_mod のテスト
//--------------------------------------
TEST(int_mod, ring_arith){
    using mint = int_mod<4>;
    mint a = 3, b = 5;
    TT_EXPECT_EQ((a + b).get_int(), 0);
    TT_EXPECT_EQ((a - b).get_int(), 2);
    TT_EXPECT_EQ((a * b).get_int(), 3);
}

TEST(int_mod, field_arith){
    using mint = int_mod<7>;
    mint a = 3, b = 5;
    TT_EXPECT_EQ((a + b).get_int(), 1);
    TT_EXPECT_EQ((a - b).get_int(), 5);
    TT_EXPECT_EQ((a * b).get_int(), 1);
    TT_EXPECT_EQ((a / b).get_int(), (a * b.inverse()).get_int());
}

TEST(int_mod, inverse_and_pow){
    using mint = int_mod<11>;
    for (int i = 1; i < 11; ++i){
        mint x = i;
        mint inv = x.inverse();
        TT_EXPECT_EQ((x * inv).get_int(), 1);
        TT_EXPECT_EQ(x.pow(0).get_int(), 1);
        TT_EXPECT_EQ(x.pow(1).get_int(), x.get_int());
    }
    // 0の逆元・負の冪での例外
    {
        mint z = 0;
        TT_EXPECT_THROW(z.inverse());
        TT_EXPECT_THROW(z.pow(-1));
    }
    // 負のべき
    {
        mint x = 3;
        TT_EXPECT_EQ(x.pow(-1).get_int(), x.inverse().get_int());
        TT_EXPECT_EQ((x.pow(-2) * x * x).get_int(), 1);
    }
}

int main(int argc, char** argv){
    return pancake::RunAllTests(argc, argv);
}
```

## 参考: ソフトウェアテスト技法
まず, このようにシステムの入出力のみを観察する手法を「ブラックボックステスト」という. ブラックボックステストでは, すべての入力パターンをテストするわけにはいかないので, 必要最低限のテストケースを選定する必要がある. それには以下の手法が知られている:  
1. 同値分割法と境界値分析
2. ディシジョンテーブル
3. 状態遷移テスト
4. 組み合わせテスト

特に, 同値分割法と境界値分析は重要である.
### 同値分割法
