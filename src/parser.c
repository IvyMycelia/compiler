#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ANSI.h"

#include "lexer.h"
#include "ast.h"
#include "parser.h"

void init_parser(Parser* ps, TokenStream* ts, const char* src) {
    ps->ts = ts;
    ps->src = src;
    ps->pos = 0;
}

Token* parser_peek(Parser* ps) {
    return peek(ps->ts, ps->pos);
}

Token* parser_advance(Parser* ps) {
    Token* token = parser_peek(ps);

    ps->pos++;
    return token;
}

void parser_expect(Parser* ps, TokenKind kind) {
    Token* curr_tok = parser_peek(ps);

    if (curr_tok->kind != kind) {
        printf(RED "Expected token: %s", token_kind_name(kind));
        exit(1);
    }
    parser_advance(ps);
}

void parser_skip_newline(Parser* ps) {
    while (parser_peek(ps)->kind == TOKEN_NEWLINE || parser_peek(ps)->kind == TOKEN_SEMI)
        parser_advance(ps);
}


AST* parse_param(Parser* ps) {
    AST* node = make_node(AST_PARAM);
    Token* name = parser_advance(ps);
    node->func_params.name_start = name->start;
    node->func_params.name_length = name->length;
    parser_expect(ps, TOKEN_COLON);
    node->func_params.type = parse_type(ps);
    return node;
}

AST* parse_func_call(Parser* ps) {
    AST* node = make_node(AST_FUNC_CALL);
    Token* name = parser_advance(ps);
    node->func_call.name_start = name->start;
    node->func_call.name_length = name->length;
    parser_expect(ps, TOKEN_LPAREN);

    AST* arg_head = NULL;
    AST* arg_tail = NULL;
    while (parser_peek(ps)->kind != TOKEN_RPAREN) {
        AST* param = parse_expr(ps, 0);
        if (arg_head == NULL) arg_head = param;
        else arg_tail->next = param;
        arg_tail = param;

        if (parser_peek(ps)->kind == TOKEN_COMMA) parser_advance(ps);
    }
    parser_expect(ps, TOKEN_RPAREN);
    node->func_call.args = arg_head;
    return node;
}

int get_precedence(TokenKind kind) {
    switch (kind) {
        case TOKEN_STAR:
        case TOKEN_SLASH: return 3;
        case TOKEN_PLUS:
        case TOKEN_MINUS: return 2;
        case TOKEN_GT:
        case TOKEN_LT:
        case TOKEN_COMP: return 1;
        default: return 0;
    }
}

AST* parse_expr(Parser* ps, int min_prec) {
    AST* left = parse_primary(ps);
    while (get_precedence(parser_peek(ps)->kind) > min_prec) {
        Token* op = parser_advance(ps);
        AST* right = parse_expr(ps, get_precedence(op->kind));
        AST* bin = make_node(AST_BINARY_OP);
        bin->binary.left = left;
        bin->binary.right = right;
        bin->binary.op = op->kind;
        left = bin;
    }
    return left;
}

AST* parse_primary(Parser* ps) {
    switch (parser_peek(ps)->kind) {
        case TOKEN_IDENTIFIER:
            if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_LPAREN)
                return parse_func_call(ps);

            Token* tok = parser_peek(ps);
            parser_advance(ps);
            AST* node = make_node(AST_VAR_REF);
            node->var_ref.name_start = tok->start;
            node->var_ref.name_length = tok->length;
            return node;
        case TOKEN_NUMBER:
            AST* lit = make_node(AST_LITERAL);
            lit->value = atoi(ps->src + parser_peek(ps)->start);
            parser_advance(ps);
            return lit;
        default:
            printf(RED "parse_primary: unexpected token kind: %d\n" RESET, parser_peek(ps)->kind);
            exit(1);
    }
}

AST* parse_return(Parser* ps) {
    AST* node = make_node(AST_RETURN);
    parser_advance(ps);
    node->ret.value = parse_expr(ps, 0);
    return node;
}

AST* parse_while(Parser* ps) {
    AST* node = make_node(AST_WHILE);
    parser_expect(ps, TOKEN_WHILE);
    node->while_loop.condition = parse_expr(ps, 0);
    parser_expect(ps, TOKEN_COLON);
    AST* body_head = NULL;
    AST* body_tail = NULL;
    while (parser_peek(ps)->kind != TOKEN_END) {
        parser_skip_newline(ps);
        if (parser_peek(ps)->kind == TOKEN_END) break;
        AST* statement = parse_statement(ps);
        if (body_head == NULL) body_head = statement;
        else body_tail->next = statement;
        body_tail = statement;
    }
    parser_expect(ps, TOKEN_END);
    node->while_loop.body = body_head;

    return node;
}

