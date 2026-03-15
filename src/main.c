#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "ANSI.h"

#include "file.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(RED "Missing arguments! Use -help for usage instructions.\n" RESET);
        return -1;
    }

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (arg[0] == '-') {
            // Skip all leading dashes
            while (*arg == '-') arg++;

            // Lowercase for case-insensitivity
            for (char *p = arg; *p; p++) *p = tolower((unsigned char)*p);
    
            if (!strcmp(arg, "help") || !strcmp(arg, "h")) {
                printf(
                    GREEN "🌸 Welcome to Flower Compiler!\n\n" RESET
        
                    BLUE "Usage:\n" RESET
                    "\tflower\t[options] <filepath>\n\n"
        
                    BLUE "Options:\n" RESET
                    "\t-help,    -h\tShow this help message\n"
                    "\t-version, -v\tShow the current version of FloC\n"
                    "\t<filepath>\tSpecify the source code file to compile\n\n"
        
                    BLUE "Example:\n" RESET
                    "\tflower -help\n"
                    "\tflower main.flo\n\n"
        
                    BLUE "Tips:\n" RESET
                    " - You can use any number of dashes before a flag, e.g., ---help\n"
                    " - Flags are " BOLD "case-insensitive" RESET ": -HELP works too!\n\n"
        
                    GREEN "Happy Compiling with Flower! 🌼\n" RESET
                );
                continue;
            }
    
            else if (!strcmp(arg, "version") || !strcmp(arg, "v")) {
                printf(
                    "Version: 0.0.4\n"
                );
                continue;
            }   
    
            else
                printf(RED "Unrecognized flag argument! Use -help for more information.\n" RESET);
        } else {
            printf(BLUE BOLD "Compiling file: %s\n" RESET, argv[i]);
            char* file = read_file(argv[i]);
            if (!file) {
                return -1;
            }

            // Initialize token handling
            TokenStream tokens;
            init_token_stream(&tokens);

            // Lexer
            lex(file, &tokens);
            // print_all_tokens(&tokens, file);

            // parser
            Parser tree;
            init_parser(&tree, &tokens, file);
            AST* ast = parse(&tree);

            // Determine output name
            char out_c[512];
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                snprintf(out_c, sizeof(out_c), "%s.c", argv[i + 1]);
                i++; // skip the output name on next iteration
            } else {
                snprintf(out_c, sizeof(out_c), "output.c");
            }

            FILE* output = fopen(out_c, "w");
            if (!output) {
                printf(RED "Failed to open output file\n" RESET);
                return -1;
            }

            if (token_stream_contains(&tokens, TOKEN_NULL) || token_stream_contains(&tokens, TOKEN_NEW))
                fprintf(output, "#include <stdlib.h>\n");

            codegen(ast, output, file);

            free_token_stream(&tokens);
            free(file);
            fclose(output);

            // Strip .c for binary name
            char bin_name[512];
            snprintf(bin_name, sizeof(bin_name), "%.*s", (int)(strlen(out_c) - 2), out_c);

            char build_cmd[1024];
            snprintf(build_cmd, sizeof(build_cmd), "clang %s -o %s && ./%s", out_c, bin_name, bin_name);
            system(build_cmd);
            printf(GREEN BOLD "Compiled %s → %s\n" RESET, argv[i], bin_name);
            break;
        }

    }

    // printf("%s", argv[1]);

    return 0;
}