#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum {
    AST_RETURN,     // return a number
    AST_LITERAL,    // A value
    AST_NULL,       // An empty value
    AST_STRING_LIT, // A string "Hello, World!"
    AST_ARRAY_LIT,  // Array literal [0, 1, 2 ..]
    AST_STRUCT_LIT, // Struct literal {0, 1, 2 ..}
    AST_STRUCT_DEF, // Struct declaration
    AST_STRUCT_FIELD, // Struct field
    AST_DOT_ACCESS, // Struct access
    AST_BINARY_OP,  // 0 + 0

    /* Memory Management */
    AST_NEW,        // malloc(sizeof(t))
    AST_PRUNE,      // free(mem)

    /* Import System */
    AST_IMPORT,     // import "custom.flo"
    AST_PROP,       // prop int test():

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
    int array_size;
    int name_start;
    int name_length;
} TypeInfo;

typedef struct AST {
    ASTkind kind;
    struct AST* next;   // Next statement,

    union {
        int value;      // For literals
        


        /* Strings */
        struct {
            int str_start;
            int str_length;
        } string;


        /* Arrays */
        struct {
            struct AST* elements;
            int arr_length;
        } array;


        /* Memory Management */
        struct {
            TypeInfo type;
        } new_alloc;

        struct {
            struct AST* ptr;
        } prune_free;


        /* Import System */
        struct {
            int path_start;
            int path_length;
            int alias_start;
            int alias_length;
            int is_system;      // 1 if <stdlib>, 0 if "file.flo"
            int has_alias;      // 1 if `as name` present
        } import;

        struct {
            struct AST* func;
        } prop;


        /* Structs */
        struct {
            int name_start;
            int name_length;
            struct AST* fields;
        } struct_def;

        struct {
            int name_start;
            int name_length;
            TypeInfo type;
        } struct_field;

        struct {
            struct AST* elements;
        } struct_lit;

        struct {
            struct AST* object;
            int field_start;
            int field_length;
            struct AST* value;
        } dot_access;



        /* Operations */
        struct {
            struct AST* left;   // left binary-op
            struct AST* right;  // right bin-op
            int op;
        } binary;



        /* Variables */
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

        
        
        /* Functions */
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



        /* Return */
        struct {
            struct AST* value;
        } ret;

        
        
        /* Conditionals */
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