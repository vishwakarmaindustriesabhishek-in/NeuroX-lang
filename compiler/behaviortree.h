#ifndef NEUROX_BEHAVIORTREE_H
#define NEUROX_BEHAVIORTREE_H

#include "common.h"
#include "ast.h"

// Behavior tree node types
typedef enum {
    BT_SEQUENCE,        // Execute children in order, fail on first failure
    BT_SELECTOR,        // Try children until one succeeds
    BT_PARALLEL,        // Execute all children simultaneously
    BT_DECORATOR,       // Modify child behavior
    BT_ACTION,          // Leaf node - execute action
    BT_CONDITION,       // Leaf node - check condition
} bt_node_type_t;

// Node status
typedef enum {
    BT_SUCCESS,
    BT_FAILURE,
    BT_RUNNING,
} bt_status_t;

// Decorator types
typedef enum {
    BT_INVERTER,        // Invert success/failure
    BT_REPEATER,        // Repeat N times or until failure
    BT_RETRY,           // Retry on failure up to N times
    BT_TIMEOUT,         // Fail if takes longer than duration
    BT_COOLDOWN,        // Prevent re-execution for duration
    BT_FORCE_SUCCESS,   // Always return success
    BT_FORCE_FAILURE,   // Always return failure
} bt_decorator_type_t;

// Behavior tree node
typedef struct bt_node_t {
    bt_node_type_t type;
    char *name;
    
    union {
        struct {
            // Composite nodes (sequence, selector, parallel)
            struct bt_node_t **children;
            size_t child_count;
        } composite;
        
        struct {
            // Decorator
            bt_decorator_type_t decorator_type;
            struct bt_node_t *child;
            int repeat_count;       // For repeater/retry
            ast_expr_t *duration;   // For timeout/cooldown
        } decorator;
        
        struct {
            // Action leaf
            ast_stmt_t *action;
        } action;
        
        struct {
            // Condition leaf
            ast_expr_t *condition;
        } condition;
    } as;
    
    int line;
    int column;
} bt_node_t;

// Behavior tree definition
typedef struct {
    char *name;
    bt_node_t *root;
    
    // Blackboard variables (shared state)
    ast_param_t **blackboard;
    size_t blackboard_count;
    
    // Tick rate
    uint32_t tick_rate_hz;
} bt_tree_t;

// Behavior tree AST integration
typedef struct {
    bt_tree_t *tree;
} ast_behaviortree_decl_t;

// Behavior tree API
bt_tree_t *bt_create(const char *name);
bt_node_t *bt_create_sequence(const char *name);
bt_node_t *bt_create_selector(const char *name);
bt_node_t *bt_create_parallel(const char *name);
bt_node_t *bt_create_action(const char *name, ast_stmt_t *action);
bt_node_t *bt_create_condition(const char *name, ast_expr_t *condition);
bt_node_t *bt_create_decorator(bt_decorator_type_t type, bt_node_t *child);

void bt_add_child(bt_node_t *parent, bt_node_t *child);
void bt_set_root(bt_tree_t *tree, bt_node_t *root);

void bt_free(bt_tree_t *tree);
void bt_print(bt_tree_t *tree);

// Code generation
void bt_generate_c(bt_tree_t *tree, FILE *out);

#endif // NEUROX_BEHAVIORTREE_H
