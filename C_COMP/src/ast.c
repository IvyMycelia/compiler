#include <stdlib.h>

#include "ast.h"
#include "parser.h"

AST* make_node(ASTkind kind) {
    AST* node = calloc(1, sizeof(AST));
    node->kind = kind;
    node->value = 0;

    return node;
}