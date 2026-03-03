typedef enum {
    AST_RETURN, AST_LITERAL, AST_BINARY_OP
} ASTkind;

typedef struct {
    ASTkind kind;
    int value;          // For literals
    struct AST* left;   // For binary-ops
    struct AST* right;
    struct AST* next;   // Next statement (in functions)
} AST;