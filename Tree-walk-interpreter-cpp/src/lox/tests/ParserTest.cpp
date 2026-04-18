// tests/parser_test.cpp
#include <gtest/gtest.h>
#include "../Parser.h"
#include "../Scanner.h"

class ParserTest : public ::testing::Test {
protected:
    std::vector<Token> scan(const std::string& code) {
        Scanner s(code);
        return s.scan_tokens();
    }
};

TEST_F(ParserTest, ParsesLiterals) {
    auto tokens = scan("123");
    Parser p(tokens);
    auto expr = p.expression();
    EXPECT_NE(expr, nullptr);
}

TEST_F(ParserTest, ParsesBinaryExpression) {
    auto tokens = scan("1 + 2");
    Parser p(tokens);
    auto expr = p.expression();
    EXPECT_NE(expr, nullptr);
}

TEST_F(ParserTest, ParsesVarDeclaration) {
    auto tokens = scan("var x = 10;");
    Parser p(tokens);
    auto stmts = p.parse();
    EXPECT_EQ(stmts.size(), 1);
}

TEST_F(ParserTest, ParsesIfStatement) {
    auto tokens = scan("if (x > 5) { print(x); }");
    Parser p(tokens);
    auto stmts = p.parse();
    EXPECT_EQ(stmts.size(), 1);
}

TEST_F(ParserTest, ParsesFunction) {
    auto tokens = scan("fun add(a, b) { return a + b; }");
    Parser p(tokens);
    auto stmts = p.parse();
    EXPECT_EQ(stmts.size(), 1);
}

TEST_F(ParserTest, ParsesClass) {
    auto tokens = scan("class Dog { bark() { print(\"woof\"); } }");
    Parser p(tokens);
    auto stmts = p.parse();
    EXPECT_EQ(stmts.size(), 1);
}