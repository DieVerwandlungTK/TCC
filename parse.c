#include "tcc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *new = calloc(1, sizeof(Node));
    new->kind = kind;
    new->lhs = lhs;
    new->rhs = rhs;

    return new;
}

Node *new_node_num(int val) {
    Node *new = calloc(1, sizeof(Node));
    new->kind = ND_NUM;
    new->val = val;

    return new;
}

Node **program(Token **token, char *user_input, Lvar **locals) { 
    Node **stmts = calloc(MAX_STMTS, sizeof(Node*));

    int i = 0;
    while((*token)->kind!=TK_END){
        stmts[i] = stmt(token, user_input, locals);
        ++i;
    }

    stmts[i] = NULL;
    return stmts;
}

Node *stmt(Token **token, char *user_input, Lvar **locals){
    Node *node;
    if(consume_ret(token)){
        node = new_node(ND_RET, expr(token, user_input, locals), NULL);
    }
    else node = expr(token, user_input, locals);
    if(consume_sym(token, ";")) return node;
    else error_at(user_input, (*token)->str, "Missing ';'.");
}

Node *expr(Token **token, char *user_input, Lvar **locals) {
    return assign(token, user_input, locals);
}

Node *assign(Token **token, char *user_input, Lvar **locals){
    Node *node = equality(token, user_input, locals);

    if(consume_sym(token, "=")){
        return new_node(ND_ASN, node, assign(token, user_input, locals));
    }
    else return node;
}

Node *equality(Token **token, char *user_input, Lvar **locals) {
    Node *node = relational(token, user_input, locals);

    while (1) {
        if (consume_sym(token, "==")) {
            node = new_node(ND_EQ, node, mul(token, user_input, locals));
            continue;
        }
        if (consume_sym(token, "!=")) {
            node = new_node(ND_NEQ, node, mul(token, user_input, locals));
            continue;
        }
        return node;
    }
}

Node *relational(Token **token, char *user_input, Lvar **locals) {
    Node *node = add(token, user_input, locals);

    while (1) {
        if (consume_sym(token, "<")) {
            node = new_node(ND_LT, node, add(token, user_input, locals));
            continue;
        }
        if (consume_sym(token, "<=")) {
            node = new_node(ND_LTE, node, add(token, user_input, locals));
            continue;
        }
        if (consume_sym(token, ">")) {
            node = new_node(ND_LT, add(token, user_input, locals), node);
            continue;
        }
        if (consume_sym(token, ">=")) {
            node = new_node(ND_LTE, add(token, user_input, locals), node);
            continue;
        }
        return node;
    }
}

Node *add(Token **token, char *user_input, Lvar **locals) {
    Node *node = mul(token, user_input, locals);

    while (1) {
        if (consume_sym(token, "+")) {
            node = new_node(ND_ADD, node, mul(token, user_input, locals));
            continue;
        }
        if (consume_sym(token, "-")) {
            node = new_node(ND_SUB, node, mul(token, user_input, locals));
            continue;
        }
        return node;
    }
}

Node *mul(Token **token, char *user_input, Lvar **locals) {
    Node *node = unary(token, user_input, locals);

    while (1) {
        if (consume_sym(token, "*")) {
            node = new_node(ND_MUL, node, unary(token, user_input, locals));
            continue;
        }
        if (consume_sym(token, "/")) {
            node = new_node(ND_DIV, node, unary(token, user_input, locals));
            continue;
        }
        return node;
    }
}

Node *unary(Token **token, char *user_input, Lvar **locals) {
    if (consume_sym(token, "+")) {
        return primary(token, user_input, locals);
    }
    if (consume_sym(token, "-")) {
        return new_node(ND_SUB, new_node_num(0), primary(token, user_input, locals));
    }
    return primary(token, user_input, locals);
}

Node *primary(Token **token, char *user_input, Lvar **locals) {
    if (consume_sym(token, "(")) {
        Node *node = expr(token, user_input, locals);

        if (consume_sym(token, ")")) {
            return node;
        }
        else {
            error_at(user_input, (*token)->str,
                     "The location of ')' is unknown.");
        }
    }

    Token *idt = consume_idt(token);
    if(idt){
        Node *node = new_node(ND_LVAR, NULL, NULL);
        Lvar *var = find_Lvar(idt, *locals);
        if(var) node->offset = var->offset;
        else{
            var = calloc(1, sizeof(Lvar));
            var->next = *locals;
            var->str = idt->str;
            var->len = idt->len;
            var->offset = (*locals)->offset + 8;
            node->offset = var->offset;
            *locals = var;
        }
        return node;
    }
    else {
        return new_node_num(consume_num(token, user_input));
    }
}