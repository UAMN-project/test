#include "parser.h"
#include "ast.h"
#include "token.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}


// 工具函数实现
const Token &Parser::peek() const {
    if (current >= tokens.size()) throw std::runtime_error("Unexpected EOF");
    return tokens[current];
}

const Token &Parser::advance() {
    if (current >= tokens.size()) throw std::runtime_error("Unexpected EOF");
    return tokens[current++];
}

bool Parser::match(TokenType type) {
    if (current < tokens.size() && tokens[current].type == type) {
        current++;
        return true;
    }
    return false;
}

bool Parser::expect(TokenType type, const char* errMsg) {
    if (match(type)) return true;
    throw std::runtime_error(errMsg);
}

// CompUnit -> FuncDef+
std::vector<std::unique_ptr<FuncDef>> Parser::parseCompUnit() {
    std::vector<std::unique_ptr<FuncDef>> funcs;
    while (current < tokens.size() && tokens[current].type != TokenType::END_OF_FILE) {
        funcs.push_back(parseFuncDef());
    }
    return funcs;
}

// FuncDef -> ("int" | "void") ID "(" (Param ("," Param)*)? ")" Block
std::unique_ptr<FuncDef> Parser::parseFuncDef() {
    std::string retType;
    if (match(TokenType::INT)) {
        retType = "int";
    } else if (match(TokenType::VOID)) {
        retType = "void";
    } else {
        throw std::runtime_error("Expected 'int' or 'void' at function definition");
    }

    if (!match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected function name");

    std::string funcName = tokens[current - 1].lexeme;

    expect(TokenType::LPAREN, "Expected '(' after function name");

    auto func = std::make_unique<FuncDef>(retType, funcName);

    if (!match(TokenType::RPAREN)) {
        do {
            expect(TokenType::INT, "Expected parameter type 'int'");
            if (!match(TokenType::IDENTIFIER))
                throw std::runtime_error("Expected parameter name");
            func->params.emplace_back("int", tokens[current - 1].lexeme);
        } while (match(TokenType::COMMA));

        expect(TokenType::RPAREN, "Expected ')' after parameter list");
    }

    func->body = parseBlock();

    return func;
}

// Block -> "{" Stmt* "}"
std::unique_ptr<Block> Parser::parseBlock() {
    expect(TokenType::LBRACE, "Expected '{' to start block");
    auto block = std::make_unique<Block>();

    while (!match(TokenType::RBRACE)) {
        block->stmts.push_back(parseStmt());
    }

    return block;
}

// Stmt -> various forms
std::unique_ptr<Stmt> Parser::parseStmt() {
    if (match(TokenType::LBRACE)) {
        current--;  // 回退一个token，让parseBlock处理左大括号
        return parseBlock();
    }

    switch (peek().type) {
        case TokenType::INT: return parseVarDecl();
        case TokenType::IF: return parseIfStmt();
        case TokenType::WHILE: return parseWhileStmt();
        case TokenType::BREAK: return parseBreakStmt();
        case TokenType::CONTINUE: return parseContinueStmt();
        case TokenType::RETURN: return parseReturnStmt();
        default: return parseAssignOrExprStmt();
    }
}

std::unique_ptr<Stmt> Parser::parseVarDecl() {
    expect(TokenType::INT, "Expected 'int' for variable declaration");

    if (!match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected variable name");

    std::string name = tokens[current - 1].lexeme;

    expect(TokenType::ASSIGN, "Expected '=' in variable declaration");

    auto initializer = parseExpr();

    expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");

    return std::make_unique<VarDeclStmt>("int", name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::parseIfStmt() {
    expect(TokenType::IF, "Expected 'if'");

    expect(TokenType::LPAREN, "Expected '(' after if");

    auto cond = parseExpr();

    expect(TokenType::RPAREN, "Expected ')' after if condition");

    auto thenBlock = parseStmt();

    std::unique_ptr<Block> thenBlk = nullptr;
    if (auto blk = dynamic_cast<Block*>(thenBlock.get())) {
        thenBlk = std::unique_ptr<Block>(static_cast<Block*>(thenBlock.release()));
    } else {
        thenBlk = std::make_unique<Block>();
        thenBlk->stmts.push_back(std::move(thenBlock));
    }

    std::unique_ptr<Block> elseBlk = nullptr;
    if (match(TokenType::ELSE)) {
        auto elseStmt = parseStmt();
        if (auto blk = dynamic_cast<Block*>(elseStmt.get())) {
            elseBlk = std::unique_ptr<Block>(static_cast<Block*>(elseStmt.release()));
        } else {
            elseBlk = std::make_unique<Block>();
            elseBlk->stmts.push_back(std::move(elseStmt));
        }
    }

    return std::make_unique<IfStmt>(std::move(cond), std::move(thenBlk), std::move(elseBlk));
}

std::unique_ptr<Stmt> Parser::parseWhileStmt() {
    expect(TokenType::WHILE, "Expected 'while'");

    expect(TokenType::LPAREN, "Expected '(' after while");

    auto cond = parseExpr();

    expect(TokenType::RPAREN, "Expected ')' after while condition");

    auto bodyStmt = parseStmt();

    std::unique_ptr<Block> bodyBlk = nullptr;
    if (auto blk = dynamic_cast<Block*>(bodyStmt.get())) {
        bodyBlk = std::unique_ptr<Block>(static_cast<Block*>(bodyStmt.release()));
    } else {
        bodyBlk = std::make_unique<Block>();
        bodyBlk->stmts.push_back(std::move(bodyStmt));
    }

    return std::make_unique<WhileStmt>(std::move(cond), std::move(bodyBlk));
}

std::unique_ptr<Stmt> Parser::parseBreakStmt() {
    expect(TokenType::BREAK, "Expected 'break'");
    expect(TokenType::SEMICOLON, "Expected ';' after break");
    return std::make_unique<BreakStmt>();
}

std::unique_ptr<Stmt> Parser::parseContinueStmt() {
    expect(TokenType::CONTINUE, "Expected 'continue'");
    expect(TokenType::SEMICOLON, "Expected ';' after continue");
    return std::make_unique<ContinueStmt>();
}

std::unique_ptr<Stmt> Parser::parseReturnStmt() {
    expect(TokenType::RETURN, "Expected 'return'");
    if (peek().type != TokenType::SEMICOLON) {
        auto expr = parseExpr();
        expect(TokenType::SEMICOLON, "Expected ';' after return expression");
        auto retStmt = std::make_unique<ReturnStmt>();
        retStmt->expr = std::move(expr);
        return retStmt;
    } else {
        expect(TokenType::SEMICOLON, "Expected ';' after return");
        return std::make_unique<ReturnStmt>();
    }
}

std::unique_ptr<Stmt> Parser::parseAssignOrExprStmt() {
    if (match(TokenType::IDENTIFIER)) {
        std::string name = tokens[current - 1].lexeme;

        if (match(TokenType::ASSIGN)) {
            auto value = parseExpr();
            expect(TokenType::SEMICOLON, "Expected ';' after assignment");
            return std::make_unique<AssignStmt>(name, std::move(value));
        } else {
            // 不是赋值，回退以解析表达式
            current--;
            auto expr = parseExpr();
            expect(TokenType::SEMICOLON, "Expected ';' after expression");
            return std::make_unique<ExprStmt>(std::move(expr));
        }
    } else {
        auto expr = parseExpr();
        expect(TokenType::SEMICOLON, "Expected ';' after expression");
        return std::make_unique<ExprStmt>(std::move(expr));
    }
}

// 递归下降表达式解析，支持优先级

std::unique_ptr<Expr> Parser::parseExpr() {
    return parseLOrExpr();
}

std::unique_ptr<Expr> Parser::parseLOrExpr() {
    auto lhs = parseLAndExpr();
    while (match(TokenType::LOGICAL_OR)) {
        std::string op = tokens[current - 1].lexeme;
        auto rhs = parseLAndExpr();
        lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parseLAndExpr() {
    auto lhs = parseRelExpr();
    while (match(TokenType::LOGICAL_AND)) {
        std::string op = tokens[current - 1].lexeme;
        auto rhs = parseRelExpr();
        lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parseRelExpr() {
    auto lhs = parseAddExpr();
    while (true) {
        if (match(TokenType::LESS)) {
            auto rhs = parseAddExpr();
            lhs = std::make_unique<BinaryExpr>("<", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::GREATER)) {
            auto rhs = parseAddExpr();
            lhs = std::make_unique<BinaryExpr>(">", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::LESS_EQUAL)) {
            auto rhs = parseAddExpr();
            lhs = std::make_unique<BinaryExpr>("<=", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::GREATER_EQUAL)) {
            auto rhs = parseAddExpr();
            lhs = std::make_unique<BinaryExpr>(">=", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::EQUAL)) {
            auto rhs = parseAddExpr();
            lhs = std::make_unique<BinaryExpr>("==", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::NOT_EQUAL)) {
            auto rhs = parseAddExpr();
            lhs = std::make_unique<BinaryExpr>("!=", std::move(lhs), std::move(rhs));
        } else {
            break;
        }
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parseAddExpr() {
    auto lhs = parseMulExpr();
    while (true) {
        if (match(TokenType::PLUS)) {
            auto rhs = parseMulExpr();
            lhs = std::make_unique<BinaryExpr>("+", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::MINUS)) {
            auto rhs = parseMulExpr();
            lhs = std::make_unique<BinaryExpr>("-", std::move(lhs), std::move(rhs));
        } else {
            break;
        }
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parseMulExpr() {
    auto lhs = parseUnaryExpr();
    while (true) {
        if (match(TokenType::MULTIPLY)) {
            auto rhs = parseUnaryExpr();
            lhs = std::make_unique<BinaryExpr>("*", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::DIVIDE)) {
            auto rhs = parseUnaryExpr();
            lhs = std::make_unique<BinaryExpr>("/", std::move(lhs), std::move(rhs));
        } else if (match(TokenType::MODULO)) {
            auto rhs = parseUnaryExpr();
            lhs = std::make_unique<BinaryExpr>("%", std::move(lhs), std::move(rhs));
        } else {
            break;
        }
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parseUnaryExpr() {
    if (match(TokenType::PLUS)) {
        return std::make_unique<UnaryExpr>("+", parseUnaryExpr());
    } else if (match(TokenType::MINUS)) {
        return std::make_unique<UnaryExpr>("-", parseUnaryExpr());
    } else if (match(TokenType::NOT)) {
        return std::make_unique<UnaryExpr>("!", parseUnaryExpr());
    }
    return parsePrimaryExpr();
}

std::unique_ptr<Expr> Parser::parsePrimaryExpr() {
    if (match(TokenType::IDENTIFIER)) {
        std::string id = tokens[current - 1].lexeme;

        if (match(TokenType::LPAREN)) {
            auto callExpr = std::make_unique<CallExpr>(id);

            if (!match(TokenType::RPAREN)) {
                do {
                    callExpr->args.push_back(parseExpr());
                } while (match(TokenType::COMMA));
                expect(TokenType::RPAREN, "Expected ')' after function call arguments");
            }
            return callExpr;
        }

        return std::make_unique<VarExpr>(id);
    } else if (match(TokenType::NUMBER)) {
        int val = std::stoi(tokens[current - 1].lexeme);
        return std::make_unique<NumberExpr>(val);
    } else if (match(TokenType::LPAREN)) {
        auto expr = parseExpr();
        expect(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    throw std::runtime_error("Expected primary expression");
}
