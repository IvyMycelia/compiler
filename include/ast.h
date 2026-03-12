#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum {
    AST_RETURN,     // return a number
    AST_LITERAL,    // A value
    AST_BINARY_OP,  // 0 + 0
    AST_VAR_DECL,   // a: int
    AST_VAR_ASS,    // a = temp
    AST_VAR_REF,    // Reading a variable's value
    AST_FUNC_DEF,   // int fib(n: int)
    AST_FUNC_CALL,  // fib(5)
    AST_WHILE,      // while i < n:
    AST_IF,         // if conidtion
    AST_PARAM,      // n: int in function signatures
} ASTkind;

typedef struct {
    TokenKind base;
    int pointer_depth;
} TypeInfo;

typedef struct AST {
    ASTkind kind;
    struct AST* next;   // Next statement,

    union {
        int value;      // For literals
        struct {
            struct AST* left;   // left binary-op
            struct AST* right;  // right bin-op
            int op;
        } binary;

        struct {
            int name_start;
            int name_length;
            TypeInfo type;
            struct AST* value;
        } var_decl;

        struct {
            int name_start;
            int name_length;
            struct AST* value;
        } var_ass;

        struct {
            int name_start;
            int name_length;
        } var_ref;

        // Function
        struct {
            int name_start;
            int name_length;
            struct AST* params;
            struct AST* body;
            TypeInfo return_type;
        } func_def;

        struct {
            int name_start;
            int name_length;
            TypeInfo type;
        } func_params;

        struct {
            int name_start;
            int name_length;
            struct AST* args;
        } func_call;

        struct {
            struct AST* value;
        } ret;

        // While Loop
        struct {
            struct AST* condition;
            struct AST* body;
        } while_loop;

        struct {
            struct AST* condition;
            struct AST* body;
            struct AST* else_branch;
        } if_condition;
    };
} AST;


AST*    make_node(ASTkind kind);

#endif