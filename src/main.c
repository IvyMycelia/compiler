#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define RED   "\033[0;31m"
#define GREEN     "\033[0;32m"
#define RESET   "\033[0m"
#define BLUE    "\033[0;34m"
#define BOLD    "\033[1m"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(RED "Missing arguments! Use -help for usage instructions.\n" RESET);
        return -1;
    } else if (argc > 2) {
        printf(RED "Too many arguments! Use -help for usage instructions.\n" RESET);
        return -1;
    }

    char *arg = argv[1];
    while (*arg == '-') arg++;
    char *p = arg;
    while (*p) {
        *p = tolower(*p);
        p++;
    }
    if (!strcmp(arg, "help") || !strcmp(arg, "h")) {
        printf(
            GREEN "🌸 Welcome to Flower Compiler!\n\n" RESET

            BLUE "Usage:\n" RESET
            "\tflower\t[options] <filepath>\n\n"

            BLUE "Options:\n" RESET
            "\t-help,  -h\tShow this help message\n"
            "\t<filepath>\tSpecify the source code file to compile\n\n"

            BLUE "Example:\n" RESET
            "\tflower -help\n"
            "\tflower main.flo\n\n"

            BLUE "Tips:\n" RESET
            " - You can use any number of dashes before a flag, e.g., ---help\n"
            " - Flags are " BOLD "case-insensitive" RESET ": -HELP works too!\n"

            GREEN "Happy Compiling with Flower! 🌼\n" RESET
        );
        return 0;
    } 
    else if (!strcmp(arg, "version") || !strcmp(arg, "v")) {
        printf(
            "Version: 0.0.1\n"
        );
        return 0;
    }
    else {
        printf(RED "Unrecognized flag argument! Use -help for more information.\n" RESET);
        return -1;
    }

    // printf("%s", argv[1]);

    return 0;
}