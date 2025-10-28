# NeuroX-lang Architecture

Complete technical architecture documentation.

## Overview

NeuroX-lang is a compiled DSL that translates `.neuro` source files into C code, which is then compiled with a real-time runtime library for robotics and IoT applications.

```
.neuro source → Compiler → C code → GCC → Executable + Runtime
```

## Compiler Pipeline

### 1. Lexer (`compiler/lexer.c`)

**Input**: Raw `.neuro` source text  
**Output**: Token stream

- Tokenizes keywords, identifiers, numbers, strings, operators
- Handles comments (`//` and `/* */`)
- Tracks line/column for error reporting
- 100+ token types for complete language coverage

**Key Functions**:
- `lexer_init()` - Initialize lexer with source
- `lexer_next_token()` - Get next token
- `token_equals()` - Compare token text

### 2. Parser (`compiler/parser.c`)

**Input**: Token stream  
**Output**: Abstract Syntax Tree (AST)

- Recursive descent parser
- Precedence climbing for expressions
- Error recovery with panic mode
- Validates syntax structure

**Grammar**:
```
robot       → "robot" IDENTIFIER "{" declaration* "}"
declaration → motor_decl | sensor_decl | task_decl | schedule_decl | ...
task_decl   → "task" IDENTIFIER "(" params ")" "{" statement* "}"
schedule    → "schedule" IDENTIFIER "@" frequency "priority" level "{" statement* "}"
statement   → expr_stmt | assign_stmt | if_stmt | wait_stmt | block
expression  → equality | comparison | term | factor | unary | primary
```

**Key Functions**:
- `parser_init()` - Initialize parser
- `parser_parse()` - Parse robot definition
- `parse_declaration()` - Parse top-level declarations
- `parse_statement()` - Parse statements
- `parse_expression()` - Parse expressions

### 3. AST (`compiler/ast.c`)

**Data Structures**:

```c
ast_robot_t       // Top-level robot definition
ast_decl_t        // Declarations (motor, task, schedule, etc.)
ast_stmt_t        // Statements (assign, if, wait, block)
ast_expr_t        // Expressions (binary, unary, call, literal)
```

**Node Types**:
- **Declarations**: MOTOR, SENSOR, GPIO, TASK, SCHEDULE, EVENT, LIMITS
- **Statements**: ASSIGN, IF, BLOCK, WAIT, RETURN
- **Expressions**: LITERAL, IDENTIFIER, BINARY, UNARY, CALL, MEMBER

**Memory Management**:
- `ast_*_create()` - Allocate nodes
- `ast_*_free()` - Recursive cleanup
- All strings are heap-allocated with `strdup()`

### 4. Type Checker (TODO)

**Planned Features**:
- Unit checking (cm, ms, deg, %, Hz)
- Type inference
- Hardware resource validation
- Limit range checking

### 5. IR (TODO)

**Planned Features**:
- Simplified intermediate representation
- Control flow graph
- Optimization passes
- Platform-independent representation

### 6. Code Generator (`tools/neuroxc.c`)

**Input**: AST  
**Output**: C source code

**Current Implementation**:
- Basic skeleton generation
- Runtime initialization
- Includes for runtime headers

**TODO**:
- Full task/schedule translation
- Hardware initialization code
- Event handler registration
- MQTT setup code
- Safety limit enforcement

## Runtime Architecture

### Core Components

#### Scheduler (`runtime/core/scheduler.c`)

**Purpose**: Real-time task scheduling with priorities

**Features**:
- Periodic task execution
- 3 priority levels (HIGH, MEDIUM, LOW)
- Jitter tracking
- Execution time statistics
- Missed deadline detection

**Data Structures**:
```c
nrx_task_t {
  name, function, context
  priority, state
  period_us, next_run_us
  exec_count, worst_jitter_us, worst_exec_us
}
```

**API**:
- `nrx_scheduler_init()` - Initialize scheduler
- `nrx_task_create()` - Create task
- `nrx_task_schedule_periodic()` - Schedule at frequency
- `nrx_scheduler_start()` - Run scheduler loop
- `nrx_scheduler_get_stats()` - Get statistics

**Scheduling Algorithm**:
1. Iterate priority levels (HIGH → MEDIUM → LOW)
2. For each task, check if `now >= next_run_us`
3. Execute task function
4. Track jitter and execution time
5. Update `next_run_us += period_us`
6. Sleep until next tick

#### Safety (`runtime/core/safety.c`)

**Purpose**: Safety monitoring and fault handling

**Features**:
- Limit enforcement (speed, turn rate, etc.)
- Emergency stop (E-stop)
- Watchdog timer
- Fault logging and recovery

**States**:
- `NORMAL` - Operating normally
- `WARNING` - Approaching limits
- `FAULT` - Fault detected, degraded mode
- `ESTOP` - Emergency stop, all actuators disabled

**Fault Codes**:
- `LIMIT_EXCEEDED` - Safety limit violated
- `WATCHDOG` - Watchdog timeout
- `ESTOP` - E-stop triggered
- `SENSOR` - Sensor failure
- `MOTOR` - Motor fault
- `COMMUNICATION` - Network/comms fault

**API**:
- `nrx_safety_init()` - Initialize safety system
- `nrx_safety_check_limit()` - Validate value against limits
- `nrx_safety_estop()` - Trigger emergency stop
- `nrx_safety_watchdog_feed()` - Reset watchdog

### HAL (Hardware Abstraction Layer)

**Purpose**: Platform-independent hardware interface

**Supported Platforms**:
- Linux (mock implementation for testing)
- ESP32 (TODO)
- STM32 (TODO)
- RP2040 (TODO)

