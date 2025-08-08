#include "semantic.h"
#include <cassert>
#include <memory>

void test_simple_function() {
    SemanticAnalyzer analyzer;
    std::vector<std::unique_ptr<FuncDef>> funcs;
    
    auto func = std::make_unique<FuncDef>("main", Type::Int);
    func->body = std::make_unique<Block>();
    
    funcs.push_back(std::move(func));
    analyzer.analyze(funcs);
}

void test_undeclared_variable() {
    SemanticAnalyzer analyzer;
    std::vector<std::unique_ptr<FuncDef>> funcs;
    
    auto func = std::make_unique<FuncDef>("main", Type::Int);
    func->body = std::make_unique<Block>();
    
    // 使用未声明的变量
    auto varExpr = std::make_unique<VarExpr>("x");
    auto exprStmt = std::make_unique<ExprStmt>(std::move(varExpr));
    func->body->stmts.push_back(std::move(exprStmt));
    
    funcs.push_back(std::move(func));
    analyzer.analyze(funcs);
}

void test_duplicate_variable() {
    SemanticAnalyzer analyzer;
    std::vector<std::unique_ptr<FuncDef>> funcs;
    
    auto func = std::make_unique<FuncDef>("main", Type::Int);
    func->body = std::make_unique<Block>();
    
    // 声明变量x
    auto declStmt1 = std::make_unique<VarDeclStmt>("x", Type::Int, std::make_unique<NumberExpr>(1));
    func->body->stmts.push_back(std::move(declStmt1));
    
    // 重复声明变量x
    auto declStmt2 = std::make_unique<VarDeclStmt>("x", Type::Int, std::make_unique<NumberExpr>(2));
    func->body->stmts.push_back(std::move(declStmt2));
    
    funcs.push_back(std::move(func));
    analyzer.analyze(funcs);
}

void test_invalid_return_type() {
    SemanticAnalyzer analyzer;
    std::vector<std::unique_ptr<FuncDef>> funcs;
    
    auto func = std::make_unique<FuncDef>("main", Type::Void);
    func->body = std::make_unique<Block>();
    
    // void函数中返回整数值
    auto returnStmt = std::make_unique<ReturnStmt>(std::make_unique<NumberExpr>(1));
    func->body->stmts.push_back(std::move(returnStmt));
    
    funcs.push_back(std::move(func));
    analyzer.analyze(funcs);
}

void test_break_continue_outside_loop() {
    SemanticAnalyzer analyzer;
    std::vector<std::unique_ptr<FuncDef>> funcs;
    
    auto func = std::make_unique<FuncDef>("main", Type::Int);
    func->body = std::make_unique<Block>();
    
    // 在循环外使用break
    auto breakStmt = std::make_unique<BreakStmt>();
    func->body->stmts.push_back(std::move(breakStmt));
    
    // 在循环外使用continue
    auto continueStmt = std::make_unique<ContinueStmt>();
    func->body->stmts.push_back(std::move(continueStmt));
    
    funcs.push_back(std::move(func));
    analyzer.analyze(funcs);
}

void test_function_call_args() {
    SemanticAnalyzer analyzer;
    std::vector<std::unique_ptr<FuncDef>> funcs;
    
    // 声明函数foo(int x, int y)
    auto foo = std::make_unique<FuncDef>("foo", Type::Int);
    foo->params.push_back(FuncDef::Param("int", "x"));
    foo->params.push_back(FuncDef::Param("int", "y"));
    foo->body = std::make_unique<Block>();
    
    // 声明main函数
    auto main = std::make_unique<FuncDef>("main", Type::Int);
    main->body = std::make_unique<Block>();
    
    // 调用foo函数，但参数数量不匹配
    std::vector<std::unique_ptr<Expr>> args;
    args.push_back(std::make_unique<NumberExpr>(1));
    auto callExpr = std::make_unique<CallExpr>("foo", std::move(args));
    auto exprStmt = std::make_unique<ExprStmt>(std::move(callExpr));
    main->body->stmts.push_back(std::move(exprStmt));
    
    funcs.push_back(std::move(foo));
    funcs.push_back(std::move(main));
    analyzer.analyze(funcs);
}

int main() {
    test_simple_function();
    test_undeclared_variable();
    test_duplicate_variable();
    test_invalid_return_type();
    test_break_continue_outside_loop();
    test_function_call_args();
    return 0;
}
