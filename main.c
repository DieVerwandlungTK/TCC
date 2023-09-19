#include "tcc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("The number of arguments is %d, but must be 2.\n", argc);
    }

    user_input = argv[1];
    token = tokenize(user_input);
    Func *funcs = program();

    for(Func *fn=funcs;fn;fn=fn->next){
        int offset = 0;
        for(LvarList *vl=fn->locals;vl;vl=vl->next){
            offset+=8;
            vl->var->offset = offset;
        }
        fn->stack_size = offset;
    }

    code_gen(funcs);

    return 0;
}