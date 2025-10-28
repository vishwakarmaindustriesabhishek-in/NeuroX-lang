#include "safety.h"
#include "scheduler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global safety state
static struct {
    nrx_safety_config_t config;
    nrx_safety_state_t state;
    nrx_fault_code_t current_fault;
    uint64_t last_watchdog_feed_us;
    bool initialized;
} g_safety;

void nrx_safety_init(nrx_safety_config_t *config) {
    memset(&g_safety, 0, sizeof(g_safety));
    
    if (config) {
        g_safety.config = *config;
    } else {
        // Default configuration
        g_safety.config.limits = NULL;
        g_safety.config.limit_count = 0;
        g_safety.config.enable_estop = true;
        g_safety.config.enable_watchdog = false;
        g_safety.config.watchdog_timeout_ms = 1000;
        g_safety.config.fault_handler = NULL;
        g_safety.config.estop_handler = NULL;
    }
    
    g_safety.state = NRX_SAFETY_NORMAL;
    g_safety.current_fault = NRX_FAULT_NONE;
    g_safety.last_watchdog_feed_us = nrx_time_now_us();
    g_safety.initialized = true;
}

void nrx_safety_update(void) {
    if (!g_safety.initialized) return;
    
    // Check watchdog
    if (g_safety.config.enable_watchdog) {
        uint64_t now = nrx_time_now_us();
        uint64_t elapsed_ms = (now - g_safety.last_watchdog_feed_us) / 1000;
        
        if (elapsed_ms > g_safety.config.watchdog_timeout_ms) {
            nrx_safety_fault(NRX_FAULT_WATCHDOG, "Watchdog timeout");
        }
    }
}

nrx_safety_state_t nrx_safety_get_state(void) {
    return g_safety.state;
}

void nrx_safety_set_state(nrx_safety_state_t state) {
    g_safety.state = state;
}

void nrx_safety_fault(nrx_fault_code_t fault, const char *message) {
    g_safety.current_fault = fault;
    g_safety.state = NRX_SAFETY_FAULT;
    
    fprintf(stderr, "[SAFETY FAULT] Code %d: %s\n", fault, message ? message : "Unknown");
    
    if (g_safety.config.fault_handler) {
        g_safety.config.fault_handler(fault);
    }
    
    // Critical faults trigger estop
    if (fault == NRX_FAULT_ESTOP || fault == NRX_FAULT_WATCHDOG) {
        nrx_safety_estop();
    }
}

void nrx_safety_clear_fault(void) {
    if (g_safety.state != NRX_SAFETY_ESTOP) {
        g_safety.current_fault = NRX_FAULT_NONE;
        g_safety.state = NRX_SAFETY_NORMAL;
    }
}

nrx_fault_code_t nrx_safety_get_fault(void) {
    return g_safety.current_fault;
}

void nrx_safety_estop(void) {
    g_safety.state = NRX_SAFETY_ESTOP;
    g_safety.current_fault = NRX_FAULT_ESTOP;
    
    fprintf(stderr, "[EMERGENCY STOP] System halted\n");
    
    if (g_safety.config.estop_handler) {
        g_safety.config.estop_handler();
    }
    
    // TODO: Stop all motors, disable actuators
}

void nrx_safety_estop_reset(void) {
    if (g_safety.state == NRX_SAFETY_ESTOP) {
        g_safety.state = NRX_SAFETY_NORMAL;
        g_safety.current_fault = NRX_FAULT_NONE;
        fprintf(stderr, "[SAFETY] E-stop reset\n");
    }
}

bool nrx_safety_is_estopped(void) {
    return g_safety.state == NRX_SAFETY_ESTOP;
}

bool nrx_safety_check_limit(nrx_limit_type_t type, float value) {
    for (size_t i = 0; i < g_safety.config.limit_count; i++) {
        nrx_limit_t *limit = &g_safety.config.limits[i];
        
        if (limit->type == type && limit->enabled) {
            if (value < limit->min_value || value > limit->max_value) {
                char msg[128];
                snprintf(msg, sizeof(msg), "Limit exceeded: type=%d, value=%f, range=[%f, %f]",
                        type, value, limit->min_value, limit->max_value);
                nrx_safety_fault(NRX_FAULT_LIMIT_EXCEEDED, msg);
                return false;
            }
        }
    }
    
    return true;
}

void nrx_safety_set_limit(nrx_limit_type_t type, float min_val, float max_val) {
    // Find existing limit or add new one
    for (size_t i = 0; i < g_safety.config.limit_count; i++) {
        if (g_safety.config.limits[i].type == type) {
            g_safety.config.limits[i].min_value = min_val;
            g_safety.config.limits[i].max_value = max_val;
            g_safety.config.limits[i].enabled = true;
            return;
        }
    }
    
    // Add new limit
    size_t new_count = g_safety.config.limit_count + 1;
    g_safety.config.limits = realloc(g_safety.config.limits, 
                                     new_count * sizeof(nrx_limit_t));
    
    g_safety.config.limits[g_safety.config.limit_count].type = type;
    g_safety.config.limits[g_safety.config.limit_count].min_value = min_val;
    g_safety.config.limits[g_safety.config.limit_count].max_value = max_val;
    g_safety.config.limits[g_safety.config.limit_count].enabled = true;
    
    g_safety.config.limit_count = new_count;
}

void nrx_safety_watchdog_feed(void) {
    g_safety.last_watchdog_feed_us = nrx_time_now_us();
}

void nrx_safety_watchdog_enable(bool enable) {
    g_safety.config.enable_watchdog = enable;
    if (enable) {
        nrx_safety_watchdog_feed();
    }
}
