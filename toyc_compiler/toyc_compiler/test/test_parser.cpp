// test_parser.cpp
#include "parser.h"
#include "token.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

void testSimpleFunction() {
    // 测试简单函数：int main() { return 42; }
    std::vector<Token> tokens = {
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "main"},
        {TokenType::LPAREN, "("},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::RETURN, "return"},
        {TokenType::NUMBER, "42"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::END_OF_FILE, ""}
    };

    Parser parser(tokens);
    auto funcs = parser.parseCompUnit();
    assert(funcs.size() == 1);
    std::cout << "Simple function test passed\n";
}

void testIfElseStatement() {
    // 测试if-else语句：
    // int test() {
    //     int x = 10;
    //     if (x > 5) { return 1; } else { return 0; }
    // }
    std::vector<Token> tokens = {
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "test"},
        {TokenType::LPAREN, "("},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "x"},
        {TokenType::ASSIGN, "="},
        {TokenType::NUMBER, "10"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::IF, "if"},
        {TokenType::LPAREN, "("},
        {TokenType::IDENTIFIER, "x"},
        {TokenType::GREATER, ">"},
        {TokenType::NUMBER, "5"},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::RETURN, "return"},
        {TokenType::NUMBER, "1"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::ELSE, "else"},
        {TokenType::LBRACE, "{"},
        {TokenType::RETURN, "return"},
        {TokenType::NUMBER, "0"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::RBRACE, "}"},
        {TokenType::END_OF_FILE, ""}
    };

    Parser parser(tokens);
    auto funcs = parser.parseCompUnit();
    assert(funcs.size() == 1);
    std::cout << "If-else statement test passed\n";
}

void testWhileLoop() {
    // 测试while循环：
    // int loop() {
    //     int i = 0;
    //     while (i < 10) {
    //         i = i + 1;
    //     }
    //     return i;
    // }
    std::vector<Token> tokens = {
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "loop"},
        {TokenType::LPAREN, "("},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "i"},
        {TokenType::ASSIGN, "="},
        {TokenType::NUMBER, "0"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::WHILE, "while"},
        {TokenType::LPAREN, "("},
        {TokenType::IDENTIFIER, "i"},
        {TokenType::LESS, "<"},
        {TokenType::NUMBER, "10"},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::IDENTIFIER, "i"},
        {TokenType::ASSIGN, "="},
        {TokenType::IDENTIFIER, "i"},
        {TokenType::PLUS, "+"},
        {TokenType::NUMBER, "1"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::RETURN, "return"},
        {TokenType::IDENTIFIER, "i"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::END_OF_FILE, ""}
    };

    Parser parser(tokens);
    auto funcs = parser.parseCompUnit();
    assert(funcs.size() == 1);
    std::cout << "While loop test passed\n";
}

void testFunctionWithParams() {
    // 测试带参数的函数：
    // int add(int a, int b) {
    //     return a + b;
    // }
    std::vector<Token> tokens = {
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "add"},
        {TokenType::LPAREN, "("},
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "a"},
        {TokenType::COMMA, ","},
        {TokenType::INT, "int"},
        {TokenType::IDENTIFIER, "b"},
        {TokenType::RPAREN, ")"},
        {TokenType::LBRACE, "{"},
        {TokenType::RETURN, "return"},
        {TokenType::IDENTIFIER, "a"},
        {TokenType::PLUS, "+"},
        {TokenType::IDENTIFIER, "b"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::RBRACE, "}"},
        {TokenType::END_OF_FILE, ""}
    };

    Parser parser(tokens);
    auto funcs = parser.parseCompUnit();
    assert(funcs.size() == 1);
    std::cout << "Function with parameters test passed\n";
}

int main() {
    try {
        testSimpleFunction();
        testIfElseStatement();
        testWhileLoop();
        testFunctionWithParams();
        std::cout << "\nAll parser tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "Parser test failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
