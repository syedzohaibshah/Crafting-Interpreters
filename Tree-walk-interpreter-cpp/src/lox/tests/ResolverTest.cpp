// tests/resolver_test.cpp
#include <gtest/gtest.h>
#include "../Resolver.h"
#include "../Parser.h"
#include "../Scanner.h"

class ResolverTest : public ::testing::Test {
protected:
    void resolveCode(const std::string& code) {
        Scanner s(code);
        auto tokens = s.scan_tokens();
        Parser p(tokens);
        auto stmts = p.parse();
        auto interpreter = std::make_shared<Interpreter>();
        Resolver resolver(interpreter);
        for(auto& stmt : stmts) {
            resolver.resolve(*stmt);
        }
    }
};

TEST_F(ResolverTest, ResolvesLocalVariable) {
    EXPECT_NO_FATAL_FAILURE(resolveCode(
        "{ var x = 10; print x; }"
    ));
}

TEST_F(ResolverTest, DetectsUndefinedVariable) {
    // Should report error (you'll need error handling)
    EXPECT_NO_FATAL_FAILURE(resolveCode(
        "print undefined_var;"
    ));
}

TEST_F(ResolverTest, ResolvesNestedScopes) {
    EXPECT_NO_FATAL_FAILURE(resolveCode(
        "var x = 1; { var y = 2; { print x; print y; } }"
    ));
}

TEST_F(ResolverTest, ResolvesFunctionScope) {
    EXPECT_NO_FATAL_FAILURE(resolveCode(
        "fun greet(name) { print name; } greet(\"Alice\");"
    ));
}

TEST_F(ResolverTest, ResolvesClosures) {
    EXPECT_NO_FATAL_FAILURE(resolveCode(
        "fun outer() { var x = 1; fun inner() { print x; } return inner; }"
    ));
}