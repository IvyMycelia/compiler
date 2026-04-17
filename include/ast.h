#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum {
    AST_RETURN,     // return a number
    AST_LITERAL,    // A value
    AST_FLOAT_LIT,  // A decimal value
    AST_NULL,       // An empty value
    AST_STRING_LIT, // A string "Hello, World!"
    AST_ARRAY_LIT,  // Array literal [0, 1, 2 ..]
    AST_STRUCT_LIT, // Struct literal {0, 1, 2 ..}
    AST_STRUCT_DEF, // Struct declaration
    AST_STRUCT_FIELD, // Struct field
    AST_DOT_ACCESS, // Struct access
    AST_BINARY_OP,  // 0 + 0

    /* Unary */
    AST_UNARY_NOT,  // not x
    AST_UNARY_NEG,  // *-1

    /* Memory Management */
    AST_NEW,        // malloc(sizeof(t))
    AST_PRUNE,      // free(mem)
    AST_DEREF_ASS,  // @ptr = 42

    /* Import System */
    AST_IMPORT,     // import "custom.flo"
    AST_PROP,       // prop int test():
    AST_ALIAS_CALL, // Alias call: math.sqrt()

    /* stdio */
    AST_PRINT,

    AST_VAR_DECL,   // a: int
    AST_VAR_ASS,    // a = temp
    AST_VAR_REF,    // Reading a variable's value
    AST_FUNC_DEF,   // int fib(n: int)
    AST_FUNC_CALL,  // fib(5)
    AST_WHILE,      // while i < n:
    AST_FOR,        // for i in 0..=10
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
        

        struct {
            int start;
            int length;
        } float_lit;


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


        /* Unary Operations */
        struct {
            struct AST* operand;
        } unary;

        /* Memory Management */
        struct {
            TypeInfo type;
        } new_alloc;

        struct {
            struct AST* ptr;
        } prune_free;

        struct {
            int name_start;
            int name_length;
            struct AST* value;
        } deref_ass;


        /* stdio */
        struct {
            struct AST* value;
        } print;


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

        struct {
            int alias_start;
            int alias_length;
            int func_start;
            int func_length;
            struct AST* args;
        } alias_call;


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
            int var_start;
            int var_length;
            int from;
            int to;
            int inclusive;
            struct AST* body;
        } for_loop;

        struct {
            struct AST* condition;
            struct AST* body;
            struct AST* else_branch;
        } if_condition;
    };
} AST;


AST*    make_node(ASTkind kind);

#endif