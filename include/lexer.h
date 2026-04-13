#ifndef LEXER_H
#define LEXER_H

// Token Types
typedef enum {
    /* Primitives */
    TOKEN_INT,          // Integer
    TOKEN_FLOAT,        // Float-point value
    TOKEN_DOUBLE,
    TOKEN_BOOL,         // true / false, 1 / 0
    TOKEN_CHAR,         // Characters (represented as integers)

    /* Types */
    TOKEN_STRING,       // Array of char (char*)
    TOKEN_STRUCT,       // Struct keyword
    TOKEN_NULL,         // Empty value

    /* Unary Operations */
    TOKEN_TILDE,        // bitwise operation
    
    /* Identifiers */
    TOKEN_IDENTIFIER,   // Variable & Function names
    TOKEN_NUMBER,       // Number
    TOKEN_FLOAT_LIT,    // Decimal number
    TOKEN_STRING_LIT,   // String literal "Hello, World!"
    TOKEN_VOID,         // Void (empty)
    
    /* Keywords */
    TOKEN_RETURN,   // Return from block
    TOKEN_WHILE,    // While loop
    TOKEN_FOR,      // For loop
    TOKEN_IN,       // in
    TOKEN_END,      // Close block
    TOKEN_IF,       // If statement
    TOKEN_ELSE,     // Else block
    TOKEN_NOT,      // opposite of a value

    /* Import System */
    TOKEN_IMPORT,   // .flo imports
    TOKEN_AS,       // Alias for imports
    TOKEN_PROP,     // Propagating functions (making them public)

    /* Memory Management */
    TOKEN_NEW,      // Automatically allocate memory
    TOKEN_PRUNE,    // Free memory
    
    /* stdio */
    TOKEN_PRINT,    // Prints out to console

    /* Operators */
    TOKEN_DOT,      // Dot access
    TOKEN_DOTDOT,   // ..
    TOKEN_DOTDOTEQ, // ..=
    TOKEN_PLUS,     // Addition
    TOKEN_MINUS,    // Subtraction
    TOKEN_STAR,     // Multiplication
    TOKEN_CARET,    // ^ for exponents (eventually)
    TOKEN_SLASH,    // Division
    TOKEN_AMPERSAND,// Memory address
    TOKEN_AT,       // Pointer / deref, @
    TOKEN_ASSIGN,   // Assign value
    TOKEN_LT,       // Less than
    TOKEN_GT,       // Greater than
    TOKEN_COMP,     // Comparison `==`

    /* Punctuation */
    TOKEN_LPAREN,   // Function handling
    TOKEN_RPAREN,   // Function handling
    TOKEN_LBRACK,   // Array handling
    TOKEN_RBRACK,   // Array handling
    TOKEN_LBRACE,   // Struct handling
    TOKEN_RBRACE,   // Struct handling
    TOKEN_COLON,    // Start block
    TOKEN_COMMA,    // Separate parameters
    TOKEN_SEMI,     // Semicolon for starting a new statement

    TOKEN_NEWLINE,  // For determining next statement

    TOKEN_EOF   // End of File
} TokenKind;

typedef struct {
    TokenKind kind;
    int start;  // Index into source buffer
    int length; // Length of token
} Token;

typedef struct {
    Token* data;
    int count;
    int capacity;
} TokenStream;

void        lex(const char* src, TokenStream* ts);

void        init_token_stream(TokenStream* ts);
void        add_token(TokenStream* ts, TokenKind kind, int start, int length);
Token*      current(TokenStream* ts, int pos);
Token*      peek(TokenStream* ts, int pos);
void        free_token_stream(TokenStream* ts);

const char* token_kind_name(TokenKind kind);
void        print_token(Token* token, const char* src);
void        print_all_tokens(TokenStream* ts, const char* src);

#endif