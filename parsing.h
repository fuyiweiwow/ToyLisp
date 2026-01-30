#ifndef parsing_h
#define parsing_h

#include "mpc/mpc.h"

typedef struct 
{
    mpc_parser_t *number;
    mpc_parser_t *string;
    mpc_parser_t *symbol;
    mpc_parser_t *s_expr;
    mpc_parser_t *q_expr;
    mpc_parser_t *expr;
    mpc_parser_t *tl;

}  tl_parser;

tl_parser *build_parser();
void destroy_parser(tl_parser *parser);

#endif //parsing_h