#include "codegen.h"
#include "ast.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <cassert>

void testSimpleFunction() {
    // 测试简单函数：int main() { return 42; }
    auto returnStmt = std::make_unique<ReturnStmt>(std::make_unique<NumberExpr>(42));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(returnStmt));
    auto func = std::make_unique<FuncDef>(Type::Int, "main", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    std::stringstream ss;
    CodeGen codegen(ss);
    codegen.generate(funcs);

    std::string output = ss.str();
    assert(!output.empty());
    assert(output.find(".globl main") != std::string::npos);
    assert(output.find("li a0, 42") != std::string::npos);
    std::cout << "Simple function test passed\n";
}

void testArithmeticOperations() {
    // 测试算术运算：int main() { return 1 + 2 * 3 - 4 / 2; }
    auto mul = std::make_unique<BinaryExpr>(
        std::make_unique<NumberExpr>(2),
        BinaryOp::Multiply,
        std::make_unique<NumberExpr>(3)
    );
    auto add = std::make_unique<BinaryExpr>(
        std::make_unique<NumberExpr>(1),
        BinaryOp::Add,
        std::move(mul)
    );
    auto div = std::make_unique<BinaryExpr>(
        std::make_unique<NumberExpr>(4),
        BinaryOp::Divide,
        std::make_unique<NumberExpr>(2)
    );
    auto sub = std::make_unique<BinaryExpr>(
        std::move(add),
        BinaryOp::Subtract,
        std::move(div)
    );
    auto returnStmt = std::make_unique<ReturnStmt>(std::move(sub));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(returnStmt));
    auto func = std::make_unique<FuncDef>(Type::Int, "main", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    std::stringstream ss;
    CodeGen codegen(ss);
    codegen.generate(funcs);

    std::string output = ss.str();
    assert(!output.empty());
    assert(output.find("mul") != std::string::npos);
    assert(output.find("add") != std::string::npos);
    assert(output.find("div") != std::string::npos);
    assert(output.find("sub") != std::string::npos);
    std::cout << "Arithmetic operations test passed\n";
}

void testIfStatement() {
    // 测试if语句：int main() { if (1 < 2) return 1; else return 0; }
    auto condition = std::make_unique<BinaryExpr>(
        std::make_unique<NumberExpr>(1),
        BinaryOp::Less,
        std::make_unique<NumberExpr>(2)
    );
    auto thenBlock = std::make_unique<Block>();
    thenBlock->stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<NumberExpr>(1)));
    auto elseBlock = std::make_unique<Block>();
    elseBlock->stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<NumberExpr>(0)));
    auto ifStmt = std::make_unique<IfStmt>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(ifStmt));
    auto func = std::make_unique<FuncDef>(Type::Int, "main", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    std::stringstream ss;
    CodeGen codegen(ss);
    codegen.generate(funcs);

    std::string output = ss.str();
    assert(!output.empty());
    assert(output.find("blt") != std::string::npos || output.find("bge") != std::string::npos);
    assert(output.find(".L") != std::string::npos);
    std::cout << "If statement test passed\n";
}

void testWhileLoop() {
    // 测试while循环：int main() { int i = 0; while (i < 5) i = i + 1; return i; }
    auto declStmt = std::make_unique<DeclStmt>("i", Type::Int, std::make_unique<NumberExpr>(0));
    auto condition = std::make_unique<BinaryExpr>(
        std::make_unique<VarExpr>("i"),
        BinaryOp::Less,
        std::make_unique<NumberExpr>(5)
    );
    auto assignStmt = std::make_unique<AssignStmt>(
        "i",
        std::make_unique<BinaryExpr>(
            std::make_unique<VarExpr>("i"),
            BinaryOp::Add,
            std::make_unique<NumberExpr>(1)
        )
    );
    auto loopBlock = std::make_unique<Block>();
    loopBlock->stmts.push_back(std::move(assignStmt));
    auto whileStmt = std::make_unique<WhileStmt>(std::move(condition), std::move(loopBlock));
    auto returnStmt = std::make_unique<ReturnStmt>(std::make_unique<VarExpr>("i"));
    auto block = std::make_unique<Block>();
    block->stmts.push_back(std::move(declStmt));
    block->stmts.push_back(std::move(whileStmt));
    block->stmts.push_back(std::move(returnStmt));
    auto func = std::make_unique<FuncDef>(Type::Int, "main", std::vector<std::pair<std::string, Type>>(), std::move(block));
    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(func));

    std::stringstream ss;
    CodeGen codegen(ss);
    codegen.generate(funcs);

    std::string output = ss.str();
    assert(!output.empty());
    assert(output.find("blt") != std::string::npos || output.find("bge") != std::string::npos);
    assert(output.find(".L") != std::string::npos);
    assert(output.find("j") != std::string::npos);
    std::cout << "While loop test passed\n";
}

void testFunctionCall() {
    // 测试函数调用：int add(int a, int b) { return a + b; } int main() { return add(1, 2); }
    auto addBody = std::make_unique<Block>();
    addBody->stmts.push_back(
        std::make_unique<ReturnStmt>(
            std::make_unique<BinaryExpr>(
                std::make_unique<VarExpr>("a"),
                BinaryOp::Add,
                std::make_unique<VarExpr>("b")
            )
        )
    );
    std::vector<std::pair<std::string, Type>> addParams = {{"a", Type::Int}, {"b", Type::Int}};
    auto addFunc = std::make_unique<FuncDef>(Type::Int, "add", std::move(addParams), std::move(addBody));

    auto args = std::vector<std::unique_ptr<Expr>>();
    args.push_back(std::make_unique<NumberExpr>(1));
    args.push_back(std::make_unique<NumberExpr>(2));
    auto callExpr = std::make_unique<CallExpr>("add", std::move(args));
    auto mainBody = std::make_unique<Block>();
    mainBody->stmts.push_back(std::make_unique<ReturnStmt>(std::move(callExpr)));
    auto mainFunc = std::make_unique<FuncDef>(Type::Int, "main", std::vector<std::pair<std::string, Type>>(), std::move(mainBody));

    std::vector<std::unique_ptr<FuncDef>> funcs;
    funcs.push_back(std::move(addFunc));
    funcs.push_back(std::move(mainFunc));

    std::stringstream ss;
    CodeGen codegen(ss);
    codegen.generate(funcs);

    std::string output = ss.str();
    assert(!output.empty());
    assert(output.find(".globl add") != std::string::npos);
    assert(output.find(".globl main") != std::string::npos);
    assert(output.find("jal") != std::string::npos);
    std::cout << "Function call test passed\n";
}

int main() {
    try {
        testSimpleFunction();
        testArithmeticOperations();
        testIfStatement();
        testWhileLoop();
        testFunctionCall();
        std::cout << "\nAll code generation tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "Code generation test failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
