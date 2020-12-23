#include <stdio.h>
#include <unistd.h>
#include <node.h>

int yyparse(struct Node **);

int main(int argc, char **argv) {
  int mode = 1;
  char ch;
  while ((ch = getopt(argc, argv, "tc")) != -1) {
    switch (ch) {
    case 't':
      mode = 0;
      break;
    case 'c':
      mode = 1;
      break;
    default:
      fprintf(stderr, "unknown option: %c\n", ch);
    }
  }
  // 抽象语法树
  struct Node *AST;
  if (yyparse(&AST) == 0) {
    if (mode) {
      genTAC(AST);
    } else {
      printAST(AST);
    }
  }
  return 0;
}