**Peripherals**:

**GPIO**:
```c
nrx_gpio_init(pin, mode)
nrx_gpio_write(pin, state)
nrx_gpio_read(pin)
```

**PWM** (for motors, servos):
```c
nrx_pwm_init(pin, frequency_hz)
nrx_pwm_set_duty(pin, duty_percent)
```

**ADC** (for analog sensors):
```c
nrx_adc_init(pin)
nrx_adc_read(pin)
```

**UART/I2C/SPI**:
```c
nrx_uart_init(port, baud_rate)
nrx_uart_write(uart, data, len)
nrx_i2c_init(port, frequency_hz)
nrx_spi_init(port, frequency_hz)
```

**Motor Control**:
```c
nrx_motor_t motor;
nrx_motor_init(&motor, pwm_pin, dir1_pin, dir2_pin)
nrx_motor_set_power(&motor, power_percent)
nrx_motor_stop(&motor)
```

**Servo Control**:
```c
nrx_servo_t servo;
nrx_servo_init(&servo, pin)
nrx_servo_set_angle(&servo, angle_deg)
```

### Network/IoT (`runtime/net/mqtt.c`)

**Purpose**: MQTT connectivity for IoT integration

**Features**:
- TLS/SSL support
- QoS levels (0, 1, 2)
- Publish/subscribe
- Connection management
- Statistics tracking

**API**:
```c
nrx_mqtt_client_t *client = nrx_mqtt_create(&config);
nrx_mqtt_connect(client);
nrx_mqtt_subscribe(client, topic, qos);
nrx_mqtt_publish(client, topic, payload, len, qos);
nrx_mqtt_loop(client);  // Process messages
```

**Configuration**:
```c
nrx_mqtt_config_t {
  broker_url, client_id
  username, password
  use_tls, ca_cert_path, client_cert_path
  message_callback, user_data
}
```

**Current Implementation**:
- Mock implementation for testing
- Production: integrate Paho MQTT or Mosquitto

## Build System

### Makefile Targets

- `make all` - Build compiler + runtime
- `make compiler` - Build neuroxc
- `make runtime` - Build runtime library
- `make test` - Run tests
- `make examples` - Compile example robots
- `make clean` - Remove build artifacts

### Directory Structure

```
build/
├── obj/           # Object files
│   ├── compiler/
│   ├── runtime/
│   └── tools/
├── bin/           # Executables + libraries
│   ├── neuroxc
│   └── libneurox_runtime.a
└── gen/           # Generated C code
```

## Compilation Flow

### Step 1: Parse .neuro file
```bash
./build/bin/neuroxc parse robot.neuro
```

### Step 2: Generate C code
```bash
./build/bin/neuroxc emit-c robot.neuro -o build/gen/robot.c
```

### Step 3: Compile with runtime
```bash
gcc -o build/bin/robot build/gen/robot.c \
    build/bin/libneurox_runtime.a -lm -lpthread
```

### Step 4: Run
```bash
./build/bin/robot
```

## Extension Points

### Adding a New Platform

1. Create `runtime/hal/hal_<platform>.c`
2. Implement all HAL functions
3. Add platform detection in `hal.h`
4. Update Makefile with platform-specific flags

### Adding a New Hardware Type

1. Add token to `lexer.h` (e.g., `TOKEN_STEPPER`)
2. Add AST node to `ast.h` (e.g., `ast_stepper_decl_t`)
3. Add parser rule in `parser.c`
4. Add HAL interface in `hal.h`
5. Implement in platform HAL files

### Adding a New Language Feature

1. Add tokens in `lexer.c`
2. Add AST nodes in `ast.h`
3. Add parser rules in `parser.c`
4. Add type checking (when implemented)
5. Add code generation
6. Add runtime support if needed

## Performance Considerations

### Scheduler
- Tick rate: 1000 Hz default (1ms resolution)
- Jitter: Typically <100μs on Linux RT
- Overhead: ~50μs per task per tick

### Memory
- AST: ~100 bytes per node
- Runtime: ~1KB per task
- MQTT: ~4KB buffer per client

### Timing Guarantees
- HIGH priority: <1ms jitter
- MEDIUM priority: <10ms jitter
- LOW priority: Best effort

## Security Model

### Firmware Signing (TODO)
- Ed25519 signatures
- Dual-bank OTA updates
- Rollback protection

### Network Security
- TLS 1.2+ for MQTT
- X.509 certificate validation
- Secure credential storage

### Access Control (TODO)
- Capability-based permissions
- Topic-level ACLs
- Audit logging

## Testing Strategy

### Unit Tests
- Lexer: Token recognition
- Parser: AST construction
- Runtime: Scheduler, safety

### Integration Tests
- End-to-end compilation
- Mock hardware simulation
- Network message flow

### Timing Tests
- Jitter measurement
- Deadline validation
- Worst-case execution time

### Hardware-in-Loop (TODO)
- Real board testing
- Sensor/actuator validation
- Network reliability

## Future Enhancements

### Compiler
- [ ] Full type checker with unit inference
- [ ] Optimization passes (dead code, constant folding)
- [ ] Better error messages with suggestions
- [ ] Language server protocol (LSP)

### Runtime
- [ ] Multi-core support
- [ ] DMA for high-speed I/O
- [ ] Sensor fusion library
- [ ] Multi-robot coordination

### Tooling
- [ ] Debugger integration
- [ ] Profiler/tracer
- [ ] Web-based dashboard
- [ ] OTA update manager

### Language
- [ ] Generics/templates
- [ ] State machines
- [ ] Behavior trees
- [ ] Vision/ML primitives
