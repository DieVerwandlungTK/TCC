#include "tcc.h"

void gen_lval(Node *node){
    if(node->kind!=ND_LVAR) error("The left operand of the assignment operator is not a lvalue.");
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->var->offset);
    printf("    push rax\n");
}

void gen(Node *node) {
    int lbegin, lend, lelse;
    switch (node->kind) {
        case ND_NUM:
            printf("    push %d\n", node->val);
            return;

        case ND_LVAR:
            gen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        
        case ND_ASN:
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
        
        case ND_RET:
            gen(node->lhs);

            printf("    pop rax\n");
            printf("    jmp .Lreturn.%s\n", func_name);
            return;
        
        case ND_IF:
            lend = label;
            ++label;

            gen(node->cond);

            printf("    pop rax\n");
            printf("    cmp rax, 0\n");

            if(node->els){
                lelse = label;
                ++label;

                printf("    je  .Lelse%03d\n", lelse);
                gen(node->then);
                printf("    jmp  .Lend%03d\n", lend);
                printf(".Lelse%03d:\n", lelse);
                gen(node->els);
                printf(".Lend%03d:\n",lend);
            }
            else{
                printf("    je  .Lend%03d\n", lend);
                gen(node->then);
                printf(".Lend%03d:\n", lend);
            }
            return;
        
        case ND_WHIL:
            lbegin = label;
            lend = label+1;
            label += 2;

            printf(".Lbegin%03d:\n", lbegin);
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je  .Lend%03d\n", lend);
            gen(node->then);
            printf("    jmp  .Lbegin%03d\n", lbegin);
            printf(".Lend%03d:\n", lend);
            return;

        case ND_FOR:
            lbegin = label;
            lend = label+1;
            label += 2;

            if(node->init) gen(node->init);
            printf(".Lbegin%03d:\n", lbegin);
            if(node->cond) {
                gen(node->cond);
                printf("    pop rax\n");
                printf("    cmp rax, 0\n");
                printf("    je  .Lend%03d\n", lend);
            }
            gen(node->then);
            if(node->inc) gen(node->inc);
            printf("    jmp  .Lbegin%03d\n", lbegin);
            printf(".Lend%03d:\n", lend);
            return;
        
        case ND_BLK:
            for(int i=0;i<node->blk_len;++i){
                gen((node->blk)[i]);
                printf("    pop rax\n");
            }
            return;
        
        case ND_FNC_CALL:
            int arg_num = 0;
            for(Node *arg = node->args;arg;arg = arg->next){
                gen(arg);
                ++arg_num;
            }

            if(arg_num>6) error("The number of function arguments must be less than 7, but is %d\n", arg_num);

            for(;arg_num>0;--arg_num){
                printf("    pop %s\n", arg_reg[arg_num-1]); // Put argument's value into a required register.
            }

            int lend = label;
            int lcall = label+1;
            label += 2;

            char *s = calloc(node->str_len, sizeof(char));
            strncpy(s, node->str, node->str_len);

            printf("    mov rax, rsp\n");
            printf("    and rax, 15\n");
            printf("    jnz .Lcall%03d\n", lcall);

            printf("    mov rax, 0\n");
            printf("    call %s\n", s);
            printf("    jmp .Lend%03d\n", lend);

            printf(".Lcall%03d:\n", lcall);
            printf("    sub rsp, 8\n");
            printf("    mov rax, 0\n");
            printf("    call %s\n", s);
            printf("    add rsp, 8\n");

            printf(".Lend%03d:\n", lend);
            printf("    push rax\n");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");
    switch (node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;

        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;

        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;

        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;

        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;

        case ND_NEQ:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;

        case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;

        case ND_LTE:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }
    printf("    push rax\n");
}

void code_gen(Func *funcs){
    printf(".intel_syntax noprefix\n");

    for(Func *fnc = funcs;fnc;fnc=fnc->next){
        func_name = calloc(fnc->len, sizeof(char));
        strncpy(func_name, fnc->str, fnc->len);
        printf(".global %s\n", func_name);
        printf("%s:\n", func_name);

        printf("    push rbp\n");       // Prologue
        printf("    mov rbp, rsp\n");
        printf("    sub rsp, %d\n", fnc->stack_size);

        int i=0;
        for(LvarList *arg = fnc->args;arg;arg = arg->next){
            printf("    mov [rbp-%d], %s\n", arg->var->offset, arg_reg[i++]);
        }

        for(Node *node = fnc->node;node;node=node->next){
            gen(node);
        }

        printf(".Lreturn.%s:\n", func_name);     // Epilogue
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
    }
}