AST* parse_if(Parser* ps, int is_elf_if) {
    AST* node = make_node(AST_IF);
    parser_expect(ps, TOKEN_IF);
    node->if_condition.condition = parse_expr(ps, 0);
    parser_expect(ps, TOKEN_COLON);

    AST* body_head = NULL;
    AST* body_tail = NULL;
    while (parser_peek(ps)->kind != TOKEN_END && parser_peek(ps)->kind != TOKEN_ELSE) {
        AST* statement = parse_statement(ps);
        if (body_head == NULL) body_head = statement;
        else body_tail->next = statement;
        body_tail = statement;
    }
    node->if_condition.body = body_head;
    if (parser_peek(ps)->kind == TOKEN_ELSE) {
        parser_advance(ps);
        if (parser_peek(ps)->kind == TOKEN_IF) {
            node->if_condition.else_branch = parse_if(ps, 1);
        } else {
            AST* else_node = make_node(AST_IF);
            else_node->if_condition.condition = NULL;
            parser_expect(ps, TOKEN_COLON);
            AST* else_body_head = NULL;
            AST* else_body_tail = NULL;
            while (parser_peek(ps)->kind != TOKEN_END && parser_peek(ps)->kind != TOKEN_ELSE) {
                AST* statement = parse_statement(ps);
                if (else_body_head == NULL) else_body_head = statement;
                else else_body_tail->next = statement;
                else_body_tail = statement;
            }
            else_node->if_condition.body = else_body_head;
            node->if_condition.else_branch = else_node;
        }
    }
    if (!is_elf_if) parser_expect(ps, TOKEN_END);
    return node;
}


AST* parse_var_decl(Parser* ps) {
    AST* node = make_node(AST_VAR_DECL);
    Token* tok = parser_advance(ps);
    node->var_decl.name_start = tok->start;
    node->var_decl.name_length = tok->length;
    parser_expect(ps, TOKEN_COLON);
    node->var_decl.type = parse_type(ps);
    
    parser_expect(ps, TOKEN_ASSIGN);
    node->var_decl.value = parse_expr(ps, 0);
    return node;
}

AST* parse_var_ass(Parser* ps) {
    AST* node = make_node(AST_VAR_ASS);
    Token* tok = parser_advance(ps);
    node->var_ass.name_start = tok->start;
    node->var_ass.name_length = tok->length;
    parser_expect(ps, TOKEN_ASSIGN);
    node->var_ass.value = parse_expr(ps, 0);
    return node;
}

AST* parse_statement(Parser* ps) {
    parser_skip_newline(ps);
    switch (parser_peek(ps)->kind) {
        case TOKEN_IDENTIFIER:
            switch (peek(ps->ts, ps->pos + 1)->kind) {
                case TOKEN_COLON:
                    return parse_var_decl(ps);
                case TOKEN_ASSIGN:
                    return parse_var_ass(ps);
                case TOKEN_LPAREN:
                    return parse_func_call(ps);
            }
            break;
        case TOKEN_WHILE:
            return parse_while(ps);
        case TOKEN_RETURN:
            return parse_return(ps);
        case TOKEN_IF:
            return parse_if(ps, 0);
            break;
        default:
            // print error or smt
    }
}

AST* parse_func_def(Parser* ps) {
    AST* node = make_node(AST_FUNC_DEF);
    node->func_def.return_type = parse_type(ps);
    
    Token* name = parser_advance(ps);
    node->func_def.name_start = name->start;
    node->func_def.name_length = name->length;

    // Set parameters of function
    parser_expect(ps, TOKEN_LPAREN);

    AST* param_head = NULL;
    AST* param_tail = NULL;
    while (parser_peek(ps)->kind != TOKEN_RPAREN) {
        AST* param = parse_param(ps);
        if (param_head == NULL) param_head = param;
        else param_tail->next = param;
        param_tail = param;

        if (parser_peek(ps)->kind == TOKEN_COMMA) parser_advance(ps);
    }
    parser_expect(ps, TOKEN_RPAREN);
    node->func_def.params = param_head;
    
    // Function body
    parser_expect(ps, TOKEN_COLON);

    AST* body_head = NULL;
    AST* body_tail = NULL;
    while (parser_peek(ps)->kind != TOKEN_END) {
        parser_skip_newline(ps);
        if (parser_peek(ps)->kind == TOKEN_END) break;
        AST* statement = parse_statement(ps);
        if (body_head == NULL) body_head = statement;
        else body_tail->next = statement;
        body_tail = statement;
    }
    parser_expect(ps, TOKEN_END);
    node->func_def.body = body_head;

    return node;
}

TypeInfo parse_type(Parser* ps) {
    TypeInfo type;
    type.pointer_depth = 0;
    while (parser_peek(ps)->kind == TOKEN_AT) {
        type.pointer_depth++;
        parser_advance(ps);
    }
    type.base = parser_advance(ps)->kind;
    return type;
}

AST* parse(Parser* ps) {
    AST* head = NULL;
    AST* tail = NULL;
    
    while (parser_peek(ps)->kind != TOKEN_EOF) {
        parser_skip_newline(ps);
        AST* node = parse_func_def(ps);
        if (head == NULL) head = node;
        else tail->next = node;
        tail = node;
    }

    return head;
}