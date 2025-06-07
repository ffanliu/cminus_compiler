#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>


// Token类型枚举
enum class TokenType {
    // 关键字
    IF, ELSE, INT, RETURN, VOID, WHILE,
    
    // 专用符号
    PLUS, MINUS, TIMES, DIVIDE, ASSIGN, EQ, NE, LT, LE, GT, GE,
    SEMICOLON, COMMA, LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE,
    
    // 标识符和数字
    ID, NUM,
    
    // 文件结束
    END_OF_FILE,
    
    // 错误标记
    ERROR
};

// Token结构
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    
    Token(TokenType t, const std::string& l, int ln) 
        : type(t), lexeme(l), line(ln) {}
};

// 词法分析器类
class Lexer {
public:
    Lexer(const std::string& source);
    
    // 获取下一个Token
    Token getNextToken();
    
    // 获取所有Token（用于测试）
    std::vector<Token> getAllTokens();

private:
    // 辅助函数
    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    
    // Token识别函数
    Token handleIdentifier();
    Token handleNumber();
    Token handleOperator();
    Token handleSymbol();
    
    // 源程序
    std::string source;
    
    // 当前位置
    size_t currentPos;
    int currentLine;
    
    // 关键字映射
    static const std::unordered_map<std::string, TokenType> keywords;
};

#endif // LEXER_H