#include "parser.h"

static void error_at(parser_t *parser, token_t *token, const char *message) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;
    
    neurox_diagnostic_t diag = {
        .filename = parser->lexer->filename,
        .line = token->line,
        .column = token->column,
        .message = message,
        .error_code = NEUROX_ERROR_SYNTAX,
    };
    neurox_report_error(&diag);
    parser->had_error = true;
}

static void error(parser_t *parser, const char *message) {
    error_at(parser, &parser->previous, message);
}

static void error_at_current(parser_t *parser, const char *message) {
    error_at(parser, &parser->current, message);
}

static void advance(parser_t *parser) {
    parser->previous = parser->current;
    
    for (;;) {
        parser->current = lexer_next_token(parser->lexer);
        if (parser->current.type != TOKEN_ERROR) break;
        
        error_at_current(parser, parser->current.start);
    }
}

static bool check(parser_t *parser, token_type_t type) {
    return parser->current.type == type;
}

static bool match(parser_t *parser, token_type_t type) {
    if (!check(parser, type)) return false;
    advance(parser);
    return true;
}

static void consume(parser_t *parser, token_type_t type, const char *message) {
    if (parser->current.type == type) {
        advance(parser);
        return;
    }
    error_at_current(parser, message);
}

static void skip_newlines(parser_t *parser) {
    while (match(parser, TOKEN_NEWLINE)) {
        // Skip
    }
}

// Forward declarations
static ast_expr_t *parse_expression(parser_t *parser);
static ast_stmt_t *parse_statement(parser_t *parser);
static ast_decl_t *parse_declaration(parser_t *parser);

