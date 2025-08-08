#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void printHelp() {
    std::cout << "ToyC Compiler v0.1.0\n"
              << "Usage: toyc [options] [file]\n"
              << "Options:\n"
              << "  -h, --help     Show this help message\n"
              << "  -v, --version  Show version information\n"
              << "  -o <file>      Write output to <file>\n";
}

void printVersion() {
    std::cout << "ToyC Compiler v0.1.0\n";
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile;
    bool hasOutputFile = false;

    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printHelp();
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printVersion();
            return 0;
        }
        else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "Error: -o option requires an argument\n";
                return 1;
            }
            outputFile = argv[++i];
            hasOutputFile = true;
        }
        else if (inputFile.empty()) {
            inputFile = argv[i];
        }
        else {
            std::cerr << "Error: multiple input files not supported\n";
            return 1;
        }
    }

    // 读取输入
    std::string sourceCode;
    if (!inputFile.empty()) {
        std::ifstream file(inputFile);
        if (!file) {
            std::cerr << "Error: cannot open input file '" << inputFile << "'\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        sourceCode = buffer.str();
    }
    else {
        // 从标准输入读取
        std::stringstream buffer;
        buffer << std::cin.rdbuf();
        sourceCode = buffer.str();
    }

    try {
        // 词法分析
        Lexer lexer(sourceCode);
        auto tokens = lexer.tokenize();

        // 语法分析
        Parser parser(tokens);
        auto ast = parser.parse();

        // 语义分析
        SemanticAnalyzer analyzer;
        analyzer.analyze(ast);

        // 代码生成
        CodeGen codegen;
        std::string output = codegen.generate(ast);

        // 输出
        if (hasOutputFile) {
            std::ofstream file(outputFile);
            if (!file) {
                std::cerr << "Error: cannot open output file '" << outputFile << "'\n";
                return 1;
            }
            file << output;
        }
        else {
            std::cout << output;
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
