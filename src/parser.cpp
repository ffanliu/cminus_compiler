#include "parser.h"
#include <iostream>
#include <sstream>

// 构造函数
Parser::Parser(Lexer& lexer) 
    : lexer(lexer), currentIndex(0) 
{
    // 预读两个Token
    tokenBuffer.push_back(lexer.getNextToken());
    tokenBuffer.push_back(lexer.getNextToken());
}

// 获取当前Token
Token Parser::currentToken() const {
    return tokenBuffer[currentIndex];
}

// 预读下一个Token
Token Parser::peekToken() const {
    return tokenBuffer[currentIndex + 1];
}

// 消费一个Token，并检查类型
void Parser::eatToken(TokenType expected) {
    if (matchToken(expected)) {
        // 移动到下一个Token
        currentIndex++;
        tokenBuffer[0] = tokenBuffer[1];
        tokenBuffer[1] = lexer.getNextToken();
    } else {
        std::ostringstream oss;
        oss << "Expected " << static_cast<int>(expected) 
            << " but found " << static_cast<int>(currentToken().type)
            << " at line " << currentToken().line;
        error(oss.str());
    }
}

// 检查当前Token类型
bool Parser::matchToken(TokenType expected) const {
    return currentToken().type == expected;
}

// 错误处理
void Parser::error(const std::string& message) const {
    std::ostringstream oss;
    oss << message 
        << " at line " << currentToken().line
        << ". Current token: " << currentToken().lexeme
        << " (type=" << static_cast<int>(currentToken().type) << ")"
        << ", Next token: " << (currentIndex + 1 < tokenBuffer.size() ? tokenBuffer[currentIndex + 1].lexeme : "none");
    throw std::runtime_error(oss.str());
}

// 解析入口
std::unique_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

// program -> declaration_list
std::unique_ptr<ProgramNode> Parser::parseProgram() {
    std::cout << "=== Starting to parse program ===\n";
    auto program = std::make_unique<ProgramNode>();
    parseDeclarationList(*program);
    std::cout << "=== Finished parsing program ===\n";
    return program;
}

// declaration_list -> declaration_list declaration | declaration
void Parser::parseDeclarationList(ProgramNode& program) {
    std::cout << "Parsing declaration list\n";
    program.declarations.push_back(parseDeclaration());
    
    while (matchToken(TokenType::INT) || matchToken(TokenType::VOID)) {
        std::cout << "Parsing additional declaration\n";
        program.declarations.push_back(parseDeclaration());
    }
}

// declaration -> var_declaration | fun_declaration
std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    // 确保当前 token 是 INT 或 VOID
    if (!(matchToken(TokenType::INT) || matchToken(TokenType::VOID))) {
        error("Expected INT or VOID at start of declaration");
    }

    Token typeToken = currentToken();
    eatToken(typeToken.type);  // 消费类型 token

    // 检查是否直接跟着 '('（函数声明无函数名）
    if (matchToken(TokenType::LPAREN)) {
        Token idToken = Token(TokenType::ID, "", typeToken.line);
        return parseFunDeclaration(typeToken, idToken);
    }

    // 否则必须有标识符
    if (!matchToken(TokenType::ID)) {
        error("Expected identifier after type specifier");
    }
    Token idToken = currentToken();
    eatToken(TokenType::ID);  // 消费 ID token

    // 检查是否是函数声明
    if (matchToken(TokenType::LPAREN)) {
        return parseFunDeclaration(typeToken, idToken);
    }
    
    // 否则是变量声明
    // 回退 token 以便 parseVarDeclaration 可以处理
    tokenBuffer.insert(tokenBuffer.begin(), idToken);
    tokenBuffer.insert(tokenBuffer.begin(), typeToken);
    currentIndex = 0;
    return parseVarDeclaration();
}

