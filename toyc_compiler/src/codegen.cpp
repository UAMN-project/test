#include "codegen.h"
#include "ast.h"
#include <iostream>
#include <cassert>

CodeGen::CodeGen(std::ostream &os) : out(os), labelCount(0) {}

void CodeGen::generate(const std::vector<std::unique_ptr<FuncDef>> &funcs) {
    for (const auto &f : funcs) {
        genFunc(f.get());
    }
}

void CodeGen::emit(const std::string &code) {
    out << "\t" << code << "\n";
}

std::string CodeGen::newLabel(const std::string &base) {
    return base + "_" + std::to_string(labelCount++);
}

std::string CodeGen::genExpr(Expr *expr) {
    if (auto num = dynamic_cast<NumberExpr *>(expr)) {
        emit("li a0, " + std::to_string(num->value));
        return "a0";
    } else if (auto var = dynamic_cast<VarExpr *>(expr)) {
        if (localVarOffset.count(var->name) == 0) {
            std::cerr << "Error: Variable '" << var->name << "' not found" << std::endl;
            return "a0";
        }
        int offset = localVarOffset[var->name];
        emit("lw a0, " + std::to_string(offset) + "(sp)");
        return "a0";
    } else if (auto bin = dynamic_cast<BinaryExpr *>(expr)) {
        genExpr(bin->lhs.get());
        emit("mv t0, a0");
        genExpr(bin->rhs.get());

        if (bin->op == "+") {
            emit("add a0, t0, a0");
        } else if (bin->op == "-") {
            emit("sub a0, t0, a0");
        } else if (bin->op == "*") {
            emit("mul a0, t0, a0");
        } else if (bin->op == "/") {
            emit("div a0, t0, a0");
        } else if (bin->op == "%") {
            emit("rem a0, t0, a0");
        } else if (bin->op == "<") {
            emit("slt a0, t0, a0");
        } else if (bin->op == ">") {
            emit("sgt a0, t0, a0");
        } else if (bin->op == "<=") {
            emit("sgt a0, a0, t0");
            emit("xori a0, a0, 1");
        } else if (bin->op == ">=") {
            emit("slt a0, a0, t0");
            emit("xori a0, a0, 1");
        } else if (bin->op == "==") {
            emit("sub a0, t0, a0");
            emit("seqz a0, a0");
        } else if (bin->op == "!=") {
            emit("sub a0, t0, a0");
            emit("snez a0, a0");
        } else {
            std::cerr << "Warning: Unsupported binary operator '" << bin->op << "'" << std::endl;
            emit("add a0, t0, a0"); // 默认加法
        }
        return "a0";
    } else if (auto call = dynamic_cast<CallExpr *>(expr)) {
        if (call->args.size() > 8) {
            std::cerr << "Error: Function call has too many arguments (max 8)" << std::endl;
            return "a0";
        }
        for (size_t i = 0; i < call->args.size(); i++) {
            genExpr(call->args[i].get());
            emit("mv a" + std::to_string(i) + ", a0");
        }
        emit("call " + call->callee);
        return "a0";
    } else if (auto unary = dynamic_cast<UnaryExpr *>(expr)) {
        genExpr(unary->operand.get());
        if (unary->op == "-") {
            emit("neg a0, a0");
        } else if (unary->op == "!") {
            emit("seqz a0, a0");
        } else {
            std::cerr << "Warning: Unsupported unary operator '" << unary->op << "'" << std::endl;
        }
        return "a0";
    }
    std::cerr << "Warning: Unknown expression type" << std::endl;
    return "a0";
}

void CodeGen::genFunc(FuncDef *func) {
    localVarOffset.clear();
    int offset = 0;

    out << ".globl " << func->name << "\n";
    out << func->name << ":\n";

    emit("addi sp, sp, -128"); // 分配栈空间

    // 参数保存
    for (size_t i = 0; i < func->params.size(); i++) {
        offset -= 4;
        localVarOffset[func->params[i].name] = offset;
        emit("sw a" + std::to_string(i) + ", " + std::to_string(offset) + "(sp)");
    }

    genBlock(func->body.get());

    emit("addi sp, sp, 128");
    emit("ret");
}

void CodeGen::genBlock(Block *block) {
    for (auto &stmt : block->stmts) {
        genStmt(stmt.get());
    }
}

void CodeGen::genStmt(Stmt *stmt) {
    if (auto decl = dynamic_cast<VarDeclStmt *>(stmt)) {
        int offset = localVarOffset.size() * -4 - 4;
        localVarOffset[decl->name] = offset;
        if (decl->initializer) {
            genExpr(decl->initializer.get());
            emit("sw a0, " + std::to_string(offset) + "(sp)");
        }
    } else if (auto assign = dynamic_cast<AssignStmt *>(stmt)) {
        int offset = localVarOffset[assign->name];
        genExpr(assign->value.get());
        emit("sw a0, " + std::to_string(offset) + "(sp)");
    } else if (auto exprStmt = dynamic_cast<ExprStmt *>(stmt)) {
        genExpr(exprStmt->expr.get());
    } else if (auto ret = dynamic_cast<ReturnStmt *>(stmt)) {
        if (ret->expr) genExpr(ret->expr.get());
        emit("addi sp, sp, 128");
        emit("ret");
    } else if (auto block = dynamic_cast<Block *>(stmt)) {
        genBlock(block);
    } else if (auto ifStmt = dynamic_cast<IfStmt *>(stmt)) {
        std::string elseLabel = newLabel("else");
        std::string endLabel = newLabel("endif");

        genExpr(ifStmt->condition.get());
        emit("beqz a0, " + elseLabel);
        genBlock(ifStmt->thenBlock.get());
        emit("j " + endLabel);
        emit(elseLabel + ":");
        if (ifStmt->elseBlock) genBlock(ifStmt->elseBlock.get());
        emit(endLabel + ":");
    } else if (auto whileStmt = dynamic_cast<WhileStmt *>(stmt)) {
        std::string loopLabel = newLabel("loop");
        std::string endLabel = newLabel("endloop");
        emit(loopLabel + ":");
        genExpr(whileStmt->condition.get());
        emit("beqz a0, " + endLabel);
        genBlock(whileStmt->body.get());
        emit("j " + loopLabel);
        emit(endLabel + ":");
    } else if (auto *breakStmt = dynamic_cast<BreakStmt*>(stmt)) {
        if (breakLabels.empty()) {
            std::cerr << "Warning: break statement outside of loop" << std::endl;
            return;
        }
        emit("j " + breakLabels.top());
    } else if (auto *continueStmt = dynamic_cast<ContinueStmt*>(stmt)) {
        if (continueLabels.empty()) {
            std::cerr << "Warning: continue statement outside of loop" << std::endl;
            return;
        }
        emit("j " + continueLabels.top());
    } else {
        // 未知语句类型，跳过而不是断言失败
        std::cerr << "Warning: Unknown statement type in codegen" << std::endl;
    }
}
