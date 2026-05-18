#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
char project_root[512];
char* current_file;


void set_current_file(char* f) {
current_file = f;
}
char* RED = "\033[0;31m";
char* YELLOW = "\033[0;33m";
char* GREEN = "\033[0;32m";
char* RESET = "\033[0m";
char* BLUE = "\033[0;34m";
char* BOLD = "\033[1m";


char* resolve_path(char* importing_file, char* import_path) {
if (import_path[0] == '<') {
return NULL;
}
char resolved[1024];
if (import_path[0] == '.'  &&  import_path[1] == '/'  ||  import_path[0] == '.'  &&  import_path[1] == '.'  &&  import_path[2] == '/') {
char dir[1024];
strncpy(dir, importing_file, sizeof(dir) - 1);
dir[sizeof(dir) - 1] = '\0';
char* last_slash = strrchr(dir, '/');
if (last_slash) {
last_slash = last_slash + 1;
*last_slash = '\0';
}
else {
strcpy(dir, "./");
}
if (strlen(dir) + strlen(import_path) + 1 > sizeof(resolved)) {
printf("%sPath too long: %s%s%s\n", RED, dir, import_path, RESET);
return NULL;
}
snprintf(resolved, sizeof(resolved), "%s%s", dir, import_path + 2);
}
else {
snprintf(resolved, sizeof(resolved), "%s/%s", project_root, import_path);
}
return strdup(resolved);
}
#include <stdlib.h>
int TOKEN_INT = 1;
int TOKEN_FLOAT = 2;
int TOKEN_DOUBLE = 3;
int TOKEN_BOOL = 4;
int TOKEN_CHAR = 5;
int TOKEN_STRING = 6;
int TOKEN_STRUCT = 7;
int TOKEN_UNION = 8;
int TOKEN_NULL = 9;
int TOKEN_TILDE = 10;
int TOKEN_IDENTIFIER = 11;
int TOKEN_NUMBER = 12;
int TOKEN_FLOAT_LIT = 13;
int TOKEN_STRING_LIT = 14;
int TOKEN_CHAR_LIT = 15;
int TOKEN_VOID = 16;
int TOKEN_RETURN = 17;
int TOKEN_CONTINUE = 18;
int TOKEN_BREAK = 19;
int TOKEN_WHILE = 20;
int TOKEN_FOR = 21;
int TOKEN_IN = 22;
int TOKEN_END = 23;
int TOKEN_IF = 24;
int TOKEN_ELSE = 25;
int TOKEN_NOT = 26;
int TOKEN_AND = 27;
int TOKEN_OR = 28;
int TOKEN_IMPORT = 29;
int TOKEN_FORWARD = 30;
int TOKEN_AS = 31;
int TOKEN_PROP = 32;
int TOKEN_NEW = 33;
int TOKEN_PRUNE = 34;
int TOKEN_SIZEOF = 35;
int TOKEN_PRINT = 36;
int TOKEN_DOT = 37;
int TOKEN_DOTDOT = 38;
int TOKEN_DOTDOTEQ = 39;
int TOKEN_PLUS = 40;
int TOKEN_MINUS = 41;
int TOKEN_STAR = 42;
int TOKEN_CARET = 43;
int TOKEN_SLASH = 44;
int TOKEN_AMPERSAND = 45;
int TOKEN_AT = 46;
int TOKEN_ASSIGN = 47;
int TOKEN_LT = 48;
int TOKEN_GT = 49;
int TOKEN_COMP = 50;
int TOKEN_NEQ = 51;
int TOKEN_GEQ = 52;
int TOKEN_LEQ = 53;
int TOKEN_LPAREN = 54;
int TOKEN_RPAREN = 55;
int TOKEN_LBRACK = 56;
int TOKEN_RBRACK = 57;
int TOKEN_LBRACE = 58;
int TOKEN_RBRACE = 59;
int TOKEN_COLON = 60;
int TOKEN_COMMA = 61;
int TOKEN_SEMI = 62;
int TOKEN_NEWLINE = 63;
int TOKEN_EOF = 64;


typedef struct Token {
int kind;
int start;
int length;
} Token;


typedef struct TokenStream {
Token* data;
int count;
int capacity;
} TokenStream;


int get_line(char* src, int pos) {
int line = 1;
for (int i = 0; ((0) > (pos)) ? i > (pos) : i < (pos); ((0) > (pos)) ? i-- : i++) {
if (src[i] == '\n') {
line = line + 1;
}
}
return line;
}


int get_col(char* src, int pos) {
int col = 1;
while (pos > 0  &&  src[pos - 1] != '\n') {
col = col + 1;
pos = pos - 1;
}
return col;
}


void lexer_init_token_stream(TokenStream* ts) {
ts->data = NULL;
ts->count = 0;
ts->capacity = 0;
}


void add_token(TokenStream* ts, int kind, int start, int length) {
if (ts->count >= ts->capacity) {
if (ts->capacity == 0) {
ts->capacity = 64;
}
else {
ts->capacity = ts->capacity * 2;
}
ts->data = realloc(ts->data, ts->capacity * sizeof(Token));
if (ts->data == NULL) {
printf("Memory allocation failed\n");
return;}
}
Token* t = ts->data + ts->count;
t->kind = kind;
t->start = start;
t->length = length;
ts->count = ts->count + 1;
}


