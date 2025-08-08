#pragma once
#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens);
    
    // 解析整个程序单元，返回函数定义列表
    std::vector<std::unique_ptr<FuncDef>> parseCompUnit();

private:
    // 表达式相关
    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Expr> parseLOrExpr();
    std::unique_ptr<Expr> parseLAndExpr();
    std::unique_ptr<Expr> parseRelExpr();
    std::unique_ptr<Expr> parseAddExpr();
    std::unique_ptr<Expr> parseMulExpr();
    std::unique_ptr<Expr> parseUnaryExpr();
    std::unique_ptr<Expr> parsePrimaryExpr();

    // 语句相关，注意部分语句构造需要参数传递
    std::unique_ptr<Stmt> parseStmt();
    std::unique_ptr<Stmt> parseVarDecl();      // 构造 VarDeclStmt，需要类型、变量名和初始化表达式
    std::unique_ptr<Stmt> parseIfStmt();       // 构造 IfStmt，需要条件表达式，then块，else块（可选）
    std::unique_ptr<Stmt> parseWhileStmt();    // 构造 WhileStmt，需要条件表达式和循环块
    std::unique_ptr<Stmt> parseBreakStmt();
    std::unique_ptr<Stmt> parseContinueStmt();
    std::unique_ptr<Stmt> parseReturnStmt();
    std::unique_ptr<Stmt> parseAssignOrExprStmt(); // 构造 AssignStmt 需要变量名和赋值表达式
    std::unique_ptr<Block> parseBlock();

    // 函数定义，构造 FuncDef 需要函数名，参数列表，函数体块
    std::unique_ptr<FuncDef> parseFuncDef();

    // 工具函数
    const Token &peek() const;
    const Token &advance();
    bool match(TokenType type);
    bool expect(TokenType type, const char *msg);

    // 异常抛出辅助函数（可以在实现中用来抛语法错误）
    [[noreturn]] void error(const char *msg) const;

private:
    const std::vector<Token> &tokens;
    size_t current = 0;
};
