%{
#include <parser.tab.h>
#include <node.h>
#include <stdlib.h>
int cur_line = 1;
void yyerror(struct Node **AST, const char *msg);
void unrecognized_char(char c);
%}


delim       [ \t]
ws          {delim}+
letter      [A-Za-z]
digit       [0-9]
id          {letter}({letter}|{digit})*
number      {digit}+(\.{digit}+)?


%%
{ws}        /* ignore white spaces */
"("         yylval.n=new_node("左括号", 0, yytext); return T_LPAREN;
")"         yylval.n=new_node("右括号", 0, yytext); return T_RPAREN;
"+"         yylval.n=new_node("算数运算符", 0, yytext); return T_PLUS;
"-"         yylval.n=new_node("算数运算符", 0, yytext); return T_MINUS;
"*"         yylval.n=new_node("算数运算符", 0, yytext); return T_MUL;
"/"         yylval.n=new_node("算数运算符", 0, yytext); return T_DIV;
"||"        yylval.n=new_node("逻辑运算符", 0, yytext); return T_OR;
"&&"        yylval.n=new_node("逻辑运算符", 0, yytext); return T_AND;
"!"         yylval.n=new_node("逻辑运算符", 0, yytext); return T_NOT;
"<"         yylval.n=new_node("关系运算符", 0, yytext); return T_LT;
"<="        yylval.n=new_node("关系运算符", 0, yytext); return T_LE;
">"         yylval.n=new_node("关系运算符", 0, yytext); return T_GT;
">="        yylval.n=new_node("关系运算符", 0, yytext); return T_GE;
"begin"     yylval.n=new_node("代码段开始", 0, yytext); return T_BEGIN;
"end"       yylval.n=new_node("代码段结束", 0, yytext); return T_END;
"true"      yylval.n=new_node("真", 0, yytext); return T_TRUE;
"false"     yylval.n=new_node("假", 0, yytext); return T_FALSE;
"if"        yylval.n=new_node("如果", 0, yytext); return T_IF;
"then"      yylval.n=new_node("然后", 0, yytext); return T_THEN;
":="        yylval.n=new_node("赋值符号", 0, yytext); return T_ASSIGN;
{number}    yylval.n=new_node("数字", 0, yytext); yylval.n->value=atoi(yytext); return T_NUMBER;
{id}        yylval.n=new_node("标识符", 0, yytext); return T_ID;
";"         yylval.n=new_node("分号", 0, yytext); return T_SEMI;
\n          cur_line++;
.           unrecognized_char(yytext[0]);
%%


int yywrap(void) {
  return 1;
}

void unrecognized_char(char c) {
  char buf[32] = "unrecognized character: ?";
  buf[24] = c;
  yyerror(NULL, buf);
}

void yyerror(struct Node **AST ,const char *msg) {
  fprintf(stderr, "Error at line %d:\n\t%s\n", cur_line, msg);
  exit(1);
}
