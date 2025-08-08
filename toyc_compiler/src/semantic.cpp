#include "semantic.h"
#include <iostream>

void SemanticAnalyzer::enterScope() {
    scopes.emplace_back();
}

void SemanticAnalyzer::exitScope() {
    if (scopes.empty()) {
        std::cerr << "Internal error: scope stack underflow\n";
        return;
    }
    scopes.pop_back();
}

bool SemanticAnalyzer::declare(const std::string& name, const Symbol& symbol) {
    if (scopes.empty()) enterScope();
    auto& current = scopes.back();
    if (current.find(name) != current.end()) {
        reportError("Variable '" + name + "' redeclared in current scope");
        return false;
    }
    current[name] = symbol;
    return true;
}

Symbol SemanticAnalyzer::lookup(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    reportError("Undeclared identifier '" + name + "'");
    return Symbol{Type::Unknown, false, {}};
}

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<FuncDef>>& funcs) {
    enterScope();
    for (const auto& func : funcs) {
        analyzeFunc(func.get());
    }
    exitScope();
}

void SemanticAnalyzer::analyzeFunc(FuncDef* func) {
    enterScope();
    for (auto& param : func->params) {
        Symbol sym{Type::Int, false, {}};
        if (!declare(param.name, sym)) {
            reportError("Duplicate parameter name: " + param.name);
        }
    }
    analyzeBlock(func->body.get());
    exitScope();
}

void SemanticAnalyzer::analyzeBlock(Block* block) {
    enterScope();
    for (auto& stmt : block->stmts) {
        analyzeStmt(stmt.get());
    }
    exitScope();
}

void SemanticAnalyzer::analyzeStmt(Stmt* stmt) {
    if (auto decl = dynamic_cast<VarDeclStmt*>(stmt)) {
        Symbol sym{Type::Int, false, {}};
        if (!declare(decl->name, sym)) {
            reportError("Variable '" + decl->name + "' redeclared");
        }
        if (decl->initializer) analyzeExpr(decl->initializer.get());
    }
    else if (auto assign = dynamic_cast<AssignStmt*>(stmt)) {
        Symbol sym = lookup(assign->name);
        if (sym.type == Type::Unknown) {
            reportError("Variable '" + assign->name + "' used before declaration");
        }
        analyzeExpr(assign->value.get());
    }
    else if (auto exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
        analyzeExpr(exprStmt->expr.get());
    }
    else if (auto ret = dynamic_cast<ReturnStmt*>(stmt)) {
        if (ret->expr) analyzeExpr(ret->expr.get());
    }
    else if (auto block = dynamic_cast<Block*>(stmt)) {
        analyzeBlock(block);
    }
    else if (auto ifStmt = dynamic_cast<IfStmt*>(stmt)) {
        analyzeExpr(ifStmt->condition.get());
        analyzeStmt(ifStmt->thenBlock.get());
        if (ifStmt->elseBlock) analyzeStmt(ifStmt->elseBlock.get());
    }
    else if (auto whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
        analyzeExpr(whileStmt->condition.get());
        analyzeStmt(whileStmt->body.get());
    }
    else if (dynamic_cast<BreakStmt*>(stmt) || dynamic_cast<ContinueStmt*>(stmt)) {
        // 可做循环上下文检测
    }
    else {
        reportError("Unknown statement type");
    }
}

void SemanticAnalyzer::analyzeExpr(Expr* expr) {
    if (auto var = dynamic_cast<VarExpr*>(expr)) {
        Symbol sym = lookup(var->name);
        if (sym.type == Type::Unknown) {
            reportError("Variable '" + var->name + "' used before declaration");
        }
    }
    else if (dynamic_cast<NumberExpr*>(expr)) {
        // 数字不需要检查
    }
    else if (auto bin = dynamic_cast<BinaryExpr*>(expr)) {
        analyzeExpr(bin->lhs.get());
        analyzeExpr(bin->rhs.get());
    }
    else if (auto unary = dynamic_cast<UnaryExpr*>(expr)) {
        analyzeExpr(unary->operand.get());
    }
    else if (auto call = dynamic_cast<CallExpr*>(expr)) {
        for (auto& arg : call->args) {
            analyzeExpr(arg.get());
        }
        // TODO: 函数调用检查
    }
    else {
        reportError("Unknown expression type");
    }
}

void SemanticAnalyzer::reportError(const std::string& msg) {
    std::cerr << "Semantic error: " << msg << std::endl;
}
