#include <stdlib.h>
#include "tcc.h"

Token *token;
char *user_input;
Lvar *locals;
Node **code;
int label = 0;