// Expression parsing
static ast_expr_t *parse_primary(parser_t *parser) {
    if (match(parser, TOKEN_NUMBER)) {
        ast_expr_t *expr = ast_expr_create(EXPR_LITERAL);
        expr->as.literal.type = LITERAL_NUMBER;
        expr->as.literal.value.number = strtod(parser->previous.start, NULL);
        expr->line = parser->previous.line;
        expr->column = parser->previous.column;
        return expr;
    }
    
    if (match(parser, TOKEN_STRING)) {
        ast_expr_t *expr = ast_expr_create(EXPR_LITERAL);
        expr->as.literal.type = LITERAL_STRING;
        // Remove quotes
        size_t len = parser->previous.length - 2;
        expr->as.literal.value.string = NEUROX_MALLOC(len + 1);
        memcpy(expr->as.literal.value.string, parser->previous.start + 1, len);
        expr->as.literal.value.string[len] = '\0';
        expr->line = parser->previous.line;
        expr->column = parser->previous.column;
        return expr;
    }
    
    if (match(parser, TOKEN_IDENTIFIER)) {
        ast_expr_t *expr = ast_expr_create(EXPR_IDENTIFIER);
        expr->as.identifier = token_to_string(&parser->previous);
        expr->line = parser->previous.line;
        expr->column = parser->previous.column;
        return expr;
    }
    
    if (match(parser, TOKEN_LEFT_PAREN)) {
        ast_expr_t *expr = parse_expression(parser);
        consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    error(parser, "Expected expression");
    return NULL;
}

static ast_expr_t *parse_call(parser_t *parser) {
    ast_expr_t *expr = parse_primary(parser);
    
    while (true) {
        if (match(parser, TOKEN_LEFT_PAREN)) {
            // Function call
            ast_expr_t *call = ast_expr_create(EXPR_CALL);
            call->as.call.callee = expr;
            call->as.call.args = NULL;
            call->as.call.arg_count = 0;
            
            if (!check(parser, TOKEN_RIGHT_PAREN)) {
                // Parse arguments
                size_t capacity = 4;
                call->as.call.args = NEUROX_MALLOC(capacity * sizeof(ast_expr_t *));
                
                do {
                    if (call->as.call.arg_count >= capacity) {
                        capacity *= 2;
                        call->as.call.args = NEUROX_REALLOC(call->as.call.args,
                                                            capacity * sizeof(ast_expr_t *));
                    }
                    call->as.call.args[call->as.call.arg_count++] = parse_expression(parser);
                } while (match(parser, TOKEN_COMMA));
            }
            
            consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after arguments");
            expr = call;
        } else if (match(parser, TOKEN_DOT)) {
            // Member access
            consume(parser, TOKEN_IDENTIFIER, "Expected property name after '.'");
            ast_expr_t *member = ast_expr_create(EXPR_MEMBER);
            member->as.member.object = expr;
            member->as.member.member = token_to_string(&parser->previous);
            expr = member;
        } else {
            break;
        }
    }
    
    return expr;
}

static ast_expr_t *parse_unary(parser_t *parser) {
    if (match(parser, TOKEN_MINUS) || match(parser, TOKEN_BANG)) {
        token_t op = parser->previous;
        ast_expr_t *expr = ast_expr_create(EXPR_UNARY);
        expr->as.unary.op = (op.type == TOKEN_MINUS) ? OP_NEG : OP_NOT;
        expr->as.unary.operand = parse_unary(parser);
        return expr;
    }
    
    return parse_call(parser);
}

static ast_expr_t *parse_factor(parser_t *parser) {
    ast_expr_t *expr = parse_unary(parser);
    
    while (match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH) || match(parser, TOKEN_PERCENT)) {
        token_t op = parser->previous;
        ast_expr_t *right = parse_unary(parser);
        
        ast_expr_t *binary = ast_expr_create(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.right = right;
        if (op.type == TOKEN_STAR) {
            binary->as.binary.op = OP_MUL;
        } else if (op.type == TOKEN_SLASH) {
            binary->as.binary.op = OP_DIV;
        } else {
            binary->as.binary.op = OP_MOD;
        }
        expr = binary;
    }
    
    return expr;
}

static ast_expr_t *parse_term(parser_t *parser) {
    ast_expr_t *expr = parse_factor(parser);
    
    while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
        token_t op = parser->previous;
        ast_expr_t *right = parse_factor(parser);
        
        ast_expr_t *binary = ast_expr_create(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.right = right;
        binary->as.binary.op = (op.type == TOKEN_PLUS) ? OP_ADD : OP_SUB;
        expr = binary;
    }
    
    return expr;
}

static ast_expr_t *parse_comparison(parser_t *parser) {
    ast_expr_t *expr = parse_term(parser);
    
    while (match(parser, TOKEN_LESS) || match(parser, TOKEN_LESS_EQUAL) ||
           match(parser, TOKEN_GREATER) || match(parser, TOKEN_GREATER_EQUAL)) {
        token_t op = parser->previous;
        ast_expr_t *right = parse_term(parser);
        
        ast_expr_t *binary = ast_expr_create(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.right = right;
        
        if (op.type == TOKEN_LESS) binary->as.binary.op = OP_LT;
        else if (op.type == TOKEN_LESS_EQUAL) binary->as.binary.op = OP_LTE;
        else if (op.type == TOKEN_GREATER) binary->as.binary.op = OP_GT;
        else binary->as.binary.op = OP_GTE;
        
        expr = binary;
    }
    
    return expr;
}

static ast_expr_t *parse_equality(parser_t *parser) {
    ast_expr_t *expr = parse_comparison(parser);
    
    while (match(parser, TOKEN_EQUAL_EQUAL) || match(parser, TOKEN_BANG_EQUAL)) {
        token_t op = parser->previous;
        ast_expr_t *right = parse_comparison(parser);
        
        ast_expr_t *binary = ast_expr_create(EXPR_BINARY);
        binary->as.binary.left = expr;
        binary->as.binary.right = right;
        binary->as.binary.op = (op.type == TOKEN_EQUAL_EQUAL) ? OP_EQ : OP_NEQ;
        expr = binary;
    }
    
    return expr;
}

static ast_expr_t *parse_expression(parser_t *parser) {
    return parse_equality(parser);
}

// Statement parsing
static ast_stmt_t *parse_block(parser_t *parser) {
    ast_stmt_t *block = ast_stmt_create(STMT_BLOCK);
    block->as.block.statements = NULL;
    block->as.block.count = 0;
    
    size_t capacity = 8;
    block->as.block.statements = NEUROX_MALLOC(capacity * sizeof(ast_stmt_t *));
    
    skip_newlines(parser);
    
    while (!check(parser, TOKEN_RIGHT_BRACE) && !check(parser, TOKEN_EOF)) {
        if (block->as.block.count >= capacity) {
            capacity *= 2;
            block->as.block.statements = NEUROX_REALLOC(block->as.block.statements,
                                                        capacity * sizeof(ast_stmt_t *));
        }
        
        block->as.block.statements[block->as.block.count++] = parse_statement(parser);
        skip_newlines(parser);
    }
    
    return block;
}

static ast_stmt_t *parse_if_statement(parser_t *parser) {
    ast_stmt_t *stmt = ast_stmt_create(STMT_IF);
    
    ast_expr_t *condition = parse_expression(parser);
    consume(parser, TOKEN_LEFT_BRACE, "Expected '{' after if condition");
    skip_newlines(parser);
    
    ast_stmt_t *then_branch = parse_block(parser);
    consume(parser, TOKEN_RIGHT_BRACE, "Expected '}' after if body");
    
    ast_stmt_t *else_branch = NULL;
    if (match(parser, TOKEN_ELSE)) {
        consume(parser, TOKEN_LEFT_BRACE, "Expected '{' after else");
        skip_newlines(parser);
        else_branch = parse_block(parser);
        consume(parser, TOKEN_RIGHT_BRACE, "Expected '}' after else body");
    }
    
    stmt->as.if_stmt.condition = condition;
    stmt->as.if_stmt.then_branch = then_branch;
    stmt->as.if_stmt.else_branch = else_branch;
    
    return stmt;
}

static ast_stmt_t *parse_statement(parser_t *parser) {
    skip_newlines(parser);
    
    if (match(parser, TOKEN_IF)) {
        return parse_if_statement(parser);
    }
    
    if (match(parser, TOKEN_WAIT)) {
        consume(parser, TOKEN_LEFT_PAREN, "Expected '(' after 'wait'");
        ast_expr_t *duration = parse_expression(parser);
        consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after wait duration");
        
        ast_stmt_t *stmt = ast_stmt_create(STMT_WAIT);
        stmt->as.wait.duration = duration;
        return stmt;
    }
    
    // Try assignment or expression statement
    ast_expr_t *expr = parse_expression(parser);
    
    if (match(parser, TOKEN_EQUAL)) {
        // Assignment
        if (expr->type != EXPR_IDENTIFIER && expr->type != EXPR_MEMBER) {
            error(parser, "Invalid assignment target");
            return NULL;
        }
        
        ast_stmt_t *stmt = ast_stmt_create(STMT_ASSIGN);
        if (expr->type == EXPR_IDENTIFIER) {
            stmt->as.assign.target = expr->as.identifier;
            expr->as.identifier = NULL; // Transfer ownership
        } else {
            // For member access, create a dotted path
            // Simplified: just use the member name
            stmt->as.assign.target = NEUROX_STRDUP("member");
        }
        stmt->as.assign.value = parse_expression(parser);
        
        ast_expr_free(expr);
        return stmt;
    }
    
    // Expression statement
    ast_stmt_t *stmt = ast_stmt_create(STMT_EXPR);
    stmt->as.expr = expr;
    return stmt;
}

// Declaration parsing
static ast_decl_t *parse_motor_decl(parser_t *parser) {
    consume(parser, TOKEN_IDENTIFIER, "Expected motor name");
    char *name = token_to_string(&parser->previous);
    
    consume(parser, TOKEN_ON, "Expected 'on' after motor name");
    consume(parser, TOKEN_IDENTIFIER, "Expected pin identifier");
    char *pin = token_to_string(&parser->previous);
    
    ast_decl_t *decl = ast_decl_create(DECL_MOTOR);
    decl->as.motor.name = name;
    decl->as.motor.pin = pin;
    
    return decl;
}

static ast_decl_t *parse_sensor_decl(parser_t *parser) {
    consume(parser, TOKEN_IDENTIFIER, "Expected sensor name");
    char *name = token_to_string(&parser->previous);
    
    consume(parser, TOKEN_ON, "Expected 'on' after sensor name");
    consume(parser, TOKEN_IDENTIFIER, "Expected pin identifier");
    char *pin = token_to_string(&parser->previous);
    
    char *sensor_type = NULL;
    if (match(parser, TOKEN_TYPE)) {
        consume(parser, TOKEN_IDENTIFIER, "Expected sensor type");
        sensor_type = token_to_string(&parser->previous);
    }
    
    ast_decl_t *decl = ast_decl_create(DECL_SENSOR);
    decl->as.sensor.name = name;
    decl->as.sensor.pin = pin;
    decl->as.sensor.sensor_type = sensor_type;
    
    return decl;
}

static ast_decl_t *parse_task_decl(parser_t *parser) {
    consume(parser, TOKEN_IDENTIFIER, "Expected task name");
    char *name = token_to_string(&parser->previous);
    
    consume(parser, TOKEN_LEFT_PAREN, "Expected '(' after task name");
    
    // Parse parameters (simplified - no type checking yet)
    ast_param_t **params = NULL;
    size_t param_count = 0;
    
    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        size_t capacity = 4;
        params = NEUROX_MALLOC(capacity * sizeof(ast_param_t *));
        
        do {
            if (param_count >= capacity) {
                capacity *= 2;
                params = NEUROX_REALLOC(params, capacity * sizeof(ast_param_t *));
            }
            
            consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
            ast_param_t *param = NEUROX_MALLOC(sizeof(ast_param_t));
            param->name = token_to_string(&parser->previous);
            param->type = NULL;
            
            if (match(parser, TOKEN_COLON)) {
                // Parse type
                consume(parser, TOKEN_IDENTIFIER, "Expected type name");
                param->type = NEUROX_MALLOC(sizeof(ast_type_t));
                param->type->name = token_to_string(&parser->previous);
                param->type->unit = UNIT_PERCENT; // Default
            }
            
            params[param_count++] = param;
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after parameters");
    consume(parser, TOKEN_LEFT_BRACE, "Expected '{' before task body");
    skip_newlines(parser);
    
    ast_stmt_t *body = parse_block(parser);
    consume(parser, TOKEN_RIGHT_BRACE, "Expected '}' after task body");
    
    ast_decl_t *decl = ast_decl_create(DECL_TASK);
    decl->as.task.name = name;
    decl->as.task.params = params;
    decl->as.task.param_count = param_count;
    decl->as.task.body = body;
    
    return decl;
}

static ast_decl_t *parse_schedule_decl(parser_t *parser) {
    consume(parser, TOKEN_IDENTIFIER, "Expected schedule name");
    char *name = token_to_string(&parser->previous);
    
    consume(parser, TOKEN_AT, "Expected '@' after schedule name");
    ast_expr_t *frequency = parse_expression(parser);
    
    ast_priority_t priority = PRIORITY_MEDIUM;
    if (match(parser, TOKEN_PRIORITY)) {
        if (match(parser, TOKEN_HIGH)) {
            priority = PRIORITY_HIGH;
        } else if (match(parser, TOKEN_MEDIUM)) {
            priority = PRIORITY_MEDIUM;
        } else if (match(parser, TOKEN_LOW)) {
            priority = PRIORITY_LOW;
        } else {
            error(parser, "Expected priority level (HIGH, MEDIUM, LOW)");
        }
    }
    
    consume(parser, TOKEN_LEFT_BRACE, "Expected '{' before schedule body");
    skip_newlines(parser);
    
    ast_stmt_t *body = parse_block(parser);
    consume(parser, TOKEN_RIGHT_BRACE, "Expected '}' after schedule body");
    
    ast_decl_t *decl = ast_decl_create(DECL_SCHEDULE);
    decl->as.schedule.name = name;
    decl->as.schedule.frequency = frequency;
    decl->as.schedule.priority = priority;
    decl->as.schedule.body = body;
    
    return decl;
}

static ast_decl_t *parse_declaration(parser_t *parser) {
    skip_newlines(parser);
    
    if (match(parser, TOKEN_MOTOR)) {
        return parse_motor_decl(parser);
    }
    
    if (match(parser, TOKEN_SENSOR)) {
        return parse_sensor_decl(parser);
    }
    
    if (match(parser, TOKEN_TASK)) {
        return parse_task_decl(parser);
    }
    
    if (match(parser, TOKEN_SCHEDULE)) {
        return parse_schedule_decl(parser);
    }
    
    error(parser, "Expected declaration");
    return NULL;
}

void parser_init(parser_t *parser, lexer_t *lexer) {
    parser->lexer = lexer;
    parser->had_error = false;
    parser->panic_mode = false;
    
    // Prime the pump
    advance(parser);
}

ast_robot_t *parser_parse(parser_t *parser) {
    skip_newlines(parser);
    
    consume(parser, TOKEN_ROBOT, "Expected 'robot' keyword");
    consume(parser, TOKEN_IDENTIFIER, "Expected robot name");
    
    char *name = token_to_string(&parser->previous);
    ast_robot_t *robot = ast_robot_create(name);
    NEUROX_FREE(name);
    
    consume(parser, TOKEN_LEFT_BRACE, "Expected '{' after robot name");
    skip_newlines(parser);
    
    size_t capacity = 16;
    robot->declarations = NEUROX_MALLOC(capacity * sizeof(ast_decl_t *));
    
    while (!check(parser, TOKEN_RIGHT_BRACE) && !check(parser, TOKEN_EOF)) {
        if (robot->decl_count >= capacity) {
            capacity *= 2;
            robot->declarations = NEUROX_REALLOC(robot->declarations,
                                                 capacity * sizeof(ast_decl_t *));
        }
        
        ast_decl_t *decl = parse_declaration(parser);
        if (decl) {
            robot->declarations[robot->decl_count++] = decl;
        }
        
        skip_newlines(parser);
    }
    
    consume(parser, TOKEN_RIGHT_BRACE, "Expected '}' after robot body");
    
    if (parser->had_error) {
        ast_robot_free(robot);
        return NULL;
    }
    
    return robot;
}
