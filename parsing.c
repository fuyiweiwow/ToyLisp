#include "parsing.h"

#define GRAMMAR                                                             \
    "                                                                       \
        number    : /-?[0-9]+/ ;                                            \
        string    : /\"(\\\\.|[^\"])*\"/ ;                                  \
        symbol    : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/;                       \
        sexpr     : '(' <expr>* ')' ;                                       \
        qexpr     : '{' <expr>* '}' ;                                       \
        expr      : <number> | <string> | <symbol> | <sexpr> | <qexpr> ;    \
        tl        : /^/ <expr>* /$/ ;                                       \
    "


tl_parser *build_parser()
{
    tl_parser *parser = malloc(sizeof(tl_parser));
    // Create some parsers
    parser->number      = mpc_new("number");
    parser->symbol      = mpc_new("symbol");
    parser->s_expr      = mpc_new("sexpr");
    parser->q_expr      = mpc_new("qexpr");
    parser->expr        = mpc_new("expr");
    parser->tl          = mpc_new("tl");
    parser->string      = mpc_new("string");

    // Define use mpc api
    mpca_lang(  MPCA_LANG_DEFAULT,
                GRAMMAR,
                parser->number,
                parser->symbol,
                parser->s_expr,
                parser->q_expr,
                parser->expr,
                parser->string,
                parser->tl);

    return parser;
}

void destroy_parser(tl_parser *parser)
{
    mpc_cleanup(
        7, 
        parser->number, 
        parser->symbol, 
        parser->expr, 
        parser->q_expr,
        parser->s_expr,
        parser->string,
        parser->tl);
}
