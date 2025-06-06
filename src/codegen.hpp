#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "ast.hpp"
#include <unordered_map>
#include <ostream>
#include <string>

class CodeGenerator
{
private:
    std::ostream &output;
    std::unordered_map<std::string, int> variables; // имя -> смещение от sp
    int stackOffset;
    int labelCounter;
    int stringCounter;
    std::unordered_map<std::string, std::string> stringLiterals; // строка -> метка

    void genDataSection();
    void genTextSection();
    std::string getNewLabel();
    std::string getStringLabel(const std::string &str);
    void allocateVariable(const std::string &name);
    int getVariableOffset(const std::string &name);
    void collectInfo(const ProgramNode &program);
    void collectInfoFromNode(const ASTNode &node);

public:
    CodeGenerator(std::ostream &out) : output(out), stackOffset(0), labelCounter(0), stringCounter(0) {}

    void generateCode(const ProgramNode &program);

    // Visitor методы для каждого типа узла
    void visit(const ProgramNode &node);
    void visit(const VarDeclNode &node);
    void visit(const AssignNode &node);
    void visit(const PrintNode &node);
    void visit(const IfNode &node);
    void visit(const WhileNode &node);
    void visit(const BlockNode &node);
    void visit(const BinaryOpNode &node);
    void visit(const UnaryOpNode &node);
    void visit(const IntLiteralNode &node);
    void visit(const StringLiteralNode &node);
    void visit(const IdentifierNode &node);
};

#endif // CODEGEN_HPP
