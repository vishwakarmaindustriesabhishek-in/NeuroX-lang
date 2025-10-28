#ifndef NEUROX_STATEMACHINE_H
#define NEUROX_STATEMACHINE_H

#include "common.h"
#include "ast.h"

// State machine node types
typedef enum {
    SM_STATE,
    SM_TRANSITION,
    SM_GUARD,
    SM_ACTION,
} sm_node_type_t;

// State types
typedef enum {
    STATE_NORMAL,
    STATE_INITIAL,
    STATE_FINAL,
    STATE_COMPOSITE,    // Contains sub-states
    STATE_PARALLEL,     // Multiple active states
} sm_state_type_t;

// Transition trigger types
typedef enum {
    TRIGGER_EVENT,      // on event
    TRIGGER_TIMEOUT,    // after duration
    TRIGGER_CONDITION,  // when condition
    TRIGGER_ALWAYS,     // immediate
} sm_trigger_type_t;

// State definition
typedef struct sm_state_t {
    char *name;
    sm_state_type_t type;
    
    // Entry/exit actions
    ast_stmt_t *on_entry;
    ast_stmt_t *on_exit;
    ast_stmt_t *on_tick;
    
    // Sub-states (for composite/parallel)
    struct sm_state_t **substates;
    size_t substate_count;
    
    // Metadata
    int line;
    int column;
} sm_state_t;

// Transition definition
typedef struct sm_transition_t {
    sm_state_t *from_state;
    sm_state_t *to_state;
    
    sm_trigger_type_t trigger_type;
    char *event_name;           // for TRIGGER_EVENT
    ast_expr_t *timeout;        // for TRIGGER_TIMEOUT
    ast_expr_t *guard;          // Condition to check
    
    ast_stmt_t *action;         // Action to execute on transition
    
    int priority;               // Higher priority checked first
} sm_transition_t;

// State machine definition
typedef struct {
    char *name;
    sm_state_t **states;
    size_t state_count;
    
    sm_transition_t **transitions;
    size_t transition_count;
    
    sm_state_t *initial_state;
    
    // Variables local to state machine
    ast_param_t **variables;
    size_t variable_count;
} sm_machine_t;

// State machine AST integration
typedef struct {
    sm_machine_t *machine;
} ast_statemachine_decl_t;

// State machine API
sm_machine_t *sm_create(const char *name);
sm_state_t *sm_add_state(sm_machine_t *sm, const char *name, sm_state_type_t type);
sm_transition_t *sm_add_transition(sm_machine_t *sm, sm_state_t *from, sm_state_t *to);
void sm_set_initial_state(sm_machine_t *sm, sm_state_t *state);

void sm_free(sm_machine_t *sm);
void sm_print(sm_machine_t *sm);

// Code generation
void sm_generate_c(sm_machine_t *sm, FILE *out);

#endif // NEUROX_STATEMACHINE_H
