#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <string.h>
#endif

#ifndef data_handling_h
#define data_handling_h

struct tl_value;
struct tl_env;
typedef struct tl_value tl_value;
typedef struct tl_env tl_env;

typedef tl_value *(*tl_builtin)(tl_env*, tl_value*);

struct tl_value{
    int type;
    long num;
    char *err;
    char *sym;

    int count;
    struct tl_value **cell;

    tl_builtin func;
};

struct tl_env{
    int count;
    char **syms;
    tl_value **vals;
};

enum { TL_VAL_NUM, TL_VAL_ERR, TL_VAL_SYM, TL_VAL_SEXPR, TL_VAL_QEXPR, TL_VAL_FUNC };

tl_env *new_env(void);

/// @brief  Search environment for symbol key k to get a copy
/// @param e 
/// @param k 
/// @return 
tl_value *tl_env_get(tl_env *e, tl_value *k);

/// @brief Add or update symbol key k with value v in environment e
/// @param e 
/// @param k 
/// @param v 
void tl_env_put(tl_env *e, tl_value *k, tl_value *v);

void destroy_env(tl_env *e);

tl_value *tl_num(long x);

tl_value *tl_err(char *err);

tl_value *tl_sym(char *sym);

tl_value *tl_sexpr(void);

tl_value *tl_qexpr(void);

tl_value *tl_func(tl_builtin func);

tl_value *tl_value_copy(tl_value *v);

void destroy_tl_value(tl_value *v);

void tl_expr_print(tl_value *v, char open, char close);

void tl_value_print(tl_value *v);

void tl_value_println(tl_value *v);

#endif //data_handling_h