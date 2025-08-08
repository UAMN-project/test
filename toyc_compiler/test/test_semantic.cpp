#include "semantic.h"
#include "ast.h"
#include <iostream>
#include <memory>
#include <cassert>

void test_simple_function() {
    // 测试简单函数定义和返回值
    auto returnStmt = std::make_unique<ReturnStmt>(std::make_unique<NumberExpr>(42));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(returnStmt));
    auto func = std::make_unique<FuncDef>(Type::Int, "main", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    SemanticAnalyzer analyzer;
    analyzer.analyze(funcs);
    std::cout << "Simple function test passed\n";
}

void test_undeclared_variable() {
    // 测试使用未声明的变量
    auto varExpr = std::make_unique<VarExpr>("x");
    auto returnStmt = std::make_unique<ReturnStmt>(std::move(varExpr));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(returnStmt));
    auto func = std::make_unique<FuncDef>(Type::Int, "test", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(funcs);
        assert(false && "Should throw exception for undeclared variable");
    } catch (const std::exception& e) {
        std::cout << "Undeclared variable test passed\n";
    }
}

void test_duplicate_variable() {
    // 测试重复声明变量
    auto declStmt1 = std::make_unique<DeclStmt>("x", Type::Int, std::make_unique<NumberExpr>(1));
    auto declStmt2 = std::make_unique<DeclStmt>("x", Type::Int, std::make_unique<NumberExpr>(2));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(declStmt1));
    block->stmts.push_back(std::move(declStmt2));
    auto func = std::make_unique<FuncDef>(Type::Int, "test", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(funcs);
        assert(false && "Should throw exception for duplicate variable");
    } catch (const std::exception& e) {
        std::cout << "Duplicate variable test passed\n";
    }
}

void test_invalid_return_type() {
    // 测试返回值类型不匹配
    auto returnStmt = std::make_unique<ReturnStmt>(std::make_unique<NumberExpr>(42));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(returnStmt));
    auto func = std::make_unique<FuncDef>(Type::Void, "test", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(funcs);
        assert(false && "Should throw exception for invalid return type");
    } catch (const std::exception& e) {
        std::cout << "Invalid return type test passed\n";
    }
}

void test_break_continue_outside_loop() {
    // 测试在循环外使用break/continue
    auto breakStmt = std::make_unique<BreakStmt>();
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(breakStmt));
    auto func = std::make_unique<FuncDef>(Type::Void, "test", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(funcs);
        assert(false && "Should throw exception for break outside loop");
    } catch (const std::exception& e) {
        std::cout << "Break outside loop test passed\n";
    }
}

void test_function_call() {
    // 测试函数调用参数检查
    auto args = std::vector<std::unique_ptr<Expr>>();
    args.push_back(std::make_unique<NumberExpr>(1));
    args.push_back(std::make_unique<NumberExpr>(2));
    auto callExpr = std::make_unique<CallExpr>("undefined_func", std::move(args));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::make_unique<ExprStmt>(std::move(callExpr)));
    auto func = std::make_unique<FuncDef>(Type::Void, "test", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(funcs);
        assert(false && "Should throw exception for undefined function call");
    } catch (const std::exception& e) {
        std::cout << "Undefined function call test passed\n";
    }
}

int main() {
    try {
        test_simple_function();
        test_undeclared_variable();
        test_duplicate_variable();
        test_invalid_return_type();
        test_break_continue_outside_loop();
        test_function_call();
        std::cout << "\nAll semantic tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "Semantic test failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
