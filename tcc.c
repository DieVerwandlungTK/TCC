#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

typedef enum{
    TK_SYMBOL,
    TK_NUM,
    TK_END
} TokenKind;

typedef struct Token Token;
struct Token{
    TokenKind kind;
    Token *next;
    char *str;
    int val;
};

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM
} NodeKind;

typedef struct Node Node;
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Token *new_token(Token *token, TokenKind kind, char *str);
Token *tokenize(char *p, char *user_input);
bool consume_sym(Token **token, char op);
int consume_num(Token **token, char *user_input);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr(Token **token, char *user_input);
Node *mul(Token **token, char *user_input);
Node *unary(Token **token, char *user_input);
Node *primary(Token **token, char *user_input);

void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *user_input, char *loc, char *fmt, ...);


int main(int argc, char **argv){
    if(argc!=2){
        error("The number of arguments is %d, but must be 2.\n", argc);
    }

    char *user_input = argv[1];

    Token *token = tokenize(user_input, user_input);
    Node *node = expr(&token, user_input);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");

    return 0;
}

Token *new_token(Token *token, TokenKind kind, char *str){
    Token *new = calloc(1, sizeof(Token));

    new->kind = kind;
    new->str = str;
    token->next = new;

    return new;
}

Token *tokenize(char *p, char *user_input){
    Token head;
    Token *tail = &head;

    while(*p){
        if(isspace(*p)){
            ++p;
            continue;
        }
        if(isdigit(*p)){
            tail = new_token(tail, TK_NUM, p);
            tail->val = strtol(p, &p, 10);
            continue;
        }
        else if(strchr("+-*/()", *p)){
            tail = new_token(tail, TK_SYMBOL, p);
            ++p;
            continue;
        }
        else error_at(user_input, tail->str, "Invalid synbol.");
    }

    new_token(tail, TK_END, NULL);
    return head.next;
}

bool consume_sym(Token **token, char op){
    if((*token)->kind==TK_SYMBOL && (*token)->str[0]==op){
        *token = (*token)->next;
        return true;
    }
    return false;
}

int consume_num(Token **token, char* user_input){
    if((*token)->kind==TK_NUM){
        int val = (*token)->val;
        *token = (*token)->next;
        return val;
    }
    error_at(user_input, (*token)->str, "Here must be a number.");
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *new = calloc(1, sizeof(Node));
    new->kind = kind;
    new->lhs = lhs;
    new->rhs = rhs;

    return new;
}

Node *new_node_num(int val){
    Node *new = calloc(1, sizeof(Node));
    new->kind = ND_NUM;
    new->val = val;

    return new;
}

Node *expr(Token **token, char *user_input){
    Node *node = mul(token, user_input);

    while(1){
        if(consume_sym(token, '+')){
            node = new_node(ND_ADD, node, mul(token, user_input));
            continue;
        }
        if(consume_sym(token, '-')){
            node = new_node(ND_SUB, node, mul(token, user_input));
            continue;
        }
        return node;
    }
}

Node *mul(Token **token, char *user_input){
    Node *node = unary(token, user_input);

    while(1){
        if(consume_sym(token, '*')){
            node = new_node(ND_MUL, node, unary(token, user_input));
            continue;
        }
        if(consume_sym(token, '/')){
            node = new_node(ND_DIV, node, unary(token, user_input));
            continue;
        }
        return node;
    }
}

Node *unary(Token **token, char *user_input){
    if(consume_sym(token, '+')){
        return primary(token, user_input);
    }
    if(consume_sym(token, '-')){
        return new_node(ND_SUB, new_node_num(0), primary(token, user_input));
    }
    return primary(token, user_input);
}

Node *primary(Token **token, char *user_input){
    if(consume_sym(token, '(')){
        Node *node = expr(token, user_input);

        if(consume_sym(token, ')')){
            return node;
        }
        else{
            error_at(user_input, (*token)->str, "The location of ')' is unknown.");
        }
    }
    else{
        Node *node = new_node_num(consume_num(token, user_input));
        return node;
    }
}

void gen(Node *node){
    if(node->kind==ND_NUM){
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");
    switch (node->kind){
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;

        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;

        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;

        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
    }
    printf("    push rax\n");
}

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *user_input, char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}