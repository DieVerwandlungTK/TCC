#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void error(char *fmt, ...);

int main(int argc, char **argv){
    if(argc!=2){
        error("The number of arguments is %d, but must be 2.\n", argc);
    }

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("    mov rax, %ld\n", strtol(p, &p, 10));

    while(*p){
        if(*p=='+') printf("    add rax, %ld\n", strtol(++p, &p, 10));
        else if(*p=='-') printf("    sub rax, %ld\n", strtol(++p, &p, 10));
        else error("Invalid syntax.");
    }

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