#include <stdlib.h>
#include "tcc.h"

Token *token;
char *user_input;
LvarList *locals;
int label = 0;
const char *arg_reg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
char *func_name;