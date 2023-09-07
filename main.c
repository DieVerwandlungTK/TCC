#include "tcc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("The number of arguments is %d, but must be 2.\n", argc);
    }

    user_input = argv[1];

    token = tokenize(user_input);
    code = calloc(MAX_STMTS+1, sizeof(Node*));
    locals = calloc(1, sizeof(Lvar*));

    program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("    push rbp\n");       //prologue
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    for(int i=0;i<MAX_STMTS;++i){
        if(code[i]==NULL) break;
        gen(code[i]);
        printf("    pop rax\n");
    }

    printf("    mov rsp, rbp\n");   //epilogue
    printf("    pop rbp\n");
    printf("    ret\n");

    return 0;
}