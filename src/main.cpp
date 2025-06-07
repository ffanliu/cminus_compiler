#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

// 读取文件内容
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

// 测试词法分析器
void testLexer(const std::string& source) {
    std::cout << "===== Testing Lexer =====\n";
    
    try {
        Lexer lexer(source);
        auto tokens = lexer.getAllTokens();
        
        for (const auto& token : tokens) {
            std::cout << "Line " << token.line << ": ";
            std::cout << "Type=" << static_cast<int>(token.type) 
                      << ", Lexeme='" << token.lexeme << "'\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Lexer error: " << e.what() << std::endl;
    }
    
    std::cout << "=========================\n\n";
}

// 测试语法分析器
void testParser(const std::string& source) {
    std::cout << "===== Testing Parser =====\n";
    
    try {
        Lexer lexer(source);
        Parser parser(lexer);
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "AST Structure:\n";
            ast->print();
        } else {
            std::cout << "Parser returned null AST\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Parser error: " << e.what() << std::endl;
    }
    
    std::cout << "==========================\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.cm>\n";
        return 1;
    }
    
    // 读取源文件
    std::string source = readFile(argv[1]);
    if (source.empty()) {
        return 1;
    }
    
    // 测试词法分析器
    testLexer(source);
    
    // 测试语法分析器
    testParser(source);
    
    return 0;
}