void lexer_lex(TokenStream* ts, char* src) {
int i = 0;
while (src[i] != '\0') {
if (src[i] == ' '  ||  src[i] == '\r') {
i = i + 1;
continue;
}
if (src[i] == '/'  &&  src[i + 1] == '/') {
while (src[i] != '\n'  &&  src[i] != '\0') {
i = i + 1;
}
continue;
}
if (src[i] == '\n') {
add_token(ts, TOKEN_NEWLINE, i, 1);
i = i + 1;
continue;
}
if (src[i] == '"') {
int start = i;
i = i + 1;
while (src[i] != '"'  &&  src[i] != '\0') {
if (src[i] == '\\') {
i = i + 1;
}
i = i + 1;
}
i = i + 1;
add_token(ts, TOKEN_STRING_LIT, start, i - start);
continue;
}
if (src[i] == '\'') {
int start = i;
i = i + 1;
while (src[i] != '\''  &&  src[i] != '\0') {
if (src[i] == '\\') {
i = i + 1;
}
i = i + 1;
}
i = i + 1;
add_token(ts, TOKEN_CHAR_LIT, start, i - start);
continue;
}
if (src[i] == ';') {
add_token(ts, TOKEN_SEMI, i, 1);
i = i + 1;
continue;
}
if (isdigit(src[i])) {
int start = i;
while (isdigit(src[i])) {
i = i + 1;
}
if (src[i] == '.'  &&  isdigit(src[i + 1])) {
i = i + 1;
while (isdigit(src[i])) {
i = i + 1;
}
add_token(ts, TOKEN_FLOAT_LIT, start, i - start);
}
else {
add_token(ts, TOKEN_NUMBER, start, i - start);
}
continue;
}
if (isalpha(src[i])  ||  src[i] == '_') {
int start = i;
while (isalnum(src[i])  ||  src[i] == '_') {
i = i + 1;
}
int length = i - start;
if (length == 2  &&  !(strncmp(src + start, "if", 2))) {
add_token(ts, TOKEN_IF, start, length);
}
else if (length == 2  &&  !(strncmp(src + start, "as", 2))) {
add_token(ts, TOKEN_AS, start, length);
}
else if (length == 2  &&  !(strncmp(src + start, "in", 2))) {
add_token(ts, TOKEN_IN, start, length);
}
else if (length == 2  &&  !(strncmp(src + start, "or", 2))) {
add_token(ts, TOKEN_OR, start, length);
}
else if (length == 3  &&  !(strncmp(src + start, "int", 3))) {
add_token(ts, TOKEN_INT, start, length);
}
else if (length == 3  &&  !(strncmp(src + start, "end", 3))) {
add_token(ts, TOKEN_END, start, length);
}
else if (length == 3  &&  !(strncmp(src + start, "new", 3))) {
add_token(ts, TOKEN_NEW, start, length);
}
else if (length == 3  &&  !(strncmp(src + start, "not", 3))) {
add_token(ts, TOKEN_NOT, start, length);
}
else if (length == 3  &&  !(strncmp(src + start, "and", 3))) {
add_token(ts, TOKEN_AND, start, length);
}
else if (length == 3  &&  !(strncmp(src + start, "for", 3))) {
add_token(ts, TOKEN_FOR, start, length);
}
else if (length == 4  &&  !(strncmp(src + start, "null", 4))) {
add_token(ts, TOKEN_NULL, start, length);
}
else if (length == 4  &&  !(strncmp(src + start, "char", 4))) {
add_token(ts, TOKEN_CHAR, start, length);
}
else if (length == 4  &&  !(strncmp(src + start, "bool", 4))) {
add_token(ts, TOKEN_BOOL, start, length);
}
else if (length == 4  &&  !(strncmp(src + start, "else", 4))) {
add_token(ts, TOKEN_ELSE, start, length);
}
else if (length == 4  &&  !(strncmp(src + start, "void", 4))) {
add_token(ts, TOKEN_VOID, start, length);
}
else if (length == 4  &&  !(strncmp(src + start, "prop", 4))) {
add_token(ts, TOKEN_PROP, start, length);
}
else if (length == 5  &&  !(strncmp(src + start, "union", 5))) {
add_token(ts, TOKEN_UNION, start, length);
}
else if (length == 5  &&  !(strncmp(src + start, "while", 5))) {
add_token(ts, TOKEN_WHILE, start, length);
}
else if (length == 5  &&  !(strncmp(src + start, "prune", 5))) {
add_token(ts, TOKEN_PRUNE, start, length);
}
else if (length == 5  &&  !(strncmp(src + start, "print", 5))) {
add_token(ts, TOKEN_PRINT, start, length);
}
else if (length == 5  &&  !(strncmp(src + start, "float", 5))) {
add_token(ts, TOKEN_FLOAT, start, length);
}
else if (length == 5  &&  !(strncmp(src + start, "break", 5))) {
add_token(ts, TOKEN_BREAK, start, length);
}
else if (length == 6  &&  !(strncmp(src + start, "return", 6))) {
add_token(ts, TOKEN_RETURN, start, length);
}
else if (length == 6  &&  !(strncmp(src + start, "string", 6))) {
add_token(ts, TOKEN_STRING, start, length);
}
else if (length == 6  &&  !(strncmp(src + start, "sizeof", 6))) {
add_token(ts, TOKEN_SIZEOF, start, length);
}
else if (length == 6  &&  !(strncmp(src + start, "struct", 6))) {
add_token(ts, TOKEN_STRUCT, start, length);
}
else if (length == 6  &&  !(strncmp(src + start, "import", 6))) {
add_token(ts, TOKEN_IMPORT, start, length);
}
else if (length == 6  &&  !(strncmp(src + start, "double", 6))) {
add_token(ts, TOKEN_DOUBLE, start, length);
}
else if (length == 7  &&  !(strncmp(src + start, "forward", 7))) {
add_token(ts, TOKEN_FORWARD, start, length);
}
else if (length == 8  &&  !(strncmp(src + start, "continue", 8))) {
add_token(ts, TOKEN_CONTINUE, start, length);
}
else {
add_token(ts, TOKEN_IDENTIFIER, start, length);
}
continue;
}
if (src[i] == '.') {
if (src[i + 1] == '.') {
if (src[i + 2] == '=') {
add_token(ts, TOKEN_DOTDOTEQ, i, 3);
i = i + 3;
continue;
}
else {
add_token(ts, TOKEN_DOTDOT, i, 2);
i = i + 2;
continue;
}
}
else {
add_token(ts, TOKEN_DOT, i, 1);
i = i + 1;
continue;
}
}
else if (src[i] == '+') {
add_token(ts, TOKEN_PLUS, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '-') {
add_token(ts, TOKEN_MINUS, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '~') {
add_token(ts, TOKEN_TILDE, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '*') {
add_token(ts, TOKEN_STAR, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '/') {
add_token(ts, TOKEN_SLASH, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '^') {
add_token(ts, TOKEN_CARET, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '&') {
add_token(ts, TOKEN_AMPERSAND, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '@') {
add_token(ts, TOKEN_AT, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '=') {
if (src[i + 1] == '=') {
add_token(ts, TOKEN_COMP, i, 2);
i = i + 2;
continue;
}
else {
add_token(ts, TOKEN_ASSIGN, i, 1);
i = i + 1;
continue;
}
}
else if (src[i] == '<') {
if (src[i + 1] == '=') {
add_token(ts, TOKEN_LEQ, i, 2);
i = i + 2;
continue;
}
else {
add_token(ts, TOKEN_LT, i, 1);
i = i + 1;
continue;
}
}
else if (src[i] == '>') {
if (src[i + 1] == '=') {
add_token(ts, TOKEN_GEQ, i, 2);
i = i + 2;
continue;
}
else {
add_token(ts, TOKEN_GT, i, 1);
i = i + 1;
continue;
}
}
else if (src[i] == '!') {
if (src[i + 1] == '=') {
add_token(ts, TOKEN_NEQ, i, 2);
i = i + 2;
continue;
}
else {
add_token(ts, TOKEN_NOT, i, 1);
i = i + 1;
continue;
}
}
else if (src[i] == '(') {
add_token(ts, TOKEN_LPAREN, i, 1);
i = i + 1;
continue;
}
else if (src[i] == ')') {
add_token(ts, TOKEN_RPAREN, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '[') {
add_token(ts, TOKEN_LBRACK, i, 1);
i = i + 1;
continue;
}
else if (src[i] == ']') {
add_token(ts, TOKEN_RBRACK, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '{') {
add_token(ts, TOKEN_LBRACE, i, 1);
i = i + 1;
continue;
}
else if (src[i] == '}') {
add_token(ts, TOKEN_RBRACE, i, 1);
i = i + 1;
continue;
}
else if (src[i] == ':') {
add_token(ts, TOKEN_COLON, i, 1);
i = i + 1;
continue;
}
else if (src[i] == ',') {
add_token(ts, TOKEN_COMMA, i, 1);
i = i + 1;
continue;
}
else {
printf("%s%s:%d:%d: error:%s Unknown char '%c' (%d)\n", RED, current_file, get_line(src, i), get_col(src, i), RESET, src[i], src[i]);
exit(1);
}
i = i + 1;
}
add_token(ts, TOKEN_EOF, i, 0);
}


char* token_kind_name(int kind) {
if (kind == TOKEN_INT) {
return "TOKEN_INT";
}
else if (kind == TOKEN_FLOAT) {
return "TOKEN_FLOAT";
}
else if (kind == TOKEN_DOUBLE) {
return "TOKEN_DOUBLE";
}
else if (kind == TOKEN_VOID) {
return "TOKEN_VOID";
}
else if (kind == TOKEN_NULL) {
return "TOKEN_NULL";
}
else if (kind == TOKEN_BOOL) {
return "TOKEN_BOOL";
}
else if (kind == TOKEN_CHAR) {
return "TOKEN_CHAR";
}
else if (kind == TOKEN_STRING) {
return "TOKEN_STRING";
}
else if (kind == TOKEN_STRING_LIT) {
return "TOKEN_STRING_LIT";
}
else if (kind == TOKEN_CHAR_LIT) {
return "TOKEN_CHAR_LIT";
}
else if (kind == TOKEN_STRUCT) {
return "TOKEN_STRUCT";
}
else if (kind == TOKEN_UNION) {
return "TOKEN_UNION";
}
else if (kind == TOKEN_IDENTIFIER) {
return "TOKEN_IDENTIFIER";
}
else if (kind == TOKEN_NUMBER) {
return "TOKEN_NUMBER";
}
else if (kind == TOKEN_RETURN) {
return "TOKEN_RETURN";
}
else if (kind == TOKEN_CONTINUE) {
return "TOKEN_CONTINUE";
}
else if (kind == TOKEN_BREAK) {
return "TOKEN_BREAK";
}
else if (kind == TOKEN_WHILE) {
return "TOKEN_WHILE";
}
else if (kind == TOKEN_IF) {
return "TOKEN_IF";
}
else if (kind == TOKEN_ELSE) {
return "TOKEN_ELSE";
}
else if (kind == TOKEN_END) {
return "TOKEN_END";
}
else if (kind == TOKEN_NOT) {
return "TOKEN_NOT";
}
else if (kind == TOKEN_FOR) {
return "TOKEN_FOR";
}
else if (kind == TOKEN_AND) {
return "TOKEN_AND";
}
else if (kind == TOKEN_IN) {
return "TOKEN_IN";
}
else if (kind == TOKEN_OR) {
return "TOKEN_OR";
}
else if (kind == TOKEN_IMPORT) {
return "TOKEN_IMPORT";
}
else if (kind == TOKEN_PROP) {
return "TOKEN_PROP";
}
else if (kind == TOKEN_AS) {
return "TOKEN_AS";
}
else if (kind == TOKEN_NEW) {
return "TOKEN_NEW";
}
else if (kind == TOKEN_PRUNE) {
return "TOKEN_PRUNE";
}
else if (kind == TOKEN_SIZEOF) {
return "TOKEN_SIZEOF";
}
else if (kind == TOKEN_PRINT) {
return "TOKEN_PRINT";
}
else if (kind == TOKEN_DOT) {
return "TOKEN_DOT";
}
else if (kind == TOKEN_DOTDOT) {
return "TOKEN_DOTDOT";
}
else if (kind == TOKEN_DOTDOTEQ) {
return "TOKEN_DOTDOTEQ";
}
else if (kind == TOKEN_PLUS) {
return "TOKEN_PLUS";
}
else if (kind == TOKEN_MINUS) {
return "TOKEN_MINUS";
}
else if (kind == TOKEN_TILDE) {
return "TOKEN_TILDE";
}
else if (kind == TOKEN_STAR) {
return "TOKEN_STAR";
}
else if (kind == TOKEN_SLASH) {
return "TOKEN_SLASH";
}
else if (kind == TOKEN_CARET) {
return "TOKEN_CARET";
}
else if (kind == TOKEN_AMPERSAND) {
return "TOKEN_AMPERSAND";
}
else if (kind == TOKEN_AT) {
return "TOKEN_AT";
}
else if (kind == TOKEN_ASSIGN) {
return "TOKEN_ASSIGN";
}
else if (kind == TOKEN_LT) {
return "TOKEN_LT";
}
else if (kind == TOKEN_GT) {
return "TOKEN_GT";
}
else if (kind == TOKEN_NEQ) {
return "TOKEN_NEQ";
}
else if (kind == TOKEN_GEQ) {
return "TOKEN_GEQ";
}
else if (kind == TOKEN_LEQ) {
return "TOKEN_LEQ";
}
else if (kind == TOKEN_COMP) {
return "TOKEN_COMP";
}
else if (kind == TOKEN_LPAREN) {
return "TOKEN_LPAREN";
}
else if (kind == TOKEN_RPAREN) {
return "TOKEN_RPAREN";
}
else if (kind == TOKEN_LBRACK) {
return "TOKEN_LBRACK";
}
else if (kind == TOKEN_RBRACK) {
return "TOKEN_RBRACK";
}
else if (kind == TOKEN_LBRACE) {
return "TOKEN_LBRACE";
}
else if (kind == TOKEN_RBRACE) {
return "TOKEN_RBRACE";
}
else if (kind == TOKEN_COLON) {
return "TOKEN_COLON";
}
else if (kind == TOKEN_COMMA) {
return "TOKEN_COMMA";
}
else if (kind == TOKEN_SEMI) {
return "TOKEN_SEMI";
}
else if (kind == TOKEN_NEWLINE) {
return "TOKEN_NEWLINE";
}
else if (kind == TOKEN_EOF) {
return "TOKEN_EOF";
}
else {
return "UNKNOWN";
}
}


void print_token(Token* token, char* src) {
printf("%-18s\t", token_kind_name(token->kind));
if (token->kind == TOKEN_IDENTIFIER  ||  token->kind == TOKEN_NUMBER) {
printf("\"%.*s\"", token->length, src + token->start);
}
printf("\n");
}


void lexer_print_all_tokens(TokenStream* ts, char* src) {
for (int i = 0; ((0) > (ts->count)) ? i > (ts->count) : i < (ts->count); ((0) > (ts->count)) ? i-- : i++) {
print_token(&(ts->data[i]), src);
}
}


Token* peek(TokenStream* ts, int index) {
if (index >= ts->count) {
return &(ts->data[ts->count - 1]);
}
return &(ts->data[index]);
}


void free_token_stream(TokenStream* ts) {
free(ts->data);
ts->data = NULL;
ts->count = 0;
ts->capacity = 0;
}
int AST_FLOW_CONTROL = 0;
int AST_LITERAL = 1;
int AST_FLOAT_LIT = 2;
int AST_CAST = 3;
int AST_NULL = 4;
int AST_STRING_LIT = 5;
int AST_ARRAY_LIT = 6;
int AST_SUBSCRIPT = 7;
int AST_STRUCT_LIT = 8;
int AST_CHAR_LIT = 9;
int AST_STRUCT_DEF = 10;
int AST_UNION_DEF = 11;
int AST_STRUCT_FIELD = 12;
int AST_DOT_ACCESS = 13;
int AST_BINARY_OP = 14;
int AST_UNARY_NOT = 15;
int AST_UNARY_NEG = 16;
int AST_NEW = 17;
int AST_PRUNE = 18;
int AST_SIZEOF = 19;
int AST_DEREF_ASS = 20;
int AST_DEREF = 21;
int AST_GET_ADDR = 22;
int AST_IMPORT = 23;
int AST_PROP = 24;
int AST_FORWARD = 25;
int AST_ALIAS_CALL = 26;
int AST_VAR_DECL = 27;
int AST_VAR_ASS = 28;
int AST_VAR_REF = 29;
int AST_FUNC_DEF = 30;
int AST_FUNC_CALL = 31;
int AST_WHILE = 32;
int AST_FOR = 33;
int AST_IF = 34;
int AST_PARAM = 35;
int AST_PRINT = 36;
typedef struct AST AST;


typedef struct TypeInfo {
int base;
int pointer_depth;
int array_size;
AST* arr_size_expr;
int name_start;
int name_length;
} TypeInfo;


typedef struct cast {
AST* value;
TypeInfo type;
} cast;


typedef struct float_lit {
int start;
int length;
} float_lit;


typedef struct char_lit {
int start;
int length;
} char_lit;


typedef struct strng {
int str_start;
int str_length;
} strng;


typedef struct array {
AST* elements;
int arr_length;
} array;


typedef struct subscript {
AST* array;
AST* index;
AST* value;
} subscript;


typedef struct unary {
AST* operand;
} unary;


typedef struct new_alloc {
TypeInfo type;
} new_alloc;


typedef struct prune_free {
AST* ptr;
} prune_free;


typedef struct size_of {
TypeInfo type;
} size_of;


typedef struct deref_ass {
int name_start;
int name_length;
AST* value;
} deref_ass;


typedef struct deref {
AST* operand;
} deref;


typedef struct get_addr {
AST* operand;
} get_addr;


typedef struct prnt {
AST* value;
} prnt;


typedef struct imprt {
int path_start;
int path_length;
int alias_start;
int alias_length;
int is_system;
int has_alias;
} imprt;


typedef struct prp {
AST* func;
} prp;


typedef struct forwrd {
int name_start;
int name_length;
int is_func;
TypeInfo return_type;
AST* params;
} forwrd;


typedef struct alias_call {
int alias_start;
int alias_length;
int func_start;
int func_length;
AST* args;
} alias_call;


typedef struct struct_def {
int name_start;
int name_length;
AST* fields;
} struct_def;


typedef struct union_def {
int name_start;
int name_length;
AST* fields;
} union_def;


typedef struct struct_field {
int name_start;
int name_length;
TypeInfo type;
} struct_field;


typedef struct struct_lit {
AST* elements;
} struct_lit;


typedef struct dot_access {
AST* object;
int field_start;
int field_length;
AST* value;
} dot_access;


typedef struct binary {
AST* left;
AST* right;
int op;
} binary;


typedef struct var_decl {
int name_start;
int name_length;
TypeInfo type;
AST* value;
} var_decl;


typedef struct var_ass {
int name_start;
int name_length;
AST* value;
} var_ass;


typedef struct var_ref {
int name_start;
int name_length;
} var_ref;


typedef struct func_def {
int name_start;
int name_length;
AST* params;
AST* body;
TypeInfo return_type;
} func_def;


typedef struct func_params {
int name_start;
int name_length;
TypeInfo type;
} func_params;


typedef struct func_call {
int name_start;
int name_length;
AST* args;
} func_call;


typedef struct flow_ctrl {
Token* base;
AST* value;
} flow_ctrl;


typedef struct while_loop {
AST* condition;
AST* body;
} while_loop;


typedef struct for_loop {
int var_start;
int var_length;
AST* from;
AST* to;
int inclusive;
AST* body;
} for_loop;


typedef struct if_condition {
AST* condition;
AST* body;
AST* else_branch;
} if_condition;


typedef union NodeData {
cast _cast;
float_lit _float_lit;
char_lit _char_lit;
strng _string;
array _array;
subscript _subscript;
unary _unary;
new_alloc _new_alloc;
prune_free _prune_free;
size_of _size_of;
deref_ass _deref_ass;
deref _deref;
get_addr _get_addr;
prnt _print;
imprt _import;
prp _prop;
forwrd _forward;
alias_call _alias_call;
struct_def _struct_def;
union_def _union_def;
struct_field _struct_field;
struct_lit _struct_lit;
dot_access _dot_access;
binary _binary;
var_decl _var_decl;
var_ass _var_ass;
var_ref _var_ref;
func_def _func_def;
func_params _func_params;
func_call _func_call;
flow_ctrl _flow_ctrl;
while_loop _while_loop;
for_loop _for_loop;
if_condition _if_condition;
} NodeData;


typedef struct AST {
int kind;
AST* next;
int value;
NodeData data;
} AST;
#include <stdlib.h>


typedef struct Parser {
TokenStream* ts;
char* src;
int pos;
int alias_start[32];
int alias_lengths[32];
int alias_count;
char* filename;
int error_count;
} Parser;


AST* make_node(int kind) {
AST* node = calloc(1, sizeof(AST));
node->kind = kind;
node->value = 0;
return node;
}


void parser_init_parser(Parser* ps, TokenStream* ts, char* src, char* filename) {
ps->ts = ts;
ps->src = src;
ps->pos = 0;
ps->alias_count = 0;
ps->filename = filename;
ps->error_count = 0;
}
void parser_error(Parser* ps, char* message);
AST* parse_var_decl(Parser* ps);
AST* parse_var_ass(Parser* ps);
AST* parse_subscript_ass(Parser* ps);
AST* parse_return(Parser* ps);
AST* parse_continue(Parser* ps);
AST* parse_break(Parser* ps);
AST* parse_while(Parser* ps);
AST* parse_if(Parser* ps, int is_elf_if);
AST* parse_for(Parser* ps);
AST* parse_expr(Parser* ps, int min_prec);
AST* parse_statement(Parser* ps);
AST* parse_primary(Parser* ps);
AST* parse_func_def(Parser* ps);
AST* parse_func_call(Parser* ps);
AST* parse_alias_call(Parser* ps);
AST* parse_param(Parser* ps);
AST* parse_struct(Parser* ps);
AST* parse_dot_ass(Parser* ps);
AST* parse_forward(Parser* ps);
AST* parse_import(Parser* ps);


Token* parser_peek(Parser* ps) {
return peek(ps->ts, ps->pos);
}


Token* parser_advance(Parser* ps) {
Token* token = parser_peek(ps);
if (token->kind != TOKEN_EOF) {
ps->pos = ps->pos + 1;
}
return token;
}


void parser_skip_newline(Parser* ps) {
while (parser_peek(ps)->kind == TOKEN_NEWLINE  ||  parser_peek(ps)->kind == TOKEN_SEMI) {
parser_advance(ps);
}
}


int parser_expect(Parser* ps, int kind) {
Token* curr_tok = parser_peek(ps);
if (curr_tok->kind != kind) {
char message[128];
snprintf(message, sizeof(message), "Expected token: %s but got '%.*s' (%s)", token_kind_name(kind), curr_tok->length, curr_tok->start + ps->src, token_kind_name(curr_tok->kind));
parser_error(ps, message);
parser_advance(ps);
return 0;
}
parser_advance(ps);
return 1;
}


TypeInfo* parse_type(Parser* ps) {
TypeInfo* type = malloc(sizeof(TypeInfo));
type->pointer_depth = 0;
type->array_size = 0;
type->arr_size_expr = NULL;
while (parser_peek(ps)->kind == TOKEN_AT) {
type->pointer_depth = type->pointer_depth + 1;
parser_advance(ps);
}
Token* base = parser_advance(ps);
type->base = base->kind;
if (base->kind == TOKEN_IDENTIFIER) {
type->name_start = base->start;
type->name_length = base->length;
}
if (parser_peek(ps)->kind == TOKEN_LBRACK) {
parser_advance(ps);
if (parser_peek(ps)->kind == TOKEN_RBRACK) {
type->array_size = -1;
}
else if (parser_peek(ps)->kind == TOKEN_NUMBER) {
type->array_size = atoi(ps->src + parser_peek(ps)->start);
parser_advance(ps);
}
else {
type->arr_size_expr = parse_expr(ps, 0);
}
parser_expect(ps, TOKEN_RBRACK);
}
return type;
}


int get_precedence(int kind) {
if (kind == TOKEN_STAR  ||  kind == TOKEN_SLASH) {
return 3;
}
else if (kind == TOKEN_AND  ||  kind == TOKEN_PLUS  ||  kind == TOKEN_MINUS) {
return 2;
}
else if (kind == TOKEN_GT  ||  kind == TOKEN_LT  ||  kind == TOKEN_OR  ||  kind == TOKEN_NEQ  ||  kind == TOKEN_LEQ  ||  kind == TOKEN_GEQ  ||  kind == TOKEN_COMP) {
return 1;
}
else if (kind == TOKEN_DOTDOT) {
return 0;
}
else if (kind == TOKEN_DOTDOTEQ) {
return 0;
}
else {
return 0;
}
}


int token_stream_contains(TokenStream* ts, int kind) {
for (int i = 0; ((0) > (ts->count)) ? i > (ts->count) : i < (ts->count); ((0) > (ts->count)) ? i-- : i++) {
Token* t = ts->data + i;
if (t->kind == kind) {
return 1;
}
}
return 0;
}


int is_alias(Parser* ps, Token* tok) {
for (int i = 0; ((0) > (ps->alias_count)) ? i > (ps->alias_count) : i < (ps->alias_count); ((0) > (ps->alias_count)) ? i-- : i++) {
if (ps->alias_lengths[i] == tok->length  &&  !(strncmp(ps->src + ps->alias_start[i], ps->src + tok->start, tok->length))) {
return 1;
}
}
return 0;
}


void parser_error(Parser* ps, char* message) {
Token* token = parser_peek(ps);
int line = get_line(ps->src, token->start);
int col = get_col(ps->src, token->start);
printf("%s%s:%d:%d: error:%s %s\n", RED, ps->filename, line, col, RESET, message);
ps->error_count = ps->error_count + 1;
}


AST* parse_var_decl(Parser* ps) {
AST* node = make_node(AST_VAR_DECL);
Token* tok = parser_advance(ps);
node->data._var_decl.name_start = tok->start;
node->data._var_decl.name_length = tok->length;
parser_expect(ps, TOKEN_COLON);
node->data._var_decl.type = *(parse_type(ps));
if (parser_peek(ps)->kind == TOKEN_ASSIGN) {
parser_advance(ps);
node->data._var_decl.value = parse_expr(ps, 0);
}
return node;
}


AST* parse_var_ass(Parser* ps) {
AST* node = make_node(AST_VAR_ASS);
Token* tok = parser_advance(ps);
node->data._var_ass.name_start = tok->start;
node->data._var_ass.name_length = tok->length;
parser_expect(ps, TOKEN_ASSIGN);
node->data._var_ass.value = parse_expr(ps, 0);
return node;
}


AST* parse_subscript_ass(Parser* ps) {
Token* name = parser_advance(ps);
AST* arr_ref = make_node(AST_VAR_REF);
arr_ref->data._var_ref.name_start = name->start;
arr_ref->data._var_ref.name_length = name->length;
parser_expect(ps, TOKEN_LBRACK);
AST* idx = parse_expr(ps, 0);
parser_expect(ps, TOKEN_RBRACK);
parser_expect(ps, TOKEN_ASSIGN);
AST* val = parse_expr(ps, 0);
AST* sub = make_node(AST_SUBSCRIPT);
sub->data._subscript.array = arr_ref;
sub->data._subscript.index = idx;
sub->data._subscript.value = val;
return sub;
}


AST* parse_return(Parser* ps) {
AST* node = make_node(AST_FLOW_CONTROL);
node->data._flow_ctrl.base = parser_peek(ps);
parser_advance(ps);
if (parser_peek(ps)->kind == TOKEN_NEWLINE  ||  parser_peek(ps)->kind == TOKEN_SEMI  ||  parser_peek(ps)->kind == TOKEN_EOF) {
node->data._flow_ctrl.value = NULL;
return node;
}
node->data._flow_ctrl.value = parse_expr(ps, 0);
return node;
}


AST* parse_continue(Parser* ps) {
AST* node = make_node(AST_FLOW_CONTROL);
node->data._flow_ctrl.value = NULL;
node->data._flow_ctrl.base = parser_peek(ps);
parser_advance(ps);
return node;
}


AST* parse_break(Parser* ps) {
AST* node = make_node(AST_FLOW_CONTROL);
node->data._flow_ctrl.value = NULL;
node->data._flow_ctrl.base = parser_peek(ps);
parser_advance(ps);
return node;
}


AST* parse_while(Parser* ps) {
AST* node = make_node(AST_WHILE);
parser_expect(ps, TOKEN_WHILE);
node->data._while_loop.condition = parse_expr(ps, 0);
parser_expect(ps, TOKEN_COLON);
AST* body_head = NULL;
AST* body_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_END) {
parser_skip_newline(ps);
if (parser_peek(ps)->kind == TOKEN_END) {
break;
}
AST* statement = parse_statement(ps);
if (body_head == NULL) {
body_head = statement;
}
else {
body_tail->next = statement;
}
body_tail = statement;
}
parser_expect(ps, TOKEN_END);
node->data._while_loop.body = body_head;
return node;
}


AST* parse_if(Parser* ps, int is_elf_if) {
AST* node = make_node(AST_IF);
parser_expect(ps, TOKEN_IF);
node->data._if_condition.condition = parse_expr(ps, 0);
parser_expect(ps, TOKEN_COLON);
AST* body_head = NULL;
AST* body_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_END  &&  parser_peek(ps)->kind != TOKEN_ELSE) {
parser_skip_newline(ps);
if (parser_peek(ps)->kind == TOKEN_END  ||  parser_peek(ps)->kind == TOKEN_ELSE) {
break;
}
AST* statement = parse_statement(ps);
if (body_head == NULL) {
body_head = statement;
}
else {
body_tail->next = statement;
}
body_tail = statement;
}
node->data._if_condition.body = body_head;
if (parser_peek(ps)->kind == TOKEN_ELSE) {
parser_advance(ps);
if (parser_peek(ps)->kind == TOKEN_IF) {
node->data._if_condition.else_branch = parse_if(ps, 1);
}
else {
AST* else_node = make_node(AST_IF);
else_node->data._if_condition.condition = NULL;
parser_expect(ps, TOKEN_COLON);
AST* else_body_head = NULL;
AST* else_body_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_END  &&  parser_peek(ps)->kind != TOKEN_ELSE) {
parser_skip_newline(ps);
if (parser_peek(ps)->kind == TOKEN_END  ||  parser_peek(ps)->kind == TOKEN_ELSE) {
break;
}
AST* statement = parse_statement(ps);
if (else_body_head == NULL) {
else_body_head = statement;
}
else {
else_body_tail->next = statement;
}
else_body_tail = statement;
}
else_node->data._if_condition.body = else_body_head;
node->data._if_condition.else_branch = else_node;
}
}
if (!(is_elf_if)) {
parser_expect(ps, TOKEN_END);
}
return node;
}


AST* parse_for(Parser* ps) {
AST* node = make_node(AST_FOR);
parser_expect(ps, TOKEN_FOR);
Token* var = parser_advance(ps);
if (var->kind != TOKEN_IDENTIFIER) {
printf("Expected loop variable after for, got %s\n", token_kind_name(var->kind));
exit(1);
}
node->data._for_loop.var_start = var->start;
node->data._for_loop.var_length = var->length;
parser_expect(ps, TOKEN_IN);
AST* from = parse_expr(ps, 0);
node->data._for_loop.from = from;
Token* op_range = parser_advance(ps);
if (op_range->kind != TOKEN_DOTDOT  &&  op_range->kind != TOKEN_DOTDOTEQ) {
printf("Expected range operator `..` OR `..=`, got %s\n", token_kind_name(op_range->kind));
exit(1);
}
node->data._for_loop.inclusive = op_range->kind == TOKEN_DOTDOTEQ;
AST* to = parse_expr(ps, 0);
node->data._for_loop.to = to;
parser_expect(ps, TOKEN_COLON);
AST* body_head = NULL;
AST* body_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_END) {
parser_skip_newline(ps);
if (parser_peek(ps)->kind == TOKEN_END) {
break;
}
AST* statement = parse_statement(ps);
if (body_head == NULL) {
body_head = statement;
}
else {
body_tail->next = statement;
}
body_tail = statement;
}
parser_expect(ps, TOKEN_END);
node->data._for_loop.body = body_head;
return node;
}


AST* parse_expr(Parser* ps, int min_prec) {
AST* left = parse_primary(ps);
if (parser_peek(ps)->kind == TOKEN_AS) {
parser_advance(ps);
AST* cast = make_node(AST_CAST);
cast->data._cast.value = left;
cast->data._cast.type = *(parse_type(ps));
left = cast;
}
while (get_precedence(parser_peek(ps)->kind) > min_prec) {
Token* op = parser_advance(ps);
AST* right = parse_expr(ps, get_precedence(op->kind));
AST* bin = make_node(AST_BINARY_OP);
bin->data._binary.left = left;
bin->data._binary.right = right;
bin->data._binary.op = op->kind;
left = bin;
}
return left;
}


AST* parse_statement(Parser* ps) {
parser_skip_newline(ps);
if (parser_peek(ps)->kind == TOKEN_IDENTIFIER) {
if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_COLON) {
return parse_var_decl(ps);
}
else if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_ASSIGN) {
return parse_var_ass(ps);
}
else if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_LPAREN) {
return parse_func_call(ps);
}
else if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_LBRACK) {
return parse_dot_ass(ps);
}
else if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_DOT) {
if (is_alias(ps, parser_peek(ps))) {
return parse_alias_call(ps);
}
else {
return parse_dot_ass(ps);
}
}
else {
printf("%s%s:%d:%d: error:%s parse_statement: unexpected token after identifier: %s\n", RED, current_file, get_line(ps->src, parser_peek(ps)->start), get_col(ps->src, parser_peek(ps)->start), RESET, token_kind_name(peek(ps->ts, ps->pos + 1)->kind));
exit(1);
}
}
else if (parser_peek(ps)->kind == TOKEN_WHILE) {
return parse_while(ps);
}
else if (parser_peek(ps)->kind == TOKEN_RETURN) {
return parse_return(ps);
}
else if (parser_peek(ps)->kind == TOKEN_BREAK) {
return parse_break(ps);
}
else if (parser_peek(ps)->kind == TOKEN_CONTINUE) {
return parse_continue(ps);
}
else if (parser_peek(ps)->kind == TOKEN_IF) {
return parse_if(ps, 0);
}
else if (parser_peek(ps)->kind == TOKEN_FOR) {
return parse_for(ps);
}
else if (parser_peek(ps)->kind == TOKEN_PRINT) {
parser_advance(ps);
AST* node = make_node(AST_PRINT);
parser_expect(ps, TOKEN_LPAREN);
node->data._print.value = parse_expr(ps, 0);
parser_expect(ps, TOKEN_RPAREN);
return node;
}
else if (parser_peek(ps)->kind == TOKEN_PRUNE) {
parser_advance(ps);
AST* node = make_node(AST_PRUNE);
node->data._prune_free.ptr = parse_expr(ps, 0);
return node;
}
else if (parser_peek(ps)->kind == TOKEN_AT) {
parser_advance(ps);
AST* node = make_node(AST_DEREF_ASS);
Token* name = parser_advance(ps);
node->data._deref_ass.name_length = name->length;
node->data._deref_ass.name_start = name->start;
parser_expect(ps, TOKEN_ASSIGN);
node->data._deref_ass.value = parse_expr(ps, 0);
return node;
}
else {
printf("%s%s:%d:%d: error:%s parse_statement: unexpected token: %s\n", RED, current_file, get_line(ps->src, parser_peek(ps)->start), get_col(ps->src, parser_peek(ps)->start), RESET, token_kind_name(parser_peek(ps)->kind));
exit(1);
}
}


AST* parse_primary(Parser* ps) {
if (parser_peek(ps)->kind == TOKEN_IDENTIFIER) {
if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_LPAREN) {
AST* node = parse_func_call(ps);
if (parser_peek(ps)->kind == TOKEN_DOT) {
parser_advance(ps);
Token* field = parser_advance(ps);
AST* dot = make_node(AST_DOT_ACCESS);
dot->data._dot_access.object = node;
dot->data._dot_access.field_length = field->length;
dot->data._dot_access.field_start = field->start;
return dot;
}
return node;
}
if (peek(ps->ts, ps->pos + 1)->kind == TOKEN_DOT  &&  is_alias(ps, parser_peek(ps))) {
return parse_alias_call(ps);
}
Token* tok = parser_peek(ps);
parser_advance(ps);
AST* node = make_node(AST_VAR_REF);
node->data._var_ref.name_start = tok->start;
node->data._var_ref.name_length = tok->length;
if (parser_peek(ps)->kind == TOKEN_AT) {
parser_advance(ps);
AST* operand = parse_primary(ps);
AST* drf = make_node(AST_DEREF);
drf->data._unary.operand = operand;
return drf;
}
while (parser_peek(ps)->kind == TOKEN_DOT  ||  parser_peek(ps)->kind == TOKEN_LBRACK) {
if (parser_peek(ps)->kind == TOKEN_DOT) {
parser_advance(ps);
Token* field = parser_advance(ps);
AST* dot = make_node(AST_DOT_ACCESS);
dot->data._dot_access.object = node;
dot->data._dot_access.field_start = field->start;
dot->data._dot_access.field_length = field->length;
node = dot;
}
else if (parser_peek(ps)->kind == TOKEN_LBRACK) {
parser_advance(ps);
AST* idx = parse_expr(ps, 0);
parser_expect(ps, TOKEN_RBRACK);
AST* sub = make_node(AST_SUBSCRIPT);
sub->data._subscript.array = node;
sub->data._subscript.index = idx;
node = sub;
}
}
return node;
}
else if (parser_peek(ps)->kind == TOKEN_NUMBER) {
AST* lit = make_node(AST_LITERAL);
lit->value = atoi(ps->src + parser_peek(ps)->start);
parser_advance(ps);
return lit;
}
else if (parser_peek(ps)->kind == TOKEN_FLOAT_LIT) {
AST* lit = make_node(AST_FLOAT_LIT);
lit->data._float_lit.start = parser_peek(ps)->start;
lit->data._float_lit.length = parser_peek(ps)->length;
parser_advance(ps);
return lit;
}
else if (parser_peek(ps)->kind == TOKEN_NULL) {
AST* nll = make_node(AST_NULL);
parser_advance(ps);
return nll;
}
else if (parser_peek(ps)->kind == TOKEN_STRING_LIT) {
AST* str = make_node(AST_STRING_LIT);
str->data._string.str_start = parser_peek(ps)->start;
str->data._string.str_length = parser_peek(ps)->length;
parser_advance(ps);
return str;
}
else if (parser_peek(ps)->kind == TOKEN_LBRACK) {
parser_advance(ps);
AST* arr = make_node(AST_ARRAY_LIT);
AST* elem_head = NULL;
AST* elem_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RBRACK) {
AST* elem = parse_expr(ps, 0);
if (elem_head == NULL) {
elem_head = elem;
}
else {
elem_tail->next = elem;
}
elem_tail = elem;
if (parser_peek(ps)->kind == TOKEN_COMMA) {
parser_advance(ps);
}
}
parser_expect(ps, TOKEN_RBRACK);
arr->data._array.elements = elem_head;
return arr;
}
else if (parser_peek(ps)->kind == TOKEN_LBRACE) {
parser_advance(ps);
AST* strct_lit = make_node(AST_STRUCT_LIT);
AST* struct_head = NULL;
AST* struct_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RBRACE) {
AST* elem = parse_expr(ps, 0);
if (struct_head == NULL) {
struct_head = elem;
}
else {
struct_tail->next = elem;
}
struct_tail = elem;
if (parser_peek(ps)->kind == TOKEN_COMMA) {
parser_advance(ps);
}
}
parser_expect(ps, TOKEN_RBRACE);
strct_lit->data._struct_lit.elements = struct_head;
return strct_lit;
}
else if (parser_peek(ps)->kind == TOKEN_LPAREN) {
parser_advance(ps);
AST* expr = parse_expr(ps, 0);
parser_expect(ps, TOKEN_RPAREN);
return expr;
}
else if (parser_peek(ps)->kind == TOKEN_NEW) {
parser_advance(ps);
AST* new_node = make_node(AST_NEW);
new_node->data._new_alloc.type = *(parse_type(ps));
return new_node;
}
else if (parser_peek(ps)->kind == TOKEN_NOT) {
parser_advance(ps);
AST* operand = parse_primary(ps);
AST* unry = make_node(AST_UNARY_NOT);
unry->data._unary.operand = operand;
return unry;
}
else if (parser_peek(ps)->kind == TOKEN_MINUS) {
parser_advance(ps);
AST* operand = parse_primary(ps);
AST* unry = make_node(AST_UNARY_NEG);
unry->data._unary.operand = operand;
return unry;
}
else if (parser_peek(ps)->kind == TOKEN_AT) {
parser_advance(ps);
AST* operand = parse_primary(ps);
AST* deref = make_node(AST_DEREF);
deref->data._deref.operand = operand;
return deref;
}
else if (parser_peek(ps)->kind == TOKEN_AMPERSAND) {
parser_advance(ps);
AST* operand = parse_primary(ps);
AST* get = make_node(AST_GET_ADDR);
get->data._get_addr.operand = operand;
return get;
}
else if (parser_peek(ps)->kind == TOKEN_SIZEOF) {
parser_advance(ps);
parser_expect(ps, TOKEN_LPAREN);
AST* node = make_node(AST_SIZEOF);
node->data._size_of.type = *(parse_type(ps));
parser_expect(ps, TOKEN_RPAREN);
return node;
}
else if (parser_peek(ps)->kind == TOKEN_CHAR_LIT) {
AST* node = make_node(AST_CHAR_LIT);
node->data._char_lit.start = parser_peek(ps)->start;
node->data._char_lit.length = parser_peek(ps)->length;
parser_advance(ps);
return node;
}
else {
printf("%s%s:%d:%d: error:%s parse_primary: unexpected token kind: %s\n", RED, current_file, get_line(ps->src, parser_peek(ps)->start), get_col(ps->src, parser_peek(ps)->start), RESET, token_kind_name(parser_peek(ps)->kind));
exit(1);
}
}


