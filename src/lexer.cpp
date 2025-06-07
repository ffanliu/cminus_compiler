#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <cstring>

// 关键字映射定义
const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"int", TokenType::INT},
    {"return", TokenType::RETURN},
    {"void", TokenType::VOID},
    {"while", TokenType::WHILE}
};

// 构造函数
Lexer::Lexer(const std::string& source) 
    : source(source), currentPos(0), currentLine(1) {}

// 查看下一个字符
char Lexer::peek() const {
    return currentPos < source.size() ? source[currentPos] : '\0';
}

// 前进一个字符
char Lexer::advance() {
    if (currentPos >= source.size()) return '\0';
    
    char c = source[currentPos++];
    if (c == '\n') currentLine++;
    return c;
}

// 跳过空白字符
void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

// 跳过注释
void Lexer::skipComment() {
    // 确认是注释开始 "/*"
    advance(); // 跳过 '/'
    advance(); // 跳过 '*'
    
    while (peek() != '\0') {
        if (peek() == '*') {
            advance(); // 跳过 '*'
            if (peek() == '/') {
                advance(); // 跳过 '/'
                return;
            }
        } else {
            advance();
        }
    }
    
    // 如果到达文件末尾但注释未结束
    throw std::runtime_error("Unterminated comment at line " + std::to_string(currentLine));
}

// 处理标识符或关键字
Token Lexer::handleIdentifier() {
    std::string lexeme;
    int startLine = currentLine;
    
    while (isalnum(peek())) {
        lexeme += advance();
    }
    
    // 检查是否是关键字
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return Token(it->second, lexeme, startLine);
    }
    
    return Token(TokenType::ID, lexeme, startLine);
}

// 处理数字
Token Lexer::handleNumber() {
    std::string lexeme;
    int startLine = currentLine;
    
    while (isdigit(peek())) {
        lexeme += advance();
    }
    
    return Token(TokenType::NUM, lexeme, startLine);
}

// 处理运算符
Token Lexer::handleOperator() {
    int startLine = currentLine;
    char first = advance();
    char next = peek();
    
    // 处理双字符运算符
    if (first == '=' && next == '=') {
        advance();
        return Token(TokenType::EQ, "==", startLine);
    }
    if (first == '!' && next == '=') {
        advance();
        return Token(TokenType::NE, "!=", startLine);
    }
    if (first == '<' && next == '=') {
        advance();
        return Token(TokenType::LE, "<=", startLine);
    }
    if (first == '>' && next == '=') {
        advance();
        return Token(TokenType::GE, ">=", startLine);
    }
    
    // 单字符运算符
    switch (first) {
        case '+': return Token(TokenType::PLUS, "+", startLine);
        case '-': return Token(TokenType::MINUS, "-", startLine);
        case '*': return Token(TokenType::TIMES, "*", startLine);
        case '/': return Token(TokenType::DIVIDE, "/", startLine);
        case '=': return Token(TokenType::ASSIGN, "=", startLine);
        case '<': return Token(TokenType::LT, "<", startLine);
        case '>': return Token(TokenType::GT, ">", startLine);
        default: 
            return Token(TokenType::ERROR, std::string(1, first), startLine);
    }
}

// 处理符号
Token Lexer::handleSymbol() {
    int startLine = currentLine;
    char c = advance();
    
    switch (c) {
        case ';': return Token(TokenType::SEMICOLON, ";", startLine);
        case ',': return Token(TokenType::COMMA, ",", startLine);
        case '(': return Token(TokenType::LPAREN, "(", startLine);
        case ')': return Token(TokenType::RPAREN, ")", startLine);
        case '[': return Token(TokenType::LBRACKET, "[", startLine);
        case ']': return Token(TokenType::RBRACKET, "]", startLine);
        case '{': return Token(TokenType::LBRACE, "{", startLine);
        case '}': return Token(TokenType::RBRACE, "}", startLine);
        default: 
            return Token(TokenType::ERROR, std::string(1, c), startLine);
    }
}

// 获取下一个Token
Token Lexer::getNextToken() {
    // 跳过空白和注释
    while (true) {
        skipWhitespace();
        
        // 检查注释
        if (peek() == '/' && currentPos < source.size() - 1 && source[currentPos+1] == '*') {
            skipComment();
        } else {
            break;
        }
    }
    
    // 文件结束
    if (peek() == '\0') {
        return Token(TokenType::END_OF_FILE, "", currentLine);
    }
    
    // 根据字符类型分发处理
    char c = peek();
    if (isalpha(c)) {
        return handleIdentifier();
    } else if (isdigit(c)) {
        return handleNumber();
    } else if (strchr("+-*/=!<>", c)) {
        return handleOperator();
    } else if (strchr(";,()[]{}", c)) {
        return handleSymbol();
    }
    
    // 未知字符
    std::string unknown(1, advance());
    return Token(TokenType::ERROR, unknown, currentLine);
}

// 获取所有Token
std::vector<Token> Lexer::getAllTokens() {
    std::vector<Token> tokens;
    Token token = getNextToken();
    
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = getNextToken();
    }
    
    tokens.push_back(token); // 添加EOF标记
    return tokens;
}