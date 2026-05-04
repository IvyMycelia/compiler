#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "lexer.h"
#include "parser.h"
#include "file.h"

#include "ANSI.h"

#include "codegen.h"

int has_stdlib = 0;
int has_stdio = 0;

void codegen(AST* ast, FILE* out, const char* src) {
    AST* curr = ast;

    while (curr != NULL) {
        if (curr->kind == AST_IMPORT)
            gen_import(curr, out, src);
        else if (curr->kind == AST_FUNC_DEF)
            gen_func_def(curr, out, src);
        else if (curr->kind == AST_STRUCT_DEF)
            gen_struct(curr, out, src);
        else if (curr->kind == AST_PROP)
            gen_func_def(curr->prop.func, out, src);
        curr = curr->next;
    }
}

void gen_statement(AST* ast, FILE* out, const char* src) {
    switch (ast->kind) {
        case AST_VAR_DECL:  gen_var_decl(ast, out, src);    break;
        case AST_VAR_ASS:   gen_var_ass(ast, out, src);     break;
        case AST_WHILE:     gen_while(ast, out, src);       break;
        case AST_FOR:       gen_for(ast, out, src);         break;
        case AST_RETURN:    gen_return(ast, out, src);      break;
        case AST_FUNC_CALL: 
            { 
                gen_func_call(ast, out, src);   
                fprintf(out, ";\n");
                break;
            }
        case AST_IF:        gen_if(ast, out, src, 0);       break;
        case AST_DOT_ACCESS:
            gen_expr(ast->dot_access.object, out, src);
            fprintf(out, ".%.*s",
                ast->dot_access.field_length,
                src + ast->dot_access.field_start
            );
            if (ast->dot_access.value != NULL) {
                fprintf(out, " = ");
                gen_expr(ast->dot_access.value, out, src);
                fprintf(out, ";\n");
            }
            break;

        case AST_PRINT:
            fprintf(out, "printf(");
            gen_expr(ast->print.value, out, src);
            fprintf(out, ");\n");
            break;
        
        case AST_PRUNE:
            fprintf(out, "free(");
            gen_expr(ast->prune_free.ptr, out, src);
            fprintf(out, ");\n");
            break;

        case AST_DEREF_ASS:
            fprintf(out, "*%.*s = ",
                ast->deref_ass.name_length,
                src + ast->deref_ass.name_start
            );
            gen_expr(ast->deref_ass.value, out, src);
            fprintf(out, ";\n");
            break;

        case AST_SUBSCRIPT:
            gen_expr(ast->subscript.array, out, src);
            fprintf(out, "[");
            gen_expr(ast->subscript.index, out, src);
            fprintf(out, "]");

            if (ast->subscript.value != NULL) {
                fprintf(out, " = ");
                gen_expr(ast->subscript.value, out, src);
            }
            fprintf(out, ";\n");
            break;

        default:
            printf(RED "Unknown statement kind: %d\n" RESET, ast->kind);
            exit(1);
    }
}

