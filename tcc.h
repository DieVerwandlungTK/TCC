#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STMTS 100

typedef enum {
    TK_SYMBOL,
    TK_NUM,
    TK_IDT,
    TK_END
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    char *str;
    int len;
    int val;
};

typedef enum {
    ND_ADD,  // "+"
    ND_SUB,  // "-"
    ND_MUL,  // "*"
    ND_DIV,  // "/"
    ND_NUM,  // Integer
    ND_EQ,   // "=="
    ND_NEQ,  // "!="
    ND_LT,   // "<"
    ND_LTE,  // "<="
    ND_ASN,  // "="
    ND_LVAR  // "Local variable"
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

typedef struct Lvar Lvar;
struct Lvar{
    Lvar *next;
    char *str;
    int len;
    int offset;
};

Token *new_token(Token *token, TokenKind kind, char *str, int len);
Token *tokenize(char *p, char *user_input);
bool consume_sym(Token **token, char *op);
int consume_num(Token **token, char *user_input);
Token *consume_idt(Token **token);
Lvar *find_Lvar(Token *token, Lvar *locals);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node **program(Token **token, char *user_input, Lvar **locals);
Node *stmt(Token **token, char *user_input, Lvar **locals);
Node *expr(Token **token, char *user_input, Lvar **locals);
Node *assign(Token **token, char *user_input, Lvar **locals);
Node *equality(Token **token, char *user_input, Lvar **locals);
Node *relational(Token **token, char *user_input, Lvar **locals);
Node *add(Token **token, char *user_input, Lvar **locals);
Node *mul(Token **token, char *user_input, Lvar **locals);
Node *unary(Token **token, char *user_input, Lvar **locals);
Node *primary(Token **token, char *user_input, Lvar **locals);

void gen_lval(Node *node);
void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *user_input, char *loc, char *fmt, ...);
bool is_valid_char(char c);