AST* parse_func_def(Parser* ps) {
AST* node = make_node(AST_FUNC_DEF);
node->data._func_def.return_type = *(parse_type(ps));
Token* name = parser_advance(ps);
node->data._func_def.name_start = name->start;
node->data._func_def.name_length = name->length;
parser_expect(ps, TOKEN_LPAREN);
AST* param_head = NULL;
AST* param_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RPAREN) {
AST* param = parse_param(ps);
if (param_head == NULL) {
param_head = param;
}
else {
param_tail->next = param;
}
param_tail = param;
if (parser_peek(ps)->kind == TOKEN_COMMA) {
parser_advance(ps);
}
}
parser_expect(ps, TOKEN_RPAREN);
node->data._func_def.params = param_head;
parser_expect(ps, TOKEN_COLON);
AST* body_head = NULL;
AST* body_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_END) {
parser_skip_newline(ps);
if (parser_peek(ps)->kind == TOKEN_END) {
break;
}
AST* statement = parse_statement(ps);
if (body_head == NULL) {
body_head = statement;
}
else {
body_tail->next = statement;
}
body_tail = statement;
}
parser_expect(ps, TOKEN_END);
node->data._func_def.body = body_head;
return node;
}


AST* parse_func_call(Parser* ps) {
AST* node = make_node(AST_FUNC_CALL);
Token* name = parser_advance(ps);
node->data._func_call.name_start = name->start;
node->data._func_call.name_length = name->length;
parser_expect(ps, TOKEN_LPAREN);
AST* arg_head = NULL;
AST* arg_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RPAREN) {
AST* param = parse_expr(ps, 0);
if (arg_head == NULL) {
arg_head = param;
}
else {
arg_tail->next = param;
}
arg_tail = param;
if (parser_peek(ps)->kind == TOKEN_COMMA) {
parser_advance(ps);
}
}
parser_expect(ps, TOKEN_RPAREN);
node->data._func_call.args = arg_head;
return node;
}