// var_declaration -> type_specifier ID | type_specifier ID [ NUM ]
std::unique_ptr<ASTNode> Parser::parseVarDeclaration() {
    Token typeToken = currentToken();
    eatToken(typeToken.type); // 消费类型说明符
    
    if (!matchToken(TokenType::ID)) {
        error("Expected identifier after type specifier");
    }
    
    Token idToken = currentToken();
    eatToken(TokenType::ID); // 消费标识符
    
    // 检查数组声明
    if (matchToken(TokenType::LBRACKET)) {
        eatToken(TokenType::LBRACKET);
        
        if (!matchToken(TokenType::NUM)) {
            error("Expected number in array declaration");
        }
        
        Token numToken = currentToken();
        eatToken(TokenType::NUM);
        eatToken(TokenType::RBRACKET);
        eatToken(TokenType::SEMICOLON);
        
        return std::make_unique<ArrayDeclarationNode>(typeToken.lexeme, idToken.lexeme, 
                                                    std::stoi(numToken.lexeme), typeToken.line);
    }
    
    eatToken(TokenType::SEMICOLON);
    return std::make_unique<VarDeclarationNode>(typeToken.lexeme, idToken.lexeme, typeToken.line);
}

// fun_declaration -> type_specifier ID ( params ) compound_stmt
std::unique_ptr<FunDeclarationNode> Parser::parseFunDeclaration(const Token& typeToken, const Token& idToken) {
    std::cout << "Parsing function declaration: " << typeToken.lexeme << " " << idToken.lexeme << "\n";
    
    auto funDecl = std::make_unique<FunDeclarationNode>(typeToken.lexeme, idToken.lexeme, typeToken.line);
    
    // 确保下一个 token 是 '('
    if (!matchToken(TokenType::LPAREN)) {
        error("Expected '(' after function name");
    }
    eatToken(TokenType::LPAREN);
    
    // 解析参数
    if (matchToken(TokenType::VOID)) {
        eatToken(TokenType::VOID);
    } else if (!matchToken(TokenType::RPAREN)) {
        parseParamList(funDecl->params);
    }
    
    // 确保下一个 token 是 ')'
    if (!matchToken(TokenType::RPAREN)) {
        error("Expected ')' after parameters");
    }
    eatToken(TokenType::RPAREN);
    
    // 解析函数体
    funDecl->body = parseCompoundStmt();
    
    return funDecl;
}

// param -> type_specifier ID | type_specifier ID []
std::unique_ptr<ParamNode> Parser::parseParam() {
    Token typeToken = currentToken();
    eatToken(typeToken.type); // 消费类型说明符
    
    if (!matchToken(TokenType::ID)) {
        error("Expected identifier in parameter");
    }
    
    Token idToken = currentToken();
    eatToken(TokenType::ID); // 消费参数名
    
    bool isArray = false;
    if (matchToken(TokenType::LBRACKET)) {
        eatToken(TokenType::LBRACKET);
        eatToken(TokenType::RBRACKET);
        isArray = true;
    }
    
    return std::make_unique<ParamNode>(typeToken.lexeme, idToken.lexeme, isArray, typeToken.line);
}

// param_list -> param_list , param | param
void Parser::parseParamList(std::vector<std::unique_ptr<ASTNode>>& params) {
    params.push_back(parseParam());
    
    while (matchToken(TokenType::COMMA)) {
        eatToken(TokenType::COMMA);
        params.push_back(parseParam());
    }
}

// compound_stmt -> { local_declarations statement_list }
std::unique_ptr<CompoundStmtNode> Parser::parseCompoundStmt() {
    int line = currentToken().line;
    eatToken(TokenType::LBRACE); // 消费 '{'
    
    auto compoundStmt = std::make_unique<CompoundStmtNode>(line);
    
    // 解析局部声明
    parseLocalDeclarations(*compoundStmt);
    
    // 解析语句列表
    parseStatementList(*compoundStmt);
    
    eatToken(TokenType::RBRACE); // 消费 '}'
    
    return compoundStmt;
}

// local_declarations -> local_declarations var_declaration | empty
void Parser::parseLocalDeclarations(CompoundStmtNode& compoundStmt) {
    while (matchToken(TokenType::INT) || matchToken(TokenType::VOID)) {
        compoundStmt.localDeclarations.push_back(parseVarDeclaration());
    }
}

