#include "evaluation.h"
#define BUILTIN_ASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        tl_value *err = tl_err_ex(fmt, ##__VA_ARGS__); \
        destroy_tl_value(args); \
        return err;\
    }

#define BUILTIN_ARGS_COUNT_ASSERT(fn, v, cnt) \
        BUILTIN_ASSERT(v, v->count == cnt, \
            "Function '%s' passed incorrect number of arguments. " \
            "Got %i, Expected %i.", \
            fn, v->count, cnt)
    

#define BUILTIN_ARGS_TYPE_ASSERT(fn, v, idx, a_type) \
        BUILTIN_ASSERT(v, v->cell[idx]->type == a_type, \
            "Function '%s' passed incorrect type for argument %i. " \
            "Got %s, Expected %s.", \
            fn, idx, tl_type_name(v->cell[idx]->type), tl_type_name(a_type))

    



void tl_env_add_builtins(tl_env *e)
{
    /* List Functions */
    tl_env_add_builtin(e, "list", builtin_list);
    tl_env_add_builtin(e, "head", builtin_head);
    tl_env_add_builtin(e, "tail", builtin_tail);
    tl_env_add_builtin(e, "eval", builtin_eval);
    tl_env_add_builtin(e, "join", builtin_join);
    tl_env_add_builtin(e, "def" , builtin_def);
    tl_env_add_builtin(e, "=", builtin_put);
    /* Mathematical Functions */
    tl_env_add_builtin(e, "+", builtin_add);
    tl_env_add_builtin(e, "-", builtin_sub);
    tl_env_add_builtin(e, "*", builtin_mul);
    tl_env_add_builtin(e, "/", builtin_div);
    tl_env_add_builtin(e, "\\", builtin_lambda);

}

tl_value *evaluate(tl_env *e, tl_value *v)
{
    /* Evaluate Sexpressions */
    if (v->type == TL_VAL_SEXPR)
    { 
        return evaluate_sexpr(e, v);
    }
    /* All other lval types remain the same */
    if (v->type == TL_VAL_SYM)
    {
        tl_value* x = tl_env_get(e, v);
        destroy_tl_value(v);
        return x;
    }
    
    return v;
}

tl_value *evaluate_sexpr(tl_env* e, tl_value *v)
{
    // Evaluate children
    for (size_t i = 0; i < v->count; i++)
    {
        v->cell[i] = evaluate(e, v->cell[i]);
    }
    
    // Error checking
    for (size_t i = 0; i < v->count; i++)
    {
        if(v->cell[i]->type == TL_VAL_ERR)
        {
            return tl_value_take(v, i);
        }
    }

    // Empty expression
    if (v->count == 0)
    {
        return v;
    }

    // Single expression
    if(v->count == 1)
    {
        return tl_value_take(v, 0);
    }

    tl_value *first = tl_value_pop(v, 0);
    if (first->type != TL_VAL_FUNC)
    {
        destroy_tl_value(first);
        destroy_tl_value(v);
        return tl_err("first element is not a function!");
    }

    tl_value *result = tl_func_call(e, first, v);
    destroy_tl_value(first);
    return result;
}

tl_value *builtin_op(tl_env* e, tl_value *v, char *op)
{
    for (size_t i = 0; i < v->count; i++)
    {
        int cell_type = v->cell[i]->type;   
        if (cell_type != TL_VAL_NUM)
        {
            destroy_tl_value(v);
            BUILTIN_ASSERT(
                v, 
                cell_type == TL_VAL_NUM, 
                "function '%s' passed incorrect type for argument %i. "
                "Got %s, Expected %s!", 
                op,
                i,
                tl_type_name(cell_type), 
                tl_type_name(TL_VAL_NUM));
        }
    }
    
    tl_value *x = tl_value_pop(v, 0);

    if (strcmp(op, "-") == 0 && v->count == 0)
    {
        x->num = -x->num;
    }

    while (v->count > 0)
    {
        tl_value *y = tl_value_pop(v, 0);

        if (strcmp(op, "+") == 0)
        {
            x->num += y->num;
        }
        else if (strcmp(op, "-") == 0)
        {
            x->num -= y->num;
        }
        else if (strcmp(op, "*") == 0)
        {
            x->num *= y->num;
        }
        else if (strcmp(op, "/") == 0)
        {
            if (y->num == 0)
            {
                destroy_tl_value(x);
                destroy_tl_value(y);
                x = tl_err("division by zero!");
                break;
            }
            x->num /= y->num;
        }

        destroy_tl_value(y);
    }

    destroy_tl_value(v);
    return x;
}

