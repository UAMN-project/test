// === ast.h ===
#pragma once
#include <string>
#include <vector>
#include <memory>

// 基类
struct ASTNode {
    virtual ~ASTNode() = default;
};

// 基类
struct Expr : ASTNode {};
struct Stmt : ASTNode {};

// Program
struct Program : ASTNode {
    std::vector<std::unique_ptr<class FuncDef>> functions;
};

// Function Definition
struct FuncDef : ASTNode {
    std::string retType, name;
    struct Param {
        std::string type, name;
        Param(const std::string& t, const std::string& n) : type(t), name(n) {}

        Param() = default;
        Param(const Param&) = default;
        Param(Param&&) = default;
        Param& operator=(const Param&) = default;
        Param& operator=(Param&&) = default;
    };
    std::vector<Param> params;
    std::unique_ptr<class Block> body;

    FuncDef(const std::string &rt, const std::string &n) : retType(rt), name(n) {}
};

// Block
struct Block : Stmt {
    std::vector<std::unique_ptr<Stmt>> stmts;
};

// Return Statement
struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> expr;
};

// Variable Declaration Statement
struct VarDeclStmt : Stmt {
    std::string varType;
    std::string name;
    std::unique_ptr<Expr> initializer;

    VarDeclStmt(const std::string &vt, const std::string &n, std::unique_ptr<Expr> init)
        : varType(vt), name(n), initializer(std::move(init)) {}
};

// Expressions
struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
};

struct NumberExpr : Expr {
    int value;
    NumberExpr(int v) : value(v) {}
};

struct UnaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> operand;
    UnaryExpr(std::string o, std::unique_ptr<Expr> e)
        : op(std::move(o)), operand(std::move(e)) {}
};

struct BinaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> lhs, rhs;
    BinaryExpr(std::string o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(std::move(o)), lhs(std::move(l)), rhs(std::move(r)) {}
};

struct CallExpr : Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
    CallExpr(std::string c) : callee(std::move(c)) {}
};
// 如果有这些语句，就需要这样补

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;

    AssignStmt(const std::string &n, std::unique_ptr<Expr> v)
        : name(n), value(std::move(v)) {}
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;

    ExprStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Block> thenBlock;
    std::unique_ptr<Block> elseBlock;

    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Block> thenBlk, std::unique_ptr<Block> elseBlk)
        : condition(std::move(cond)), thenBlock(std::move(thenBlk)), elseBlock(std::move(elseBlk)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Block> body;

    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Block> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct BreakStmt : Stmt {};

struct ContinueStmt : Stmt {};
