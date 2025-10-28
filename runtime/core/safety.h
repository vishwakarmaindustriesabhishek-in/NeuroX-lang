#ifndef NEUROX_SAFETY_H
#define NEUROX_SAFETY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Safety state
typedef enum {
    NRX_SAFETY_NORMAL,
    NRX_SAFETY_WARNING,
    NRX_SAFETY_FAULT,
    NRX_SAFETY_ESTOP,
} nrx_safety_state_t;

// Fault codes
typedef enum {
    NRX_FAULT_NONE = 0,
    NRX_FAULT_LIMIT_EXCEEDED,
    NRX_FAULT_WATCHDOG,
    NRX_FAULT_ESTOP,
    NRX_FAULT_SENSOR,
    NRX_FAULT_MOTOR,
    NRX_FAULT_COMMUNICATION,
    NRX_FAULT_POWER,
} nrx_fault_code_t;

// Limit types
typedef enum {
    NRX_LIMIT_SPEED,
    NRX_LIMIT_TURN_RATE,
    NRX_LIMIT_ACCELERATION,
    NRX_LIMIT_POWER,
} nrx_limit_type_t;

// Limit definition
typedef struct {
    nrx_limit_type_t type;
    float min_value;
    float max_value;
    bool enabled;
} nrx_limit_t;

// Safety configuration
typedef struct {
    nrx_limit_t *limits;
    size_t limit_count;
    
    bool enable_estop;
    bool enable_watchdog;
    uint32_t watchdog_timeout_ms;
    
    void (*fault_handler)(nrx_fault_code_t fault);
    void (*estop_handler)(void);
} nrx_safety_config_t;

// Safety API
void nrx_safety_init(nrx_safety_config_t *config);
void nrx_safety_update(void);

// State management
nrx_safety_state_t nrx_safety_get_state(void);
void nrx_safety_set_state(nrx_safety_state_t state);

// Fault handling
void nrx_safety_fault(nrx_fault_code_t fault, const char *message);
void nrx_safety_clear_fault(void);
nrx_fault_code_t nrx_safety_get_fault(void);

// Emergency stop
void nrx_safety_estop(void);
void nrx_safety_estop_reset(void);
bool nrx_safety_is_estopped(void);

// Limit checking
bool nrx_safety_check_limit(nrx_limit_type_t type, float value);
void nrx_safety_set_limit(nrx_limit_type_t type, float min_val, float max_val);

// Watchdog
void nrx_safety_watchdog_feed(void);
void nrx_safety_watchdog_enable(bool enable);

#endif // NEUROX_SAFETY_H
