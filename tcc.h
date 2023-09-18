#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STMTS 1000
#define MAX_BLK_STMTS 100

typedef enum {
    TK_SYMBOL,
    TK_NUM,
    TK_IDT,
    TK_IF,
    TK_ELS,
    TK_WHIL,
    TK_FOR,
    TK_RET,
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
    ND_LVAR, // "local variable"
    ND_FNC,  // "function"
    ND_IF,   // "if"
    ND_WHIL, // "while"
    ND_FOR,  // "for"
    ND_RET,  // "return"
    ND_BLK
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    
    Node *lhs;
    Node *rhs;

    Node *then;
    Node *cond;
    Node *els;

    Node *init;
    Node *inc;

    Node **blk;
    int blk_len;
    
    int val;

    int offset;

    Node *args;
    Node *next;
    char *str;
    int str_len;
};

typedef struct Lvar Lvar;
struct Lvar{
    Lvar *next;
    char *str;
    int len;
    int offset;
};

typedef struct Alc Alc;
struct Alc{
    Alc *bottom;
    int val;
};

Token *new_token(Token *tok, TokenKind kind, char *str, int len);   // Return a new token.
Token *tokenize(char *p);   // Return a token string generated from a input string.

bool consume_sym(char *op);     // Consume a designated token.
Token *consume_idt();   // Consume an idntifier
int consume_num();    // Consume a number.
bool consume_reserved(TokenKind kind);      // Consume a designated reserved word.

Lvar *find_Lvar(Token *tok);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *fnc_args();

void gen_lval(Node *node);
void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

bool is_valid_char(char c);

extern Token *token;
extern char *user_input;
extern Lvar *locals;
extern Node **code;
extern int label;
extern Alc *lvar_alloc;
extern const char *arg_reg[];