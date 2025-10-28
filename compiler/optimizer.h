#ifndef NEUROX_OPTIMIZER_H
#define NEUROX_OPTIMIZER_H

#include "common.h"
#include "ast.h"

// Optimization passes
typedef enum {
    OPT_CONSTANT_FOLDING,       // Evaluate constant expressions at compile time
    OPT_DEAD_CODE_ELIMINATION,  // Remove unreachable code
    OPT_COMMON_SUBEXPR,         // Eliminate redundant computations
    OPT_LOOP_UNROLLING,         // Unroll small loops
    OPT_FUNCTION_INLINING,      // Inline small functions
    OPT_STRENGTH_REDUCTION,     // Replace expensive ops with cheaper ones
    OPT_TAIL_CALL,              // Optimize tail recursion
    OPT_REGISTER_ALLOCATION,    // Optimize variable placement
} opt_pass_t;

// Optimization level
typedef enum {
    OPT_LEVEL_NONE,     // -O0: No optimization
    OPT_LEVEL_SIZE,     // -Os: Optimize for size
    OPT_LEVEL_SPEED,    // -O2: Optimize for speed
    OPT_LEVEL_MAX,      // -O3: Maximum optimization
} opt_level_t;

// Optimization configuration
typedef struct {
    opt_level_t level;
    bool enable_passes[16];
    bool preserve_debug_info;
    bool aggressive_inlining;
    uint32_t max_inline_size;
    uint32_t max_unroll_count;
} opt_config_t;

// Optimizer context
typedef struct opt_context_t opt_context_t;

// Optimizer API
opt_context_t *opt_create(opt_config_t *config);
void opt_free(opt_context_t *ctx);

// Run optimization passes
ast_robot_t *opt_optimize_robot(opt_context_t *ctx, ast_robot_t *robot);
ast_expr_t *opt_optimize_expr(opt_context_t *ctx, ast_expr_t *expr);
ast_stmt_t *opt_optimize_stmt(opt_context_t *ctx, ast_stmt_t *stmt);

// Individual optimization passes
ast_expr_t *opt_constant_fold(ast_expr_t *expr);
ast_stmt_t *opt_eliminate_dead_code(ast_stmt_t *stmt);
ast_expr_t *opt_eliminate_common_subexpr(ast_expr_t *expr);
ast_stmt_t *opt_unroll_loops(ast_stmt_t *stmt, uint32_t max_unroll);
ast_stmt_t *opt_inline_functions(ast_stmt_t *stmt, uint32_t max_size);

// Analysis passes
typedef struct {
    size_t total_nodes;
    size_t eliminated_nodes;
    size_t folded_constants;
    size_t inlined_functions;
    size_t unrolled_loops;
    float size_reduction_percent;
    float estimated_speedup;
} opt_stats_t;

void opt_get_stats(opt_context_t *ctx, opt_stats_t *stats);
void opt_print_stats(opt_stats_t *stats);

// Constant evaluation
typedef struct {
    enum {
        CONST_INT,
        CONST_FLOAT,
        CONST_BOOL,
        CONST_STRING,
    } type;
    union {
        int64_t int_val;
        double float_val;
        bool bool_val;
        char *string_val;
    } value;
} const_value_t;

bool opt_try_eval_const(ast_expr_t *expr, const_value_t *result);

// Data flow analysis
typedef struct {
    char **defined_vars;
    size_t defined_count;
    char **used_vars;
    size_t used_count;
    char **live_vars;
    size_t live_count;
} dataflow_info_t;

dataflow_info_t *opt_analyze_dataflow(ast_stmt_t *stmt);
void opt_free_dataflow_info(dataflow_info_t *info);

// Control flow graph
typedef struct cfg_node_t {
    ast_stmt_t *stmt;
    struct cfg_node_t **successors;
    size_t successor_count;
    struct cfg_node_t **predecessors;
    size_t predecessor_count;
} cfg_node_t;

typedef struct {
    cfg_node_t *entry;
    cfg_node_t *exit;
    cfg_node_t **nodes;
    size_t node_count;
} cfg_t;

cfg_t *opt_build_cfg(ast_stmt_t *stmt);
void opt_free_cfg(cfg_t *cfg);

// Loop detection
typedef struct {
    cfg_node_t *header;
    cfg_node_t **body;
    size_t body_count;
    bool is_reducible;
} loop_info_t;

loop_info_t *opt_detect_loops(cfg_t *cfg);

#endif // NEUROX_OPTIMIZER_H