tl_value *builtin_add(tl_env *e, tl_value *v)
{
    return builtin_op(e, v, "+");
}

tl_value *builtin_sub(tl_env *e, tl_value *v)
{
    return builtin_op(e, v, "-");
}

tl_value *builtin_mul(tl_env *e, tl_value *v)
{
    return builtin_op(e, v, "*");
}

tl_value *builtin_div(tl_env *e, tl_value *v)
{
    return builtin_op(e, v, "/");
}

tl_value *builtin_head(tl_env *e, tl_value *v)
{
    BUILTIN_ASSERT(
        v, 
        v->count == 1, 
        "function 'head' passed invalid parameters. "
        "Got %i, Expected %i!", 
        v->count, 
        1);
    BUILTIN_ASSERT(
        v, 
        v->cell[0]->type == TL_VAL_QEXPR, 
        "function 'head' passed incorrect type for argument 0. "
        "Got %s, Expected %s!", 
        tl_type_name(v->cell[0]->type), 
        tl_type_name(TL_VAL_QEXPR));
    BUILTIN_ASSERT(v, v->cell[0]->count != 0, "function 'head' passed {}!");

    tl_value *x = tl_value_take(v, 0);
    while (x->count > 1)
    {
        destroy_tl_value(tl_value_pop(x, 1));
    }

    return x;
}

tl_value *builtin_tail(tl_env* e, tl_value *v)
{
    BUILTIN_ASSERT(
        v, 
        v->count == 1, 
        "function 'tail' passed invalid parameters. "
        "Got %i, Expected %i!", 
        v->count, 
        1);
    BUILTIN_ASSERT(
        v, 
        v->cell[0]->type == TL_VAL_QEXPR, 
        "function 'tail' passed incorrect type for argument 0. "
        "Got %s, Expected %s!", 
        tl_type_name(v->cell[0]->type), 
        tl_type_name(TL_VAL_QEXPR));
    BUILTIN_ASSERT(v, v->cell[0]->count != 0, "function 'tail' passed {}!");

    tl_value *x = tl_value_take(v, 0);
    destroy_tl_value(tl_value_pop(x, 0));

    return x;
}

tl_value *builtin_list(tl_env* e, tl_value *v)
{
    v->type = TL_VAL_QEXPR;
    return v;
}

tl_value *builtin_eval(tl_env* e, tl_value *v)
{
    BUILTIN_ASSERT(
        v, 
        v->count == 1, 
        "function 'eval' passed invalid parameters. "
        "Got %i, Expected %i!", 
        v->count, 
        1);
    BUILTIN_ASSERT(
        v, 
        v->cell[0]->type == TL_VAL_QEXPR, 
        "function 'eval' passed incorrect type for argument 0. "
        "Got %s, Expected %s!", 
        tl_type_name(v->cell[0]->type), 
        tl_type_name(TL_VAL_QEXPR));

    tl_value *x = tl_value_take(v, 0);
    x->type = TL_VAL_SEXPR;

    return evaluate(e, x);
}

tl_value *builtin_join(tl_env* e,tl_value *v)
{
    for (size_t i = 0; i < v->count; i++)
    {
        BUILTIN_ASSERT(v, v->cell[i]->type == TL_VAL_QEXPR, "function 'join' passed incorrect type!");
    }
    
    tl_value *x = tl_value_pop(v, 0);
    while (x->count)
    {
        x = tl_value_join(x, tl_value_pop(v, 0));
    }

    destroy_tl_value(v);
    return x;
}

