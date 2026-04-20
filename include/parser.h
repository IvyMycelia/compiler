#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    TokenStream* ts;
    const char* src;
    int pos;
    int alias_start[32];    // Start position of alias names
    int alias_lengths[32];  // Lenghts of alias names
    int alias_count;
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
AST*    parse_for(Parser* ps);
AST*    parse_return(Parser* ps);
AST*    parse_param(Parser* ps);
AST*    parse_struct(Parser* ps);
AST*    parse_dot_ass(Parser* ps);
AST*    parse_import(Parser* ps);
AST*    parse_alias_call(Parser* ps);

AST*    parse(Parser* ps);

int     get_precedence(TokenKind kind);
int     token_stream_contains(TokenStream* ts, TokenKind kind);
int     is_alias(Parser* ps, Token* tok);

TypeInfo parse_type(Parser* ps);


#endif