#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <cstring>

#ifndef TT_ENABLE_COLOR
#  define TT_ENABLE_COLOR 1        // 1で有効, 0で無効
#endif

#ifdef _WIN32
#include <windows.h>
#endif


#if TT_ENABLE_COLOR
#  define TT_CLR_RESET  "\x1b[0m"
#  define TT_CLR_RED    "\x1b[31m"
#  define TT_CLR_GREEN  "\x1b[32m"
#  define TT_CLR_YELLOW "\x1b[33m"
#  define TT_CLR_CYAN   "\x1b[36m"
#  define TT_CLR_BOLD   "\x1b[1m"
#else
#  define TT_CLR_RESET  ""
#  define TT_CLR_RED    ""
#  define TT_CLR_GREEN  ""
#  define TT_CLR_YELLOW ""
#  define TT_CLR_CYAN   ""
#  define TT_CLR_BOLD   ""
#endif


namespace pancake {

struct TestInfo {
    std::string suite;
    std::string name;
    std::function<bool()> body;
};

inline std::vector<TestInfo>& registry() {
    static std::vector<TestInfo> r;
    return r;
}

struct Registrar {
    Registrar(const char* suite, const char* name, std::function<bool()> body) {
        registry().push_back({suite, name, std::move(body)});
    }
};

//--------------------------------------
// Assertions
//--------------------------------------
namespace detail {
inline void report(bool ok, const char* file, int line,
                   const char* expr, const std::string& msg,
                   bool fatal, bool& current_failed, bool& current_fatal) {
    if (ok) return;
    current_failed = true;
    std::cerr << (fatal ? TT_CLR_RED "[  FAILED  ] " : TT_CLR_YELLOW "[ NONFATAL ] ")
              << file << ":" << line << ": " << expr;
    if (!msg.empty()) std::cerr << " -> " << msg;
    std::cerr << TT_CLR_RESET << std::endl;
    if (fatal) current_fatal = true;
}

template<class L, class R>
inline std::string to_str(const L& l, const R& r) {
    using std::to_string;
    return std::string("lhs=") + to_string(l) + ", rhs=" + to_string(r);
}
} // namespace detail

#define TT_EXPECT_TRUE(expr) do {                                     \
    bool _ok = static_cast<bool>(expr);                               \
    ::pancake::detail::report(_ok, __FILE__, __LINE__, #expr, "",     \
                               false, _tt_failed_, _tt_fatal_);       \
} while(0)

#define TT_ASSERT_TRUE(expr) do {                                     \
    bool _ok = static_cast<bool>(expr);                               \
    ::pancake::detail::report(_ok, __FILE__, __LINE__, #expr, "",     \
                               true, _tt_failed_, _tt_fatal_);        \
    if (_tt_fatal_) return;                                           \
} while(0)

#define TT_EXPECT_EQ(lhs, rhs) do {                                   \
    auto _l = (lhs); auto _r = (rhs);                                 \
    bool _ok = (_l == _r);                                            \
    ::pancake::detail::report(_ok, __FILE__, __LINE__,                \
        #lhs " == " #rhs,                                             \
        _ok ? "" : ::pancake::detail::to_str(_l, _r),                 \
        false, _tt_failed_, _tt_fatal_);                              \
} while(0)

#define TT_ASSERT_EQ(lhs, rhs) do {                                   \
    auto _l = (lhs); auto _r = (rhs);                                 \
    bool _ok = (_l == _r);                                            \
    ::pancake::detail::report(_ok, __FILE__, __LINE__,                \
        #lhs " == " #rhs,                                             \
        _ok ? "" : ::pancake::detail::to_str(_l, _r),                 \
        true, _tt_failed_, _tt_fatal_);                               \
    if (_tt_fatal_) return;                                           \
} while(0)

//--------------------------------------
// Base Test class (fixture support)
//--------------------------------------
class Test {
public:
    virtual ~Test() = default;
    virtual void SetUp() {}
    virtual void TearDown() {}
    virtual void TestBody() = 0;
    bool RunOne() {
        _tt_failed_ = _tt_fatal_ = false;
        SetUp();
        if (!_tt_fatal_) TestBody();
        TearDown();
        return _tt_failed_;
    }
protected:
    bool _tt_failed_ = false;
    bool _tt_fatal_ = false;
};

//--------------------------------------
// TEST / TEST_F macros
//--------------------------------------
#define TEST(Suite, Name)                                              \
class Suite##_##Name##_Test : public ::pancake::Test {                 \
public: void TestBody() override; };                                   \
static ::pancake::Registrar Suite##_##Name##_registrar(                \
    #Suite, #Name, []()->bool{ Suite##_##Name##_Test t;                \
                                 return t.RunOne(); });                \
void Suite##_##Name##_Test::TestBody()

#define TEST_F(Fixture, Name)                                          \
class Fixture##_##Name##_Test : public Fixture {                       \
public: void TestBody() override; };                                   \
static ::pancake::Registrar Fixture##_##Name##_registrar(              \
    #Fixture, #Name, []()->bool{ Fixture##_##Name##_Test t;            \
                                     return t.RunOne(); });            \
void Fixture##_##Name##_Test::TestBody()

//--------------------------------------
// Runner
//--------------------------------------
inline int RunAllTests(int argc, char** argv) {
#if defined(_WIN32) && TT_ENABLE_COLOR
    {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h != INVALID_HANDLE_VALUE) {
            DWORD m;
            if (GetConsoleMode(h, &m)) SetConsoleMode(h, m | 0x0004);
        }
    }
#endif

    std::string filter = "*";
    for (int i=1; i<argc; ++i) {
        if (std::strncmp(argv[i], "--filter=", 9) == 0)
            filter = argv[i] + 9;
    }
    auto matches = [&](const TestInfo& ti){
        if (filter == "*") return true;
        std::string full = ti.suite + "." + ti.name;
        if (filter == full) return true;
        if (filter.back()=='*')
            return full.rfind(filter.substr(0, filter.size()-1), 0) == 0;
        return false;
    };

    std::cout << TT_CLR_BOLD << "[==========] Running "
              << registry().size() << " tests" << TT_CLR_RESET << "\n";

    int failed = 0, run = 0;
    for (auto& t : registry()) {
        if (!matches(t)) continue;
        ++run;
        std::cout << TT_CLR_CYAN << "[ RUN      ] " << TT_CLR_RESET
                  << t.suite << "." << t.name << "\n";

        bool f = t.body();
        if (f) {
            ++failed;
            std::cout << TT_CLR_RED << "[ FAILED  ] " << TT_CLR_RESET
                      << t.suite << "." << t.name << "\n";
        } else {
            std::cout << TT_CLR_GREEN << "[       OK ] " << TT_CLR_RESET
                      << t.suite << "." << t.name << "\n";
        }
    }

    std::cout << TT_CLR_BOLD << "[==========] "
              << run << " tests ran.  "
              << (failed ? TT_CLR_RED : TT_CLR_GREEN)
              << (failed ? failed : 0) << " failed."
              << TT_CLR_RESET << "\n";

    return failed ? 1 : 0;
}
} // namespace pancake
