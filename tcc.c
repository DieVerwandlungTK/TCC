#include <stdlib.h>
#include "tcc.h"

Token *token;
char *user_input;
Lvar *locals;
Node **code;
int label = 0;
Alc *lvar_alloc;
const char *arg_reg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};