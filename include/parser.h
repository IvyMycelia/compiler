#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    TokenStream* ts;
    const char* src;
    int pos;
} Parser;

void    init_parser(Parser* ps, TokenStream* ts, const char* src);
Token*  parser_peek(Parser* ps);
Token*  parser_advance(Parser* ps);
void    parser_expect(Parser* ps, TokenKind kind);
void    parser_skip_newline(Parser* ps);

AST*    parse_statement(Parser* ps);
AST*    parse_expr(Parser* ps, int min_prec);
AST*    parse_primary(Parser* ps);
AST*    parse_func_def(Parser* ps);
AST*    parse_func_call(Parser* ps);
AST*    parse_var_decl(Parser* ps);
AST*    parse_var_ass(Parser* ps);
AST*    parse_while(Parser* ps);
AST*    parse_if(Parser* ps, int is_else_if);
AST*    parse_return(Parser* ps);
AST*    parse_param(Parser* ps);
AST*    parse_struct(Parser* ps);
AST*    parse(Parser* ps);
int     get_precedence(TokenKind kind);
TypeInfo parse_type(Parser* ps);


#endif