AST* parse_alias_call(Parser* ps) {
AST* node = make_node(AST_ALIAS_CALL);
Token* alias = parser_advance(ps);
node->data._alias_call.alias_start = alias->start;
node->data._alias_call.alias_length = alias->length;
parser_expect(ps, TOKEN_DOT);
Token* func = parser_advance(ps);
node->data._alias_call.func_start = func->start;
node->data._alias_call.func_length = func->length;
parser_expect(ps, TOKEN_LPAREN);
AST* arg_head = NULL;
AST* arg_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RPAREN) {
AST* arg = parse_expr(ps, 0);
if (arg_head == NULL) {
arg_head = arg;
}
else {
arg_tail->next = arg;
}
arg_tail = arg;
if (parser_peek(ps)->kind == TOKEN_COMMA) {
parser_advance(ps);
}
}
parser_expect(ps, TOKEN_RPAREN);
node->data._alias_call.args = arg_head;
return node;
}


AST* parse_param(Parser* ps) {
AST* node = make_node(AST_PARAM);
Token* name = parser_advance(ps);
node->data._func_params.name_start = name->start;
node->data._func_params.name_length = name->length;
parser_expect(ps, TOKEN_COLON);
node->data._func_params.type = *(parse_type(ps));
return node;
}


AST* parse_struct(Parser* ps) {
if (!(parser_expect(ps, TOKEN_STRUCT))) {
return NULL;
}
AST* node = make_node(AST_STRUCT_DEF);
Token* name = parser_advance(ps);
if (name->kind != TOKEN_IDENTIFIER) {
parser_error(ps, "Expected struct name");
return NULL;
}
node->data._struct_def.name_start = name->start;
node->data._struct_def.name_length = name->length;
if (!(parser_expect(ps, TOKEN_LBRACE))) {
return NULL;
}
AST* field_head = NULL;
AST* field_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RBRACE) {
while (parser_peek(ps)->kind == TOKEN_NEWLINE  ||  parser_peek(ps)->kind == TOKEN_COMMA  ||  parser_peek(ps)->kind == TOKEN_SEMI) {
parser_advance(ps);
}
if (parser_peek(ps)->kind == TOKEN_RBRACE) {
break;
}
if (parser_peek(ps)->kind == TOKEN_EOF) {
parser_error(ps, "Unexpected end of file in struct body");
return node;
}
AST* field = make_node(AST_STRUCT_FIELD);
Token* fname = parser_advance(ps);
if (fname->kind != TOKEN_IDENTIFIER) {
parser_error(ps, "Expected field name");
continue;
}
field->data._struct_field.name_start = fname->start;
field->data._struct_field.name_length = fname->length;
if (!(parser_expect(ps, TOKEN_COLON))) {
continue;
}
field->data._struct_field.type = *(parse_type(ps));
if (field_head == NULL) {
field_head = field;
}
else {
field_tail->next = field;
}
field_tail = field;
}
parser_expect(ps, TOKEN_RBRACE);
node->data._struct_def.fields = field_head;
return node;
}


