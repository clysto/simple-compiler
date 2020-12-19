#include "node.h"
// #include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIST_SIZT 32

int id = 0;
int addr = 0;
int next_instr = 1;
struct Quadruple TAC[128];

char* gen_addr() {
  char* ret = (char*)malloc(sizeof(char) * 8);
  sprintf(ret, "t%d", addr++);
  return ret;
}

int get_next_instr() { return next_instr++; }

struct Node* get_child_node(struct Node* root, int index) {
  struct Node* temp = root->l;
  for (int i = 0; i < index - 1; ++i) {
    temp = temp->r;
  }
  return temp;
}

struct Node* new_node(char* name, int children_num, ...) {
  va_list valist;
  struct Node* node = (struct Node*)malloc(sizeof(struct Node));
  struct Node* temp;
  va_start(valist, children_num);
  node->name = name;
  node->l = NULL;
  node->r = NULL;
  node->p = NULL;
  node->token = NULL;
  node->id = id++;
  // children_num > 0 非终结符
  if (children_num > 0) {
    temp = va_arg(valist, struct Node*);
    node->l = temp;
    // 记录父节点指针
    temp->p = node;

    // 孩子兄弟表示法
    if (children_num >= 2) {
      for (int i = 0; i < children_num - 1; ++i) {
        temp->r = va_arg(valist, struct Node*);
        // 记录父节点指针
        temp = temp->r;
        temp->p = node;
      }
    }
  }
  // children_num == 0 终结符
  else {
    char* yytext = va_arg(valist, char*);
    char* token = (char*)malloc(sizeof(char) * strlen(yytext));
    strcpy(token, yytext);
    node->token = token;
  }
  va_end(valist);
  return node;
}

// 后序遍历语法树
void visitAST(struct Node* root, void (*visitor)(struct Node*)) {
  struct Node* temp = root->l;
  while (temp) {
    visitAST(temp, visitor);
    temp = temp->r;
  }
  visitor(root);
}

void print_node(struct Node* node) {
  if (node->token != NULL) {
    // 终结符
    printf("\t%d [label=\"%s\"]\n", node->id, node->token);
  } else {
    // 非终结符
    printf("\t%d [label=\"%s\"]\n", node->id, node->name);
  }
  // 建立父子关系
  if (node->p != NULL) {
    printf("\t%d -> %d\n", node->p->id, node->id);
  }
}

void printAST(struct Node* AST) {
  printf("digraph AST {\n");
  printf("\tnode [fontname = Consolas, shape = box]\n");
  printf("\tedge [fontname = Consolas]\n");
  visitAST(AST, print_node);
  printf("}\n");
}

void gen(char* op, char* arg1, char* arg2, char* result) {
  int instr = get_next_instr();
  struct Quadruple q;
  q.op = op;
  q.arg1 = arg1;
  q.arg2 = arg2;
  q.result = result;
  TAC[instr] = q;
}

// 回填
void back_patch(int* list, int val) {
  if (!list) return;
  int i = 0;
  char* result = (char*)malloc(sizeof(char) * 8);
  sprintf(result, "%d", val);
  while (list[i]) {
    TAC[list[i]].result = result;
    i++;
  }
  free(list);
}

int* make_list(int val) {
  int* ret = (int*)malloc(sizeof(int) * LIST_SIZT);
  for (int i = 0; i < LIST_SIZT; ++i) ret[i] = 0;
  ret[0] = val;
  return ret;
}

int* merge_list(int* list1, int* list2) {
  int end = 0, start = 0;
  while (list1[end]) end++;
  while (list2[start]) list1[end++] = list2[start++];
  free(list2);
  return list1;
}

