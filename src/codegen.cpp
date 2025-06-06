#include "codegen.hpp"
#include <iostream>
#include <sstream>

std::string CodeGenerator::getNewLabel()
{
    return "L" + std::to_string(labelCounter++);
}

std::string CodeGenerator::getStringLabel(const std::string &str)
{
    auto it = stringLiterals.find(str);
    if (it != stringLiterals.end())
    {
        return it->second;
    }
    std::string label = "str" + std::to_string(stringCounter++);
    stringLiterals[str] = label;
    return label;
}

void CodeGenerator::allocateVariable(const std::string &name)
{
    if (variables.find(name) == variables.end())
    {
        stackOffset += 4; // 4 байта для int или указателя на строку
        variables[name] = stackOffset;
    }
}

int CodeGenerator::getVariableOffset(const std::string &name)
{
    auto it = variables.find(name);
    if (it != variables.end())
    {
        return it->second;
    }
    return 0; // Ошибка: переменная не найдена
}

void CodeGenerator::genDataSection()
{
    output << ".data\n";
    // Генерируем строковые литералы
    for (const auto &pair : stringLiterals)
    {
        // Убираем кавычки из строки
        std::string str = pair.first;
        if (str.length() >= 2 && str[0] == '"' && str[str.length() - 1] == '"')
        {
            str = str.substr(1, str.length() - 2);
        }
        output << pair.second << ": .string \"" << str << "\"\n";
    }
    output << "\n";
}

void CodeGenerator::genTextSection()
{
    output << ".text\n";
    output << ".globl _start\n";
    output << "_start:\n";

    // Настройка стека
    if (stackOffset > 0)
    {
        output << "    addi sp, sp, -" << stackOffset << "\n";
    }
}

void CodeGenerator::generateCode(const ProgramNode &program)
{
    // Первый проход: собираем все переменные и строковые литералы
    collectInfo(program);

    // Генерируем секцию данных
    genDataSection();

    // Генерируем секцию кода
    genTextSection();

    // Генерируем код для всех операторов
    for (const auto &stmt : program.statements)
    {
        stmt->accept(*this);
    }

    // Завершение программы
    output << "    # Exit program\n";
    output << "    li a7, 93     # exit system call\n";
    output << "    li a0, 0      # exit status\n";
    output << "    ecall\n";
}

void CodeGenerator::collectInfo(const ProgramNode &program)
{
    for (const auto &stmt : program.statements)
    {
        collectInfoFromNode(*stmt);
    }
}

void CodeGenerator::collectInfoFromNode(const ASTNode &node)
{
    // Здесь мы используем visitor pattern для сбора информации
    // Но нам нужен отдельный метод для этого
    if (auto varDecl = dynamic_cast<const VarDeclNode *>(&node))
    {
        allocateVariable(varDecl->name);
    }
    else if (auto block = dynamic_cast<const BlockNode *>(&node))
    {
        for (const auto &stmt : block->statements)
        {
            collectInfoFromNode(*stmt);
        }
    }
    else if (auto assign = dynamic_cast<const AssignNode *>(&node))
    {
        collectInfoFromNode(*assign->expression);
    }
    else if (auto print = dynamic_cast<const PrintNode *>(&node))
    {
        collectInfoFromNode(*print->expression);
    }
    else if (auto ifNode = dynamic_cast<const IfNode *>(&node))
    {
        collectInfoFromNode(*ifNode->condition);
        collectInfoFromNode(*ifNode->thenStmt);
        if (ifNode->elseStmt)
        {
            collectInfoFromNode(*ifNode->elseStmt);
        }
    }
    else if (auto whileNode = dynamic_cast<const WhileNode *>(&node))
    {
        collectInfoFromNode(*whileNode->condition);
        collectInfoFromNode(*whileNode->body);
    }
    else if (auto binOp = dynamic_cast<const BinaryOpNode *>(&node))
    {
        collectInfoFromNode(*binOp->left);
        collectInfoFromNode(*binOp->right);
    }
    else if (auto unOp = dynamic_cast<const UnaryOpNode *>(&node))
    {
        collectInfoFromNode(*unOp->operand);
    }
    else if (auto strLit = dynamic_cast<const StringLiteralNode *>(&node))
    {
        getStringLabel(strLit->value);
    }
}

void CodeGenerator::visit(const ProgramNode &node)
{
    // Этот метод вызывается для генерации кода программы
    for (const auto &stmt : node.statements)
    {
        stmt->accept(*this);
    }
}

void CodeGenerator::visit(const VarDeclNode &node)
{
    allocateVariable(node.name);
}

