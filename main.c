#include "tcc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("The number of arguments is %d, but must be 2.\n", argc);
    }

    char *user_input = argv[1];

    Token *token = tokenize(user_input, user_input);
    Node *node = expr(&token, user_input);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");

    return 0;
}