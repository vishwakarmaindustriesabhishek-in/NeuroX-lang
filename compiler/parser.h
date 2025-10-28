#ifndef NEUROX_PARSER_H
#define NEUROX_PARSER_H

#include "common.h"
#include "lexer.h"
#include "ast.h"

typedef struct {
    lexer_t *lexer;
    token_t current;
    token_t previous;
    bool had_error;
    bool panic_mode;
} parser_t;

// Parser API
void parser_init(parser_t *parser, lexer_t *lexer);
ast_robot_t *parser_parse(parser_t *parser);

#endif // NEUROX_PARSER_H
