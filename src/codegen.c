#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "lexer.h"
#include "ANSI.h"

#include "codegen.h"

void codegen(AST* ast, FILE* out, const char* src) {
    AST* curr = ast;
    while (curr != NULL) {
        gen_func_def(curr, out, src);
        curr = curr->next;
    }
}

void gen_statement(AST* ast, FILE* out, const char* src) {
    switch (ast->kind) {
        case AST_VAR_DECL:  gen_var_decl(ast, out, src);    break;
        case AST_VAR_ASS:   gen_var_ass(ast, out, src);     break;
        case AST_WHILE:     gen_while(ast, out, src);       break;
        case AST_RETURN:    gen_return(ast, out, src);      break;
        case AST_FUNC_CALL: gen_func_call(ast, out, src);   break;
        case AST_IF:        gen_if(ast, out, src, 0);       break;
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

        case AST_BINARY_OP: 
            gen_expr(ast->binary.left, out, src);
            fprintf(out, " %s ", token_to_string(ast->binary.op));
            gen_expr(ast->binary.right, out, src);
            break;

        case AST_FUNC_CALL: 
            gen_func_call(ast, out, src);
            break;

        default:
            printf(RED "Could not generate expression: %d\n" RESET, ast->kind);
            exit(1);
    }
}

void gen_func_def(AST* ast, FILE* out, const char* src) {
    // Emit: int fib(
    fprintf(out, "\n\n");
    typeinfo_to_string(ast->func_def.return_type, out);
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

void gen_param(AST* param, FILE* out, const char* src) {
    // Emit: int v,
    typeinfo_to_string(param->func_params.type, out);
    fprintf(out, " %.*s",
        param->func_params.name_length,
        src + param->func_params.name_start
    );
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
    fprintf(out, ");\n");
}

void gen_var_decl(AST* ast, FILE* out, const char* src) {
    typeinfo_to_string(ast->var_decl.type, out);
    fprintf(out, " %.*s = ", 
        ast->var_decl.name_length,
        src + ast->var_decl.name_start
    );
    gen_expr(ast->var_decl.value, out, src);
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


const char* token_to_string(TokenKind kind) {
    switch (kind) {
        case TOKEN_INT:         return "int";
        case TOKEN_CHAR:        return "char";
        case TOKEN_VOID:        return "void";
        case TOKEN_RETURN:      return "return";
        case TOKEN_WHILE:       return "while";
        case TOKEN_IF:          return "if";
        case TOKEN_ELSE:        return "else";
        case TOKEN_END:         return "}";
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
        case TOKEN_COMP:        return "==";
        case TOKEN_LPAREN:      return "(";
        case TOKEN_RPAREN:      return ")";
        case TOKEN_COLON:       return "{";
        case TOKEN_COMMA:       return ",";
        case TOKEN_SEMI:        return ";";
        case TOKEN_NEWLINE:     return "\n";
        default:                return "ERROR";
    }
}

void typeinfo_to_string(TypeInfo type, FILE* out) {
    fprintf(out, "%s", token_to_string(type.base));
    int i = 0;
    while (i++ < type.pointer_depth)
        fprintf(out, "*");
}