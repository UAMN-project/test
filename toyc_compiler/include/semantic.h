#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>

enum class Type { Int, Void, Unknown };

struct Symbol {
    Type type = Type::Unknown;
    bool isFunction = false;
    std::vector<Type> paramTypes;
};

class SemanticAnalyzer {
public:
    void analyze(const std::vector<std::unique_ptr<FuncDef>>& funcs);

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;

    void enterScope();
    void exitScope();

    bool declare(const std::string& name, const Symbol& symbol);
    Symbol lookup(const std::string& name);

    void analyzeFunc(FuncDef* func);
    void analyzeBlock(Block* block);
    void analyzeStmt(Stmt* stmt);
    void analyzeExpr(Expr* expr);

    void reportError(const std::string& msg);
};

#endif // SEMANTIC_H