void gen_expr(AST* ast, FILE* out, const char* src) {
    switch (ast->kind) {
        case AST_VAR_REF:
            fprintf(out, "%.*s",
                ast->var_ref.name_length,
                src + ast->var_ref.name_start
            );
            break;

        case AST_LITERAL:
            fprintf(out, "%d",
                ast->value
            );
            break;

        case AST_FLOAT_LIT:
            fprintf(out, "%.*s",
                ast->float_lit.length,
                src + ast->float_lit.start
            );
            break;

        case AST_NULL:
            fprintf(out, "NULL");
            break;

        case AST_BINARY_OP: 
            gen_expr(ast->binary.left, out, src);
            fprintf(out, " %s ", token_to_string(ast->binary.op));
            gen_expr(ast->binary.right, out, src);
            break;

        case AST_FUNC_CALL: 
            gen_func_call(ast, out, src);
            break;

        case AST_STRING_LIT:
            fprintf(out, "%.*s",
                ast->string.str_length,
                src + ast->string.str_start
            );
            break;

        case AST_ARRAY_LIT:
            fprintf(out, "{");
            AST* elem = ast->array.elements;
            while (elem != NULL) {
                gen_expr(elem, out, src);
                if (elem->next != NULL) fprintf(out, ", ");
                elem = elem->next;
            }
            fprintf(out, "}");
            break;

        case AST_STRUCT_LIT:
            fprintf(out, "{");
            AST* struct_elem = ast->struct_lit.elements;
            while (struct_elem != NULL) {
                gen_expr(struct_elem, out, src);
                if (struct_elem->next != NULL) fprintf(out, ", ");
                struct_elem = struct_elem->next;
            }
            fprintf(out, "}");
            break;

        case AST_CAST:
            fprintf(out, "(");
            typeinfo_to_string(ast->cast.type, out, src);
            fprintf(out, ")(");
            gen_expr(ast->cast.value, out, src);
            fprintf(out, ")");
            break;

        case AST_DOT_ACCESS:
            gen_expr(ast->dot_access.object, out, src);
            fprintf(out, ".%.*s",
                ast->dot_access.field_length,
                src + ast->dot_access.field_start
            );
            break;

        case AST_NEW:
            if (ast->new_alloc.type.array_size > 0) {
                fprintf(out, "malloc(sizeof(");
                typeinfo_to_string(ast->new_alloc.type, out, src);
                fprintf(out, ") * %d", ast->new_alloc.type.array_size);
            } else {
                fprintf(out, "malloc(sizeof(");
                typeinfo_to_string(ast->new_alloc.type, out, src);
                fprintf(out, "))");
            }
            break;

        case AST_ALIAS_CALL:
            fprintf(out, "%.*s_%.*s(",
                ast->alias_call.alias_length,
                ast->alias_call.alias_start + src,
                ast->alias_call.func_length,
                src + ast->alias_call.func_start
            );
            AST* arg = ast->alias_call.args;
            while (arg != NULL) {
                gen_expr(arg, out, src);
                if (arg->next != NULL) fprintf(out, ", ");
                arg = arg->next;
            }
            fprintf(out, ")");
            break;

        case AST_UNARY_NOT:
            fprintf(out, "!(");
            gen_expr(ast->unary.operand, out, src);
            fprintf(out, ")");
            break;
        
        case AST_UNARY_NEG:
            fprintf(out, "-");
            gen_expr(ast->unary.operand, out, src);
            break;

        case AST_DEREF:
            fprintf(out, "*(");
            gen_expr(ast->deref.operand, out, src);
            fprintf(out, ")");
            break;

        case AST_SIZEOF:
            fprintf(out, "sizeof(");
            typeinfo_to_string(ast->size_of.type, out, src);
            fprintf(out, ")");
            break;

        case AST_CHAR_LIT:
            fprintf(out, "%.*s",
                ast->char_lit.length,
                ast->char_lit.start + src
            );
            break;

        case AST_SUBSCRIPT:
            gen_expr(ast->subscript.array, out, src);
            fprintf(out, "[");
            gen_expr(ast->subscript.index, out, src);
            fprintf(out, "]");
            break;

        default:
            printf("gen_expr received node addr: %p, kind: %d\n", ast, ast->kind);
            exit(1);
    }
}

void gen_func_def(AST* ast, FILE* out, const char* src) {
    // Emit: int fib(
    fprintf(out, "\n\n");
    typeinfo_to_string(ast->func_def.return_type, out, src);
    fprintf(out, " %.*s(",
        ast->func_def.name_length,
        src + ast->func_def.name_start
    );
    
    // Emit: params
    AST* param = ast->func_def.params;
    while (param != NULL) {
        gen_param(param, out, src);
        if (param->next != NULL) fprintf(out, ", ");
        param = param->next;
    }
    fprintf(out, ") {\n");

    // Emit: body
    AST* statement = ast->func_def.body;
    while (statement != NULL) {
        gen_statement(statement, out, src);
        statement = statement->next;
    }
    fprintf(out, "}\n");
}

void gen_func_def_aliased(AST* ast, FILE* out, const char* src, const char* alias, int alias_len) {
    // Emit: int fib(
    fprintf(out, "\n\n");
    typeinfo_to_string(ast->func_def.return_type, out, src);
    fprintf(out, " %.*s_%.*s(",
        alias_len, alias,
        ast->func_def.name_length,
        src + ast->func_def.name_start
    );
    
    // Emit: params
    AST* param = ast->func_def.params;
    while (param != NULL) {
        gen_param(param, out, src);
        if (param->next != NULL) fprintf(out, ", ");
        param = param->next;
    }
    fprintf(out, ") {\n");

    // Emit: body
    AST* statement = ast->func_def.body;
    while (statement != NULL) {
        gen_statement(statement, out, src);
        statement = statement->next;
    }
    fprintf(out, "}\n");
}

