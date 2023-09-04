#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TK_SYMBOL, TK_NUM, TK_END } TokenKind;

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
    ND_LTE   // "<="
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Token *new_token(Token *token, TokenKind kind, char *str, int len);
Token *tokenize(char *p, char *user_input);
bool consume_sym(Token **token, char *op);
int consume_num(Token **token, char *user_input);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr(Token **token, char *user_input);
Node *equality(Token **token, char *user_input);
Node *relational(Token **token, char *user_input);
Node *add(Token **token, char *user_input);
Node *mul(Token **token, char *user_input);
Node *unary(Token **token, char *user_input);
Node *primary(Token **token, char *user_input);

void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *user_input, char *loc, char *fmt, ...);