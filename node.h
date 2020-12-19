struct Node {
  char *name;
  char *token;
  char *place;
  int id;
  int value;
  struct Node *l;
  struct Node *r;
  struct Node *p;
  int *true_list;
  int *false_list;
  int *chain;
  int code_begin;
};

struct Quadruple {
  char *op;
  char *arg1;
  char *arg2;
  char *result;
};

struct Node *new_node(char *, int, ...);

void visitAST(struct Node *, void (*)(struct Node *));

void printNode(struct Node *);

void printAST(struct Node *);

void genTAC(struct Node *);