AST* parse_union(Parser* ps) {
parser_expect(ps, TOKEN_UNION);
AST* node = make_node(AST_UNION_DEF);
Token* name = parser_advance(ps);
node->data._struct_def.name_start = name->start;
node->data._struct_def.name_length = name->length;
parser_expect(ps, TOKEN_LBRACE);
AST* field_head = NULL;
AST* field_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RBRACE) {
while (parser_peek(ps)->kind == TOKEN_NEWLINE  ||  parser_peek(ps)->kind == TOKEN_COMMA  ||  parser_peek(ps)->kind == TOKEN_SEMI) {
parser_advance(ps);
}
if (parser_peek(ps)->kind == TOKEN_RBRACE) {
break;
}
AST* field = make_node(AST_STRUCT_FIELD);
Token* fname = parser_advance(ps);
field->data._struct_field.name_start = fname->start;
field->data._struct_field.name_length = fname->length;
parser_expect(ps, TOKEN_COLON);
field->data._struct_field.type = *(parse_type(ps));
if (field_head == NULL) {
field_head = field;
}
else {
field_tail->next = field;
}
field_tail = field;
}
parser_expect(ps, TOKEN_RBRACE);
node->data._struct_def.fields = field_head;
return node;
}


AST* parse_dot_ass(Parser* ps) {
AST* expr = parse_expr(ps, 0);
if (parser_peek(ps)->kind == TOKEN_ASSIGN) {
parser_advance(ps);
AST* val = parse_expr(ps, 0);
if (expr->kind == AST_DOT_ACCESS) {
expr->data._dot_access.value = val;
}
else if (expr->kind == AST_SUBSCRIPT) {
expr->data._subscript.value = val;
}
else {
printf("%s%s:%d:%d: error:%s Invalid assignment target\n", RED, current_file, get_line(ps->src, parser_peek(ps)->start), get_col(ps->src, parser_peek(ps)->start), RESET);
exit(1);
}
}
return expr;
}


AST* parse_import(Parser* ps) {
parser_expect(ps, TOKEN_IMPORT);
AST* node = make_node(AST_IMPORT);
if (parser_peek(ps)->kind == TOKEN_LT) {
parser_advance(ps);
Token* path = parser_advance(ps);
node->data._import.path_start = path->start;
node->data._import.path_length = path->length;
parser_expect(ps, TOKEN_GT);
node->data._import.is_system = 1;
if (parser_peek(ps)->kind == TOKEN_AS) {
parser_advance(ps);
Token* alias = parser_advance(ps);
node->data._import.alias_start = alias->start;
node->data._import.alias_length = alias->length;
node->data._import.has_alias = 1;
ps->alias_start[ps->alias_count] = alias->start;
ps->alias_lengths[ps->alias_count] = alias->length;
ps->alias_count = ps->alias_count + 1;
}
else {
node->data._import.has_alias = 0;
}
}
else {
Token* path = parser_advance(ps);
node->data._import.path_start = path->start;
node->data._import.path_length = path->length;
node->data._import.is_system = 0;
if (parser_peek(ps)->kind == TOKEN_AS) {
parser_advance(ps);
Token* alias = parser_advance(ps);
node->data._import.alias_start = alias->start;
node->data._import.alias_length = alias->length;
node->data._import.has_alias = 1;
ps->alias_start[ps->alias_count] = alias->start;
ps->alias_lengths[ps->alias_count] = alias->length;
ps->alias_count = ps->alias_count + 1;
}
else {
node->data._import.has_alias = 0;
}
}
return node;
}


AST* parse_forward(Parser* ps) {
parser_expect(ps, TOKEN_FORWARD);
AST* node = make_node(AST_FORWARD);
if (parser_peek(ps)->kind == TOKEN_STRUCT) {
parser_advance(ps);
Token* name = parser_advance(ps);
node->data._forward.name_start = name->start;
node->data._forward.name_length = name->length;
node->data._forward.is_func = 0;
}
else {
node->data._forward.is_func = 1;
node->data._forward.return_type = *(parse_type(ps));
Token* name = parser_advance(ps);
node->data._forward.name_start = name->start;
node->data._forward.name_length = name->length;
parser_expect(ps, TOKEN_LPAREN);
AST* param_head = NULL;
AST* param_tail = NULL;
while (parser_peek(ps)->kind != TOKEN_RPAREN) {
AST* param = parse_param(ps);
if (param_head == NULL) {
param_head = param;
}
else {
param_tail->next = param;
}
param_tail = param;
if (parser_peek(ps)->kind == TOKEN_COMMA) {
parser_advance(ps);
}
}
parser_expect(ps, TOKEN_RPAREN);
node->data._forward.params = param_head;
}
return node;
}


