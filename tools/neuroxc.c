#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

static void print_usage(const char *prog_name) {
    printf("NeuroX Compiler (neuroxc) v%d.%d.%d\n\n",
           NEUROX_VERSION_MAJOR, NEUROX_VERSION_MINOR, NEUROX_VERSION_PATCH);
    printf("Usage: %s <command> [options] <input>\n\n", prog_name);
    printf("Commands:\n");
    printf("  emit-c <file>      Generate C code from .neuro file\n");
    printf("  parse <file>       Parse and print AST (debug)\n");
    printf("  lex <file>         Tokenize and print tokens (debug)\n");
    printf("  check <file>       Type check only\n");
    printf("  format <file>      Format .neuro file\n");
    printf("  lint <file>        Lint .neuro file\n");
    printf("\nOptions:\n");
    printf("  -o <file>          Output file\n");
    printf("  -h, --help         Show this help\n");
    printf("  -v, --version      Show version\n");
}

static char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", path);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';
    
    fclose(file);
    return buffer;
}

static int cmd_lex(const char *input_file) {
    char *source = read_file(input_file);
    if (!source) return 1;
    
    lexer_t lexer;
    lexer_init(&lexer, source, input_file);
    
    printf("Tokens from '%s':\n", input_file);
    printf("----------------------------------------\n");
    
    token_t token;
    do {
        token = lexer_next_token(&lexer);
        if (token.type != TOKEN_NEWLINE && token.type != TOKEN_EOF) {
            token_print(&token);
        }
    } while (token.type != TOKEN_EOF);
    
    free(source);
    return 0;
}

static int cmd_parse(const char *input_file) {
    char *source = read_file(input_file);
    if (!source) return 1;
    
    lexer_t lexer;
    lexer_init(&lexer, source, input_file);
    
    parser_t parser;
    parser_init(&parser, &lexer);
    
    ast_robot_t *robot = parser_parse(&parser);
    
    if (robot) {
        printf("AST for '%s':\n", input_file);
        printf("----------------------------------------\n");
        ast_robot_print(robot);
        ast_robot_free(robot);
        free(source);
        return 0;
    } else {
        fprintf(stderr, "Parse failed\n");
        free(source);
        return 1;
    }
}

static int cmd_emit_c(const char *input_file, const char *output_file) {
    char *source = read_file(input_file);
    if (!source) return 1;
    
    lexer_t lexer;
    lexer_init(&lexer, source, input_file);
    
    parser_t parser;
    parser_init(&parser, &lexer);
    
    ast_robot_t *robot = parser_parse(&parser);
    
    if (!robot) {
        fprintf(stderr, "Parse failed\n");
        free(source);
        return 1;
    }
    
    // Open output file
    FILE *out = stdout;
    if (output_file) {
        out = fopen(output_file, "w");
        if (!out) {
            fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
            ast_robot_free(robot);
            free(source);
            return 1;
        }
    }
    
    // Generate C code (simplified codegen)
    fprintf(out, "// Generated from %s\n", input_file);
    fprintf(out, "#include \"runtime/core/scheduler.h\"\n");
    fprintf(out, "#include \"runtime/core/safety.h\"\n");
    fprintf(out, "#include \"runtime/hal/hal.h\"\n");
    fprintf(out, "#include \"runtime/net/mqtt.h\"\n");
    fprintf(out, "#include <stdio.h>\n\n");
    
    fprintf(out, "// Robot: %s\n", robot->name);
    fprintf(out, "// TODO: Full code generation\n\n");
    
    fprintf(out, "int main(void) {\n");
    fprintf(out, "    printf(\"NeuroX Robot: %s\\n\");\n", robot->name);
    fprintf(out, "    \n");
    fprintf(out, "    // Initialize runtime\n");
    fprintf(out, "    nrx_scheduler_config_t sched_config = {0};\n");
    fprintf(out, "    nrx_scheduler_init(&sched_config);\n");
    fprintf(out, "    \n");
    fprintf(out, "    nrx_safety_config_t safety_config = {0};\n");
    fprintf(out, "    nrx_safety_init(&safety_config);\n");
    fprintf(out, "    \n");
    fprintf(out, "    // TODO: Initialize hardware, tasks, schedules\n");
    fprintf(out, "    \n");
    fprintf(out, "    printf(\"Starting scheduler...\\n\");\n");
    fprintf(out, "    nrx_scheduler_start();\n");
    fprintf(out, "    \n");
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
    
    if (output_file) {
        fclose(out);
        printf("Generated C code: %s\n", output_file);
    }
    
    ast_robot_free(robot);
    free(source);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char *command = argv[1];
    
    if (strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (strcmp(command, "-v") == 0 || strcmp(command, "--version") == 0) {
        printf("neuroxc v%d.%d.%d\n",
               NEUROX_VERSION_MAJOR, NEUROX_VERSION_MINOR, NEUROX_VERSION_PATCH);
        return 0;
    }
    
    if (strcmp(command, "lex") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing input file\n");
            return 1;
        }
        return cmd_lex(argv[2]);
    }
    
    if (strcmp(command, "parse") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing input file\n");
            return 1;
        }
        return cmd_parse(argv[2]);
    }
    
    if (strcmp(command, "emit-c") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: Missing input file\n");
            return 1;
        }
        
        const char *input_file = argv[2];
        const char *output_file = NULL;
        
        // Check for -o option
        for (int i = 3; i < argc - 1; i++) {
            if (strcmp(argv[i], "-o") == 0) {
                output_file = argv[i + 1];
                break;
            }
        }
        
        return cmd_emit_c(input_file, output_file);
    }
    
    fprintf(stderr, "Error: Unknown command '%s'\n", command);
    print_usage(argv[0]);
    return 1;
}
