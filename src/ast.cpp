#include "ast.h"
#include <iostream>
#include <iomanip>
#include <map>

// 辅助函数：打印缩进
void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }
}

// 打印Token类型名称
std::string tokenTypeToString(TokenType type) {
    static const std::map<TokenType, std::string> typeMap = {
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::INT, "INT"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::VOID, "VOID"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::TIMES, "TIMES"},
        {TokenType::DIVIDE, "DIVIDE"},
        {TokenType::ASSIGN, "ASSIGN"},
        {TokenType::EQ, "EQ"},
        {TokenType::NE, "NE"},
        {TokenType::LT, "LT"},
        {TokenType::LE, "LE"},
        {TokenType::GT, "GT"},
        {TokenType::GE, "GE"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::LPAREN, "LPAREN"},
        {TokenType::RPAREN, "RPAREN"},
        {TokenType::LBRACKET, "LBRACKET"},
        {TokenType::RBRACKET, "RBRACKET"},
        {TokenType::LBRACE, "LBRACE"},
        {TokenType::RBRACE, "RBRACE"},
        {TokenType::ID, "ID"},
        {TokenType::NUM, "NUM"},
        {TokenType::END_OF_FILE, "EOF"},
        {TokenType::ERROR, "ERROR"}
    };
    
    auto it = typeMap.find(type);
    return it != typeMap.end() ? it->second : "UNKNOWN";
}

// ProgramNode打印
void ProgramNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Program:\n";
    for (const auto& decl : declarations) {
        decl->print(indent + 1);
    }
}

// VarDeclarationNode打印
void VarDeclarationNode::print(int indent) const {
    printIndent(indent);
    std::cout << "VarDeclaration: " << typeSpecifier << " " << identifier;
    if (isArray) {
        std::cout << "[" << arraySize << "]";
    }
    std::cout << "\n";
}

// ArrayDeclarationNode打印
void ArrayDeclarationNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ArrayDeclaration: " << typeSpecifier << " " 
              << identifier << "[" << arraySize << "]\n";
}

// FunDeclarationNode打印
void FunDeclarationNode::print(int indent) const {
    printIndent(indent);
    std::cout << "FunDeclaration: " << returnType << " " << identifier << "(\n";
    
    for (const auto& param : params) {
        param->print(indent + 1);
    }
    
    printIndent(indent);
    std::cout << ")\n";
    
    if (body) {
        body->print(indent + 1);
    }
}

// ParamNode打印
void ParamNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Param: " << typeSpecifier << " " << identifier;
    if (isArray) {
        std::cout << "[]";
    }
    std::cout << "\n";
}

// CompoundStmtNode打印
void CompoundStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "CompoundStmt: {\n";
    
    printIndent(indent + 1);
    std::cout << "LocalDeclarations:\n";
    for (const auto& decl : localDeclarations) {
        decl->print(indent + 2);
    }
    
    printIndent(indent + 1);
    std::cout << "Statements:\n";
    for (const auto& stmt : statements) {
        stmt->print(indent + 2);
    }
    
    printIndent(indent);
    std::cout << "}\n";
}

// ExpressionStmtNode打印
void ExpressionStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ExpressionStmt: ";
    if (expression) {
        std::cout << "\n";
        expression->print(indent + 1);
    } else {
        std::cout << ";\n";
    }
}

// SelectionStmtNode打印
void SelectionStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "IfStmt:\n";
    
    printIndent(indent + 1);
    std::cout << "Condition:\n";
    condition->print(indent + 2);
    
    printIndent(indent + 1);
    std::cout << "Then:\n";
    ifBranch->print(indent + 2);
    
    if (elseBranch) {
        printIndent(indent + 1);
        std::cout << "Else:\n";
        elseBranch->print(indent + 2);
    }
}

// IterationStmtNode打印
void IterationStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "WhileStmt:\n";
    
    printIndent(indent + 1);
    std::cout << "Condition:\n";
    condition->print(indent + 2);
    
    printIndent(indent + 1);
    std::cout << "Body:\n";
    body->print(indent + 2);
}

// ReturnStmtNode打印
void ReturnStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ReturnStmt:";
    if (expression) {
        std::cout << "\n";
        expression->print(indent + 1);
    } else {
        std::cout << " (void)\n";
    }
}

// AssignExprNode打印
void AssignExprNode::print(int indent) const {
    printIndent(indent);
    std::cout << "AssignExpression:\n";
    
    printIndent(indent + 1);
    std::cout << "Left:\n";
    var->print(indent + 2);
    
    printIndent(indent + 1);
    std::cout << "Right:\n";
    expression->print(indent + 2);
}

// SimpleExprNode打印
void SimpleExprNode::print(int indent) const {
    printIndent(indent);
    std::cout << "SimpleExpression (";
    std::cout << tokenTypeToString(relop) << "):\n";
    
    printIndent(indent + 1);
    std::cout << "Left:\n";
    left->print(indent + 2);
    
    printIndent(indent + 1);
    std::cout << "Right:\n";
    right->print(indent + 2);
}

// VarNode打印
void VarNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Variable: " << identifier;
    if (index) {
        std::cout << "[\n";
        index->print(indent + 1);
        printIndent(indent);
        std::cout << "]";
    }
    std::cout << "\n";
}

// CallNode打印
void CallNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Call: " << identifier << "(\n";
    
    for (const auto& arg : args) {
        arg->print(indent + 1);
    }
    
    printIndent(indent);
    std::cout << ")\n";
}

// NumNode打印
void NumNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Number: " << value << "\n";
}

// BinOpNode打印
void BinOpNode::print(int indent) const {
    printIndent(indent);
    std::cout << "BinaryOp: " << tokenTypeToString(op) << "\n";
    
    printIndent(indent + 1);
    std::cout << "Left:\n";
    left->print(indent + 2);
    
    printIndent(indent + 1);
    std::cout << "Right:\n";
    right->print(indent + 2);
}