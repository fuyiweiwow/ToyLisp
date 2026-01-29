#include "mpc/mpc.h"
#include "data_handling.h"
#ifndef evaluation_h
#define evaluation_h

void tl_env_add_builtins(tl_env *e);

tl_value *evaluate(tl_env* e ,tl_value* v);

tl_value *evaluate_sexpr(tl_env* e, tl_value *v);

tl_value *builtin_op(tl_env* e, tl_value *v, char *op);

tl_value *builtin_add(tl_env* e, tl_value *v);

tl_value *builtin_sub(tl_env* e, tl_value *v);

tl_value *builtin_mul(tl_env* e, tl_value *v);

tl_value *builtin_div(tl_env* e, tl_value *v);

tl_value *builtin_head(tl_env* e, tl_value *v);

tl_value *builtin_tail(tl_env* e, tl_value *v);

tl_value *builtin_list(tl_env* e, tl_value *v);

tl_value *builtin_eval(tl_env* e, tl_value *v);

tl_value *builtin_join(tl_env* e, tl_value *v);

tl_value *builtin_def(tl_env* e, tl_value *v);

void tl_env_add_builtin(tl_env *e, char *name, tl_builtin func);

tl_value *tl_value_join(tl_value *x, tl_value *y);

tl_value *tl_value_pop(tl_value *v, int i);

tl_value *tl_value_take(tl_value *v, int i);

tl_value *tl_value_read_num(mpc_ast_t *t);

tl_value *tl_value_read(mpc_ast_t *t);

tl_value *tl_value_add_cell(tl_value *v, tl_value *x);

#endif //evaluation_h