// statement_list -> statement_list statement | empty
void Parser::parseStatementList(CompoundStmtNode& compoundStmt) {
    while (true) {
        TokenType type = currentToken().type;
        if (type == TokenType::SEMICOLON || 
            type == TokenType::ID || 
            type == TokenType::NUM || 
            type == TokenType::LPAREN || 
            type == TokenType::LBRACE || 
            type == TokenType::IF || 
            type == TokenType::WHILE || 
            type == TokenType::RETURN) {
            
            compoundStmt.statements.push_back(parseStatement());
        } else {
            break;
        }
    }
}

// statement -> expression_stmt | compound_stmt | selection_stmt | iteration_stmt | return_stmt
std::unique_ptr<ASTNode> Parser::parseStatement() {
    switch (currentToken().type) {
        case TokenType::LBRACE:
            return parseCompoundStmt();
            
        case TokenType::IF:
            return parseSelectionStmt();
            
        case TokenType::WHILE:
            return parseIterationStmt();
            
        case TokenType::RETURN:
            return parseReturnStmt();
            
        case TokenType::SEMICOLON:
        case TokenType::ID:
        case TokenType::NUM:
        case TokenType::LPAREN:
            return parseExpressionStmt();
            
        default:
            error("Unexpected token in statement");
            return nullptr;
    }
}

// expression_stmt -> expression ; | ;
std::unique_ptr<ExpressionStmtNode> Parser::parseExpressionStmt() {
    auto exprStmt = std::make_unique<ExpressionStmtNode>(currentToken().line);
    
    if (!matchToken(TokenType::SEMICOLON)) {
        exprStmt->expression = parseExpression();
    }
    
    eatToken(TokenType::SEMICOLON);
    return exprStmt;
}

// selection_stmt -> IF ( expression ) statement | IF ( expression ) statement ELSE statement
std::unique_ptr<SelectionStmtNode> Parser::parseSelectionStmt() {
    int line = currentToken().line;
    eatToken(TokenType::IF); // 消费 'if'
    eatToken(TokenType::LPAREN); // 消费 '('
    
    auto selectionStmt = std::make_unique<SelectionStmtNode>(line);
    selectionStmt->condition = parseExpression();
    
    eatToken(TokenType::RPAREN); // 消费 ')'
    selectionStmt->ifBranch = parseStatement();
    
    // 检查 else 分支
    if (matchToken(TokenType::ELSE)) {
        eatToken(TokenType::ELSE);
        selectionStmt->elseBranch = parseStatement();
    }
    
    return selectionStmt; // 错误：有极字
}

// iteration_stmt -> WHILE ( expression ) statement
std::unique_ptr<IterationStmtNode> Parser::parseIterationStmt() {
    int line = currentToken().line;
    eatToken(TokenType::WHILE); // 消费 'while'
    eatToken(TokenType::LPAREN); // 消费 '('
    
    auto iterationStmt = std::make_unique<IterationStmtNode>(line);
    iterationStmt->condition = parseExpression();
    
    eatToken(TokenType::RPAREN); // 消费 ')'
    iterationStmt->body = parseStatement();
    
    return iterationStmt;
}

// return_stmt -> RETURN | RETURN expression
std::unique_ptr<ReturnStmtNode> Parser::parseReturnStmt() {
    int line = currentToken().line;
    eatToken(TokenType::RETURN); // 消费 'return'
    
    auto returnStmt = std::make_unique<ReturnStmtNode>(line);
    
    if (!matchToken(TokenType::SEMICOLON)) {
        returnStmt->expression = parseExpression();
    }
    
    return returnStmt;
}

// expression -> var = expression | simple_expression
std::unique_ptr<ASTNode> Parser::parseExpression() {
    // 检查是否是赋值表达式
    if (matchToken(TokenType::ID) && peekToken().type == TokenType::ASSIGN) {
        auto var = parseVar();
        eatToken(TokenType::ASSIGN); // 消费 '='
        
        auto assignExpr = std::make_unique<AssignExprNode>(var->line);
        assignExpr->var = std::move(var);
        assignExpr->expression = parseExpression();
        
        return assignExpr;
    }
    
    // 否则是简单表达式
    return parseSimpleExpression();
}

