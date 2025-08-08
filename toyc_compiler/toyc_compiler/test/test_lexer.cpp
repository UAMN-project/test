#include "lexer.h"
#include <iostream>
#include <cassert>
#include <sstream>

void printToken(const Token& token) {
    std::cout << "[" << token.line << ":" << token.column << "] ";
    std::cout << "Type: ";

    switch (token.type) {
        case TokenType::INT: std::cout << "INT"; break;
        case TokenType::VOID: std::cout << "VOID"; break;
        case TokenType::IF: std::cout << "IF"; break;
        case TokenType::ELSE: std::cout << "ELSE"; break;
        case TokenType::WHILE: std::cout << "WHILE"; break;
        case TokenType::RETURN: std::cout << "RETURN"; break;
        case TokenType::BREAK: std::cout << "BREAK"; break;
        case TokenType::CONTINUE: std::cout << "CONTINUE"; break;
        case TokenType::IDENTIFIER: std::cout << "IDENTIFIER"; break;
        case TokenType::NUMBER: std::cout << "NUMBER"; break;
        case TokenType::PLUS: std::cout << "PLUS"; break;
        case TokenType::MINUS: std::cout << "MINUS"; break;
        case TokenType::MULTIPLY: std::cout << "MULTIPLY"; break;
        case TokenType::DIVIDE: std::cout << "DIVIDE"; break;
        case TokenType::MODULO: std::cout << "MODULO"; break;
        case TokenType::LESS: std::cout << "LESS"; break;
        case TokenType::GREATER: std::cout << "GREATER"; break;
        case TokenType::LESS_EQUAL: std::cout << "LESS_EQUAL"; break;
        case TokenType::GREATER_EQUAL: std::cout << "GREATER_EQUAL"; break;
        case TokenType::ASSIGN: std::cout << "ASSIGN"; break;
        case TokenType::EQUAL: std::cout << "EQUAL"; break;
        case TokenType::NOT_EQUAL: std::cout << "NOT_EQUAL"; break;
        case TokenType::LOGICAL_AND: std::cout << "LOGICAL_AND"; break;
        case TokenType::LOGICAL_OR: std::cout << "LOGICAL_OR"; break;
        case TokenType::NOT: std::cout << "NOT"; break;
        case TokenType::LPAREN: std::cout << "LPAREN"; break;
        case TokenType::RPAREN: std::cout << "RPAREN"; break;
        case TokenType::LBRACE: std::cout << "LBRACE"; break;
        case TokenType::RBRACE: std::cout << "RBRACE"; break;
        case TokenType::SEMICOLON: std::cout << "SEMICOLON"; break;
        case TokenType::COMMA: std::cout << "COMMA"; break;
        case TokenType::COMMENT_LINE: std::cout << "COMMENT_LINE"; break;
        case TokenType::COMMENT_BLOCK: std::cout << "COMMENT_BLOCK"; break;
        case TokenType::END_OF_FILE: std::cout << "EOF"; break;
        case TokenType::UNKNOWN: std::cout << "UNKNOWN"; break;
        default: std::cout << "UNEXPECTED"; break;
    }

    std::cout << ", Lexeme: \"" << token.lexeme << "\"\n";
}

void testBasicTokenization() {
    std::string sampleCode = R"(
        int main() {
            int a = 42;
            if (a > 0) {
                a = a + 1;
            } else {
                a = a - 1;
            }
            return a;
        }
    )";

    Lexer lexer(sampleCode);
    auto tokens = lexer.tokenize();
    std::cout << "\nBasic Tokenization Test:\n";
    for (const auto& token : tokens) {
        printToken(token);
    }
}

void testComments() {
    std::string sampleCode = R"(
        // This is a line comment
        int x = 10; /* This is a block comment */
        int y = 20;
        /*
         * Multi-line
         * block comment
         */
        int z = 30;
    )";

    std::cout << "\nComment Test:\n";
    Lexer lexer(sampleCode);
    auto tokens = lexer.tokenize();
    for (const auto& token : tokens) {
        printToken(token);
    }
}

void testErrorHandling() {
    std::vector<std::string> errorCases = {
        "int x = @;",           // 非法字符
        "int y = ++1;",         // 不支持的递增运算符
        "int z = --1;",         // 不支持的递减运算符
        "/* Unterminated comment" // 未终止的块注释
    };

    std::cout << "\nError Handling Test:\n";
    for (const auto& code : errorCases) {
        try {
            Lexer lexer(code);
            auto tokens = lexer.tokenize();
            std::cout << "Unexpected success for: " << code << std::endl;
        } catch (const std::exception& ex) {
            std::cout << "Expected error caught: " << ex.what() << std::endl;
        }
    }
}

int main() {
    try {
        testBasicTokenization();
        testComments();
        testErrorHandling();
        std::cout << "\nAll tests completed successfully!\n";
    } catch (const std::exception& ex) {
        std::cerr << "Test failed: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
