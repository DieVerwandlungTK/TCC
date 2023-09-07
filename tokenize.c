#include "tcc.h"

Token *new_token(Token *token, TokenKind kind, char *str, int len) {
    Token *new = calloc(1, sizeof(Token));

    new->kind = kind;
    new->str = str;
    new->len = len;
    token->next = new;

    return new;
}

Token *tokenize(char *p, char *user_input) {
    Token head;
    Token *tail = &head;
    tail->len = 0;
    while (*p) {
        if (isspace(*p)) {
            ++p;
            continue;
        }
        if (isdigit(*p)) {
            tail = new_token(tail, TK_NUM, p, 0);
            tail->val = strtol(p, &p, 10);
            continue;
        }
        if (!memcmp(p, ">=", 2) || !memcmp(p, "<=", 2) || !memcmp(p, "==", 2)
            || !memcmp(p, "!=", 2)) {
            tail = new_token(tail, TK_SYMBOL, p, 2);
            p += 2;
            continue;
        }
        if (strchr("+-*/()<>=;", *p)) {
            tail = new_token(tail, TK_SYMBOL, p, 1);
            ++p;
            continue;
        }
        if(is_valid_char(*p)){
            char *tmp = p;
            while(is_valid_char(*p)) ++p;
            tail = new_token(tail, TK_IDT, tmp, p-tmp);
            continue;
        }
        error_at(user_input, tail->str, "Invalid symbol.");
    }

    new_token(tail, TK_END, --p, 0);
    return head.next;
}

bool consume_sym(Token **token, char *op) {
    if ((*token)->kind==TK_SYMBOL && (*token)->len==strlen(op) && !memcmp((*token)->str, op, strlen(op))){
        *token = (*token)->next;
        return true;
    }
    return false;
}

Token *consume_idt(Token **token) {
    if ((*token)->kind==TK_IDT){
        Token *idt = *token;
        *token = (*token)->next;
        return idt;
    }
    else return NULL;
}

int consume_num(Token **token, char *user_input) {
    if ((*token)->kind == TK_NUM) {
        int val = (*token)->val;
        *token = (*token)->next;
        return val;
    }
    error_at(user_input, (*token)->str, "Here must be a number.");
}

Lvar *find_Lvar(Token *token, Lvar *locals){
    for(Lvar *var=locals;var;var = var->next){
        if(token->len==var->len && !memcmp(token->str, var->str, var->len)){
            return var;
        }
    }
    return NULL;
}