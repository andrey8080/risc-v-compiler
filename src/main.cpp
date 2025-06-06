#include "ast.hpp"
#include "codegen.hpp"
#include <cstdio>
#include <iostream>
#include <fstream>

extern int yyparse();
extern FILE *yyin;
extern int yydebug;

int main(int argc, char *argv[])
{
    // Инициализируем корень AST
    root.reset();

    // Включаем отладку парсера
    // yydebug = 1;

    // Если передан входной файл
    if (argc > 1)
    {
        yyin = fopen(argv[1], "r");
        if (!yyin)
        {
            printf("Error: Could not open input file %s\n", argv[1]);
            return 1;
        }
        printf("Opened file: %s\n", argv[1]);
    }

    printf("Starting parsing...\n");
    int res = yyparse();
    printf("Parsing result: %d\n", res);

    // Закрываем входной файл
    if (argc > 1 && yyin)
    {
        fclose(yyin);
    }

    // Переносим владение из parsed_program в root
    if (res == 0 && parsed_program != nullptr)
    {
        root.reset(parsed_program);
        parsed_program = nullptr;
    }

    if (res == 0 && root)
    {
        printf("Parsing completed successfully.\n");

        // Визуализация AST
        printf("\n=== AST ===\n");
        root->print();

        // Генерация кода
        printf("\n=== Generated RISC-V Assembly ===\n");
        if (argc > 2)
        {
            // Если задан файл для вывода
            std::ofstream outFile(argv[2]);
            if (outFile.is_open())
            {
                CodeGenerator generator(outFile);
                generator.generateCode(*root);
                outFile.close();
                printf("Assembly code written to %s\n", argv[2]);
            }
            else
            {
                printf("Error: Could not open output file %s\n", argv[2]);
                return 1;
            }
        }
        else
        {
            // Выводим в stdout
            CodeGenerator generator(std::cout);
            generator.generateCode(*root);
        }
    }
    else
    {
        printf("Parsing failed.\n");
    }
    return res;
}
