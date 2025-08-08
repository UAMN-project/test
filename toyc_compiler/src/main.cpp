#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void printUsage(const char* programName) {
    std::cerr << "用法: " << programName << " [选项] [输入文件]\n"
              << "选项:\n"
              << "  -h, --help     显示帮助信息\n"
              << "  -o <文件>      指定输出文件（默认为标准输出）\n"
              << "  -v, --version  显示版本信息\n"
              << "如果没有指定输入文件，将从标准输入读取\n";
}

void printVersion() {
    std::cout << "ToyC编译器 v1.0.0\n";
}

std::string readInput(std::istream& input) {
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile;
    bool hasOutputFile = false;

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printVersion();
            return 0;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "错误: -o 选项需要一个文件名参数\n";
                return 1;
            }
            outputFile = argv[++i];
            hasOutputFile = true;
        } else if (argv[i][0] == '-') {
            std::cerr << "错误: 未知选项 " << argv[i] << "\n";
            return 1;
        } else if (inputFile.empty()) {
            inputFile = argv[i];
        } else {
            std::cerr << "错误: 只能指定一个输入文件\n";
            return 1;
        }
    }

    try {
        // 读取输入
        std::string sourceCode;
        if (!inputFile.empty()) {
            std::ifstream file(inputFile);
            if (!file) {
                std::cerr << "错误: 无法打开输入文件 '" << inputFile << "'\n";
                return 1;
            }
            sourceCode = readInput(file);
        } else {
            sourceCode = readInput(std::cin);
        }

        // 词法分析
        Lexer lexer;
        auto tokens = lexer.tokenize(sourceCode);

        // 语法分析
        Parser parser(tokens);
        auto ast = parser.parseCompUnit();

        // 语义分析
        SemanticAnalyzer analyzer;
        analyzer.analyze(ast);

        // 代码生成
        std::ostream* output = &std::cout;
        std::ofstream outFile;
        if (hasOutputFile) {
            outFile.open(outputFile);
            if (!outFile) {
                std::cerr << "错误: 无法打开输出文件 '" << outputFile << "'\n";
                return 1;
            }
            output = &outFile;
        }

        CodeGen codegen(*output);
        codegen.generate(ast);

        if (hasOutputFile) {
            outFile.close();
        }

    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
