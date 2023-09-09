#include "tcc.h"

void gen_lval(Node *node){
    if(node->kind!=ND_LVAR) error("The left operand of the assignment operator is not a lvalue.");
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
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

            printf("    mov rsp, rbp\n");   //epilogue
            printf("    pop rbp\n");
            printf("    ret\n");
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