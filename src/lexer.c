#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ANSI.h"

#include "lexer.h"


void lex(const char* src, TokenStream* ts) {
    int i = 0;

    while(src[i] != '\0') {
        if (isspace(src[i]) || src[i] == '\r') {
            i++;
            continue;
        }

        if (isalpha(src[i])) {
            int start = i;

            while (isalnum(src[i]))
                i++;
            
            int length = i - start;

            if (length == 3 && !strncmp(src + start, "int", 3))
                add_token(ts, TOKEN_INT, start, length);
            else if (length == 3 && !strncmp(src + start, "end", 3))
                add_token(ts, TOKEN_END, start, length);
            else if (length == 6 && !strncmp(src + start, "return", 6))
                add_token(ts, TOKEN_RETURN, start, length);
            else if (length == 5 && !strncmp(src + start, "while", 5))
                add_token(ts, TOKEN_WHILE, start, length);
            else
                add_token(ts, TOKEN_IDENTIFIER, start, length);
            continue;
        }

        if (isdigit(src[i])) {
            int start = i;

            while (isdigit(src[i]))
                i++;

            add_token(ts, TOKEN_NUMBER, start, i - start);
            continue;
        }

        printf("CHAR: '%c' (%d)\n", src[i], src[i]);
        switch (src[i]) {
            case '+': add_token(ts, TOKEN_PLUS, i++, 1); break;
            case '-': add_token(ts, TOKEN_MINUS, i++, 1); break;
            case '*': add_token(ts, TOKEN_STAR, i++, 1); break;
            case '/': add_token(ts, TOKEN_SLASH, i++, 1); break;
            case '&': add_token(ts, TOKEN_AMPERSAND, i++, 1); break;
            case '=': add_token(ts, TOKEN_ASSIGN, i++, 1); break;
            case '<': add_token(ts, TOKEN_LT, i++, 1); break;
            case '>': add_token(ts, TOKEN_GT, i++, 1); break;
            case '(': add_token(ts, TOKEN_LPAREN, i++, 1); break;
            case ')': add_token(ts, TOKEN_RPAREN, i++, 1); break;
            case ':': add_token(ts, TOKEN_COLON, i++, 1); break;
            case ',': add_token(ts, TOKEN_COMMA, i++, 1); break;
            default:
                printf(RED "Unknown character: %c\n", src[i]);
                exit(1);
        }
    }

    add_token(ts, TOKEN_EOF, i, 0);
}

void init_token_stream(TokenStream* ts) {
    ts->data = NULL;
    ts->count = 0;
    ts->capacity = 0;
}

void add_token(TokenStream* ts, TokenKind kind, int start, int length) {
    if (ts->count >= ts->capacity) {
        ts->capacity = ts->capacity == 0 ? 64 : ts->capacity * 2;
        Token* new_data = realloc(ts->data, ts->capacity * sizeof(Token));
        if (!new_data) {
            perror(RED "There was an error lexxing a token");
            return;
        }
        ts->data = new_data;
    }

    ts->data[ts->count].kind = kind;
    ts->data[ts->count].length = length;
    ts->data[ts->count].start = start;
    ts->count++;
}

Token* current(TokenStream* ts, int pos) {
    return &ts->data[pos];
}

Token* peek(TokenStream* ts, int index) {
    if (index >= ts->count)
        return &ts->data[ts->count - 1]; // EoF
    return &ts->data[index];
}

void free_token_stream(TokenStream* ts) {
    free(ts->data);
    ts->data = NULL;
    ts->count = 0;
    ts->capacity = 0;
}

const char* token_kind_name(TokenKind kind) {
    switch (kind) {
        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_NUMBER: return "TOKEN_NUMBER";
        case TOKEN_RETURN: return "TOKEN_RETURN";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_END: return "TOKEN_END";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_STAR: return "TOKEN_STAR";
        case TOKEN_SLASH: return "TOKEN_SLASH";
        case TOKEN_AMPERSAND: return "TOKEN_AMPERSAND";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_LT: return "TOKEN_LT";
        case TOKEN_GT: return "TOKEN_GT";
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_COLON: return "TOKEN_COLON";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_EOF: return "TOKEN_EOF";
        default: return "UNKNOWN";
    }
}

void print_token(Token* token, const char* src) {
    printf("%-18s\t", token_kind_name(token->kind));

    if (token->kind == TOKEN_IDENTIFIER ||
        token->kind == TOKEN_NUMBER) {
            printf("\"%.*s\"", token->length, src + token->start);
    }

    printf("\n");
}

void print_all_tokens(TokenStream* ts, const char* src) {
    for (int i = 0; i < ts->count; i++)
        print_token(&ts->data[i], src);
}