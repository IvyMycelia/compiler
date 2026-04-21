#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

extern int has_stdlib;
extern int has_stdio;

void codegen(AST* ast, FILE* out, const char* src);
void gen_statement(AST* ast, FILE* out, const char* src);
void gen_expr(AST* ast, FILE* out, const char* src);
void gen_func_def(AST* ast, FILE* out, const char* src);
void gen_func_def_aliased(AST* ast, FILE* out, const char* src, const char* alias, int alias_len);
void gen_param(AST* param, FILE* out, const char* src);
void gen_func_call(AST* ast, FILE* out, const char* src);
void gen_var_decl(AST* ast, FILE* out, const char* src);
void gen_var_ass(AST* ast, FILE* out, const char* src);
void gen_struct(AST* ast, FILE* out, const char* src);
void gen_while(AST* ast, FILE* out, const char* src);
void gen_for(AST* ast, FILE* out, const char* src);
void gen_if(AST* ast, FILE* out, const char* src, int is_else_if);
void gen_return(AST* ast, FILE* out, const char* src);
void gen_import(AST* ast, FILE* out, const char* src);

void emit_includes(AST* ast, FILE* out, const char* src, TokenStream* ts);
void typeinfo_to_string(TypeInfo type, FILE* out, const char* src);
const char* token_to_string(TokenKind kind);

#endif