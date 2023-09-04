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
        if (strchr("+-*/()<>", *p)) {
            tail = new_token(tail, TK_SYMBOL, p, 1);
            ++p;
            continue;
        }
        error_at(user_input, tail->str, "Invalid synbol.");
    }

    new_token(tail, TK_END, NULL, 0);
    return head.next;
}

bool consume_sym(Token **token, char *op) {
    if ((*token)->kind == TK_SYMBOL
        && !memcmp((*token)->str, op, (*token)->len)) {
        *token = (*token)->next;
        return true;
    }
    return false;
}

int consume_num(Token **token, char *user_input) {
    if ((*token)->kind == TK_NUM) {
        int val = (*token)->val;
        *token = (*token)->next;
        return val;
    }
    error_at(user_input, (*token)->str, "Here must be a number.");
}