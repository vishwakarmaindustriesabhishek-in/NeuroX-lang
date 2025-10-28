#define _POSIX_C_SOURCE 200809L

#include "scheduler.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Platform-specific timing (Linux implementation)
#ifdef __linux__
#include <sys/time.h>
#include <unistd.h>

uint64_t nrx_time_now_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}

void nrx_delay_us(uint32_t us) {
    usleep(us);
}

void nrx_delay_ms(uint32_t ms) {
    usleep(ms * 1000);
}
#else
// Embedded platform stubs
uint64_t nrx_time_now_us(void) {
    // TODO: Implement for embedded platform
    return 0;
}

void nrx_delay_us(uint32_t us) {
    // TODO: Implement for embedded platform
}

void nrx_delay_ms(uint32_t ms) {
    // TODO: Implement for embedded platform
}
#endif

// Global scheduler state
static struct {
    nrx_scheduler_config_t config;
    nrx_task_t *task_lists[NRX_PRIORITY_COUNT];
    bool running;
    uint64_t start_time_us;
    nrx_scheduler_stats_t stats;
} g_scheduler;

void nrx_scheduler_init(nrx_scheduler_config_t *config) {
    memset(&g_scheduler, 0, sizeof(g_scheduler));
    
    if (config) {
        g_scheduler.config = *config;
    } else {
        // Default configuration
        g_scheduler.config.tick_rate_hz = 1000;
        g_scheduler.config.enable_stats = true;
        g_scheduler.config.enable_watchdog = false;
        g_scheduler.config.watchdog_timeout_ms = 1000;
    }
    
    for (int i = 0; i < NRX_PRIORITY_COUNT; i++) {
        g_scheduler.task_lists[i] = NULL;
    }
    
    g_scheduler.running = false;
    g_scheduler.start_time_us = nrx_time_now_us();
}

nrx_task_t *nrx_task_create(const char *name, nrx_task_fn_t function,
                            void *context, nrx_priority_t priority) {
    nrx_task_t *task = malloc(sizeof(nrx_task_t));
    if (!task) return NULL;
    
    memset(task, 0, sizeof(nrx_task_t));
    task->name = name;
    task->function = function;
    task->context = context;
    task->priority = priority;
    task->state = NRX_TASK_IDLE;
    task->next = NULL;
    
    return task;
}

void nrx_task_schedule_periodic(nrx_task_t *task, uint32_t frequency_hz) {
    if (!task || frequency_hz == 0) return;
    
    task->period_us = 1000000 / frequency_hz;
    task->next_run_us = nrx_time_now_us() + task->period_us;
    task->state = NRX_TASK_READY;
    
    // Add to priority queue
    nrx_priority_t prio = task->priority;
    if (g_scheduler.task_lists[prio] == NULL) {
        g_scheduler.task_lists[prio] = task;
    } else {
        // Insert at end
        nrx_task_t *current = g_scheduler.task_lists[prio];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = task;
    }
}

void nrx_task_suspend(nrx_task_t *task) {
    if (task) {
        task->state = NRX_TASK_SUSPENDED;
    }
}

void nrx_task_resume(nrx_task_t *task) {
    if (task) {
        task->state = NRX_TASK_READY;
    }
}

void nrx_task_delete(nrx_task_t *task) {
    if (!task) return;
    
    // Remove from task list
    for (int prio = 0; prio < NRX_PRIORITY_COUNT; prio++) {
        nrx_task_t **current = &g_scheduler.task_lists[prio];
        while (*current) {
            if (*current == task) {
                *current = task->next;
                break;
            }
            current = &(*current)->next;
        }
    }
    
    free(task);
}

static void nrx_scheduler_tick(void) {
    uint64_t now = nrx_time_now_us();
    
    // Iterate through priority levels
    for (int prio = 0; prio < NRX_PRIORITY_COUNT; prio++) {
        nrx_task_t *task = g_scheduler.task_lists[prio];
        
        while (task) {
            if (task->state == NRX_TASK_READY && now >= task->next_run_us) {
                // Calculate jitter
                int64_t jitter = (int64_t)(now - task->next_run_us);
                if (jitter < 0) jitter = -jitter;
                if ((uint32_t)jitter > task->worst_jitter_us) {
                    task->worst_jitter_us = (uint32_t)jitter;
                }
                
                // Execute task
                task->state = NRX_TASK_RUNNING;
                uint64_t start = nrx_time_now_us();
                
                task->function(task->context);
                
                uint64_t end = nrx_time_now_us();
                uint32_t exec_time = (uint32_t)(end - start);
                
                // Update statistics
                if (exec_time > task->worst_exec_us) {
                    task->worst_exec_us = exec_time;
                }
                
                // Update average (simple moving average)
                task->avg_exec_us = (task->avg_exec_us * task->exec_count + exec_time) 
                                   / (task->exec_count + 1);
                
                task->exec_count++;
                task->last_run_us = start;
                task->next_run_us += task->period_us;
                task->state = NRX_TASK_READY;
                
                g_scheduler.stats.tasks_executed++;
                
                // Check for missed deadline
                if (exec_time > task->period_us) {
                    g_scheduler.stats.missed_deadlines++;
                }
            }
            
            task = task->next;
        }
    }
}

void nrx_scheduler_start(void) {
    g_scheduler.running = true;
    g_scheduler.start_time_us = nrx_time_now_us();
    
    uint32_t tick_period_us = 1000000 / g_scheduler.config.tick_rate_hz;
    
    while (g_scheduler.running) {
        uint64_t tick_start = nrx_time_now_us();
        
        nrx_scheduler_tick();
        
        // Sleep until next tick
        uint64_t tick_end = nrx_time_now_us();
        uint32_t elapsed = (uint32_t)(tick_end - tick_start);
        
        if (elapsed < tick_period_us) {
            nrx_delay_us(tick_period_us - elapsed);
        }
    }
}

void nrx_scheduler_stop(void) {
    g_scheduler.running = false;
}

void nrx_scheduler_get_stats(nrx_scheduler_stats_t *stats) {
    if (stats) {
        *stats = g_scheduler.stats;
    }
}

void nrx_task_get_stats(nrx_task_t *task, nrx_scheduler_stats_t *stats) {
    if (!task || !stats) return;
    
    stats->tasks_scheduled = (uint32_t)task->exec_count;
    stats->tasks_executed = (uint32_t)task->exec_count;
    stats->max_jitter_us = task->worst_jitter_us;
    stats->cpu_usage_percent = 0; // TODO: Calculate
}
