cmake_minimum_required(VERSION 3.10)
project(ToyCCompiler)

# 强制使用C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 默认构建类型为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件列表
set(COMPILER_SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/semantic.cpp
    src/codegen.cpp
)

# 主编译器可执行文件
add_executable(toyc ${COMPILER_SOURCES})

# 测试文件列表
set(TEST_SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/semantic.cpp
    src/codegen.cpp
)

# 词法分析器测试
add_executable(test_lexer
    test/test_lexer.cpp
    src/lexer.cpp
)
target_include_directories(test_lexer PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 语法分析器测试
add_executable(test_parser
    test/test_parser.cpp
    ${TEST_SOURCES}
)
target_include_directories(test_parser PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 语义分析器测试
add_executable(test_semantic
    test/test_semantic.cpp
    ${TEST_SOURCES}
)
target_include_directories(test_semantic PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 代码生成器测试
add_executable(test_codegen
    test/test_codegen.cpp
    ${TEST_SOURCES}
)
target_include_directories(test_codegen PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 添加测试
enable_testing()
add_test(NAME LexerTest COMMAND test_lexer)
add_test(NAME ParserTest COMMAND test_parser)
add_test(NAME SemanticTest COMMAND test_semantic)
add_test(NAME CodeGenTest COMMAND test_codegen)

# 安装规则
install(TARGETS toyc
    RUNTIME DESTINATION bin
)
