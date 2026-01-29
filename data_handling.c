#include "data_handling.h"

tl_env *new_env(void)
{
    tl_env *e = malloc(sizeof(tl_env));
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

tl_value *tl_env_get(tl_env *e, tl_value *k)
{
    for (size_t i = 0; i < e->count; i++)
    {
        if (strcmp(e->syms[i], k->sym) == 0)
        {
            return tl_value_copy(e->vals[i]);
        }
    }
    

    return tl_err("unbound symbol!");
}

void tl_env_put(tl_env *e, tl_value *k, tl_value *v)
{
    for (size_t i = 0; i < e->count; i++)
    {
        if(strcmp(e->syms[i], k->sym) == 0)
        {
            destroy_tl_value(e->vals[i]);
            e->vals[i] = tl_value_copy(v);
            return;
        }
    }
    
    //No existing entry found
    e->count++;
    e->syms = realloc(e->syms, sizeof(char*) * e->count);
    e->vals = realloc(e->vals, sizeof(tl_value*) * e->count);

    e->vals[e->count - 1] = tl_value_copy(v);
    e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
    strcpy(e->syms[e->count - 1], k->sym);

}

void destroy_env(tl_env *e)
{
    for (size_t i = 0; i < e->count; i++)
    {
        free(e->syms[i]);
        destroy_tl_value(e->vals[i]);
    }
    
    free(e->syms);
    free(e->vals);
    free(e);
}

tl_value *tl_num(long x)
{
    tl_value *v = malloc(sizeof(tl_value));
    v->type = TL_VAL_NUM;
    v->num = x;
    return v;
}

tl_value *tl_err(char *err)
{
    tl_value *v = malloc(sizeof(tl_value));
    v->type = TL_VAL_ERR;
    v->err = malloc(strlen(err) + 1);
    strcpy(v->err, err);
    return v;
}

tl_value *tl_sym(char *sym)
{
    tl_value *v = malloc(sizeof(tl_value));
    v->type = TL_VAL_SYM;
    v->sym = malloc(strlen(sym) + 1);
    strcpy(v->sym, sym);
    return v;
}

tl_value *tl_sexpr(void)
{
    tl_value *v = malloc(sizeof(tl_value));
    v->type = TL_VAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

tl_value *tl_qexpr(void)
{
    tl_value *v = malloc(sizeof(tl_value));
    v->type = TL_VAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

tl_value *tl_func(tl_builtin func)
{
    tl_value *v = malloc(sizeof(tl_value));
    v->type = TL_VAL_FUNC;
    v->func = func;
    return v;
}

tl_value *tl_value_copy(tl_value *v)
{
    tl_value * copy = malloc(sizeof(tl_value));
    copy->type = v->type;

    switch (v->type)
    {
    case TL_VAL_NUM:
        copy->num = v->num;
        break;
    case TL_VAL_FUNC:
        copy->func = v->func;
        break;
    case TL_VAL_ERR:
        copy->err = malloc(strlen(v->err) + 1);
        strcpy(copy->err, v->err);
        break;
    case TL_VAL_SYM:
        copy->sym = malloc(strlen(v->sym) + 1);
        strcpy(copy->sym, v->sym);
        break;
    case TL_VAL_QEXPR:
    case TL_VAL_SEXPR:
        copy->count = v->count;
        copy->cell = malloc(sizeof(tl_value*) * v->count);
        for (int i = 0; i < v->count; i++)
        {
            copy->cell[i] = tl_value_copy(v->cell[i]);
        }
        break;
    }

    return copy;
}

void destroy_tl_value(tl_value *v)
{
    switch (v->type)
    {
        case TL_VAL_NUM:
            break;
        case TL_VAL_ERR:
            free(v->err);
            break;  
        case TL_VAL_SYM:
            free(v->sym);   
            break;
        case TL_VAL_QEXPR:
        case TL_VAL_SEXPR:
            for (int i = 0; i < v->count; i++)
            {
                destroy_tl_value(v->cell[i]);
            } 
            free(v->cell);
            break;
        case TL_VAL_FUNC:
            break;
    }

    free(v);
}

void tl_expr_print(tl_value *v, char open, char close)
{
    putchar(open);
    for (int i = 0; i < v->count; i++)
    {
        // Print value contained within
        tl_value_print(v->cell[i]);

        // Don't print trailing space if last element
        if (i != (v->count - 1))
        {
            putchar(' ');
        }
    }
    putchar(close);

}

void tl_value_print(tl_value *v)
{
    switch (v->type)
    {
        case TL_VAL_NUM:
            printf("%ld", v->num);
            break;
        case TL_VAL_ERR:
            printf("Error: %s", v->err);
            break; 
        case TL_VAL_SYM:
            printf("%s", v->sym);
            break;
        case TL_VAL_SEXPR:
            tl_expr_print(v, '(', ')');
            break;
        case TL_VAL_QEXPR:
            tl_expr_print(v, '{', '}');
            break;
        case TL_VAL_FUNC:
            printf("<function>");
            break;
    }
}

void tl_value_println(tl_value *v)
{
    tl_value_print(v);
    putchar('\n');
}