AST* parser_parse(Parser* ps) {
AST* head = NULL;
AST* tail = NULL;
while (parser_peek(ps)->kind != TOKEN_EOF) {
if (ps->error_count >= 10) {
printf("%sToo many errors, exiting.%s\n", RED, RESET);
break;
}
parser_skip_newline(ps);
if (parser_peek(ps)->kind == TOKEN_EOF) {
break;
}
AST* node;
if (parser_peek(ps)->kind == TOKEN_IMPORT) {
node = parse_import(ps);
}
else if (parser_peek(ps)->kind == TOKEN_STRUCT) {
node = parse_struct(ps);
}
else if (parser_peek(ps)->kind == TOKEN_UNION) {
node = parse_union(ps);
}
else if (parser_peek(ps)->kind == TOKEN_PROP) {
parser_advance(ps);
AST* func = parse_func_def(ps);
AST* prop_node = make_node(AST_PROP);
prop_node->data._prop.func = func;
node = prop_node;
}
else if (parser_peek(ps)->kind == TOKEN_FORWARD) {
node = parse_forward(ps);
}
else if (parser_peek(ps)->kind == TOKEN_IDENTIFIER  &&  peek(ps->ts, ps->pos + 1)->kind == TOKEN_COLON) {
node = parse_var_decl(ps);
}
else {
node = parse_func_def(ps);
}
if (head == NULL) {
head = node;
}
else {
tail->next = node;
}
tail = node;
}
return head;
}
#include <stdlib.h>


char* file_read_file(char* path) {
FILE* f = fopen(path, "r");
if (f == NULL) {
return NULL;
}
fseek(f, 0, 2);
int length = ftell(f);
fseek(f, 0, 0);
char* buffer = malloc(sizeof(char) * (length + 1));
fread(buffer, 1, length, f);
buffer[length] = '\0';
fclose(f);
return buffer;
}
#include <stdlib.h>
int has_stdlib = 0;
int has_stdio = 0;


void emit_includes(AST* ast, FILE* out, char* src, TokenStream* ts) {
if (!(has_stdlib)  &&  token_stream_contains(ts, TOKEN_NULL)  ||  token_stream_contains(ts, TOKEN_NEW)) {
fprintf(out, "#include <stdlib.h>\n");
has_stdlib = 1;
}
if (!(has_stdio)  &&  token_stream_contains(ts, TOKEN_PRINT)) {
fprintf(out, "#include <stdio.h>\n");
has_stdio = 1;
}
}


char* token_to_string(int kind) {
if (kind == TOKEN_INT) {
return "int";
}
else if (kind == TOKEN_BOOL) {
return "int";
}
else if (kind == TOKEN_FLOAT) {
return "float";
}
else if (kind == TOKEN_DOUBLE) {
return "double";
}
else if (kind == TOKEN_CHAR) {
return "char";
}
else if (kind == TOKEN_STRING) {
return "char*";
}
else if (kind == TOKEN_VOID) {
return "void";
}
else if (kind == TOKEN_RETURN) {
return "return";
}
else if (kind == TOKEN_WHILE) {
return "while";
}
else if (kind == TOKEN_IF) {
return "if";
}
else if (kind == TOKEN_ELSE) {
return "else";
}
else if (kind == TOKEN_END) {
return "}";
}
else if (kind == TOKEN_OR) {
return " || ";
}
else if (kind == TOKEN_AND) {
return " && ";
}
else if (kind == TOKEN_DOT) {
return ".";
}
else if (kind == TOKEN_PLUS) {
return "+";
}
else if (kind == TOKEN_MINUS) {
return "-";
}
else if (kind == TOKEN_STAR) {
return "*";
}
else if (kind == TOKEN_SLASH) {
return "/";
}
else if (kind == TOKEN_CARET) {
return "^";
}
else if (kind == TOKEN_AT) {
return "@";
}
else if (kind == TOKEN_AMPERSAND) {
return "&";
}
else if (kind == TOKEN_ASSIGN) {
return "=";
}
else if (kind == TOKEN_LT) {
return "<";
}
else if (kind == TOKEN_GT) {
return ">";
}
else if (kind == TOKEN_GEQ) {
return ">=";
}
else if (kind == TOKEN_LEQ) {
return "<=";
}
else if (kind == TOKEN_NEQ) {
return "!=";
}
else if (kind == TOKEN_COMP) {
return "==";
}
else if (kind == TOKEN_LPAREN) {
return "(";
}
else if (kind == TOKEN_RPAREN) {
return ")";
}
else if (kind == TOKEN_LBRACE) {
return "{";
}
else if (kind == TOKEN_RBRACE) {
return "}";
}
else if (kind == TOKEN_COLON) {
return ":";
}
else if (kind == TOKEN_COMMA) {
return ",";
}
else if (kind == TOKEN_SEMI) {
return "";
}
else if (kind == TOKEN_NEWLINE) {
return "\n";
}
else {
return "ERROR";
}
}


void typeinfo_to_string(TypeInfo* type, FILE* out, char* src) {
if (type->base == TOKEN_IDENTIFIER) {
fprintf(out, "%.*s", type->name_length, src + type->name_start);
}
else {
fprintf(out, "%s", token_to_string(type->base));
}
int i = 0;
while (i < type->pointer_depth) {
i = i + 1;
fprintf(out, "*");
}
}
void gen_expr(AST* ast, FILE* out, char* src);
void gen_statement(AST* ast, FILE* out, char* src);


void gen_if(AST* ast, FILE* out, char* src, int is_else_if) {
if (is_else_if) {
if (ast->data._if_condition.condition == NULL) {
fprintf(out, "else {\n");
}
else {
fprintf(out, "else if (");
gen_expr(ast->data._if_condition.condition, out, src);
fprintf(out, ") {\n");
}
}
else {
fprintf(out, "if (");
gen_expr(ast->data._if_condition.condition, out, src);
fprintf(out, ") {\n");
}
AST* statement = ast->data._if_condition.body;
while (statement != NULL) {
gen_statement(statement, out, src);
statement = statement->next;
}
fprintf(out, "}\n");
if (ast->data._if_condition.else_branch != NULL) {
gen_if(ast->data._if_condition.else_branch, out, src, 1);
}
}


void gen_while(AST* ast, FILE* out, char* src) {
fprintf(out, "while (");
gen_expr(ast->data._while_loop.condition, out, src);
fprintf(out, ") {\n");
AST* statement = ast->data._while_loop.body;
while (statement != NULL) {
gen_statement(statement, out, src);
statement = statement->next;
}
fprintf(out, "}\n");
}


void gen_for(AST* ast, FILE* out, char* src) {
char* desc_op;
char* asc_op;
if (ast->data._for_loop.inclusive) {
desc_op = ">=";
asc_op = "<=";
}
else {
desc_op = ">";
asc_op = "<";
}
char* add_op = "++";
char* sub_op = "--";
AST* from = ast->data._for_loop.from;
AST* to = ast->data._for_loop.to;
if (from->kind == AST_LITERAL  &&  to->kind == AST_LITERAL) {
int reverse;
if (from->value > to->value) {
reverse = 1;
}
else {
reverse = 0;
}
char* op;
char* math_op;
if (reverse) {
op = desc_op;
math_op = sub_op;
}
else {
op = asc_op;
math_op = add_op;
}
fprintf(out, "for (int %.*s = ", ast->data._for_loop.var_length, src + ast->data._for_loop.var_start);
gen_expr(ast->data._for_loop.from, out, src);
fprintf(out, "; %.*s %s ", ast->data._for_loop.var_length, src + ast->data._for_loop.var_start, op);
gen_expr(ast->data._for_loop.to, out, src);
fprintf(out, "; %.*s%s) {\n", ast->data._for_loop.var_length, src + ast->data._for_loop.var_start, math_op);
}
else {
fprintf(out, "for (int %.*s = ", ast->data._for_loop.var_length, src + ast->data._for_loop.var_start);
gen_expr(ast->data._for_loop.from, out, src);
fprintf(out, "; ((");
gen_expr(ast->data._for_loop.from, out, src);
fprintf(out, ") > (");
gen_expr(ast->data._for_loop.to, out, src);
fprintf(out, ")) ? %.*s %s (", ast->data._for_loop.var_length, src + ast->data._for_loop.var_start, desc_op);
gen_expr(ast->data._for_loop.to, out, src);
fprintf(out, ") : %.*s %s (", ast->data._for_loop.var_length, src + ast->data._for_loop.var_start, asc_op);
gen_expr(ast->data._for_loop.to, out, src);
fprintf(out, "); ((");
gen_expr(ast->data._for_loop.from, out, src);
fprintf(out, ") > (");
gen_expr(ast->data._for_loop.to, out, src);
fprintf(out, ")) ? %.*s-- : %.*s++) {\n", ast->data._for_loop.var_length, src + ast->data._for_loop.var_start, ast->data._for_loop.var_length, src + ast->data._for_loop.var_start);
}
AST* statement = ast->data._for_loop.body;
while (statement != NULL) {
gen_statement(statement, out, src);
statement = statement->next;
}
fprintf(out, "}\n");
}


void gen_return(AST* ast, FILE* out, char* src) {
if (ast->data._flow_ctrl.value == NULL) {
fprintf(out, "return;");
}
else {
fprintf(out, "return ");
gen_expr(ast->data._flow_ctrl.value, out, src);
fprintf(out, ";\n");
}
}


void gen_continue(AST* ast, FILE* out, char* src) {
fprintf(out, "continue;\n");
}


void gen_break(AST* ast, FILE* out, char* src) {
fprintf(out, "break;\n");
}


typedef struct defined_struct {
char* name;
int length;
} defined_struct;
defined_struct defined_structs[256];
int defined_count = 0;


void gen_struct(AST* ast, FILE* out, char* src) {
for (int i = 0; ((0) > (defined_count)) ? i > (defined_count) : i < (defined_count); ((0) > (defined_count)) ? i-- : i++) {
if (defined_structs[i].length == ast->data._struct_def.name_length  &&  !(strncmp(defined_structs[i].name, ast->data._struct_def.name_start + src, ast->data._struct_def.name_length))) {
return;}
}
fprintf(out, "\n\ntypedef struct %.*s {\n", ast->data._struct_def.name_length, src + ast->data._struct_def.name_start);
AST* field = ast->data._struct_def.fields;
while (field != NULL) {
typeinfo_to_string(&(field->data._struct_field.type), out, src);
fprintf(out, " %.*s", field->data._struct_field.name_length, src + field->data._struct_field.name_start);
if (field->data._struct_field.type.array_size == -1) {
fprintf(out, "[]");
}
else if (field->data._struct_field.type.array_size > 0) {
fprintf(out, "[%d]", field->data._struct_field.type.array_size);
}
else if (field->data._struct_field.type.arr_size_expr != NULL) {
fprintf(out, "[");
gen_expr(field->data._struct_field.type.arr_size_expr, out, src);
fprintf(out, "]");
}
fprintf(out, ";\n");
field = field->next;
}
fprintf(out, "} %.*s;\n", ast->data._struct_def.name_length, src + ast->data._struct_def.name_start);
defined_structs[defined_count].name = strndup(src + ast->data._struct_def.name_start, ast->data._struct_def.name_length);
defined_structs[defined_count].length = ast->data._struct_def.name_length;
defined_count = defined_count + 1;
}


typedef struct defined_union {
char* name;
int length;
} defined_union;
defined_union defined_unions[256];
int defined_union_count = 0;


