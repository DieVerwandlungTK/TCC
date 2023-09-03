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

Token *new_token(Token *token, TokenKind kind, char *str);
Token *tokenize(char *p, char *user_input);
bool consume_sym(Token **token, char op);
int consume_num(Token **token, char *user_input);
void error(char *fmt, ...);
void error_at(char *user_input, char *loc, char *fmt, ...);

int main(int argc, char **argv){
    if(argc!=2){
        error("The number of arguments is %d, but must be 2.\n", argc);
    }

    char *user_input = argv[1];

    Token *token = tokenize(user_input, user_input);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("    mov rax, %d\n", consume_num(&token, user_input));

    while((token->kind)!=TK_END){
        if(consume_sym(&token, '+')){
            printf("    add rax, %d\n", consume_num(&token, user_input));
            continue;
        }
        if(consume_sym(&token, '-')){
            printf("    sub rax, %d\n", consume_num(&token, user_input));
            continue;
        }
        error_at(user_input, token->str, "Invalid syntax.");
    }

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
        else if(strchr("+-", *p)){
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