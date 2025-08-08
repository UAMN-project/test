#include "lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& src)
    : source(src), start(0), current(0), line(1), column(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();
        start = current;

        if (isAtEnd()) break;

        char c = advance();

        if (std::isalpha(c) || c == '_') {
            tokens.push_back(identifier());
        } else if (std::isdigit(c)) {
            tokens.push_back(number());
        } else {
            current--; column--; // put back for operatorOrDelimiter
            tokens.push_back(operatorOrDelimiter());
        }
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

bool Lexer::isAtEnd() const {
    return current >= source.size();
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    current++;
    column++;
    return source[current - 1];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.size()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                line++;
                column = 1;
                break;
            case '/':
                if (peekNext() == '/') {
                    // 跳过单行注释
                    advance(); advance(); // 跳过 //
                    while (!isAtEnd() && peek() != '\n') advance();
                } else if (peekNext() == '*') {
                    // 跳过多行注释
                    advance(); advance(); // 跳过 /*
                    while (!isAtEnd()) {
                        if (peek() == '*' && peekNext() == '/') {
                            advance(); advance(); // 跳过 */
                            break;
                        }
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (isAtEnd()) {
                        throw std::runtime_error("Unterminated block comment at line " + 
                            std::to_string(line));
                    }
                } else {
                    return; // 这是除法运算符
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();

    std::string lexeme = source.substr(start, current - start);

    TokenType type;
    if (lexeme == "int") type = TokenType::INT;
    else if (lexeme == "void") type = TokenType::VOID;
    else if (lexeme == "if") type = TokenType::IF;
    else if (lexeme == "else") type = TokenType::ELSE;
    else if (lexeme == "while") type = TokenType::WHILE;
    else if (lexeme == "return") type = TokenType::RETURN;
    else if (lexeme == "break") type = TokenType::BREAK;
    else if (lexeme == "continue") type = TokenType::CONTINUE;
    else type = TokenType::IDENTIFIER;

    return makeToken(type, lexeme);
}

Token Lexer::number() {
    while (std::isdigit(peek())) advance();
    std::string lexeme = source.substr(start, current - start);
    return makeToken(TokenType::NUMBER, lexeme);
}

Token Lexer::operatorOrDelimiter() {
    char c = advance();
    std::string lexeme(1, c);

    auto makeError = [this](const std::string& msg) {
        throw std::runtime_error("Lexical error at line " + std::to_string(line) + 
            ", column " + std::to_string(column) + ": " + msg);
    };

    switch (c) {
        case '+':
            if (match('+')) makeError("Unexpected '++', increment operator not supported");
            return makeToken(TokenType::PLUS, lexeme);
        case '-':
            if (match('-')) makeError("Unexpected '--', decrement operator not supported");
            return makeToken(TokenType::MINUS, lexeme);
        case '*': return makeToken(TokenType::MULTIPLY, lexeme);
        case '/':
            if (match('/')) {
                // 处理单行注释
                while (!isAtEnd() && peek() != '\n') advance();
                std::string comment = source.substr(start, current - start);
                return makeToken(TokenType::COMMENT_LINE, comment);
            } else if (match('*')) {
                // 处理多行注释
                int startLine = line;
                while (!isAtEnd()) {
                    if (peek() == '*' && peekNext() == '/') {
                        advance(); advance(); // 跳过 */
                        std::string comment = source.substr(start, current - start);
                        return makeToken(TokenType::COMMENT_BLOCK, comment);
                    }
                    if (peek() == '\n') {
                        line++;
                        column = 1;
                    }
                    advance();
                }
                throw std::runtime_error("Unterminated block comment starting at line " + 
                    std::to_string(startLine));
            }
            return makeToken(TokenType::DIVIDE, lexeme);
        case '%': return makeToken(TokenType::MODULO, lexeme);
        case '<':
            if (match('=')) return makeToken(TokenType::LESS_EQUAL, "<=");
            return makeToken(TokenType::LESS, "<");
        case '>':
            if (match('=')) return makeToken(TokenType::GREATER_EQUAL, ">=");
            return makeToken(TokenType::GREATER, ">");
        case '=':
            if (match('=')) return makeToken(TokenType::EQUAL, "==");
            return makeToken(TokenType::ASSIGN, "=");
        case '!':
            if (match('=')) return makeToken(TokenType::NOT_EQUAL, "!=");
            return makeToken(TokenType::NOT, "!");
        case '&':
            if (match('&')) return makeToken(TokenType::LOGICAL_AND, "&&");
            break;
        case '|':
            if (match('|')) return makeToken(TokenType::LOGICAL_OR, "||");
            break;
        case '(': return makeToken(TokenType::LPAREN, lexeme);
        case ')': return makeToken(TokenType::RPAREN, lexeme);
        case '{': return makeToken(TokenType::LBRACE, lexeme);
        case '}': return makeToken(TokenType::RBRACE, lexeme);
        case ',': return makeToken(TokenType::COMMA, lexeme);
        case ';': return makeToken(TokenType::SEMICOLON, lexeme);
    }

    return makeToken(TokenType::UNKNOWN, lexeme);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, line, column - (int)lexeme.size() + 1);
}