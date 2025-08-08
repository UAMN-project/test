#include "include/ast.h"
#include <memory>

int main() {
    // 测试FuncDef构造函数
    auto func1 = std::make_unique<FuncDef>("int", "main");
    auto func2 = std::make_unique<FuncDef>("int", Type::Int);
    
    // 测试VarDeclStmt构造函数
    auto var1 = std::make_unique<VarDeclStmt>("int", "x", nullptr);
    auto var2 = std::make_unique<VarDeclStmt>("int", Type::Int, nullptr);
    
    return 0;
}