// var -> ID | ID [ expression ]
std::unique_ptr<VarNode> Parser::parseVar() {
    if (!matchToken(TokenType::ID)) {
        error("Expected identifier for variable");
    }
    
    Token idToken = currentToken();
    eatToken(TokenType::ID); // 消费标识符
    
    auto varNode = std::make_unique<VarNode>(idToken.lexeme, idToken.line);
    
    // 检查数组索引
    if (matchToken(TokenType::LBRACKET)) {
        eatToken(TokenType::LBRACKET);
        varNode->index = parseExpression();
        eatToken(TokenType::RBRACKET);
    }
    
    return varNode;
}

// simple_expression -> additive_expression relop additive_expression | additive_expression
std::unique_ptr<ASTNode> Parser::parseSimpleExpression() {
    auto left = parseAdditiveExpression();
    
    // 检查关系运算符
    TokenType op = currentToken().type;
    if (op == TokenType::LT || op == TokenType::LE || 
        op == TokenType::GT || op == TokenType::GE || 
        op == TokenType::EQ || op == TokenType::NE) {
        
        eatToken(op);
        auto simpleExpr = std::make_unique<SimpleExprNode>(left->line);
        simpleExpr->left = std::move(left);
        simpleExpr->relop = op;
        simpleExpr->right = parseAdditiveExpression();
        
        return simpleExpr;
    }
    
    return left;
}

// additive_expression -> additive_expression addop term | term
std::unique_ptr<ASTNode> Parser::parseAdditiveExpression() {
    auto left = parseTerm();
    
    while (matchToken(TokenType::PLUS) || matchToken(TokenType::MINUS)) {
        TokenType op = currentToken().type;
        eatToken(op);
        
        auto binOp = std::make_unique<BinOpNode>(op, left->line);
        binOp->left = std::move(left);
        binOp->right = parseTerm();
        left = std::move(binOp);
    }
    
    return left;
}

// term -> term mulop factor | factor
std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();
    
    while (matchToken(TokenType::TIMES) || matchToken(TokenType::DIVIDE)) {
        TokenType op = currentToken().type;
        eatToken(op);
        
        auto binOp = std::make_unique<BinOpNode>(op, left->line);
        binOp->left = std::move(left);
        binOp->right = parseFactor();
        left = std::move(binOp);
    }
    
    return left;
}

// factor -> ( expression ) | var | call | NUM
std::unique_ptr<ASTNode> Parser::parseFactor() {
    switch (currentToken().type) {
        case TokenType::LPAREN: {
            eatToken(TokenType::LPAREN);
            auto expr = parseExpression();
            eatToken(TokenType::RPAREN);
            return expr;
        }
            
        case TokenType::ID: {
            // 区分变量和函数调用
            if (peekToken().type == TokenType::LPAREN) {
                return parseCall();
            }
            return parseVar();
        }
            
        case TokenType::NUM: {
            Token numToken = currentToken();
            eatToken(TokenType::NUM);
            return std::make_unique<NumNode>(std::stoi(numToken.lexeme), numToken.line);
        }
            
        default:
            error("Unexpected token in factor");
            return nullptr;
    }
}

// call -> ID ( args )
std::unique_ptr<CallNode> Parser::parseCall() {
    Token idToken = currentToken();
    eatToken(TokenType::ID); // 消费函数名
    eatToken(TokenType::LPAREN); // 消费 '('
    
    auto callNode = std::make_unique<CallNode>(idToken.lexeme, idToken.line);
    
    if (!matchToken(TokenType::RPAREN)) {
        parseArgList(callNode->args);
    }
    
    eatToken(TokenType::RPAREN); // 消费 ')'
    
    return callNode;
}

// args -> arg_list | empty
void Parser::parseArgList(std::vector<std::unique_ptr<ASTNode>>& args) {
    args.push_back(parseExpression());
    
    while (matchToken(TokenType::COMMA)) {
        eatToken(TokenType::COMMA);
        args.push_back(parseExpression());
    }
}