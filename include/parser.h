#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <iostream>

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<ProgramNode> parse();
    
private:
    // 辅助函数
    Token currentToken() const;
    Token peekToken() const;
    void eatToken(TokenType expected);
    bool matchToken(TokenType expected) const;
    void error(const std::string& message) const;
    
    // 解析函数
    std::unique_ptr<ProgramNode> parseProgram();
    void parseDeclarationList(ProgramNode& program);
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<ASTNode> parseVarDeclaration();
    std::unique_ptr<FunDeclarationNode> parseFunDeclaration(const Token& typeToken, const Token& idToken);
    std::unique_ptr<ParamNode> parseParam();
    void parseParamList(std::vector<std::unique_ptr<ASTNode>>& params);
    std::unique_ptr<CompoundStmtNode> parseCompoundStmt();
    void parseLocalDeclarations(CompoundStmtNode& compoundStmt);
    void parseStatementList(CompoundStmtNode& compoundStmt);
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ExpressionStmtNode> parseExpressionStmt();
    std::unique_ptr<SelectionStmtNode> parseSelectionStmt();
    std::unique_ptr<IterationStmtNode> parseIterationStmt();
    std::unique_ptr<ReturnStmtNode> parseReturnStmt();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<VarNode> parseVar();
    std::unique_ptr<ASTNode> parseSimpleExpression();
    std::unique_ptr<ASTNode> parseAdditiveExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<CallNode> parseCall();
    void parseArgList(std::vector<std::unique_ptr<ASTNode>>& args);
    
    // 词法分析器引用
    Lexer& lexer;
    
    // Token缓冲区
    std::vector<Token> tokenBuffer;
    size_t currentIndex;
};

#endif // PARSER_H