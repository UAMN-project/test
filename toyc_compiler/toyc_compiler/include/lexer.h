#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "token.h"

class Lexer {
public:
    explicit Lexer(const std::string& src);

    std::vector<Token> tokenize();

private:
    std::string source;
    size_t start;
    size_t current;
    int line;
    int column;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);

    void skipWhitespace();
    Token identifier();
    Token number();
    Token operatorOrDelimiter();
    Token makeToken(TokenType type, const std::string& lexeme);
};

#endif
