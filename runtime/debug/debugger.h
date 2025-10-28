#ifndef NEUROX_DEBUGGER_H
#define NEUROX_DEBUGGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Debugger configuration
typedef struct {
    uint16_t port;              // Debug server port
    bool enable_breakpoints;
    bool enable_watchpoints;
    bool enable_profiling;
    bool enable_tracing;
} nrx_debug_config_t;

// Debugger handle
typedef struct nrx_debugger_t nrx_debugger_t;

// Debugger initialization
nrx_debugger_t *nrx_debugger_init(nrx_debug_config_t *config);
void nrx_debugger_deinit(nrx_debugger_t *dbg);
void nrx_debugger_start(nrx_debugger_t *dbg);
void nrx_debugger_stop(nrx_debugger_t *dbg);

// Breakpoints
typedef struct {
    uint32_t id;
    const char *file;
    uint32_t line;
    const char *condition;      // Optional condition expression
    bool enabled;
    uint32_t hit_count;
} nrx_breakpoint_t;

uint32_t nrx_debugger_add_breakpoint(nrx_debugger_t *dbg, const char *file, uint32_t line);
int nrx_debugger_remove_breakpoint(nrx_debugger_t *dbg, uint32_t id);
int nrx_debugger_enable_breakpoint(nrx_debugger_t *dbg, uint32_t id, bool enable);
int nrx_debugger_set_condition(nrx_debugger_t *dbg, uint32_t id, const char *condition);

// Watchpoints (data breakpoints)
typedef enum {
    NRX_WATCH_READ,
    NRX_WATCH_WRITE,
    NRX_WATCH_READ_WRITE,
} nrx_watch_type_t;

typedef struct {
    uint32_t id;
    const char *variable;
    void *address;
    size_t size;
    nrx_watch_type_t type;
    bool enabled;
} nrx_watchpoint_t;

uint32_t nrx_debugger_add_watchpoint(nrx_debugger_t *dbg, const char *variable, 
                                     void *address, size_t size, nrx_watch_type_t type);
int nrx_debugger_remove_watchpoint(nrx_debugger_t *dbg, uint32_t id);

// Execution control
void nrx_debugger_pause(nrx_debugger_t *dbg);
void nrx_debugger_continue(nrx_debugger_t *dbg);
void nrx_debugger_step_over(nrx_debugger_t *dbg);
void nrx_debugger_step_into(nrx_debugger_t *dbg);
void nrx_debugger_step_out(nrx_debugger_t *dbg);

// Stack trace
typedef struct {
    const char *function;
    const char *file;
    uint32_t line;
    void *frame_pointer;
} nrx_stack_frame_t;

int nrx_debugger_get_stacktrace(nrx_debugger_t *dbg, nrx_stack_frame_t *frames, size_t max_frames);

// Variable inspection
typedef enum {
    NRX_VAR_INT,
    NRX_VAR_FLOAT,
    NRX_VAR_STRING,
    NRX_VAR_POINTER,
    NRX_VAR_STRUCT,
    NRX_VAR_ARRAY,
} nrx_var_type_t;

typedef struct {
    const char *name;
    nrx_var_type_t type;
    void *address;
    size_t size;
    char value_str[256];
} nrx_variable_t;

int nrx_debugger_get_locals(nrx_debugger_t *dbg, nrx_variable_t *vars, size_t max_vars);
int nrx_debugger_get_globals(nrx_debugger_t *dbg, nrx_variable_t *vars, size_t max_vars);
int nrx_debugger_eval_expression(nrx_debugger_t *dbg, const char *expr, char *result, size_t result_len);

// Memory inspection
int nrx_debugger_read_memory(nrx_debugger_t *dbg, void *address, uint8_t *buffer, size_t len);
int nrx_debugger_write_memory(nrx_debugger_t *dbg, void *address, const uint8_t *buffer, size_t len);

// Profiling
typedef struct {
    const char *function;
    uint64_t call_count;
    uint64_t total_time_us;
    uint64_t min_time_us;
    uint64_t max_time_us;
    uint64_t avg_time_us;
} nrx_profile_entry_t;

int nrx_debugger_get_profile(nrx_debugger_t *dbg, nrx_profile_entry_t *entries, size_t max_entries);
void nrx_debugger_reset_profile(nrx_debugger_t *dbg);

// Tracing
typedef enum {
    NRX_TRACE_FUNCTION_ENTRY,
    NRX_TRACE_FUNCTION_EXIT,
    NRX_TRACE_TASK_SWITCH,
    NRX_TRACE_INTERRUPT,
    NRX_TRACE_CUSTOM,
} nrx_trace_type_t;

typedef struct {
    nrx_trace_type_t type;
    uint64_t timestamp_us;
    const char *name;
    uint32_t task_id;
    char details[128];
} nrx_trace_event_t;

void nrx_debugger_enable_tracing(nrx_debugger_t *dbg, bool enable);
int nrx_debugger_get_trace(nrx_debugger_t *dbg, nrx_trace_event_t *events, size_t max_events);
void nrx_debugger_clear_trace(nrx_debugger_t *dbg);

// Custom trace points
void nrx_trace(const char *name, const char *details);

// Performance counters
typedef struct {
    uint64_t cpu_cycles;
    uint64_t instructions;
    uint64_t cache_misses;
    uint64_t branch_misses;
    float cpu_usage_percent;
} nrx_perf_counters_t;

int nrx_debugger_get_perf_counters(nrx_debugger_t *dbg, nrx_perf_counters_t *counters);

// Core dumps
int nrx_debugger_generate_coredump(nrx_debugger_t *dbg, const char *path);

// Remote debugging (GDB protocol)
int nrx_debugger_start_gdb_server(nrx_debugger_t *dbg, uint16_t port);

// Debug logging
typedef enum {
    NRX_LOG_TRACE,
    NRX_LOG_DEBUG,
    NRX_LOG_INFO,
    NRX_LOG_WARN,
    NRX_LOG_ERROR,
    NRX_LOG_FATAL,
} nrx_log_level_t;

void nrx_log(nrx_log_level_t level, const char *file, int line, const char *fmt, ...);

#define NRX_LOG_TRACE(...) nrx_log(NRX_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define NRX_LOG_DEBUG(...) nrx_log(NRX_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define NRX_LOG_INFO(...)  nrx_log(NRX_LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define NRX_LOG_WARN(...)  nrx_log(NRX_LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define NRX_LOG_ERROR(...) nrx_log(NRX_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define NRX_LOG_FATAL(...) nrx_log(NRX_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

// Assertions
#define NRX_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            nrx_log(NRX_LOG_FATAL, __FILE__, __LINE__, "Assertion failed: %s", msg); \
            __builtin_trap(); \
        } \
    } while(0)

#endif // NEUROX_DEBUGGER_H
