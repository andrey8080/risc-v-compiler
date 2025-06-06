%{
#include "ast.hpp"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

// Forward declarations
class ASTNode;
class ProgramNode;

extern int yylineno;
void yyerror(const char* s);
extern int yylex();

// External reference to the parsed program
extern ProgramNode* parsed_program;
%}

%union {
    int int_val;
    char* str_val;
    void* node;
}

%define parse.error verbose

%token <int_val> INTEGER
%token <str_val> STRING
%token <str_val> IDENTIFIER

%token INT_TYPE STRING_TYPE IF ELSE WHILE PRINT
%token NOT AND OR
%token EQ NEQ LE GE LT GT
%token PLUS MINUS MUL DIV ASSIGN
%token SEMI LBRACE RBRACE LPAREN RPAREN

%type <node> statement expr program statement_list

%left OR
%left AND
%left EQ NEQ
%left LT LE GT GE
%left PLUS MINUS
%left MUL DIV
%right NOT

%%

program:
      statement { 
        if ($1 != nullptr) {
          $$ = (void*)new ProgramNode(); 
          ((ProgramNode*)$$)->addStatement(std::unique_ptr<ASTNode>((ASTNode*)$1));
          parsed_program = (ProgramNode*)$$;
        } else {
          YYERROR;
        }
      }
    | program statement { 
        if ($1 != nullptr && $2 != nullptr) {
          ((ProgramNode*)$1)->addStatement(std::unique_ptr<ASTNode>((ASTNode*)$2)); 
          $$ = $1; 
          parsed_program = (ProgramNode*)$$;
        } else {
          YYERROR;
        }
      }
    ;

statement:
      INT_TYPE IDENTIFIER SEMI { 
        $$ = (void*)new VarDeclNode(VarType::INT, std::string($2)); 
        free($2);
      }
    | STRING_TYPE IDENTIFIER SEMI { 
        $$ = (void*)new VarDeclNode(VarType::STRING, std::string($2)); 
        free($2);
      }
    | IDENTIFIER ASSIGN expr SEMI { 
        auto* assign_node = new AssignNode(std::string($1), std::unique_ptr<ASTNode>((ASTNode*)$3));
        $$ = (void*)assign_node;
        free($1);
      }
    | PRINT expr SEMI { 
        $$ = (void*)new PrintNode(std::unique_ptr<ASTNode>((ASTNode*)$2)); 
      }
    | IF LPAREN expr RPAREN statement { 
        $$ = (void*)new IfNode(std::unique_ptr<ASTNode>((ASTNode*)$3), std::unique_ptr<ASTNode>((ASTNode*)$5)); 
      }
    | IF LPAREN expr RPAREN statement ELSE statement { 
        $$ = (void*)new IfNode(std::unique_ptr<ASTNode>((ASTNode*)$3), std::unique_ptr<ASTNode>((ASTNode*)$5), std::unique_ptr<ASTNode>((ASTNode*)$7)); 
      }
    | WHILE LPAREN expr RPAREN statement { 
        $$ = (void*)new WhileNode(std::unique_ptr<ASTNode>((ASTNode*)$3), std::unique_ptr<ASTNode>((ASTNode*)$5)); 
      }
    | LBRACE statement_list RBRACE { 
        $$ = $2;  // statement_list уже возвращает BlockNode
      }
    ;

statement_list:
      /* empty */ {
        $$ = (void*)new BlockNode();
      }
    | statement_list statement {
        ((BlockNode*)$1)->addStatement(std::unique_ptr<ASTNode>((ASTNode*)$2));
        $$ = $1;
      }
    ;

expr:
      expr PLUS expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::PLUS, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr MINUS expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::MINUS, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr MUL expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::MUL, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr DIV expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::DIV, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr EQ expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::EQ, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr NEQ expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::NEQ, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr LE expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::LE, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr GE expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::GE, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr LT expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::LT, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr GT expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::GT, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr AND expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::AND, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | expr OR expr { 
        $$ = (void*)new BinaryOpNode(BinOpType::OR, std::unique_ptr<ASTNode>((ASTNode*)$1), std::unique_ptr<ASTNode>((ASTNode*)$3)); 
      }
    | NOT expr { 
        $$ = (void*)new UnaryOpNode(std::unique_ptr<ASTNode>((ASTNode*)$2)); 
      }
    | LPAREN expr RPAREN { 
        $$ = $2; 
      }
    | INTEGER { 
        $$ = (void*)new IntLiteralNode($1); 
      }
    | STRING { 
        $$ = (void*)new StringLiteralNode(std::string($1)); 
        free($1);
      }
    | IDENTIFIER { 
        $$ = (void*)new IdentifierNode(std::string($1)); 
        free($1);
      }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parser error at line %d: %s\n", yylineno, s);
}
