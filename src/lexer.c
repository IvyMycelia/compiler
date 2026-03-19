#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ANSI.h"

#include "lexer.h"


void lex(const char* src, TokenStream* ts) {
    int i = 0;

    while(src[i] != '\0') {
        if (src[i] == '/' && src[i+1] == '/') {
            while (src[i] != '\n' && src[i] != '\0')
                i++;
            continue;
        }
        if (src[i] == '\n') {
            add_token(ts, TOKEN_NEWLINE, i++, 1);
            continue;
        }
        if (src[i] == ';') {
            add_token(ts, TOKEN_SEMI, i++, 1);
            continue;
        }

        if (src[i] == ' ' || src[i] == '\r') {
            i++;
            continue;
        }

        if (src[i] == '"') {
            int start = i++;
            while (src[i] != '"' && src[i] != '\0')
                i++;
            i++;
            add_token(ts, TOKEN_STRING_LIT, start, i - start);
            continue;
        }

        if (isalpha(src[i])) {
            int start = i;

            while (isalnum(src[i]))
                i++;
            
            int length = i - start;

            if (length == 2 && !strncmp(src + start, "if", 2))
                add_token(ts, TOKEN_IF, start, length);
            else if (length == 2 && !strncmp(src + start, "as", 2))
                add_token(ts, TOKEN_AS, start, length);
            else if (length == 3 && !strncmp(src + start, "int", 3))
                add_token(ts, TOKEN_INT, start, length);
            else if (length == 3 && !strncmp(src + start, "end", 3))
                add_token(ts, TOKEN_END, start, length);
            else if (length == 3 && !strncmp(src + start, "new", 3))
                add_token(ts, TOKEN_NEW, start, length);
            else if (length == 4 && !strncmp(src + start, "null", 4))
                add_token(ts, TOKEN_NULL, start, length);
            else if (length == 4 && !strncmp(src + start, "char", 4))
                add_token(ts, TOKEN_CHAR, start, length);
            else if (length == 4 && !strncmp(src + start, "bool", 4))
                add_token(ts, TOKEN_BOOL, start, length);
            else if (length == 4 && !strncmp(src + start, "else", 4))
                add_token(ts, TOKEN_ELSE, start, length);
            else if (length == 4 && !strncmp(src + start, "void", 4))
                add_token(ts, TOKEN_VOID, start, length);
            else if (length == 4 && !strncmp(src + start, "prop", 4))
                add_token(ts, TOKEN_PROP, start, length);
            else if (length == 5 && !strncmp(src + start, "while", 5))
                add_token(ts, TOKEN_WHILE, start, length);
            else if (length == 5 && !strncmp(src + start, "prune", 5))
                add_token(ts, TOKEN_PRUNE, start, length);
            else if (length == 5 && !strncmp(src + start, "print", 5))
                add_token(ts, TOKEN_PRINT, start, length);
            else if (length == 5 && !strncmp(src + start, "float", 5))
                add_token(ts, TOKEN_FLOAT, start, length);
            else if (length == 6 && !strncmp(src + start, "return", 6))
                add_token(ts, TOKEN_RETURN, start, length);
            else if (length == 6 && !strncmp(src + start, "string", 6))
                add_token(ts, TOKEN_STRING, start, length);
            else if (length == 6 && !strncmp(src + start, "struct", 6))
                add_token(ts, TOKEN_STRUCT, start, length);
            else if (length == 6 && !strncmp(src + start, "import", 6))
                add_token(ts, TOKEN_IMPORT, start, length);
            else if (length == 6 && !strncmp(src + start, "double", 6))
                add_token(ts, TOKEN_DOUBLE, start, length);
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

        // printf("CHAR: '%c' (%d)\n", src[i], src[i]);
        switch (src[i]) {
            case '.': add_token(ts, TOKEN_DOT, i++, 1); break;
            case '+': add_token(ts, TOKEN_PLUS, i++, 1); break;
            case '-': add_token(ts, TOKEN_MINUS, i++, 1); break;
            case '*': add_token(ts, TOKEN_STAR, i++, 1); break;
            case '/': add_token(ts, TOKEN_SLASH, i++, 1); break;
            case '^': add_token(ts, TOKEN_CARET, i++, 1); break;
            case '&': add_token(ts, TOKEN_AMPERSAND, i++, 1); break;
            case '@': add_token(ts, TOKEN_AT, i++, 1); break;
            case '=': 
                if (src[i+1] == '=') { add_token(ts, TOKEN_COMP, i, 2); i+=2; }
                else add_token(ts, TOKEN_ASSIGN, i++, 1);
                break;
            case '<': add_token(ts, TOKEN_LT, i++, 1); break;
            case '>': add_token(ts, TOKEN_GT, i++, 1); break;
            case '(': add_token(ts, TOKEN_LPAREN, i++, 1); break;
            case ')': add_token(ts, TOKEN_RPAREN, i++, 1); break;
            case '[': add_token(ts, TOKEN_LBRACK, i++, 1); break;
            case ']': add_token(ts, TOKEN_RBRACK, i++, 1); break;
            case '{': add_token(ts, TOKEN_LBRACE, i++, 1); break;
            case '}': add_token(ts, TOKEN_RBRACE, i++, 1); break;
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
        /* Primitives */
        case TOKEN_INT:         return "TOKEN_INT";
        case TOKEN_FLOAT:       return "TOKEN_FLOAT";
        case TOKEN_DOUBLE:      return "TOKEN_DOUBLE";
        case TOKEN_VOID:        return "TOKEN_VOID";
        case TOKEN_NULL:        return "TOKEN_NULL";
        case TOKEN_BOOL:        return "TOKEN_BOOL";
        case TOKEN_CHAR:        return "TOKEN_CHAR";
        case TOKEN_STRING:      return "TOKEN_STRING";

        /* Identifiers */
        case TOKEN_STRUCT:      return "TOKEN_STRUCT";
        case TOKEN_IDENTIFIER:  return "TOKEN_IDENTIFIER";
        case TOKEN_NUMBER:      return "TOKEN_NUMBER";

        /* Keywords */
        case TOKEN_RETURN:      return "TOKEN_RETURN";
        case TOKEN_WHILE:       return "TOKEN_WHILE";
        case TOKEN_IF:          return "TOKEN_IF";
        case TOKEN_ELSE:        return "TOKEN_ELSE";
        case TOKEN_END:         return "TOKEN_END";

        /* Import System */
        case TOKEN_IMPORT:      return "TOKEN_IMPORT";
        case TOKEN_PROP:        return "TOKEN_PROP";

        /* Memory Management */
        case TOKEN_AS:          return "TOKEN_AS";
        case TOKEN_NEW:         return "TOKEN_NEW";
        case TOKEN_PRUNE:       return "TOKEN_PRUNE";

        /* stdio */
        case TOKEN_PRINT:       return "TOKEN_PRINT";

        /* Operators */
        case TOKEN_DOT:         return "TOKEN_DOT";
        case TOKEN_PLUS:        return "TOKEN_PLUS";
        case TOKEN_MINUS:       return "TOKEN_MINUS";
        case TOKEN_STAR:        return "TOKEN_STAR";
        case TOKEN_SLASH:       return "TOKEN_SLASH";
        case TOKEN_CARET:       return "TOKEN_CARET";
        case TOKEN_AMPERSAND:   return "TOKEN_AMPERSAND";
        case TOKEN_AT:          return "TOKEN_AT";
        case TOKEN_ASSIGN:      return "TOKEN_ASSIGN";
        case TOKEN_LT:          return "TOKEN_LT";
        case TOKEN_GT:          return "TOKEN_GT";
        case TOKEN_COMP:        return "TOKEN_COMP";

        /* Punctuation */
        case TOKEN_LPAREN:      return "TOKEN_LPAREN";
        case TOKEN_RPAREN:      return "TOKEN_RPAREN";
        case TOKEN_LBRACK:      return "TOKEN_LBRACK";
        case TOKEN_RBRACK:      return "TOKEN_RBRACK";
        case TOKEN_LBRACE:      return "TOKEN_LBRACE";
        case TOKEN_RBRACE:      return "TOKEN_RBRACE";
        case TOKEN_COLON:       return "TOKEN_COLON";
        case TOKEN_COMMA:       return "TOKEN_COMMA";
        case TOKEN_SEMI:        return "TOKEN_SEMI";

        case TOKEN_NEWLINE:     return "TOKEN_NEWLINE";

        case TOKEN_EOF:         return "TOKEN_EOF";

        default:                return "UNKNOWN";
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