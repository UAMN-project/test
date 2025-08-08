#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    INT, VOID, IF, ELSE, WHILE, RETURN, BREAK, CONTINUE,
    IDENTIFIER, NUMBER,
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    ASSIGN, EQUAL, NOT_EQUAL, LOGICAL_AND, LOGICAL_OR, NOT,
    LPAREN, RPAREN, LBRACE, RBRACE, COMMA, SEMICOLON,
    COMMENT_LINE, COMMENT_BLOCK,  // 添加注释类型
    END_OF_FILE, UNKNOWN
    // 其他需要的 TokenType...
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line = 0;
    int column = 0;

    Token() = default;

    Token(TokenType t, const char* lex)
        : type(t), lexeme(lex) {}

    Token(TokenType t, const std::string& lex, int l, int c)
        : type(t), lexeme(lex), line(l), column(c) {}
};

#endif // TOKEN_H