void gen_param(AST* param, FILE* out, const char* src) {
    // Emit: int v,
    typeinfo_to_string(param->func_params.type, out, src);
    fprintf(out, " %.*s",
        param->func_params.name_length,
        src + param->func_params.name_start
    );

    if (param->func_params.type.array_size == -1)
        fprintf(out, "[]");
    else if (param->func_params.type.array_size != 0)
        fprintf(out, "[%d]", param->func_params.type.array_size);
}

void gen_func_call(AST* ast, FILE* out, const char* src) {
    fprintf(out, "%.*s(",
        ast->func_call.name_length,
        src + ast->func_call.name_start
    );

    AST* arg = ast->func_call.args;
    while (arg != NULL) {
    gen_expr(arg, out, src);
        if (arg->next != NULL) fprintf(out, ", ");
        arg = arg->next;
    }
    fprintf(out, ")");
}

void gen_var_decl(AST* ast, FILE* out, const char* src) {
    typeinfo_to_string(ast->var_decl.type, out, src);
    fprintf(out, " %.*s", 
        ast->var_decl.name_length,
        src + ast->var_decl.name_start
    );
    if (ast->var_decl.type.array_size == -1)
        fprintf(out, "[]");
    else if (ast->var_decl.type.array_size != 0)
        fprintf(out, "[%d]", ast->var_decl.type.array_size);

    if (ast->var_decl.value != NULL) {
        fprintf(out, " = ");
        gen_expr(ast->var_decl.value, out, src);
    } 
    fprintf(out, ";\n");
}

void gen_var_ass(AST* ast, FILE* out, const char* src) {
    fprintf(out, "%.*s = ",
        ast->var_ass.name_length,
        src + ast->var_ass.name_start
    );
    gen_expr(ast->var_ass.value, out, src);
    fprintf(out, ";\n");
}

void gen_struct(AST* ast, FILE* out, const char* src) {
    fprintf(out, "\n\ntypedef struct {\n");
    AST* field = ast->struct_def.fields;
    while (field != NULL) {
        typeinfo_to_string(field->struct_field.type, out, src);
        fprintf(out, " %.*s;\n",
            field->struct_field.name_length,
            src + field->struct_field.name_start
        );
        field = field->next;
    }
    fprintf(out, "} %.*s;\n",
        ast->struct_def.name_length,
        src + ast->struct_def.name_start
    );
}

void gen_while(AST* ast, FILE* out, const char* src) {
    fprintf(out, "while (");
    gen_expr(ast->while_loop.condition, out, src);
    fprintf(out, ") {\n");
    
    AST* statement = ast->while_loop.body;
    while (statement != NULL) {
        gen_statement(statement, out, src);
        statement = statement->next;
    }
    fprintf(out, "}\n");
}

void gen_for(AST* ast, FILE* out, const char* src) {
    const char* desc_op = ast->for_loop.inclusive ? ">=" : ">";
    const char* asc_op = ast->for_loop.inclusive ? "<=" : "<";

    if (ast->for_loop.from->kind == AST_LITERAL &&
        ast->for_loop.to->kind == AST_LITERAL) {
        int reverse = ast->for_loop.from->value > ast->for_loop.to->value;
        fprintf(out, "for (int %.*s = ",
            ast->for_loop.var_length,
            src + ast->for_loop.var_start
        );
        gen_expr(ast->for_loop.from, out, src);
        fprintf(out, "; %.*s %s ",
            ast->for_loop.var_length,
            src + ast->for_loop.var_start,
            reverse ? desc_op : asc_op
        );
        gen_expr(ast->for_loop.to, out, src);
        fprintf(out, "; %.*s%s) {\n",
            ast->for_loop.var_length,
            src + ast->for_loop.var_start,
            reverse ? "--" : "++"
        );
    } else {
        fprintf(out, "for (int %.*s = ",
            ast->for_loop.var_length,
            src + ast->for_loop.var_start
        );
        gen_expr(ast->for_loop.from, out, src);

        fprintf(out, "; ((");
        gen_expr(ast->for_loop.from, out, src);
        fprintf(out, ") > (");
        gen_expr(ast->for_loop.to, out, src);
        fprintf(out, ")) ? %.*s %s (",
            ast->for_loop.var_length,
            src + ast->for_loop.var_start,
            desc_op
        );
        gen_expr(ast->for_loop.to, out, src);
        fprintf(out, ") : %.*s %s (",
            ast->for_loop.var_length,
            src + ast->for_loop.var_start,
            asc_op
        );
        gen_expr(ast->for_loop.to, out, src);

        fprintf(out, "); ((");
        gen_expr(ast->for_loop.from, out, src);
        fprintf(out, ") > (");
        gen_expr(ast->for_loop.to, out, src);
        fprintf(out, ")) ? %.*s-- : %.*s++) {\n",
            ast->for_loop.var_length,
            src + ast->for_loop.var_start,
            ast->for_loop.var_length,
            src + ast->for_loop.var_start
        );
    }

    AST* statement = ast->for_loop.body;
    while (statement != NULL) {
        gen_statement(statement, out, src);
        statement = statement->next;
    }
    fprintf(out, "}\n");
}


