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

Func *program() { 
    int i=0;
    Func head;
    Func *tail = &head;
    while(token->kind!=TK_END){
        if(i>=MAX_STMTS) error("Too many statements.\n");
        locals = NULL;
        tail->next = function();
        tail = tail->next;
        ++i;
    }
    return head.next;
}

LvarList *read_args(){
    Token *idt = consume_idt();
    if(!idt) error_at(token->str, "Here must be an argument idntifier.");

    Lvar *var = calloc(1, sizeof(Lvar));
    var->str = idt->str;
    var->len = idt->len;

    LvarList head;
    LvarList *tail = head.next = calloc(1, sizeof(LvarList));
    tail->var = var;

    LvarList *vl = calloc(1, sizeof(LvarList));
    vl->var = var;
    locals = vl;

    while(!consume_sym(")")){
        if(consume_sym(",")){
            Token *idt = consume_idt();
            if(!idt) error_at(token->str, "Here must be an argument idntifier.");

            var = calloc(1, sizeof(Lvar));
            var->str = idt->str;
            var->len = idt->len;
            
            tail->next = calloc(1, sizeof(LvarList));
            tail = tail->next;
            tail->var = var;

            vl = calloc(1, sizeof(LvarList));
            vl->var = var;
            vl->next = locals;
            locals = vl;
        }
        else error_at(token->str, "Missing ','.");
    }
    return head.next;
}

Func *function(){
    Token *idt = consume_idt();
    if(!idt) error_at(token->str, "Here must be a function idntifier.");
    if(!consume_sym("(")) error_at(token->str, "Missing '('.");

    Func *func = calloc(1, sizeof(Func));
    func->str = idt->str;
    func->len = idt->len;
    if(!consume_sym(")")) func->args = read_args();
    if(consume_sym("{")){
        Node head;
        Node *tail = &head;
        while(!consume_sym("}")){
            tail->next = stmt();
            tail = tail->next;
        }
        func->node = head.next;
    }
    else{
        func->node = stmt();
    }
    func->locals = locals;
    return func;
}

Node *stmt(){
    Node *node;

    if(consume_reserved(TK_RET)){
        node = new_node(ND_RET, expr(), NULL);

        if(consume_sym(";")) return node;
        else error_at(token->str, "Missing ';'.");
    }

    else if(consume_reserved(TK_IF)){
        node = new_node(ND_IF, NULL, NULL);

        if(consume_sym("(")){
            node->cond = expr();
            if(consume_sym(")")){
                node->then = stmt();
                if(consume_reserved(TK_ELS)){
                    node->els = stmt();
                    return node;
                }
                else return node;
            }
            else error_at(token->str, "Missing ')'.");
        }
        else error_at(token->str, "Missing '('.");
    }

    else if(consume_reserved(TK_WHIL)){
        node = new_node(ND_WHIL, NULL, NULL);

        if(consume_sym("(")){
            node->cond = expr();
            if(consume_sym(")")){
                node->then = stmt();
                return node;
            }
            else error_at(token->str, "Missing ')'.");
        }
        else error_at(token->str, "Missing '('.");
    }
    else if(consume_reserved(TK_FOR)){
        node = new_node(ND_FOR, NULL, NULL);
        
        if(consume_sym("(")){
            if(!consume_sym(";")){
                node->init = expr();
                if(!consume_sym(";")) error_at(token->str, "Missing ';'.");
            }
            if(!consume_sym(";")){
                node->cond = expr();
                if(!consume_sym(";")) error_at(token->str, "Missing ';'.");
            }

            if(consume_sym(")")){
                node->then = stmt();
                return node;
            }
            else{
                node->inc = expr();
                if(consume_sym(")")){
                    node->then = stmt();
                    return node;
                }
                else error_at(token->str, "Missing ')'.");
            }
        }
        else error_at(token->str, "Missing '('.");
    }
    else if(consume_sym("{")){
        node = new_node(ND_BLK, NULL, NULL);
        node->blk = calloc(MAX_BLK_STMTS, sizeof(*node));
        while(!consume_sym("}")){
            if(node->blk_len>=MAX_BLK_STMTS) error_at(token->str, "Too many statements in this block.");
            (node->blk)[node->blk_len] = stmt();
            ++(node->blk_len);
        }
        return node;
    }
    else{
        node = expr();
        if(consume_sym(";")) return node;
        else error_at(token->str, "Missing ';'.");
    }
}

Node *expr() {
    return assign();
}

Node *assign(){
    Node *node = equality();

    if(consume_sym("=")){
        return new_node(ND_ASN, node, assign());
    }
    else return node;
}

Node *equality() {
    Node *node = relational();

    while (1) {
        if (consume_sym("==")) {
            node = new_node(ND_EQ, node, mul());
            continue;
        }
        if (consume_sym("!=")) {
            node = new_node(ND_NEQ, node, mul());
            continue;
        }
        return node;
    }
}

Node *relational() {
    Node *node = add();

    while (1) {
        if (consume_sym("<")) {
            node = new_node(ND_LT, node, add());
            continue;
        }
        if (consume_sym("<=")) {
            node = new_node(ND_LTE, node, add());
            continue;
        }
        if (consume_sym(">")) {
            node = new_node(ND_LT, add(), node);
            continue;
        }
        if (consume_sym(">=")) {
            node = new_node(ND_LTE, add(), node);
            continue;
        }
        return node;
    }
}

Node *add() {
    Node *node = mul();

    while (1) {
        if (consume_sym("+")) {
            node = new_node(ND_ADD, node, mul());
            continue;
        }
        if (consume_sym("-")) {
            node = new_node(ND_SUB, node, mul());
            continue;
        }
        return node;
    }
}

Node *mul() {
    Node *node = unary();

    while (1) {
        if (consume_sym("*")) {
            node = new_node(ND_MUL, node, unary());
            continue;
        }
        if (consume_sym("/")) {
            node = new_node(ND_DIV, node, unary());
            continue;
        }
        return node;
    }
}

Node *unary() {
    if (consume_sym("+")) {
        return primary();
    }
    if (consume_sym("-")) {
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node *primary() {
    if (consume_sym("(")) {
        Node *node = expr();

        if (consume_sym(")")) {
            return node;
        }
        else {
            error_at(token->str, "Missing ')'.");;
        }
    }

    Token *idt = consume_idt();
    if(idt){
        Node *node;
        if(consume_sym("(")){
            node = new_node(ND_FNC_CALL, NULL, NULL);
            node->str = idt->str;
            node->str_len = idt->len;
            node->args = fnc_args();
        }
        else{
            node = new_node(ND_LVAR, NULL, NULL);
            Lvar *var = find_Lvar(idt);
            if(!var){
                var = calloc(1, sizeof(Lvar));
                var->str = idt->str;
                var->len = idt->len;

                LvarList *lv = calloc(1, sizeof(LvarList));
                lv->var = var;
                lv->next = locals;
                locals = lv;
            }
            node->var = var;
        }
        return node;
    }
    else return new_node_num(consume_num());
}

Node *fnc_args(){
    if(consume_sym(")")) return NULL;
    Node head;
    Node *tail = &head;
    tail->next = assign();
    tail = tail->next;
    while(!consume_sym(")")){
        if(consume_sym(",")){
            tail->next = assign();
            tail = tail->next;
        }
        else error_at(token->str, "Missing ','.");
    }
    return head.next;
}