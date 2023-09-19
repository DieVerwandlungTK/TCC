#include "tcc.h"

Token *new_token(Token *token, TokenKind kind, char *str, int len) {
    Token *new = calloc(1, sizeof(Token));

    new->kind = kind;
    new->str = str;
    new->len = len;
    token->next = new;

    return new;
}

Token *tokenize(char *p) {
    Token head;
    Token *tail = &head;
    tail->len = 0;
    while (*p) {
        if(isspace(*p)) {
            ++p;
            continue;
        }
        if(isdigit(*p)) {
            tail = new_token(tail, TK_NUM, p, 0);
            tail->val = strtol(p, &p, 10);
            continue;
        }
        if(!strncmp(p, ">=", 2) || !strncmp(p, "<=", 2) || !strncmp(p, "==", 2)
            || !strncmp(p, "!=", 2)) {
            tail = new_token(tail, TK_SYMBOL, p, 2);
            p += 2;
            continue;
        }
        if(strchr("+-*/()<>=;{},", *p)) {
            tail = new_token(tail, TK_SYMBOL, p, 1);
            ++p;
            continue;
        }
        if(!strncmp(p, "return", 6) && !is_valid_char(p[6])){
            tail = new_token(tail, TK_RET, p, 6);
            p += 6;
            continue;
        }
        if(!strncmp(p, "if", 2) && !is_valid_char(p[2])){
            tail = new_token(tail, TK_IF, p, 2);
            p += 2;
            continue;
        }
        if(!strncmp(p, "else", 4) && !is_valid_char(p[4])){
            tail = new_token(tail, TK_ELS, p, 2);
            p += 4;
            continue;
        }
        if(!strncmp(p, "while", 5) && !is_valid_char(p[5])){
            tail = new_token(tail, TK_WHIL, p, 5);
            p += 5;
            continue;
        }
        if(!strncmp(p, "for", 3) && !is_valid_char(p[3])){
            tail = new_token(tail, TK_FOR, p, 3);
            p += 3;
            continue;
        }
        if(is_valid_char(*p)){
            char *tmp = p;
            while(is_valid_char(*p)) ++p;
            tail = new_token(tail, TK_IDT, tmp, p-tmp);
            continue;
        }
        error_at(tail->str, "Invalid symbol.");
    }

    new_token(tail, TK_END, --p, 0);
    return head.next;
}

bool consume_sym(char *op) {
    if (token->kind==TK_SYMBOL && token->len==strlen(op) && !memcmp(token->str, op, strlen(op))){
        token = token->next;
        return true;
    }
    return false;
}

Token *consume_idt() {
    if (token->kind==TK_IDT){
        Token *idt = token;
        token = token->next;
        return idt;
    }
    else return NULL;
}

int consume_num() {
    if (token->kind == TK_NUM) {
        int val = token->val;
        token = token->next;
        return val;
    }
    error_at(token->str, "Here must be a number.");
}

bool consume_reserved(TokenKind kind){
    if(token->kind == kind){
        token = token->next;
        return true;
    }
    else return false;
}

Lvar *find_Lvar(Token *tok){
    for(LvarList *vl=locals;vl;vl=vl->next){
        Lvar *var = vl->var;
        if(tok->len==var->len && !memcmp(tok->str, var->str, var->len)){
            return var;
        }
    }
    return NULL;
}