void genTAC_visitor(struct Node* node) {
  if (strcmp(node->name, "算数表达式") == 0) {
    node->place = gen_addr();
    gen(get_child_node(node, 2)->token, get_child_node(node, 1)->place,
        get_child_node(node, 3)->place, node->place);

  } else if (strcmp(node->name, "常量表达式") == 0) {
    node->place = gen_addr();
    char* arg1 = (char*)malloc(sizeof(char) * 8);
    sprintf(arg1, "%d", node->l->value);
    gen(":=", arg1, "-", node->place);

  } else if (strcmp(node->name, "变量表达式") == 0) {
    node->place = node->l->token;

  } else if (strcmp(node->name, "取负表达式") == 0) {
    node->place = gen_addr();
    gen("neg", get_child_node(node, 2)->place, "-", node->place);

  } else if (strcmp(node->name, "赋值表达式") == 0) {
    node->place = gen_addr();
    gen(":=", get_child_node(node, 3)->place, "-", node->l->token);

  } else if (strcmp(node->name, "括号表达式") == 0) {
    node->place = get_child_node(node, 2)->place;

  } else if (strcmp(node->name, "关系表达式") == 0) {
    char* op = (char*)malloc(sizeof(char) * 8);
    // 拉链
    node->true_list = make_list(next_instr);
    node->code_begin = next_instr;
    node->false_list = make_list(next_instr + 1);
    sprintf(op, "j%s", get_child_node(node, 2)->token);
    gen(op, node->l->place, get_child_node(node, 3)->place, "*");
    gen("jmp", "-", "-", "*");

  } else if (strcmp(node->name, "分支语句头") == 0) {
    // 回填
    back_patch(get_child_node(node, 2)->true_list, next_instr);
    // 保存假链到node
    node->chain = get_child_node(node, 2)->false_list;

  } else if (strcmp(node->name, "逻辑或") == 0) {
    // 回填假链
    back_patch(node->l->false_list, get_child_node(node, 3)->code_begin);
    node->code_begin = node->l->code_begin;
    // 合并真链
    node->true_list =
        merge_list(node->l->true_list, get_child_node(node, 3)->true_list);
    // 指定node的假链
    node->false_list = get_child_node(node, 3)->false_list;

  } else if (strcmp(node->name, "逻辑与") == 0) {
    // 回填真链
    back_patch(node->l->true_list, get_child_node(node, 3)->code_begin);
    node->code_begin = node->l->code_begin;
    // 合并假链
    node->false_list =
        merge_list(node->l->false_list, get_child_node(node, 3)->false_list);
    // 指定node的真链
    node->true_list = get_child_node(node, 3)->true_list;

  } else if (strcmp(node->name, "括号布尔") == 0) {
    // 直接继承综合属性
    node->true_list = get_child_node(node, 2)->true_list;
    node->false_list = get_child_node(node, 2)->false_list;
    node->code_begin = get_child_node(node, 2)->code_begin;

  } else if (strcmp(node->name, "逻辑反") == 0) {
    // 交换真链假链
    node->true_list = get_child_node(node, 2)->false_list;
    node->false_list = get_child_node(node, 2)->true_list;
    node->code_begin = get_child_node(node, 2)->code_begin;

  } else if (strcmp(node->name, "真标志") == 0) {
    node->true_list = make_list(next_instr);
    node->false_list = make_list(0);
    node->code_begin = next_instr;
    gen("jmp", "-", "-", "*");

  } else if (strcmp(node->name, "假标志") == 0) {
    node->false_list = make_list(next_instr);
    node->true_list = make_list(0);
    node->code_begin = next_instr;
    gen("jmp", "-", "-", "*");

  } else if (strcmp(node->name, "分支语句") == 0) {
    // 找到假出口, 回填假链
    back_patch(node->l->chain, next_instr);
  }
}

void genTAC(struct Node* AST) {
  visitAST(AST, genTAC_visitor);
  for (int i = 1; i < next_instr; ++i) {
    printf("%3d| %3s, %4s, %4s, %4s |\n", i, TAC[i].op, TAC[i].arg1,
           TAC[i].arg2, TAC[i].result);
  }
}