tl_value *builtin_put(tl_env *e, tl_value *v)
{
    return builtin_var(e, v, "=");
}

tl_value *builtin_def(tl_env *e, tl_value *v)
{
    return builtin_var(e, v, "def");
}

tl_value *builtin_lambda(tl_env *e, tl_value *v)
{
    BUILTIN_ARGS_COUNT_ASSERT("\\", v, 2);
    BUILTIN_ARGS_TYPE_ASSERT("\\", v, 0, TL_VAL_QEXPR);
    BUILTIN_ARGS_TYPE_ASSERT("\\", v, 1, TL_VAL_QEXPR);

    for (size_t i = 0; i < v->cell[0]->count; i++)
    {
        BUILTIN_ASSERT(
            v, 
            v->cell[0]->cell[i]->type == TL_VAL_SYM, 
            "cannot define non-symbol. Got %s, Expected %s!", 
            tl_type_name(v->cell[0]->cell[i]->type), 
            tl_type_name(TL_VAL_SYM));
    }

    tl_value *formals = tl_value_pop(v, 0);
    tl_value *body = tl_value_pop(v, 0);
    destroy_tl_value(v);
    return tl_lambda(formals, body);
}

tl_value *builtin_var(tl_env *e, tl_value *v, char *func)
{
    BUILTIN_ARGS_TYPE_ASSERT(func, v, 0, TL_VAL_QEXPR);
    tl_value *syms = v->cell[0];
    for (size_t i = 0; i < syms->count; i++)
    {
        BUILTIN_ASSERT(
            v, 
            syms->cell[i]->type == TL_VAL_SYM, 
            "function '%s' cannot define non-symbol. Got %s, Expected %s!", 
            func,
            tl_type_name(syms->cell[i]->type), 
            tl_type_name(TL_VAL_SYM));
    }

    BUILTIN_ASSERT(
        v, 
        syms->count == v->count - 1, 
        "function '%s' cannot define incorrect number of values to symbols. Got %i, Expected %i!", 
        func,
        syms->count,
        v->count - 1);

    for (size_t i = 0; i < syms->count; i++)
    {
        if (strcmp(func, "def") == 0)
        {
            tl_env_put_global(e, syms->cell[i], v->cell[i + 1]);
        }

        if (strcmp(func, "=") == 0)
        {
            tl_env_put(e, syms->cell[i], v->cell[i + 1]);
        }
    }
       
    destroy_tl_value(v);
    return tl_sexpr();
}

void tl_env_add_builtin(tl_env *e, char *name, tl_builtin func)
{
    tl_value *k = tl_sym(name);
    tl_value *v = tl_func(func);
    tl_env_put(e, k, v);
    destroy_tl_value(k);
    destroy_tl_value(v);
}

