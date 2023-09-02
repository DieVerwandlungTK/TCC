#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void error(char *fmt, ...);

int main(int argc, char **argv){
    if(argc!=2){
        error("The number of arguments is %d, but must be 2.\n", argc);
    }

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("    mov rax, %ld\n", strtol(argv[1], NULL, 10));
    printf("    ret\n");

    return 0;
}

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}