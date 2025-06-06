#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>

// Базовый класс для всех узлов AST
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
    virtual void accept(class CodeGenerator &generator) = 0;
};

// Типы переменных
enum class VarType
{
    INT,
    STRING
};

// Узел программы (корневой)
class ProgramNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void addStatement(std::unique_ptr<ASTNode> stmt)
    {
        statements.push_back(std::move(stmt));
    }

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел объявления переменной
class VarDeclNode : public ASTNode
{
public:
    VarType type;
    std::string name;

    VarDeclNode(VarType t, const std::string &n) : type(t), name(n) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел присваивания
class AssignNode : public ASTNode
{
public:
    std::string variable;
    std::unique_ptr<ASTNode> expression;

    AssignNode(const std::string &var, std::unique_ptr<ASTNode> expr)
        : variable(var), expression(std::move(expr)) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел печати
class PrintNode : public ASTNode
{
public:
    std::unique_ptr<ASTNode> expression;

    PrintNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел условного оператора
class IfNode : public ASTNode
{
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenStmt;
    std::unique_ptr<ASTNode> elseStmt; // может быть nullptr

    IfNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then,
           std::unique_ptr<ASTNode> els = nullptr)
        : condition(std::move(cond)), thenStmt(std::move(then)), elseStmt(std::move(els)) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел цикла while
class WhileNode : public ASTNode
{
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    WhileNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел блока
class BlockNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void addStatement(std::unique_ptr<ASTNode> stmt)
    {
        statements.push_back(std::move(stmt));
    }

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Типы бинарных операций
enum class BinOpType
{
    PLUS,
    MINUS,
    MUL,
    DIV,
    EQ,
    NEQ,
    LT,
    LE,
    GT,
    GE,
    AND,
    OR
};

// Узел бинарной операции
class BinaryOpNode : public ASTNode
{
public:
    BinOpType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(BinOpType o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел унарной операции
class UnaryOpNode : public ASTNode
{
public:
    std::unique_ptr<ASTNode> operand;

    UnaryOpNode(std::unique_ptr<ASTNode> op) : operand(std::move(op)) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел целочисленного литерала
class IntLiteralNode : public ASTNode
{
public:
    int value;

    IntLiteralNode(int v) : value(v) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел строкового литерала
class StringLiteralNode : public ASTNode
{
public:
    std::string value;

    StringLiteralNode(const std::string &v) : value(v) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Узел идентификатора
class IdentifierNode : public ASTNode
{
public:
    std::string name;

    IdentifierNode(const std::string &n) : name(n) {}

    void print(int indent = 0) const override;
    void accept(class CodeGenerator &generator) override;
};

// Глобальная переменная для корня AST
extern std::unique_ptr<ProgramNode> root;
extern class ProgramNode *parsed_program;

#endif // AST_HPP