void gen_union(AST* ast, FILE* out, char* src) {
for (int i = 0; ((0) > (defined_union_count)) ? i > (defined_union_count) : i < (defined_union_count); ((0) > (defined_union_count)) ? i-- : i++) {
if (defined_unions[i].length == ast->data._union_def.name_length  &&  !(strncmp(defined_unions[i].name, ast->data._union_def.name_start + src, ast->data._union_def.name_length))) {
return;}
}
fprintf(out, "\n\ntypedef union %.*s {\n", ast->data._union_def.name_length, src + ast->data._union_def.name_start);
AST* field = ast->data._union_def.fields;
while (field != NULL) {
typeinfo_to_string(&(field->data._struct_field.type), out, src);
fprintf(out, " %.*s", field->data._union_def.name_length, src + field->data._union_def.name_start);
if (field->data._struct_field.type.array_size == -1) {
fprintf(out, "[]");
}
else if (field->data._struct_field.type.array_size > 0) {
fprintf(out, "[%d]", ast->data._struct_field.type.array_size);
}
else if (field->data._struct_field.type.arr_size_expr != NULL) {
fprintf(out, "[");
gen_expr(field->data._struct_field.type.arr_size_expr, out, src);
fprintf(out, "]");
}
fprintf(out, ";\n");
field = field->next;
}
fprintf(out, "} %.*s;\n", ast->data._union_def.name_length, src + ast->data._union_def.name_start);
defined_unions[defined_union_count].name = strndup(src + ast->data._union_def.name_start, ast->data._union_def.name_length);
defined_unions[defined_union_count].length = ast->data._union_def.name_length;
defined_union_count = defined_union_count + 1;
}


void gen_param(AST* param, FILE* out, char* src) {
typeinfo_to_string(&(param->data._func_params.type), out, src);
fprintf(out, " %.*s", param->data._func_params.name_length, src + param->data._func_params.name_start);
if (param->data._func_params.type.array_size == -1) {
fprintf(out, "[]");
}
else if (param->data._func_params.type.array_size != 0) {
fprintf(out, "[%d]", param->data._func_params.type.array_size);
}
else if (param->data._func_params.type.arr_size_expr != NULL) {
fprintf(out, "[");
gen_expr(param->data._func_params.type.arr_size_expr, out, src);
fprintf(out, "]");
}
}


void gen_func_def(AST* ast, FILE* out, char* src) {
fprintf(out, "\n\n");
typeinfo_to_string(&(ast->data._func_def.return_type), out, src);
fprintf(out, " %.*s(", ast->data._func_def.name_length, src + ast->data._func_def.name_start);
AST* param = ast->data._func_def.params;
while (param != NULL) {
gen_param(param, out, src);
if (param->next != NULL) {
fprintf(out, ", ");
}
param = param->next;
}
fprintf(out, ") {\n");
AST* statement = ast->data._func_def.body;
while (statement != NULL) {
gen_statement(statement, out, src);
statement = statement->next;
}
fprintf(out, "}\n");
}


void gen_func_def_aliased(AST* ast, FILE* out, char* src, char* alias, int alias_len) {
fprintf(out, "\n\n");
typeinfo_to_string(&(ast->data._func_def.return_type), out, src);
fprintf(out, " %.*s_%.*s(", alias_len, alias, ast->data._func_def.name_length, src + ast->data._func_def.name_start);
AST* param = ast->data._func_def.params;
while (param != NULL) {
gen_param(param, out, src);
if (param->next != NULL) {
fprintf(out, ", ");
}
param = param->next;
}
fprintf(out, ") {\n");
AST* statement = ast->data._func_def.body;
while (statement != NULL) {
gen_statement(statement, out, src);
statement = statement->next;
}
fprintf(out, "}\n");
}


void gen_func_call(AST* ast, FILE* out, char* src) {
fprintf(out, "%.*s(", ast->data._func_call.name_length, src + ast->data._func_call.name_start);
AST* arg = ast->data._func_call.args;
while (arg != NULL) {
gen_expr(arg, out, src);
if (arg->next != NULL) {
fprintf(out, ", ");
}
arg = arg->next;
}
fprintf(out, ")");
}


void gen_var_decl(AST* ast, FILE* out, char* src) {
typeinfo_to_string(&(ast->data._var_decl.type), out, src);
fprintf(out, " %.*s", ast->data._var_decl.name_length, src + ast->data._var_decl.name_start);
if (ast->data._var_decl.type.array_size == -1) {
fprintf(out, "[]");
}
else if (ast->data._var_decl.type.array_size != 0) {
fprintf(out, "[%d]", ast->data._var_decl.type.array_size);
}
else if (ast->data._var_decl.type.arr_size_expr != NULL) {
fprintf(out, "[");
gen_expr(ast->data._var_decl.type.arr_size_expr, out, src);
fprintf(out, "]");
}
if (ast->data._var_decl.value != NULL) {
fprintf(out, " = ");
gen_expr(ast->data._var_decl.value, out, src);
}
fprintf(out, ";\n");
}


void gen_var_ass(AST* ast, FILE* out, char* src) {
fprintf(out, "%.*s = ", ast->data._var_ass.name_length, src + ast->data._var_ass.name_start);
gen_expr(ast->data._var_ass.value, out, src);
fprintf(out, ";\n");
}


void gen_forward(AST* ast, FILE* out, char* src) {
if (ast->data._forward.is_func) {
typeinfo_to_string(&(ast->data._forward.return_type), out, src);
fprintf(out, " %.*s(", ast->data._forward.name_length, src + ast->data._forward.name_start);
AST* param = ast->data._forward.params;
while (param != NULL) {
gen_param(param, out, src);
if (param->next != NULL) {
fprintf(out, ", ");
}
param = param->next;
}
fprintf(out, ");\n");
}
else {
fprintf(out, "typedef struct %.*s %.*s;\n", ast->data._forward.name_length, src + ast->data._forward.name_start, ast->data._forward.name_length, src + ast->data._forward.name_start);
}
}


typedef struct ImportCache {
char* path;
AST* ast;
char* src;
} ImportCache;
ImportCache import_cache[64];
int cache_count = 0;


typedef struct EmittedImport {
char* path;
char* alias;
} EmittedImport;
EmittedImport emitted_imports[256];
int emitted_count = 0;
char* in_progress[64];
int in_progress_count = 0;


void gen_import(AST* ast, FILE* out, char* src) {
if (ast->data._import.is_system) {
char name[64];
snprintf(name, sizeof(name), "%.*s", ast->data._import.path_length, src + ast->data._import.path_start);
if (!(strcmp(name, "stdio"))  &&  !(has_stdio)) {
fprintf(out, "#include <stdio.h>\n");
has_stdio = 1;
}
else if (!(strcmp(name, "stdlib"))  &&  !(has_stdlib)) {
fprintf(out, "#include <stdlib.h>\n");
has_stdlib = 1;
}
else if (strcmp(name, "stdio")  &&  strcmp(name, "stdlib")) {
fprintf(out, "#include <%s.h>\n", name);
}
return;}
char raw_path[256];
snprintf(raw_path, sizeof(raw_path), "%.*s", ast->data._import.path_length - 2, src + ast->data._import.path_start + 1);
char* original = current_file;
char* path = resolve_path(current_file, raw_path);
current_file = path;
char alias[64] = "";
if (ast->data._import.has_alias) {
snprintf(alias, sizeof(alias), "%.*s", ast->data._import.alias_length, src + ast->data._import.alias_start);
}
for (int i = 0; ((0) > (emitted_count)) ? i > (emitted_count) : i < (emitted_count); ((0) > (emitted_count)) ? i-- : i++) {
if (!(strcmp(emitted_imports[i].path, path))  &&  !(strcmp(emitted_imports[i].alias, alias))) {
return;}
}
for (int i = 0; ((0) > (in_progress_count)) ? i > (in_progress_count) : i < (in_progress_count); ((0) > (in_progress_count)) ? i-- : i++) {
if (!(strcmp(in_progress[i], path))) {
printf("Circular import detected: %s\n", path);
exit(1);
}
}
in_progress[in_progress_count] = strdup(path);
in_progress_count = in_progress_count + 1;
AST* imported_ast = NULL;
char* imported_src = NULL;
TokenStream* cached_ts = NULL;
for (int i = 0; ((0) > (cache_count)) ? i > (cache_count) : i < (cache_count); ((0) > (cache_count)) ? i-- : i++) {
if (!(strcmp(import_cache[i].path, path))) {
imported_ast = import_cache[i].ast;
imported_src = import_cache[i].src;
break;
}
}
if (imported_ast == NULL) {
imported_src = file_read_file(path);
if (!(imported_src)) {
printf("%sCould not import file: %s\nfrom file:%s\nwith root: %s%s\n", RED, path, original, project_root, RESET);
exit(1);
}
set_current_file(path);
printf("Importing: %s\n", path);
TokenStream* ts = malloc(sizeof(TokenStream));
lexer_init_token_stream(ts);
lexer_lex(ts, imported_src);
if (strcmp(path, "PATH") == 0) {
lexer_print_all_tokens(ts, imported_src);
}
if (!(has_stdlib)  &&  token_stream_contains(ts, TOKEN_NEW)  ||  token_stream_contains(ts, TOKEN_NULL)) {
fprintf(out, "#include <stdlib.h>\n");
has_stdlib = 1;
}
if (!(has_stdio)  &&  token_stream_contains(ts, TOKEN_PRINT)) {
fprintf(out, "#include <stdio.h>\n");
has_stdio = 1;
}
Parser p;
parser_init_parser(&(p), ts, imported_src, path);
imported_ast = parser_parse(&(p));
import_cache[cache_count].path = strdup(path);
import_cache[cache_count].ast = imported_ast;
import_cache[cache_count].src = imported_src;
cache_count = cache_count + 1;
}
emitted_imports[emitted_count].path = strdup(path);
emitted_imports[emitted_count].alias = strdup(alias);
emitted_count = emitted_count + 1;
AST* curr = imported_ast;
while (curr != NULL) {
if (curr->kind == AST_IMPORT) {
gen_import(curr, out, imported_src);
}
else if (curr->kind == AST_STRUCT_DEF) {
gen_struct(curr, out, imported_src);
}
else if (curr->kind == AST_UNION_DEF) {
gen_union(curr, out, imported_src);
}
else if (curr->kind == AST_PROP) {
if (ast->data._import.has_alias) {
gen_func_def_aliased(curr->data._prop.func, out, imported_src, alias, strlen(alias));
}
else {
gen_func_def(curr->data._prop.func, out, imported_src);
}
}
else if (curr->kind == AST_FORWARD) {
gen_forward(curr, out, imported_src);
}
else if (curr->kind == AST_FUNC_DEF) {
gen_func_def(curr, out, imported_src);
}
else if (curr->kind == AST_VAR_DECL) {
gen_var_decl(curr, out, imported_src);
}
curr = curr->next;
}
free(path);
current_file = original;
in_progress_count = in_progress_count - 1;
free(in_progress[in_progress_count]);
}


