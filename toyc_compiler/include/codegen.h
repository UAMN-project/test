#pragma once
#include "ast.h"
#include <ostream>
#include <unordered_map>
#include <string>

class CodeGen {
public:
    CodeGen(std::ostream &out);
    void genBlock(Block *block);
    void generate(const std::vector<std::unique_ptr<FuncDef>> &funcs);

private:
    std::ostream &out;
    int labelCount = 0;
    std::unordered_map<std::string, int> localVarOffset;

    void genFunc(FuncDef *func);
    void genStmt(Stmt *stmt);
    std::string genExpr(Expr *expr);
    void emit(const std::string &instr);
    std::string newLabel(const std::string &base);
};