void gen_if(AST* ast, FILE* out, const char* src, int is_else_if) {
    if (is_else_if) {
        if (ast->if_condition.condition == NULL)
            fprintf(out, "else {\n");
        else {
            fprintf(out, "else if (");
            gen_expr(ast->if_condition.condition, out, src);
            fprintf(out, ") {\n");
        }
    } else {
        fprintf(out, "if (");
        gen_expr(ast->if_condition.condition, out, src);
        fprintf(out, ") {\n");
    }

    AST* statement = ast->if_condition.body;
    while (statement != NULL) {
        gen_statement(statement, out, src);
        statement = statement->next;
    }
    fprintf(out, "}\n");

    if (ast->if_condition.else_branch != NULL) 
        gen_if(ast->if_condition.else_branch, out, src, 1);
}

void gen_return(AST* ast, FILE* out, const char* src) {
    // Emit: return n
    fprintf(out, "return ");
    gen_expr(ast->ret.value, out, src);
    fprintf(out, ";\n");
}

/* Import Cache */
typedef struct {
    char* path;
    AST* ast;
    char* src;
} ImportCache;

static ImportCache import_cache[64];
static int cache_count = 0;

// Track emitted alias+file combos to prevent duplicates
typedef struct {
    char* path;
    char* alias;
} EmittedImport;

static EmittedImport emitted_imports[256];
static int emitted_count = 0;

// Currently processing (for circular detection)
static char* in_progress[64];
static int in_progress_count = 0;

void gen_import(AST* ast, FILE* out, const char* src) {
    if (ast->import.is_system) {
        char name[64];
        snprintf(name, sizeof(name), "%.*s",
            ast->import.path_length,
            src + ast->import.path_start
        );
        if (!strcmp(name, "stdio") && !has_stdio) {
            fprintf(out, "#include <stdio.h>\n");
            has_stdio = 1;
        } else if (!strcmp(name, "stdlib") && !has_stdlib) {
            fprintf(out, "#include <stdlib.h>\n");
            has_stdlib = 1;
        } else if (strcmp(name, "stdio") && strcmp(name, "stdlib")) {
            fprintf(out, "#include <%s.h>\n", name);
        }
        return;
    }

    char path[256];
    snprintf(path, sizeof(path), "%.*s",
        ast->import.path_length - 2,
        src + ast->import.path_start + 1
    );

    char alias[64] = "";
    if (ast->import.has_alias) {
        snprintf(alias, sizeof(alias), "%.*s",
            ast->import.alias_length,
            src + ast->import.alias_start
        );
    }

    // Check if this exact path+alias combo already emitted
    for (int i = 0; i < emitted_count; i++) {
        if (!strcmp(emitted_imports[i].path, path) &&
            !strcmp(emitted_imports[i].alias, alias)) {
            return;
        }
    }

    // Check for circular imports
    for (int i = 0; i < in_progress_count; i++) {
        if (!strcmp(in_progress[i], path)) {
            printf(RED "Circular import detected: %s\n" RESET, path);
            exit(1);
        }
    }

    // Mark as in progress
    in_progress[in_progress_count++] = strdup(path);

    // Check cache for already-parsed AST
    AST* imported_ast = NULL;
    char* imported_src = NULL;
    TokenStream* cached_ts = NULL;

    for (int i = 0; i < cache_count; i++) {
        if (!strcmp(import_cache[i].path, path)) {
            imported_ast = import_cache[i].ast;
            imported_src = import_cache[i].src;
            break;
        }
    }

    // If not cached, read and parse
    if (imported_ast == NULL) {
        imported_src = read_file(path);
        if (!imported_src) {
            printf(RED "Could not import file: %s\n" RESET, path);
            exit(1);
        }
        TokenStream* ts = malloc(sizeof(TokenStream));
        init_token_stream(ts);
        lex(imported_src, ts);

        if (!has_stdlib && (token_stream_contains(ts, TOKEN_NEW) ||
            token_stream_contains(ts, TOKEN_NULL))) {
            fprintf(out, "#include <stdlib.h>\n");
            has_stdlib = 1;
        }
        if (!has_stdio && token_stream_contains(ts, TOKEN_PRINT)) {
            fprintf(out, "#include <stdio.h>\n");
            has_stdio = 1;
        }

        Parser p;
        init_parser(&p, ts, imported_src);
        imported_ast = parse(&p);

        // Store in cache
        import_cache[cache_count].path = strdup(path);
        import_cache[cache_count].ast = imported_ast;
        import_cache[cache_count].src = imported_src;
        cache_count++;
    }

    // Record this path+alias as emitted
    emitted_imports[emitted_count].path = strdup(path);
    emitted_imports[emitted_count].alias = strdup(alias);
    emitted_count++;

    // Emit functions
    AST* curr = imported_ast;
    while (curr != NULL) {
        if (curr->kind == AST_IMPORT)
            gen_import(curr, out, imported_src);
        else if (curr->kind == AST_STRUCT_DEF)
            gen_struct(curr, out, imported_src);
        else if (curr->kind == AST_PROP) {
            if (ast->import.has_alias)
                gen_func_def_aliased(curr->prop.func, out, imported_src,
                    alias, strlen(alias));
            else
                gen_func_def(curr->prop.func, out, imported_src);
        } else if (curr->kind == AST_FUNC_DEF)
            gen_func_def(curr, out, imported_src);
        curr = curr->next;
    }

    // Remove from in_progress
    in_progress_count--;
    free(in_progress[in_progress_count]);
}

