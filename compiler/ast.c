#include "ast.h"

ast_expr_t *ast_expr_create(ast_expr_type_t type) {
    ast_expr_t *expr = NEUROX_MALLOC(sizeof(ast_expr_t));
    expr->type = type;
    expr->line = 0;
    expr->column = 0;
    return expr;
}

ast_stmt_t *ast_stmt_create(ast_stmt_type_t type) {
    ast_stmt_t *stmt = NEUROX_MALLOC(sizeof(ast_stmt_t));
    stmt->type = type;
    stmt->line = 0;
    stmt->column = 0;
    return stmt;
}

ast_decl_t *ast_decl_create(ast_decl_type_t type) {
    ast_decl_t *decl = NEUROX_MALLOC(sizeof(ast_decl_t));
    decl->type = type;
    decl->line = 0;
    decl->column = 0;
    return decl;
}

ast_robot_t *ast_robot_create(const char *name) {
    ast_robot_t *robot = NEUROX_MALLOC(sizeof(ast_robot_t));
    robot->name = NEUROX_STRDUP(name);
    robot->declarations = NULL;
    robot->decl_count = 0;
    return robot;
}

void ast_expr_free(ast_expr_t *expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case EXPR_LITERAL:
            if (expr->as.literal.type == LITERAL_STRING) {
                NEUROX_FREE(expr->as.literal.value.string);
            }
            break;
        case EXPR_IDENTIFIER:
            NEUROX_FREE(expr->as.identifier);
            break;
        case EXPR_BINARY:
            ast_expr_free(expr->as.binary.left);
            ast_expr_free(expr->as.binary.right);
            break;
        case EXPR_UNARY:
            ast_expr_free(expr->as.unary.operand);
            break;
        case EXPR_CALL:
            ast_expr_free(expr->as.call.callee);
            for (size_t i = 0; i < expr->as.call.arg_count; i++) {
                ast_expr_free(expr->as.call.args[i]);
            }
            NEUROX_FREE(expr->as.call.args);
            break;
        case EXPR_MEMBER:
            ast_expr_free(expr->as.member.object);
            NEUROX_FREE(expr->as.member.member);
            break;
        case EXPR_UNIT:
            ast_expr_free(expr->as.unit.value);
            break;
    }
    
    NEUROX_FREE(expr);
}

void ast_stmt_free(ast_stmt_t *stmt) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case STMT_EXPR:
            ast_expr_free(stmt->as.expr);
            break;
        case STMT_ASSIGN:
            NEUROX_FREE(stmt->as.assign.target);
            ast_expr_free(stmt->as.assign.value);
            break;
        case STMT_IF:
            ast_expr_free(stmt->as.if_stmt.condition);
            ast_stmt_free(stmt->as.if_stmt.then_branch);
            ast_stmt_free(stmt->as.if_stmt.else_branch);
            break;
        case STMT_BLOCK:
            for (size_t i = 0; i < stmt->as.block.count; i++) {
                ast_stmt_free(stmt->as.block.statements[i]);
            }
            NEUROX_FREE(stmt->as.block.statements);
            break;
        case STMT_WAIT:
            ast_expr_free(stmt->as.wait.duration);
            break;
        case STMT_RETURN:
            ast_expr_free(stmt->as.return_value);
            break;
    }
    
    NEUROX_FREE(stmt);
}

void ast_decl_free(ast_decl_t *decl) {
    if (!decl) return;
    
    switch (decl->type) {
        case DECL_MOTOR:
            NEUROX_FREE(decl->as.motor.name);
            NEUROX_FREE(decl->as.motor.pin);
            break;
        case DECL_TASK:
            NEUROX_FREE(decl->as.task.name);
            for (size_t i = 0; i < decl->as.task.param_count; i++) {
                NEUROX_FREE(decl->as.task.params[i]->name);
                NEUROX_FREE(decl->as.task.params[i]->type);
                NEUROX_FREE(decl->as.task.params[i]);
            }
            NEUROX_FREE(decl->as.task.params);
            ast_stmt_free(decl->as.task.body);
            break;
        case DECL_SCHEDULE:
            NEUROX_FREE(decl->as.schedule.name);
            ast_expr_free(decl->as.schedule.frequency);
            ast_stmt_free(decl->as.schedule.body);
            break;
        default:
            // TODO: Free other declaration types
            break;
    }
    
    NEUROX_FREE(decl);
}

void ast_robot_free(ast_robot_t *robot) {
    if (!robot) return;
    
    NEUROX_FREE(robot->name);
    for (size_t i = 0; i < robot->decl_count; i++) {
        ast_decl_free(robot->declarations[i]);
    }
    NEUROX_FREE(robot->declarations);
    NEUROX_FREE(robot);
}

// Printing utilities
static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

void ast_expr_print(ast_expr_t *expr, int indent) {
    if (!expr) return;
    
    print_indent(indent);
    switch (expr->type) {
        case EXPR_LITERAL:
            if (expr->as.literal.type == LITERAL_NUMBER) {
                printf("Literal: %g\n", expr->as.literal.value.number);
            } else if (expr->as.literal.type == LITERAL_STRING) {
                printf("Literal: \"%s\"\n", expr->as.literal.value.string);
            }
            break;
        case EXPR_IDENTIFIER:
            printf("Identifier: %s\n", expr->as.identifier);
            break;
        case EXPR_BINARY:
            printf("Binary:\n");
            ast_expr_print(expr->as.binary.left, indent + 1);
            ast_expr_print(expr->as.binary.right, indent + 1);
            break;
        case EXPR_MEMBER:
            printf("Member: .%s\n", expr->as.member.member);
            ast_expr_print(expr->as.member.object, indent + 1);
            break;
        default:
            printf("Expression (type %d)\n", expr->type);
            break;
    }
}

void ast_stmt_print(ast_stmt_t *stmt, int indent) {
    if (!stmt) return;
    
    print_indent(indent);
    switch (stmt->type) {
        case STMT_EXPR:
            printf("ExprStmt:\n");
            ast_expr_print(stmt->as.expr, indent + 1);
            break;
        case STMT_ASSIGN:
            printf("Assign: %s =\n", stmt->as.assign.target);
            ast_expr_print(stmt->as.assign.value, indent + 1);
            break;
        case STMT_BLOCK:
            printf("Block:\n");
            for (size_t i = 0; i < stmt->as.block.count; i++) {
                ast_stmt_print(stmt->as.block.statements[i], indent + 1);
            }
            break;
        default:
            printf("Statement (type %d)\n", stmt->type);
            break;
    }
}

void ast_decl_print(ast_decl_t *decl, int indent) {
    if (!decl) return;
    
    print_indent(indent);
    switch (decl->type) {
        case DECL_MOTOR:
            printf("Motor: %s on %s\n", decl->as.motor.name, decl->as.motor.pin);
            break;
        case DECL_TASK:
            printf("Task: %s\n", decl->as.task.name);
            ast_stmt_print(decl->as.task.body, indent + 1);
            break;
        case DECL_SCHEDULE:
            printf("Schedule: %s\n", decl->as.schedule.name);
            ast_stmt_print(decl->as.schedule.body, indent + 1);
            break;
        default:
            printf("Declaration (type %d)\n", decl->type);
            break;
    }
}

void ast_robot_print(ast_robot_t *robot) {
    if (!robot) return;
    
    printf("Robot: %s\n", robot->name);
    for (size_t i = 0; i < robot->decl_count; i++) {
        ast_decl_print(robot->declarations[i], 1);
    }
}
