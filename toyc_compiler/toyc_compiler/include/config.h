#pragma once

// 是否开启调试输出
#define TOYC_DEBUG false

// 最大嵌套块层数
#define MAX_BLOCK_DEPTH 256

// 默认整数类型
#define DEFAULT_INT_TYPE "int"

// 默认返回值类型
#define DEFAULT_RETURN_TYPE "int"

// 错误码定义
enum class ErrorCode {
    SUCCESS = 0,
    SYNTAX_ERROR = 1,
    SEMANTIC_ERROR = 2,
    CODEGEN_ERROR = 3
};

// 工具宏
#define UNREACHABLE() \
    do { fprintf(stderr, "Unreachable code reached: %s:%d\n", __FILE__, __LINE__); exit(1); } while(0)
