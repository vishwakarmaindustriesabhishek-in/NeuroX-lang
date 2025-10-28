#ifndef NEUROX_AST_H
#define NEUROX_AST_H

#include "common.h"
#include "lexer.h"

// Forward declarations
typedef struct ast_expr_t ast_expr_t;
typedef struct ast_stmt_t ast_stmt_t;
typedef struct ast_decl_t ast_decl_t;

// Expression types
typedef enum {
    EXPR_LITERAL,
    EXPR_IDENTIFIER,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL,
    EXPR_MEMBER,
    EXPR_UNIT,
} ast_expr_type_t;

// Statement types
typedef enum {
    STMT_EXPR,
    STMT_ASSIGN,
    STMT_IF,
    STMT_BLOCK,
    STMT_WAIT,
    STMT_RETURN,
} ast_stmt_type_t;

// Declaration types
typedef enum {
    DECL_ROBOT,
    DECL_MOTOR,
    DECL_SERVO,
    DECL_SENSOR,
    DECL_GPIO,
    DECL_BUS,
    DECL_NET,
    DECL_TOPIC,
    DECL_LIMITS,
    DECL_TASK,
    DECL_SCHEDULE,
    DECL_EVENT,
} ast_decl_type_t;

// Literal value
typedef struct {
    enum {
        LITERAL_NUMBER,
        LITERAL_STRING,
        LITERAL_BOOL,
    } type;
    union {
        double number;
        char *string;
        bool boolean;
    } value;
} ast_literal_t;

// Binary operator
typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_LTE,
    OP_GT,
    OP_GTE,
    OP_AND,
    OP_OR,
} ast_binary_op_t;

// Unary operator
typedef enum {
    OP_NEG,
    OP_NOT,
} ast_unary_op_t;

// Unit type
typedef enum {
    UNIT_PERCENT,
    UNIT_MS,
    UNIT_CM,
    UNIT_DEG,
    UNIT_HZ,
    UNIT_DEG_PER_SEC,
} ast_unit_type_t;

// Expression nodes
typedef struct {
    ast_expr_t *left;
    ast_binary_op_t op;
    ast_expr_t *right;
} ast_binary_expr_t;

typedef struct {
    ast_unary_op_t op;
    ast_expr_t *operand;
} ast_unary_expr_t;

typedef struct {
    ast_expr_t *callee;
    ast_expr_t **args;
    size_t arg_count;
} ast_call_expr_t;

typedef struct {
    ast_expr_t *object;
    char *member;
} ast_member_expr_t;

typedef struct {
    ast_expr_t *value;
    ast_unit_type_t unit;
} ast_unit_expr_t;

struct ast_expr_t {
    ast_expr_type_t type;
    union {
        ast_literal_t literal;
        char *identifier;
        ast_binary_expr_t binary;
        ast_unary_expr_t unary;
        ast_call_expr_t call;
        ast_member_expr_t member;
        ast_unit_expr_t unit;
    } as;
    int line;
    int column;
};

// Statement nodes
typedef struct {
    char *target;
    ast_expr_t *value;
} ast_assign_stmt_t;

typedef struct {
    ast_expr_t *condition;
    ast_stmt_t *then_branch;
    ast_stmt_t *else_branch;
} ast_if_stmt_t;

typedef struct {
    ast_stmt_t **statements;
    size_t count;
} ast_block_stmt_t;

typedef struct {
    ast_expr_t *duration;
} ast_wait_stmt_t;

struct ast_stmt_t {
    ast_stmt_type_t type;
    union {
        ast_expr_t *expr;
        ast_assign_stmt_t assign;
        ast_if_stmt_t if_stmt;
        ast_block_stmt_t block;
        ast_wait_stmt_t wait;
        ast_expr_t *return_value;
    } as;
    int line;
    int column;
};

// Type annotation
typedef struct {
    char *name;
    ast_unit_type_t unit;
} ast_type_t;

// Parameter
typedef struct {
    char *name;
    ast_type_t *type;
} ast_param_t;

// Hardware declarations
typedef struct {
    char *name;
    char *pin;
} ast_motor_decl_t;

typedef struct {
    char *name;
    char *pin;
} ast_servo_decl_t;

typedef struct {
    char *name;
    char *pin;
    char *sensor_type;
} ast_sensor_decl_t;

typedef struct {
    char *name;
    char *pin;
    char *mode;
} ast_gpio_decl_t;

typedef struct {
    char *name;
    char *bus_type;
    int address;
} ast_bus_decl_t;

// Network declarations
typedef struct {
    char *broker;
    char *client_id;
    bool use_tls;
} ast_net_decl_t;

typedef struct {
    char *name;
    char *path;
} ast_topic_decl_t;

// Limit entry
typedef struct {
    char *name;
    ast_expr_t *value;
    bool is_max;
} ast_limit_entry_t;

typedef struct {
    ast_limit_entry_t **entries;
    size_t count;
} ast_limits_decl_t;

// Task declaration
typedef struct {
    char *name;
    ast_param_t **params;
    size_t param_count;
    ast_stmt_t *body;
} ast_task_decl_t;

// Schedule declaration
typedef enum {
    PRIORITY_HIGH,
    PRIORITY_MEDIUM,
    PRIORITY_LOW,
} ast_priority_t;

typedef struct {
    char *name;
    ast_expr_t *frequency;
    ast_priority_t priority;
    ast_stmt_t *body;
} ast_schedule_decl_t;

// Event declaration
typedef enum {
    EVENT_MESSAGE,
    EVENT_GPIO,
} ast_event_type_t;

typedef struct {
    ast_event_type_t type;
    char *source;
    char *var_name;
    ast_stmt_t *handler;
} ast_event_decl_t;

// Declaration node
struct ast_decl_t {
    ast_decl_type_t type;
    union {
        ast_motor_decl_t motor;
        ast_servo_decl_t servo;
        ast_sensor_decl_t sensor;
        ast_gpio_decl_t gpio;
        ast_bus_decl_t bus;
        ast_net_decl_t net;
        ast_topic_decl_t topic;
        ast_limits_decl_t limits;
        ast_task_decl_t task;
        ast_schedule_decl_t schedule;
        ast_event_decl_t event;
    } as;
    int line;
    int column;
};

// Robot (top-level)
typedef struct {
    char *name;
    ast_decl_t **declarations;
    size_t decl_count;
} ast_robot_t;

// AST utilities
ast_expr_t *ast_expr_create(ast_expr_type_t type);
ast_stmt_t *ast_stmt_create(ast_stmt_type_t type);
ast_decl_t *ast_decl_create(ast_decl_type_t type);
ast_robot_t *ast_robot_create(const char *name);

void ast_expr_free(ast_expr_t *expr);
void ast_stmt_free(ast_stmt_t *stmt);
void ast_decl_free(ast_decl_t *decl);
void ast_robot_free(ast_robot_t *robot);

// AST printing (for debugging)
void ast_expr_print(ast_expr_t *expr, int indent);
void ast_stmt_print(ast_stmt_t *stmt, int indent);
void ast_decl_print(ast_decl_t *decl, int indent);
void ast_robot_print(ast_robot_t *robot);

#endif // NEUROX_AST_H
