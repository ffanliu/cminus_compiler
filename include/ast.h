#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "lexer.h"

// AST节点类型
enum class ASTNodeType {
    PROGRAM,
    VAR_DECLARATION,
    ARRAY_DECLARATION,
    FUN_DECLARATION,
    PARAM,
    COMPOUND_STMT,
    EXPRESSION_STMT,
    SELECTION_STMT,
    ITERATION_STMT,
    RETURN_STMT,
    ASSIGN_EXPR,
    SIMPLE_EXPR,
    VAR,
    CALL,
    NUM,
    BIN_OP
};

// AST节点基类
class ASTNode {
public:
    ASTNodeType type;
    int line;
    
    ASTNode(ASTNodeType t, int ln) : type(t), line(ln) {}
    virtual ~ASTNode() = default;
    
    // 打印AST结构
    virtual void print(int indent = 0) const = 0;
};

// 程序节点
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations;
    
    ProgramNode() : ASTNode(ASTNodeType::PROGRAM, 1) {}
    void print(int indent = 0) const override;
};

// 变量声明节点
class VarDeclarationNode : public ASTNode {
public:
    std::string typeSpecifier;
    std::string identifier;
    bool isArray;
    int arraySize; // 仅当isArray为true时有效
    
    VarDeclarationNode(const std::string& type, const std::string& id, int ln)
        : ASTNode(ASTNodeType::VAR_DECLARATION, ln), 
          typeSpecifier(type), identifier(id), isArray(false), arraySize(0) {}
    void print(int indent = 0) const override;
};

// 数组声明节点
class ArrayDeclarationNode : public ASTNode {
public:
    std::string typeSpecifier;
    std::string identifier;
    int arraySize;
    
    ArrayDeclarationNode(const std::string& type, const std::string& id, int size, int ln)
        : ASTNode(ASTNodeType::ARRAY_DECLARATION, ln), 
          typeSpecifier(type), identifier(id), arraySize(size) {}
    void print(int indent = 0) const override;
};

// 函数声明节点
class FunDeclarationNode : public ASTNode {
public:
    std::string returnType;
    std::string identifier;
    std::vector<std::unique_ptr<ASTNode>> params;
    std::unique_ptr<ASTNode> body; // CompoundStmtNode
    
    FunDeclarationNode(const std::string& type, const std::string& id, int ln)
        : ASTNode(ASTNodeType::FUN_DECLARATION, ln), 
          returnType(type), identifier(id) {}
    void print(int indent = 0) const override;
};

// 参数节点
class ParamNode : public ASTNode {
public:
    std::string typeSpecifier;
    std::string identifier;
    bool isArray;
    
    ParamNode(const std::string& type, const std::string& id, bool array, int ln)
        : ASTNode(ASTNodeType::PARAM, ln), 
          typeSpecifier(type), identifier(id), isArray(array) {}
    void print(int indent = 0) const override;
};

// 复合语句节点
class CompoundStmtNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> localDeclarations;
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    CompoundStmtNode(int ln) : ASTNode(ASTNodeType::COMPOUND_STMT, ln) {}
    void print(int indent = 0) const override;
};

// 表达式语句节点
class ExpressionStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression; // 可能为nullptr
    
    ExpressionStmtNode(int ln) : ASTNode(ASTNodeType::EXPRESSION_STMT, ln) {}
    void print(int indent = 0) const override;
};

// 选择语句节点
class SelectionStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> ifBranch;
    std::unique_ptr<ASTNode> elseBranch; // 可能为nullptr
    
    SelectionStmtNode(int ln) : ASTNode(ASTNodeType::SELECTION_STMT, ln) {}
    void print(int indent = 0) const override;
};

// 循环语句节点
class IterationStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    IterationStmtNode(int ln) : ASTNode(ASTNodeType::ITERATION_STMT, ln) {}
    void print(int indent = 0) const override;
};

// 返回语句节点
class ReturnStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression; // 可能为nullptr
    
    ReturnStmtNode(int ln) : ASTNode(ASTNodeType::RETURN_STMT, ln) {}
    void print(int indent = 0) const override;
};

// 赋值表达式节点
class AssignExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> var;
    std::unique_ptr<ASTNode> expression;
    
    AssignExprNode(int ln) : ASTNode(ASTNodeType::ASSIGN_EXPR, ln) {}
    void print(int indent = 0) const override;
};

// 简单表达式节点
class SimpleExprNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    TokenType relop; // 关系运算符
    
    SimpleExprNode(int ln) : ASTNode(ASTNodeType::SIMPLE_EXPR, ln), relop(TokenType::ERROR) {}
    void print(int indent = 0) const override;
};

// 变量节点
class VarNode : public ASTNode {
public:
    std::string identifier;
    std::unique_ptr<ASTNode> index; // 数组索引，可能为nullptr
    
    VarNode(const std::string& id, int ln)
        : ASTNode(ASTNodeType::VAR, ln), identifier(id) {}
    void print(int indent = 0) const override;
};

// 函数调用节点
class CallNode : public ASTNode {
public:
    std::string identifier;
    std::vector<std::unique_ptr<ASTNode>> args;
    
    CallNode(const std::string& id, int ln)
        : ASTNode(ASTNodeType::CALL, ln), identifier(id) {}
    void print(int indent = 0) const override;
};

// 数字节点
class NumNode : public ASTNode {
public:
    int value;
    
    NumNode(int val, int ln) : ASTNode(ASTNodeType::NUM, ln), value(val) {}
    void print(int indent = 0) const override;
};

// 二元操作节点
class BinOpNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    TokenType op;
    
    BinOpNode(TokenType opType, int ln) 
        : ASTNode(ASTNodeType::BIN_OP, ln), op(opType) {}
    void print(int indent = 0) const override;
};

#endif // AST_H