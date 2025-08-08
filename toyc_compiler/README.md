# ToyC Compiler

一个简单的C语言子集编译器，支持将ToyC源代码编译为RISC-V 32位汇编代码。

## 功能特性

- 支持基本的C语言语法：
  - 整数类型（int）和void类型
  - 函数定义和调用
  - 变量声明和赋值
  - 算术运算（+, -, *, /, %）
  - 关系运算（<, >, <=, >=, ==, !=）
  - 控制流语句（if-else, while）
  - break和continue语句
  - 单行注释（//）和多行注释（/* */）

## 编译器架构

编译器由以下主要组件构成：

1. 词法分析器（Lexer）：将源代码转换为token序列
2. 语法分析器（Parser）：将token序列转换为抽象语法树（AST）
3. 语义分析器（Semantic Analyzer）：进行类型检查和作用域分析
4. 代码生成器（Code Generator）：将AST转换为RISC-V汇编代码

## 构建要求

- CMake 3.10或更高版本
- 支持C++17的编译器
- Windows/Linux/macOS操作系统

## 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 构建项目
cmake --build .

# 运行测试（可选）
ctest
```

## 使用方法

```bash
# 从文件编译
./toyc input.c > output.s

# 从标准输入编译
./toyc > output.s
```

## 示例

输入（test.c）：
```c
int add(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    return add(x, y);
}
```

编译：
```bash
./toyc test.c > test.s
```

输出（test.s）：
```assembly
.globl add
add:
    # 函数序言
    addi sp, sp, -8
    sw ra, 4(sp)
    sw fp, 0(sp)
    addi fp, sp, 8
    
    # 函数体
    lw a1, -12(fp)  # 加载参数a
    lw a2, -16(fp)  # 加载参数b
    add a0, a1, a2  # 计算a + b
    
    # 函数尾声
    lw ra, 4(sp)
    lw fp, 0(sp)
    addi sp, sp, 8
    ret

.globl main
main:
    # 函数序言
    addi sp, sp, -8
    sw ra, 4(sp)
    sw fp, 0(sp)
    addi fp, sp, 8
    
    # 分配局部变量空间
    addi sp, sp, -8
    
    # x = 10
    li t0, 10
    sw t0, -8(fp)
    
    # y = 20
    li t0, 20
    sw t0, -12(fp)
    
    # 准备函数调用参数
    lw a0, -8(fp)   # 加载x
    lw a1, -12(fp)  # 加载y
    
    # 调用add函数
    jal add
    
    # 函数尾声
    lw ra, 4(sp)
    lw fp, 0(sp)
    addi sp, sp, 16
    ret
```

## 测试

项目包含以下测试套件：

- `test_lexer`：词法分析器测试
- `test_parser`：语法分析器测试
- `test_semantic`：语义分析器测试
- `test_codegen`：代码生成器测试

运行所有测试：
```bash
ctest
```

## 许可证

本项目采用MIT许可证。详见[LICENSE](LICENSE)文件。