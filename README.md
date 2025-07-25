# NenePancake
C++のテストフレームワークです.  
環境によってはセキュリティソフトにウイルスとして検出される場合があります. その場合は適宜スキャンを無効化してください.

```cpp
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
まず, このようにシステムの入出力のみを観察する手法を「ブラックボックステスト」といいます. ブラックボックステストでは, すべての入力パターンをテストするわけにはいかないので, 必要最低限のテストケースを選定する必要があります. それには以下の手法が知られています:  
1. 同値分割法と境界値分析
2. ディシジョンテーブル
3. 状態遷移テスト
4. 組み合わせテスト

特に, 同値分割法と境界値分析は重要です. 基本的にこの二つの手法を合わせてテストケースを作成します.
### 同値分割法
同値分割法は, 出力が等しい入力パターンを同一視する手法です. 例えば`int_mod<3>`の入力は2^32通りありますが, 出力は3通りしかないので, テストケースは3通りで済みます.

### 境界値分析
境界値分析は出力が切り替わるような値の前後をテストする手法です. 例えば`int_mod<100> a; a.get_int();`は`a=99`まではそのままの値が得られますが, `a=100`では0が得られます. このようなとき, `a=99`と`a=100`を両方テストし, 境界における変化を観察します. また, 入力の定義域の最大値と最小値, およびその1つ外側の値を入力し, 正しく機能するか/例外が返るかを観察します.

### Tips
テストケースの期待値を計算するときは対話モードのPythonやJuliaなどを使うと便利.
```bash
               _
   _       _ _(_)_     |  Documentation: https://docs.julialang.org
  (_)     | (_) (_)    |
   _ _   _| |_  __ _   |  Type "?" for help, "]?" for Pkg help.
  | | | | | | |/ _` |  |
  | | |_| | | | (_| |  |  Version 1.11.6 (2025-07-09)
 _/ |\__'_|_|_|\__'_|  |  Official https://julialang.org/ release
|__/                   |

julia> p = 2^(31)-1
2147483647

julia> a = 13523734547
13523734547

julia> b = 5293453
5293453

julia> (a+b)%p
1529028000
```
