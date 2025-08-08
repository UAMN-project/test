#include "codegen.h"
#include "ast.h"
#include "semantic.h"
#include <memory>
#include <vector>
#include <string>

void testSimpleFunction() {
    auto block = std::make_unique<Block>();
    auto returnStmt = std::make_unique<ReturnStmt>();
    returnStmt->expr = std::make_unique<NumberExpr>(42);
    block->stmts.push_back(std::move(returnStmt));

    auto func = std::make_unique<FuncDef>("int", "main");
    func->body = std::move(block);

    CodeGen codegen;
    codegen.generate({func.get()});
}

void testArithmeticOperations() {
    auto block = std::make_unique<Block>();

    // 声明变量 x = 5 * 3
    auto declStmt = std::make_unique<VarDeclStmt>(
        "int", "x",
        std::make_unique<BinaryExpr>(
            "*",
            std::make_unique<NumberExpr>(5),
            std::make_unique<NumberExpr>(3)
        )
    );
    block->stmts.push_back(std::move(declStmt));

    // x = x + 2
    auto assignStmt = std::make_unique<AssignStmt>(
        "x",
        std::make_unique<BinaryExpr>(
            "+",
            std::make_unique<VarExpr>("x"),
            std::make_unique<NumberExpr>(2)
        )
    );
    block->stmts.push_back(std::move(assignStmt));

    // return x / 2
    auto returnStmt = std::make_unique<ReturnStmt>();
    returnStmt->expr = std::make_unique<BinaryExpr>(
        "/",
        std::make_unique<VarExpr>("x"),
        std::make_unique<NumberExpr>(2)
    );
    block->stmts.push_back(std::move(returnStmt));

    auto func = std::make_unique<FuncDef>("int", "main");
    func->body = std::move(block);

    CodeGen codegen;
    codegen.generate({func.get()});
}

void testIfStatement() {
    auto block = std::make_unique<Block>();

    // int x = 10
    auto declStmt = std::make_unique<VarDeclStmt>(
        "int", "x",
        std::make_unique<NumberExpr>(10)
    );
    block->stmts.push_back(std::move(declStmt));

    // if (x < 20) { return 1; } else { return 0; }
    auto ifStmt = std::make_unique<IfStmt>(
        std::make_unique<BinaryExpr>(
            "<",
            std::make_unique<VarExpr>("x"),
            std::make_unique<NumberExpr>(20)
        ),
        std::make_unique<Block>(),
        std::make_unique<Block>()
    );

    auto thenReturn = std::make_unique<ReturnStmt>();
    thenReturn->expr = std::make_unique<NumberExpr>(1);
    ifStmt->thenBlock->stmts.push_back(std::move(thenReturn));

    auto elseReturn = std::make_unique<ReturnStmt>();
    elseReturn->expr = std::make_unique<NumberExpr>(0);
    ifStmt->elseBlock->stmts.push_back(std::move(elseReturn));

    block->stmts.push_back(std::move(ifStmt));

    auto func = std::make_unique<FuncDef>("int", "main");
    func->body = std::move(block);

    CodeGen codegen;
    codegen.generate({func.get()});
}

void testWhileLoop() {
    auto block = std::make_unique<Block>();

    // int i = 0
    auto declStmt = std::make_unique<VarDeclStmt>("int", "i", std::make_unique<NumberExpr>(0));
    block->stmts.push_back(std::move(declStmt));

    // while (i < 10) { i = i + 1; }
    auto whileStmt = std::make_unique<WhileStmt>(
        std::make_unique<BinaryExpr>(
            "<",
            std::make_unique<VarExpr>("i"),
            std::make_unique<NumberExpr>(10)
        ),
        std::make_unique<Block>()
    );

    auto assignStmt = std::make_unique<AssignStmt>(
        "i",
        std::make_unique<BinaryExpr>(
            "+",
            std::make_unique<VarExpr>("i"),
            std::make_unique<NumberExpr>(1)
        )
    );
    whileStmt->body->stmts.push_back(std::move(assignStmt));

    block->stmts.push_back(std::move(whileStmt));

    // return i
    auto returnStmt = std::make_unique<ReturnStmt>();
    returnStmt->expr = std::make_unique<VarExpr>("i");
    block->stmts.push_back(std::move(returnStmt));

    auto func = std::make_unique<FuncDef>("int", "main");
    func->body = std::move(block);

    CodeGen codegen;
    codegen.generate({func.get()});
}

int main() {
    testSimpleFunction();
    testArithmeticOperations();
    testIfStatement();
    testWhileLoop();
    return 0;
}