tl_value *tl_func_call(tl_env *e, tl_value *f, tl_value *a)
{
    if(f->func)
    {
        return f->func(e, a);
    }

    int arg_count_given = a->count;
    int formals_count = f->formals ? f->formals->count : 0;

    while(a->count)
    {
        if(!f->formals || f->formals->count == 0)
        {
            destroy_tl_value(a);
            return tl_err_ex(
                "function passed invalid arguments. Got %i, Expected %i!", 
                arg_count_given, 
                formals_count);
        }

        tl_value *sym = tl_value_pop(f->formals, 0);

        if (strcmp(sym->sym, "&") == 0)
        {
            if (f->formals->count != 1)
            {
                destroy_tl_value(a);
                destroy_tl_value(sym);
                return tl_err_ex("function format invalid. Symbol '&' not followed by single symbol.");
            }

            tl_value *nsym = tl_value_pop(f->formals, 0);
            tl_env_put(f->env, nsym, builtin_list(e, a));
            destroy_tl_value(sym);
            destroy_tl_value(nsym);
            break;
        }

        tl_value *val = tl_value_pop(a, 0);

        tl_env_put(f->env, sym, val);

        destroy_tl_value(sym);
        destroy_tl_value(val);
    }

    destroy_tl_value(a);
    if (f->formals->count > 0 &&
    strcmp(f->formals->cell[0]->sym, "&") == 0)
    {
        if (f->formals->count != 2)
        {
            return tl_err_ex("function format invalid. Symbol '&' not followed by single symbol.");
        }

        destroy_tl_value(tl_value_pop(f->formals, 0));
        tl_value *sym = tl_value_pop(f->formals, 0);
        tl_value *val = tl_qexpr();

        tl_env_put(f->env, sym, val);
        destroy_tl_value(sym);
        destroy_tl_value(val);
    }

    if(f->formals->count == 0)
    {
        // All formals bound, evaluate
        f->env->parent_env = e;
        return builtin_eval(f->env, tl_value_add_cell(tl_sexpr(), tl_value_copy(f->body))); 
    }

    return tl_value_copy(f);
}

tl_value *tl_value_join(tl_value *x, tl_value *y)
{
    while (y->count)
    {
        x = tl_value_add_cell(x, tl_value_pop(y, 0));
    }

    destroy_tl_value(y);
    return x;
}

tl_value *tl_value_pop(tl_value *v, int i)
{
    tl_value * x = v->cell[i];

    memmove(&v->cell[i], &v->cell[i + 1], sizeof(tl_value*) * (v->count - i - 1));
    v->count--;
    v->cell = realloc(v->cell, sizeof(tl_value*) * v->count);
    return x;
}

tl_value *tl_value_take(tl_value *v, int i)
{
    tl_value *x = tl_value_pop(v, i);
    destroy_tl_value(v);
    return x;
}

tl_value *tl_value_read_num(mpc_ast_t *t)
{
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return (errno != ERANGE) ? tl_num(x) : tl_err("invalid number");
}

tl_value *tl_value_read(mpc_ast_t *t)
{
    if (strstr(t->tag, "number"))
    {
        return tl_value_read_num(t);
    }

    if (strstr(t->tag, "symbol"))
    {
        return tl_sym(t->contents);
    }

    tl_value *x = NULL;
    // If is root (>)
    if (strcmp(t->tag, ">") == 0)
    {
        x = tl_sexpr();
    }
    if (strstr(t->tag, "sexpr"))
    {
        x = tl_sexpr();
    }
    if (strstr(t->tag, "qexpr"))
    {
        x = tl_qexpr();
    }

    for (size_t i = 0; i < t->children_num; ++i)
    {
        if (strcmp(t->children[i]->contents, "(") == 0) 
        {
            continue;
        }
        if (strcmp(t->children[i]->contents, ")") == 0) 
        { 
            continue; 
        }
        if (strcmp(t->children[i]->contents, "{") == 0) 
        {
            continue;
        }
        if (strcmp(t->children[i]->contents, "}") == 0) 
        { 
            continue; 
        }
        if (strcmp(t->children[i]->tag, "regex") == 0) 
        { 
            continue; 
        }
        x = tl_value_add_cell(x, tl_value_read(t->children[i]));
    }
    
    return x;
}

tl_value *tl_value_add_cell(tl_value *v, tl_value *x)
{
    v->count++;
    v->cell = realloc(v->cell, sizeof(tl_value*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

char *tl_type_name(int type)
{
    switch (type)
    {
        case TL_VAL_NUM:   return "Number";
        case TL_VAL_ERR:   return "Error";
        case TL_VAL_SYM:   return "Symbol";
        case TL_VAL_SEXPR: return "S-Expression";
        case TL_VAL_QEXPR: return "Q-Expression";
        case TL_VAL_FUNC:  return "Function";
        default:           return "Unknown";
    }    
}