void gen_expr(AST* ast, FILE* out, char* src) {
if (ast->kind == AST_VAR_REF) {
fprintf(out, "%.*s", ast->data._var_ref.name_length, src + ast->data._var_ref.name_start);
}
else if (ast->kind == AST_LITERAL) {
fprintf(out, "%d", ast->value);
}
else if (ast->kind == AST_FLOAT_LIT) {
fprintf(out, "%.*s", ast->data._float_lit.length, src + ast->data._float_lit.start);
}
else if (ast->kind == AST_NULL) {
fprintf(out, "NULL");
}
else if (ast->kind == AST_BINARY_OP) {
gen_expr(ast->data._binary.left, out, src);
fprintf(out, " %s ", token_to_string(ast->data._binary.op));
gen_expr(ast->data._binary.right, out, src);
}
else if (ast->kind == AST_FUNC_CALL) {
gen_func_call(ast, out, src);
}
else if (ast->kind == AST_STRING_LIT) {
fprintf(out, "%.*s", ast->data._string.str_length, src + ast->data._string.str_start);
}
else if (ast->kind == AST_ARRAY_LIT) {
fprintf(out, "{");
AST* elem = ast->data._array.elements;
while (elem != NULL) {
gen_expr(elem, out, src);
if (elem->next != NULL) {
fprintf(out, ", ");
}
elem = elem->next;
}
fprintf(out, "}");
}
else if (ast->kind == AST_STRUCT_LIT) {
fprintf(out, "{");
AST* struct_elem = ast->data._struct_lit.elements;
while (struct_elem != NULL) {
gen_expr(struct_elem, out, src);
if (struct_elem->next != NULL) {
fprintf(out, ", ");
struct_elem = struct_elem->next;
}
}
fprintf(out, "}");
}
else if (ast->kind == AST_CAST) {
fprintf(out, "(");
typeinfo_to_string(&(ast->data._cast.type), out, src);
fprintf(out, ")(");
gen_expr(ast->data._cast.value, out, src);
fprintf(out, ")");
}
else if (ast->kind == AST_DOT_ACCESS) {
AST* object = ast->data._dot_access.object;
gen_expr(object, out, src);
int obj_kind = object->kind;
if (obj_kind == AST_DOT_ACCESS  ||  obj_kind == AST_SUBSCRIPT) {
fprintf(out, ".%.*s", ast->data._dot_access.field_length, src + ast->data._dot_access.field_start);
}
else {
fprintf(out, "->%.*s", ast->data._dot_access.field_length, src + ast->data._dot_access.field_start);
}
}
else if (ast->kind == AST_NEW) {
fprintf(out, "malloc(sizeof(");
typeinfo_to_string(&(ast->data._new_alloc.type), out, src);
fprintf(out, ")");
if (ast->data._new_alloc.type.arr_size_expr != NULL) {
fprintf(out, " * (");
gen_expr(ast->data._new_alloc.type.arr_size_expr, out, src);
fprintf(out, ")");
}
else if (ast->data._new_alloc.type.array_size > 0) {
fprintf(out, " * %d", ast->data._new_alloc.type.array_size);
}
fprintf(out, ")");
}
else if (ast->kind == AST_ALIAS_CALL) {
fprintf(out, "%.*s_%.*s(", ast->data._alias_call.alias_length, ast->data._alias_call.alias_start + src, ast->data._alias_call.func_length, src + ast->data._alias_call.func_start);
AST* arg = ast->data._alias_call.args;
while (arg != NULL) {
gen_expr(arg, out, src);
if (arg->next != NULL) {
fprintf(out, ", ");
}
arg = arg->next;
}
fprintf(out, ")");
}
else if (ast->kind == AST_UNARY_NOT) {
fprintf(out, "!(");
gen_expr(ast->data._unary.operand, out, src);
fprintf(out, ")");
}
else if (ast->kind == AST_UNARY_NEG) {
fprintf(out, "-");
gen_expr(ast->data._unary.operand, out, src);
}
else if (ast->kind == AST_DEREF) {
fprintf(out, "*(");
gen_expr(ast->data._deref.operand, out, src);
fprintf(out, ")");
}
else if (ast->kind == AST_GET_ADDR) {
fprintf(out, "&(");
gen_expr(ast->data._get_addr.operand, out, src);
fprintf(out, ")");
}
else if (ast->kind == AST_SIZEOF) {
fprintf(out, "sizeof(");
typeinfo_to_string(&(ast->data._size_of.type), out, src);
fprintf(out, ")");
}
else if (ast->kind == AST_CHAR_LIT) {
fprintf(out, "%.*s", ast->data._char_lit.length, ast->data._char_lit.start + src);
}
else if (ast->kind == AST_SUBSCRIPT) {
gen_expr(ast->data._subscript.array, out, src);
fprintf(out, "[");
gen_expr(ast->data._subscript.index, out, src);
fprintf(out, "]");
}
else {
printf("gen_expr received node addr: %p, kind: %d\n", ast, ast->kind);
exit(1);
}
}


void gen_statement(AST* ast, FILE* out, char* src) {
if (ast->kind == AST_VAR_DECL) {
gen_var_decl(ast, out, src);
}
else if (ast->kind == AST_VAR_ASS) {
gen_var_ass(ast, out, src);
}
else if (ast->kind == AST_WHILE) {
gen_while(ast, out, src);
}
else if (ast->kind == AST_FOR) {
gen_for(ast, out, src);
}
else if (ast->kind == AST_FLOW_CONTROL) {
Token* base = ast->data._flow_ctrl.base;
if (base->kind == TOKEN_RETURN) {
gen_return(ast, out, src);
}
else if (base->kind == TOKEN_CONTINUE) {
gen_continue(ast, out, src);
}
else if (base->kind == TOKEN_BREAK) {
gen_break(ast, out, src);
}
else {
Token* base = ast->data._flow_ctrl.base;
printf("%s%s:%d:%d: error:%s Unexpected token: %s\n", RED, current_file, get_line(src, base->start), get_col(src, base->start), RESET, token_kind_name(base->kind));
exit(1);
}
}
else if (ast->kind == AST_FUNC_CALL) {
gen_func_call(ast, out, src);
fprintf(out, ";\n");
}
else if (ast->kind == AST_IF) {
gen_if(ast, out, src, 0);
}
else if (ast->kind == AST_DOT_ACCESS) {
AST* object = ast->data._dot_access.object;
gen_expr(object, out, src);
int obj_kind = object->kind;
if (obj_kind == AST_DOT_ACCESS  ||  obj_kind == AST_SUBSCRIPT) {
fprintf(out, ".%.*s", ast->data._dot_access.field_length, src + ast->data._dot_access.field_start);
}
else {
fprintf(out, "->%.*s", ast->data._dot_access.field_length, src + ast->data._dot_access.field_start);
}
if (ast->data._dot_access.value != NULL) {
fprintf(out, " = ");
gen_expr(ast->data._dot_access.value, out, src);
fprintf(out, ";\n");
}
}
else if (ast->kind == AST_PRINT) {
fprintf(out, "printf(");
gen_expr(ast->data._print.value, out, src);
fprintf(out, ");\n");
}
else if (ast->kind == AST_PRUNE) {
fprintf(out, "free(");
gen_expr(ast->data._prune_free.ptr, out, src);
fprintf(out, ");\n");
}
else if (ast->kind == AST_DEREF_ASS) {
fprintf(out, "*%.*s = ", ast->data._deref_ass.name_length, src + ast->data._deref_ass.name_start);
gen_expr(ast->data._deref_ass.value, out, src);
fprintf(out, ";\n");
}
else if (ast->kind == AST_SUBSCRIPT) {
gen_expr(ast->data._subscript.array, out, src);
fprintf(out, "[");
gen_expr(ast->data._subscript.index, out, src);
fprintf(out, "]");
if (ast->data._subscript.value != NULL) {
fprintf(out, " = ");
gen_expr(ast->data._subscript.value, out, src);
}
fprintf(out, ";\n");
}
else if (ast->kind == AST_ALIAS_CALL) {
gen_expr(ast, out, src);
fprintf(out, ";\n");
}
else {
printf("Unknown statement kind: %d\n", ast->kind);
exit(1);
}
}


void codegen(AST* ast, FILE* out, char* src) {
AST* curr = ast;
while (curr != NULL) {
if (curr->kind == AST_IMPORT) {
gen_import(curr, out, src);
}
else if (curr->kind == AST_FUNC_DEF) {
gen_func_def(curr, out, src);
}
else if (curr->kind == AST_STRUCT_DEF) {
gen_struct(curr, out, src);
}
else if (curr->kind == AST_UNION_DEF) {
gen_union(curr, out, src);
}
else if (curr->kind == AST_PROP) {
gen_func_def(curr->data._prop.func, out, src);
}
else if (curr->kind == AST_FORWARD) {
gen_forward(curr, out, src);
}
else if (curr->kind == AST_VAR_DECL) {
gen_var_decl(curr, out, src);
}
curr = curr->next;
}
}


int main(int argc, char* argv[]) {
getcwd(project_root, sizeof(project_root));
if (argc < 2) {
printf("%sMissing arguments! Use -help for usage instructions.%s\n", RED, RESET);
return -1;
}
for (int i = 1; ((1) > (argc)) ? i > (argc) : i < (argc); ((1) > (argc)) ? i-- : i++) {
char* arg = argv[i];
if (arg[0] == '-') {
while (arg[0] == '-') {
arg = arg + 1;
}
for (int i = 0; ((0) > (strlen(arg))) ? i > (strlen(arg)) : i < (strlen(arg)); ((0) > (strlen(arg))) ? i-- : i++) {
arg[i] = tolower((char)(arg[i]));
}
if (!(strcmp(arg, "help"))  ||  !(strcmp(arg, "h"))) {
printf("%s🌸 Welcome to Flower Compiler!\n\n%s", GREEN, RESET);
printf("%sUsage:\n%s", BLUE, RESET);
printf("\tflower\t[options] <filepath>\n\n");
printf("%sOptions:\n%s", BLUE, RESET);
printf("\t-help,    -h\tShow this help message\n");
printf("\t-version, -v\tShow the current version of FloC\n");
printf("\t<filepath>\tSpecify the source code file to compile\n\n");
printf("%sExample:\n%s", BLUE, RESET);
printf("\tflower -help\n");
printf("\tflower main.flo\n\n");
printf("%sTips:\n%s", BLUE, RESET);
printf(" - You can use any number of dashes before a flag, e.g., ---help\n");
printf(" - Flags are case-insensitive: -HELP works too!\n\n");
printf("%sHappy Compiling with Flower! 🌼\n%s", GREEN, RESET);
continue;
}
else if (!(strcmp(arg, "version"))  ||  !(strcmp(arg, "v"))) {
printf("Version: 0.0.4\n");
continue;
}
else {
printf("%sUnrecognized flag argument! Use -help for more information.\n%s", RED, RESET);
}
}
else {
printf("%sCompiling file: %s%s\n", BLUE, argv[i], RESET);
char abs_path[512];
realpath(argv[i], abs_path);
set_current_file(abs_path);
char* file = file_read_file(argv[i]);
if (!(file)) {
return -1;
}
TokenStream* tokens = malloc(sizeof(TokenStream));
lexer_init_token_stream(tokens);
printf("Lexing...\n");
fflush(stdout);
lexer_lex(tokens, file);
printf("Parsing...\n");
fflush(stdout);
Parser tree;
parser_init_parser(&(tree), tokens, file, argv[i]);
AST* ast = parser_parse(&(tree));
char out_c[512];
if (i + 1 < argc  &&  argv[i + 1][0] != '-') {
snprintf(out_c, sizeof(out_c), "%s.c", argv[i + 1]);
}
else {
snprintf(out_c, sizeof(out_c), "output.c");
}
FILE* output = fopen(out_c, "w");
if (!(output)) {
printf("%sFailed to open output file\n%s", RED, RESET);
return -1;
}
int has_output = 0;
if (token_stream_contains(tokens, TOKEN_PRINT)) {
has_output = 1;
}
printf("Codegen...\n");
fflush(stdout);
emit_includes(ast, output, file, tokens);
codegen(ast, output, file);
free_token_stream(tokens);
free(tokens);
free(file);
fclose(output);
char bin_name[512];
snprintf(bin_name, sizeof(bin_name), "%.*s", (int)(strlen(out_c) - 2), out_c);
char build_cmd[1024];
snprintf(build_cmd, sizeof(build_cmd), "clang %s -o %s", out_c, bin_name);
system(build_cmd);
if (has_output) {
printf("%s\nOUTPUT:\n", YELLOW);
fflush(stdout);
printf("%s", RESET);
fflush(stdout);
snprintf(build_cmd, sizeof(build_cmd), "./%s", bin_name);
}
system(build_cmd);
printf("\n");
printf("%sCompiled %s → %s%s\n", GREEN, out_c, bin_name, RESET);
break;
}
}
}
