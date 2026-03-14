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
        printf(RED "Expected token: %s but got %s\n", token_kind_name(kind), token_kind_name(curr_tok->kind));
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

            if (parser_peek(ps)->kind == TOKEN_DOT) {
                parser_advance(ps);
                Token* field = parser_advance(ps);
                AST* dot = make_node(AST_DOT_ACCESS);
                dot->dot_access.object = node;
                dot->dot_access.field_start = field->start;
                dot->dot_access.field_length = field->length;
                return dot;
            }
            return node;

        case TOKEN_NUMBER:
            AST* lit = make_node(AST_LITERAL);
            lit->value = atoi(ps->src + parser_peek(ps)->start);
            parser_advance(ps);
            return lit;

        case TOKEN_NULL:
            AST* null = make_node(AST_NULL);
            parser_advance(ps);
            return null;

        case TOKEN_STRING_LIT:
            AST* str = make_node(AST_STRING_LIT);
            str->string.str_start = parser_peek(ps)->start;
            str->string.str_length = parser_peek(ps)->length;
            parser_advance(ps);
            return str;

        case TOKEN_LBRACK:
            parser_advance(ps);
            AST* arr = make_node(AST_ARRAY_LIT);
            AST* elem_head = NULL;
            AST* elem_tail = NULL;
            
            while (parser_peek(ps)->kind != TOKEN_RBRACK) {
                AST* elem = parse_expr(ps, 0);
                if (elem_head == NULL) elem_head = elem;
                else elem_tail->next = elem;
                elem_tail = elem;
                if (parser_peek(ps)->kind == TOKEN_COMMA) parser_advance(ps);
            }

            parser_expect(ps, TOKEN_RBRACK);
            arr->array.elements = elem_head;
            return arr;

        case TOKEN_LBRACE:
            parser_advance(ps);
            AST* struct_lit = make_node(AST_STRUCT_LIT);
            AST* struct_head = NULL;
            AST* struct_tail = NULL;

            while (parser_peek(ps)->kind != TOKEN_RBRACE) {
                AST* elem = parse_expr(ps, 0);
                if (struct_head == NULL) struct_head = elem;
                else struct_tail->next = elem;
                struct_tail = elem;
                if (parser_peek(ps)->kind == TOKEN_COMMA) parser_advance(ps);
            }

            parser_expect(ps, TOKEN_RBRACE);
            struct_lit->struct_lit.elements = struct_head;
            return struct_lit;

        default:
            printf(RED "parse_primary: unexpected token kind: %s\n" RESET, token_kind_name(parser_peek(ps)->kind));
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
    
    if (parser_peek(ps)->kind == TOKEN_ASSIGN) {
        parser_advance(ps);
        node->var_decl.value = parse_expr(ps, 0);
    }
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

AST* parse_dot_ass(Parser* ps) {
    AST* node = make_node(AST_DOT_ACCESS);
    Token* tok = parser_advance(ps);

    AST* object = make_node(AST_VAR_REF);
    object->var_ref.name_start = tok->start;
    object->var_ref.name_length = tok->length;
    node->dot_access.object = object;

    parser_expect(ps, TOKEN_DOT);

    Token* field = parser_advance(ps);
    node->dot_access.field_start = field->start;
    node->dot_access.field_length = field->length;

    parser_expect(ps, TOKEN_ASSIGN);
    node->dot_access.value = parse_expr(ps, 0);

    return node;
}

AST* parse_statement(Parser* ps) {
    parser_skip_newline(ps);
    // printf("parse_statement: token kind %d\n", parser_peek(ps)->kind);
    switch (parser_peek(ps)->kind) {
        case TOKEN_IDENTIFIER:
            switch (peek(ps->ts, ps->pos + 1)->kind) {
                case TOKEN_COLON:
                    return parse_var_decl(ps);
                case TOKEN_ASSIGN:
                    return parse_var_ass(ps);
                case TOKEN_LPAREN:
                    return parse_func_call(ps);
                case TOKEN_DOT:
                    return parse_dot_ass(ps);
            }
            break;

        case TOKEN_WHILE:
            return parse_while(ps);

        case TOKEN_RETURN:
            return parse_return(ps);
            
        case TOKEN_IF:
            return parse_if(ps, 0);

            
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

AST* parse_struct(Parser* ps) {
    parser_expect(ps, TOKEN_STRUCT);

    AST* node = make_node(AST_STRUCT_DEF);

    Token* name = parser_advance(ps);
    node->struct_def.name_start = name->start;
    node->struct_def.name_length = name->length;

    parser_expect(ps, TOKEN_LBRACE);

    AST* field_head = NULL;
    AST* field_tail = NULL;
    while (parser_peek(ps)->kind != TOKEN_RBRACE) {
        while ( parser_peek(ps)->kind == TOKEN_NEWLINE ||
                parser_peek(ps)->kind == TOKEN_COMMA ||
                parser_peek(ps)->kind == TOKEN_SEMI) parser_advance(ps);
        if (parser_peek(ps)->kind == TOKEN_RBRACE) break;
        AST* field = make_node(AST_STRUCT_FIELD);
        Token* fname = parser_advance(ps);
        field->struct_field.name_start = fname->start;
        field->struct_field.name_length = fname->length;
        parser_expect(ps, TOKEN_COLON);
        field->struct_field.type = parse_type(ps);
        if (field_head == NULL) field_head = field;
        else field_tail->next = field;
        field_tail = field;
    }
    parser_expect(ps, TOKEN_RBRACE);
    node->struct_def.fields = field_head;

    return node;
}

TypeInfo parse_type(Parser* ps) {
    TypeInfo type;
    type.pointer_depth = 0;
    type.array_size = 0;

    
    while (parser_peek(ps)->kind == TOKEN_AT) {
        type.pointer_depth++;
        parser_advance(ps);
    }

    Token* base = parser_advance(ps);
    type.base = base->kind;

    if (base->kind == TOKEN_IDENTIFIER) {
        type.name_start = base->start;
        type.name_length = base->length;
    }

    if (parser_peek(ps)->kind == TOKEN_LBRACK) {
        parser_advance(ps);
        if (parser_peek(ps)->kind == TOKEN_NUMBER) {
            type.array_size = atoi(ps->src + parser_peek(ps)->start);
            parser_advance(ps);
        } else type.array_size = -1; // Unsized
        parser_expect(ps, TOKEN_RBRACK);
    }

    return type;
}

int token_stream_contains(TokenStream* ts, TokenKind kind) {
    for (int i = 0; i < ts->count; i++)
        if (ts->data[i].kind == kind)
            return 1;
    return 0;
}

AST* parse(Parser* ps) {
    AST* head = NULL;
    AST* tail = NULL;
    
    while (parser_peek(ps)->kind != TOKEN_EOF) {
        parser_skip_newline(ps);
        if (parser_peek(ps)->kind == TOKEN_EOF) break;
        AST* node;

        if (parser_peek(ps)->kind == TOKEN_STRUCT)
            node = parse_struct(ps);
        else
            node = parse_func_def(ps);
        if (head == NULL) head = node;
        else tail->next = node;
        tail = node;
    }

    return head;
}