#include <stdio.h>
#include <stdlib.h>
#include "parsing.h"
#include "evaluation.h"

#define BUFFER_SIZE 2048
static char buffer[BUFFER_SIZE];

#ifdef _WIN32
#include <string.h>

char* realine(char* prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, BUFFER_SIZE, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

tl_parser *parser = NULL;

tl_value* builtin_load_ex(tl_env *e, tl_value *v)
{
    return builtin_load(e, v, parser->tl);
}

int main(int argc, char** argv)
{
    tl_env *env = new_env();
    tl_env_add_builtins(env);
    parser = build_parser();

    tl_env_add_builtin(env, "load", builtin_load_ex);

    puts("ToyLisp Version 0.0.0.1");
    puts("Press Ctrl + c to exit\n");

    if (argc >= 2)
    {
        for (size_t i = 1; i < argc; i++)
        {
            tl_value *args = tl_value_add_cell(tl_sexpr(), tl_str(argv[i]));
            tl_value *x = builtin_load(env, args, parser->tl);
            if (x->type == TL_VAL_ERR)
            {
                tl_value_println(x);
            }

            destroy_tl_value(x);
        }
    }

    for(;;)
    {
        char *input = realine("tl> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, parser->tl, &r)) 
        {
            //mpc_ast_print(r.output);
            tl_value *out = tl_value_read(r.output);
            //tl_value_println(out);

            tl_value *result = evaluate(env, out);
            tl_value_println(result);
            mpc_ast_delete(r.output);
            destroy_tl_value(out);
        }               
        else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    destroy_parser(parser);

    return 0;
}

