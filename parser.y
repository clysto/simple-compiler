%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#define YYDEBUG 1
void yyerror(struct Node **AST ,const char *msg);
int yylex (void);
%}

%define parse.error verbose

%parse-param {struct Node **AST}

%union {
  struct Node* n;
}

// 终结符
%token <n> T_LPAREN    "("
%token <n> T_RPAREN    ")"
%token <n> T_PLUS      "+"
%token <n> T_MINUS     "-"
%token <n> T_MUL       "*"
%token <n> T_DIV       "/"
%token <n> T_BOOLEAN   "boolean"
%token <n> T_ASSIGN    ":="
%token <n> T_NUMBER    "number"
%token <n> T_ID        "identifier"
%token <n> T_SEMI      ";"
%token <n> T_OR        "||"
%token <n> T_AND       "&&"
%token <n> T_NOT       "!"
%token <n> T_BEGIN     "begin"
%token <n> T_END       "end"
%token <n> T_TRUE      "true"
%token <n> T_FALSE     "false"
%token <n> T_IF        "if"
%token <n> T_THEN      "then"
%token <n> T_LT        "<"
%token <n> T_LE        "<="
%token <n> T_GT        ">"
%token <n> T_GE        ">="

// 非终结符
%type <n> Q
%type <n> S
%type <n> C
%type <n> K
%type <n> A
%type <n> L
%type <n> E
%type <n> B


// 算符优先级
%left  T_PLUS T_MINUS
%left  T_MUL T_DIV
%right UMINUS
%left  T_AND T_OR T_NOT

%%
Q   :   S                          {$$=new_node("程序", 1, $1); *AST=$$;}  
    ;

S   :   C S                        {$$=new_node("分支语句", 2, $1, $2);}  
    |   T_BEGIN K T_END            {$$=new_node("语句", 3, $1, $2, $3);}   
    |   A                          {$$=new_node("语句", 1, $1);} 
    ;

C   :   T_IF B T_THEN              {$$=new_node("分支语句头", 3, $1, $2, $3);}      
    ;

L   :   K S                        {$$=new_node("复合语句", 2, $1, $2);}     
    |   S                          {$$=new_node("复合语句", 1, $1);} 
    ;

K   :   L T_SEMI                   {$$=new_node("带分号的语句", 2, $1, $2);}
    ;

A   :   T_ID T_ASSIGN E            {$$=new_node("赋值表达式", 3, $1, $2, $3);}
    ;

E   :   E T_PLUS E                 {$$=new_node("算数表达式", 3, $1, $2, $3);}
    |   E T_MINUS E                {$$=new_node("算数表达式", 3, $1, $2, $3);}
    |   E T_MUL E                  {$$=new_node("算数表达式", 3, $1, $2, $3);}
    |   E T_DIV E                  {$$=new_node("算数表达式", 3, $1, $2, $3);}
    |   T_MINUS E %prec UMINUS     {$$=new_node("取负表达式", 2, $1, $2);}
    |   T_LPAREN E T_RPAREN        {$$=new_node("括号表达式", 3, $1, $2, $3);}
    |   T_ID                       {$$=new_node("变量表达式", 1, $1);}
    |   T_NUMBER                   {$$=new_node("常量表达式", 1, $1);}
    ;

B   :   B T_OR B                   {$$=new_node("逻辑或", 3, $1, $2, $3);}   
    |   B T_AND B                  {$$=new_node("逻辑与", 3, $1, $2, $3);}   
    |   T_NOT B                    {$$=new_node("逻辑反", 2, $1, $2);}   
    |   T_LPAREN B T_RPAREN        {$$=new_node("括号布尔", 3, $1, $2, $3);} 
    |   E T_LT E                   {$$=new_node("关系表达式", 3, $1, $2, $3);} 
    |   E T_LE E                   {$$=new_node("关系表达式", 3, $1, $2, $3);} 
    |   E T_GT E                   {$$=new_node("关系表达式", 3, $1, $2, $3);} 
    |   E T_GE E                   {$$=new_node("关系表达式", 3, $1, $2, $3);}
    |   T_TRUE                     {$$=new_node("真标志", 1, $1);}
    |   T_FALSE                    {$$=new_node("假标志", 1, $1);} 
    ;

%%
