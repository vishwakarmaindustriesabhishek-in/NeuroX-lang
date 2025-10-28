#ifndef NEUROX_SCHEDULER_H
#define NEUROX_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Task priority levels
typedef enum {
    NRX_PRIORITY_HIGH = 0,
    NRX_PRIORITY_MEDIUM = 1,
    NRX_PRIORITY_LOW = 2,
    NRX_PRIORITY_COUNT = 3,
} nrx_priority_t;

// Task state
typedef enum {
    NRX_TASK_IDLE,
    NRX_TASK_READY,
    NRX_TASK_RUNNING,
    NRX_TASK_WAITING,
    NRX_TASK_SUSPENDED,
} nrx_task_state_t;

// Task function signature
typedef void (*nrx_task_fn_t)(void *context);

// Task control block
typedef struct nrx_task_t {
    const char *name;
    nrx_task_fn_t function;
    void *context;
    
    nrx_priority_t priority;
    nrx_task_state_t state;
    
    // Periodic scheduling
    uint32_t period_us;        // Period in microseconds
    uint64_t next_run_us;      // Next scheduled run time
    uint64_t last_run_us;      // Last execution time
    
    // Statistics
    uint64_t exec_count;       // Number of executions
    uint32_t worst_jitter_us;  // Worst jitter observed
    uint32_t worst_exec_us;    // Worst execution time
    uint32_t avg_exec_us;      // Average execution time
    
    // Linked list
    struct nrx_task_t *next;
} nrx_task_t;

// Scheduler configuration
typedef struct {
    uint32_t tick_rate_hz;     // Scheduler tick rate
    bool enable_stats;         // Enable statistics collection
    bool enable_watchdog;      // Enable watchdog
    uint32_t watchdog_timeout_ms;
} nrx_scheduler_config_t;

// Scheduler API
void nrx_scheduler_init(nrx_scheduler_config_t *config);
void nrx_scheduler_start(void);
void nrx_scheduler_stop(void);

// Task management
nrx_task_t *nrx_task_create(const char *name, nrx_task_fn_t function, 
                            void *context, nrx_priority_t priority);
void nrx_task_schedule_periodic(nrx_task_t *task, uint32_t frequency_hz);
void nrx_task_suspend(nrx_task_t *task);
void nrx_task_resume(nrx_task_t *task);
void nrx_task_delete(nrx_task_t *task);

// Timing utilities
uint64_t nrx_time_now_us(void);
void nrx_delay_us(uint32_t us);
void nrx_delay_ms(uint32_t ms);

// Statistics
typedef struct {
    uint32_t tasks_scheduled;
    uint32_t tasks_executed;
    uint32_t missed_deadlines;
    uint32_t max_jitter_us;
    uint32_t cpu_usage_percent;
} nrx_scheduler_stats_t;

void nrx_scheduler_get_stats(nrx_scheduler_stats_t *stats);
void nrx_task_get_stats(nrx_task_t *task, nrx_scheduler_stats_t *stats);

#endif // NEUROX_SCHEDULER_H