void emit_includes(AST* ast, FILE* out, const char* src, TokenStream* ts) {
    if (!has_stdlib && (token_stream_contains(ts, TOKEN_NULL) || token_stream_contains(ts, TOKEN_NEW))) {
        fprintf(out, "#include <stdlib.h>\n");
        has_stdlib = 1;
    }

    if (!has_stdio && token_stream_contains(ts, TOKEN_PRINT)) {
        fprintf(out, "#include <stdio.h>\n");
        has_stdio = 1;
    }
}

const char* token_to_string(TokenKind kind) {
    switch (kind) {
        /* Primitives */
        case TOKEN_INT:         return "int";
        case TOKEN_BOOL:        return "int";
        case TOKEN_FLOAT:       return "float";
        case TOKEN_DOUBLE:      return "double";
        case TOKEN_CHAR:        return "char";
        case TOKEN_STRING:      return "char*";
        case TOKEN_VOID:        return "void";

        /* Keywords */
        case TOKEN_RETURN:      return "return";
        case TOKEN_WHILE:       return "while";
        case TOKEN_IF:          return "if";
        case TOKEN_ELSE:        return "else";
        case TOKEN_END:         return "}";

        /* Operators */
        case TOKEN_DOT:         return ".";
        case TOKEN_PLUS:        return "+";
        case TOKEN_MINUS:       return "-";
        case TOKEN_STAR:        return "*";
        case TOKEN_SLASH:       return "/";
        case TOKEN_CARET:       return "^";
        case TOKEN_AT:          return "@";
        case TOKEN_AMPERSAND:   return "&";
        case TOKEN_ASSIGN:      return "=";
        case TOKEN_LT:          return "<";
        case TOKEN_GT:          return ">";
        case TOKEN_GEQ:         return ">=";
        case TOKEN_LEQ:         return "<=";
        case TOKEN_NEQ:         return "!=";
        case TOKEN_COMP:        return "==";

        /* Punctuation */
        case TOKEN_LPAREN:      return "(";
        case TOKEN_RPAREN:      return ")";
        case TOKEN_LBRACE:      return "{";
        case TOKEN_RBRACE:      return "}";
        case TOKEN_COLON:       return ":";
        case TOKEN_COMMA:       return ",";
        case TOKEN_SEMI:        return ";";

        case TOKEN_NEWLINE:     return "\n";
        default:                return "ERROR";
    }
}

void typeinfo_to_string(TypeInfo type, FILE* out, const char* src) {
    if (type.base == TOKEN_IDENTIFIER)
        fprintf(out, "%.*s", type.name_length, src + type.name_start);
    else
        fprintf(out, "%s", token_to_string(type.base));
    int i = 0;
    while (i++ < type.pointer_depth)
        fprintf(out, "*");
}