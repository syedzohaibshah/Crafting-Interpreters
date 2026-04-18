
#include <gtest/gtest.h>
#include "../Scanner.h"

class ScannerComprehensiveTest : public ::testing::Test {
protected:
    std::vector<Token> scan(const std::string& code) {
        Scanner s(code);
        return s.scan_tokens();
    }
};

// ===== LITERALS =====
TEST_F(ScannerComprehensiveTest, ScansNumbers) {
    auto tokens = scan("123 45.67 0 999");
    EXPECT_EQ(tokens.size(), 5); // 4 numbers + EOF
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
}

TEST_F(ScannerComprehensiveTest, ScansStrings) {
    auto tokens = scan("\"hello\" \"world\"");
    EXPECT_EQ(tokens.size(), 3); // 2 strings + EOF
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[1].type, TokenType::STRING);
}

TEST_F(ScannerComprehensiveTest, ScansStringWithEscapes) {
    auto tokens = scan("\"hello\\nworld\"");
    EXPECT_GT(tokens.size(), 0);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
}

TEST_F(ScannerComprehensiveTest, ScansKeywords) {
    auto tokens = scan("var fun if else while for return class print true false nil and or");
    EXPECT_EQ(tokens[0].type, TokenType::VAR);
    EXPECT_EQ(tokens[1].type, TokenType::FUN);
    EXPECT_EQ(tokens[2].type, TokenType::IF);
    EXPECT_EQ(tokens[3].type, TokenType::ELSE);
    EXPECT_EQ(tokens[4].type, TokenType::WHILE);
    EXPECT_EQ(tokens[5].type, TokenType::FOR);
    EXPECT_EQ(tokens[6].type, TokenType::RETURN);
    EXPECT_EQ(tokens[7].type, TokenType::CLASS);
    EXPECT_EQ(tokens[8].type, TokenType::PRINT);
    EXPECT_EQ(tokens[9].type, TokenType::TRUE);
    EXPECT_EQ(tokens[10].type, TokenType::FALSE);
    EXPECT_EQ(tokens[11].type, TokenType::NIL);
    EXPECT_EQ(tokens[12].type, TokenType::AND);
    EXPECT_EQ(tokens[13].type, TokenType::OR);
}

TEST_F(ScannerComprehensiveTest, ScansIdentifiers) {
    auto tokens = scan("x myVar _private camelCase");
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[3].type, TokenType::IDENTIFIER);
}

// ===== SINGLE CHAR OPERATORS =====
TEST_F(ScannerComprehensiveTest, ScansSingleCharOperators) {
    auto tokens = scan("+ - * / ( ) { } , . ; :");
    EXPECT_EQ(tokens[0].type, TokenType::PLUS);
    EXPECT_EQ(tokens[1].type, TokenType::MINUS);
    EXPECT_EQ(tokens[2].type, TokenType::STAR);
    EXPECT_EQ(tokens[3].type, TokenType::SLASH);
    EXPECT_EQ(tokens[4].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[6].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[7].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[8].type, TokenType::COMMA);
    EXPECT_EQ(tokens[9].type, TokenType::DOT);
    EXPECT_EQ(tokens[10].type, TokenType::SEMICOLON);
}

// ===== MULTI CHAR OPERATORS =====
TEST_F(ScannerComprehensiveTest, ScansMultiCharOperators) {
    auto tokens = scan("! != = == < <= > >=");
    EXPECT_EQ(tokens[0].type, TokenType::BANG);
    EXPECT_EQ(tokens[1].type, TokenType::BANG_EQUAL);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[4].type, TokenType::LESS);
    EXPECT_EQ(tokens[5].type, TokenType::LESS_EQUAL);
    EXPECT_EQ(tokens[6].type, TokenType::GREATER);
    EXPECT_EQ(tokens[7].type, TokenType::GREATER_EQUAL);
}

// ===== COMMENTS =====
TEST_F(ScannerComprehensiveTest, SkipsComments) {
    auto tokens = scan("123 // this is a comment\n456");
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens.size(), 3); // 2 numbers + EOF
}

TEST_F(ScannerComprehensiveTest, IgnoresCommentToEndOfLine) {
    auto tokens = scan("var x = 10; // comment\nvar y = 20;");
    int var_count = 0;
    for(const auto& t : tokens) {
        if(t.type == TokenType::VAR) var_count++;
    }
    EXPECT_EQ(var_count, 2);
}

// ===== WHITESPACE =====
TEST_F(ScannerComprehensiveTest, IgnoresWhitespace) {
    auto tokens = scan("   123   456   ");
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens.size(), 3); // 2 numbers + EOF
}

// ===== LINE TRACKING =====
TEST_F(ScannerComprehensiveTest, TracksLineNumbers) {
    auto tokens = scan("var x;\nvar y;\nvar z;");
    EXPECT_EQ(tokens[0].line, 1); // var
    EXPECT_EQ(tokens[3].line, 2); // var
    EXPECT_EQ(tokens[6].line, 3); // var
}

// ===== ERROR CASES =====
TEST_F(ScannerComprehensiveTest, HandlesBareSlash) {
    // Single slash not followed by another slash is division
    auto tokens = scan("10 / 2");
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].type, TokenType::SLASH);
    EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
}

TEST_F(ScannerComprehensiveTest, ScansCompleteExpression) {
    auto tokens = scan("var x = 10 + 20;");
    EXPECT_EQ(tokens[0].type, TokenType::VAR);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4].type, TokenType::PLUS);
    EXPECT_EQ(tokens[5].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[6].type, TokenType::SEMICOLON);
}