void CodeGenerator::visit(const AssignNode &node)
{
    // Вычисляем выражение справа (результат в a0)
    node.expression->accept(*this);

    // Сохраняем в переменную
    int offset = getVariableOffset(node.variable);
    output << "    sw a0, -" << offset << "(sp)   # " << node.variable << " = a0\n";
}

void CodeGenerator::visit(const PrintNode &node)
{
    // Вычисляем выражение (результат в a0)
    node.expression->accept(*this);

    // Системный вызов для печати целого числа
    output << "    # Print integer\n";
    output << "    mv a1, a0     # move value to a1\n";
    output << "    li a7, 1      # print int system call\n";
    output << "    ecall\n";

    // Печатаем перевод строки
    output << "    li a7, 11     # print char system call\n";
    output << "    li a0, 10     # newline character\n";
    output << "    ecall\n";
}

void CodeGenerator::visit(const IfNode &node)
{
    std::string elseLabel = getNewLabel();
    std::string endLabel = getNewLabel();

    // Вычисляем условие
    node.condition->accept(*this);

    // Если результат 0 (false), переходим к else
    output << "    beqz a0, " << elseLabel << "\n";

    // Генерируем код для then
    node.thenStmt->accept(*this);
    output << "    j " << endLabel << "\n";

    // Генерируем код для else (если есть)
    output << elseLabel << ":\n";
    if (node.elseStmt)
    {
        node.elseStmt->accept(*this);
    }

    output << endLabel << ":\n";
}

void CodeGenerator::visit(const WhileNode &node)
{
    std::string startLabel = getNewLabel();
    std::string endLabel = getNewLabel();

    output << startLabel << ":\n";

    // Вычисляем условие
    node.condition->accept(*this);

    // Если результат 0 (false), выходим из цикла
    output << "    beqz a0, " << endLabel << "\n";

    // Генерируем тело цикла
    node.body->accept(*this);

    // Переходим к началу цикла
    output << "    j " << startLabel << "\n";

    output << endLabel << ":\n";
}

void CodeGenerator::visit(const BlockNode &node)
{
    for (const auto &stmt : node.statements)
    {
        stmt->accept(*this);
    }
}

void CodeGenerator::visit(const BinaryOpNode &node)
{
    // Вычисляем левый операнд
    node.left->accept(*this);
    output << "    addi sp, sp, -4   # save left operand\n";
    output << "    sw a0, 0(sp)\n";

    // Вычисляем правый операнд
    node.right->accept(*this);
    output << "    mv t1, a0         # right operand to t1\n";
    output << "    lw a0, 0(sp)      # restore left operand\n";
    output << "    addi sp, sp, 4\n";

    // Выполняем операцию
    switch (node.op)
    {
    case BinOpType::PLUS:
        output << "    add a0, a0, t1\n";
        break;
    case BinOpType::MINUS:
        output << "    sub a0, a0, t1\n";
        break;
    case BinOpType::MUL:
        output << "    mul a0, a0, t1\n";
        break;
    case BinOpType::DIV:
        output << "    div a0, a0, t1\n";
        break;
    case BinOpType::EQ:
        output << "    sub a0, a0, t1\n";
        output << "    seqz a0, a0\n";
        break;
    case BinOpType::NEQ:
        output << "    sub a0, a0, t1\n";
        output << "    snez a0, a0\n";
        break;
    case BinOpType::LT:
        output << "    slt a0, a0, t1\n";
        break;
    case BinOpType::LE:
        output << "    slt t2, t1, a0\n";
        output << "    xori a0, t2, 1\n";
        break;
    case BinOpType::GT:
        output << "    slt a0, t1, a0\n";
        break;
    case BinOpType::GE:
        output << "    slt t2, a0, t1\n";
        output << "    xori a0, t2, 1\n";
        break;
    case BinOpType::AND:
        output << "    and a0, a0, t1\n";
        break;
    case BinOpType::OR:
        output << "    or a0, a0, t1\n";
        break;
    }
}

void CodeGenerator::visit(const UnaryOpNode &node)
{
    // Вычисляем операнд
    node.operand->accept(*this);

    // NOT operation: если a0 == 0, то результат 1, иначе 0
    output << "    seqz a0, a0\n";
}

void CodeGenerator::visit(const IntLiteralNode &node)
{
    output << "    li a0, " << node.value << "\n";
}

void CodeGenerator::visit(const StringLiteralNode &node)
{
    std::string label = getStringLabel(node.value);
    output << "    la a0, " << label << "\n";
}

void CodeGenerator::visit(const IdentifierNode &node)
{
    int offset = getVariableOffset(node.name);
    output << "    lw a0, -" << offset << "(sp)   # load " << node.name << "\n";
}