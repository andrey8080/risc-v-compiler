#include "ast.hpp"
#include "codegen.hpp"
#include <iostream>

// Глобальная переменная для корня AST
std::unique_ptr<ProgramNode> root;
ProgramNode *parsed_program = nullptr;

void indent_print(int indent)
{
    for (int i = 0; i < indent; ++i)
    {
        std::cout << "  ";
    }
}

// Реализация ProgramNode
void ProgramNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "Program:" << std::endl;
    std::cout << "  Number of statements: " << statements.size() << std::endl;

    for (size_t i = 0; i < statements.size(); ++i)
    {
        if (statements[i])
        {
            statements[i]->print(indent + 1);
        }
        else
        {
            std::cout << "  Statement " << i << " is nullptr!" << std::endl;
        }
    }
}

void ProgramNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация VarDeclNode
void VarDeclNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "VarDecl: " << (type == VarType::INT ? "int" : "string")
              << " " << name << std::endl;
}

void VarDeclNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация AssignNode
void AssignNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "Assign: " << variable << " =" << std::endl;
    expression->print(indent + 1);
}

void AssignNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация PrintNode
void PrintNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "Print:" << std::endl;
    expression->print(indent + 1);
}

void PrintNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация IfNode
void IfNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "If:" << std::endl;
    indent_print(indent + 1);
    std::cout << "Condition:" << std::endl;
    if (condition)
    {
        condition->print(indent + 2);
    }
    else
    {
        std::cout << "  condition is nullptr!" << std::endl;
    }
    indent_print(indent + 1);
    std::cout << "Then:" << std::endl;
    if (thenStmt)
    {
        thenStmt->print(indent + 2);
    }
    else
    {
        std::cout << "  thenStmt is nullptr!" << std::endl;
    }
    if (elseStmt)
    {
        indent_print(indent + 1);
        std::cout << "Else:" << std::endl;
        elseStmt->print(indent + 2);
    }
}

void IfNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация WhileNode
void WhileNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "While:" << std::endl;
    indent_print(indent + 1);
    std::cout << "Condition:" << std::endl;
    if (condition)
    {
        condition->print(indent + 2);
    }
    else
    {
        std::cout << "  condition is nullptr!" << std::endl;
    }
    indent_print(indent + 1);
    std::cout << "Body:" << std::endl;
    if (body)
    {
        body->print(indent + 2);
    }
    else
    {
        std::cout << "  body is nullptr!" << std::endl;
    }
}

void WhileNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация BlockNode
void BlockNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "Block:" << std::endl;
    for (const auto &stmt : statements)
    {
        stmt->print(indent + 1);
    }
}

void BlockNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация BinaryOpNode
void BinaryOpNode::print(int indent) const
{
    indent_print(indent);
    std::string opStr;
    switch (op)
    {
    case BinOpType::PLUS:
        opStr = "+";
        break;
    case BinOpType::MINUS:
        opStr = "-";
        break;
    case BinOpType::MUL:
        opStr = "*";
        break;
    case BinOpType::DIV:
        opStr = "/";
        break;
    case BinOpType::EQ:
        opStr = "==";
        break;
    case BinOpType::NEQ:
        opStr = "!=";
        break;
    case BinOpType::LT:
        opStr = "<";
        break;
    case BinOpType::LE:
        opStr = "<=";
        break;
    case BinOpType::GT:
        opStr = ">";
        break;
    case BinOpType::GE:
        opStr = ">=";
        break;
    case BinOpType::AND:
        opStr = "and";
        break;
    case BinOpType::OR:
        opStr = "or";
        break;
    }
    std::cout << "BinaryOp: " << opStr << std::endl;
    left->print(indent + 1);
    right->print(indent + 1);
}

void BinaryOpNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация UnaryOpNode
void UnaryOpNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "UnaryOp: not" << std::endl;
    operand->print(indent + 1);
}

void UnaryOpNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация IntLiteralNode
void IntLiteralNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "IntLiteral: " << value << std::endl;
}

void IntLiteralNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация StringLiteralNode
void StringLiteralNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "StringLiteral: " << value << std::endl;
}

void StringLiteralNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}

// Реализация IdentifierNode
void IdentifierNode::print(int indent) const
{
    indent_print(indent);
    std::cout << "Identifier: " << name << std::endl;
}

void IdentifierNode::accept(CodeGenerator &generator)
{
    generator.visit(*this);
}