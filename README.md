# NeuroX-lang

**The Most Powerful Robotics DSL Ever Created**

A production-grade domain-specific language for robotics, IoT, and autonomous systems with real-time determinism, safety-first design, ML/vision integration, swarm coordination, and enterprise security.

## Vision

**Purpose**: Production-grade DSL for robotics + IoT + hardware control  
**Philosophy**:
- Real-time determinism (predictable scheduling, bounded jitter)
- Safety-first (limits, estop, watchdogs, fault recovery)
- IoT-native (MQTT, OTA, telemetry, edge integration)
- Portable runtime (Linux RT, FreeRTOS, STM32, ESP32, RP2040)
- Security-first (TLS, signed firmware, ACLs)

## Core Features

### Hardware & Real-Time
- **Hardware bindings**: Motors, servos, sensors, GPIO, cameras, buses (I2C/SPI/CAN/UART)
- **Real-time scheduling**: Priority-based with <100μs jitter
- **Safety system**: Limits, E-stop, watchdog, fault recovery

### Advanced AI & Autonomy
- **State Machines**: Hierarchical FSM with guards and actions
- **Behavior Trees**: Reactive AI for complex decision-making
- **Computer Vision**: Object detection, tracking, feature extraction
- **Machine Learning**: TensorFlow Lite integration, on-device inference
- **SLAM**: Simultaneous localization and mapping

### Multi-Robot Systems
- **Swarm Coordination**: Formation control, task allocation
- **Consensus Algorithms**: Distributed decision-making
- **Flocking Behavior**: Separation, alignment, cohesion
- **Coverage Control**: Area exploration and monitoring

### Security & Production
- **Encryption**: AES, ChaCha20, TLS/SSL
- **Firmware Signing**: Cryptographic verification
- **OTA Updates**: Dual-bank with rollback
- **Access Control**: Role-based permissions
- **Audit Logging**: Tamper-proof event trails

### Development Tools
- **Debugger**: Breakpoints, watchpoints, profiling
- **Optimizer**: Dead code elimination, constant folding, inlining
- **Monitoring**: Real-time telemetry and dashboards
- **Testing**: Unit tests, integration tests, hardware-in-loop

### Language Interoperability
- **Python**: Native C API, ctypes, CFFI bindings
- **C/C++**: Direct compilation target
- **Rust**: FFI bindings via crates.io
- **JavaScript/TypeScript**: Node.js addon, WebAssembly
- **Go**: cgo bindings
- **Java/Kotlin**: JNI bindings
- **C#/.NET**: P/Invoke bindings
- **Julia**: ccall interface
- **MATLAB**: MEX functions
- **R**: Rcpp bindings

## Quick Example

```neuro
robot ExampleBot {
  motor left on M1
  motor right on M2
  sensor dist on UART0 type Distance
  
  limits {
    speed max 60%
    turn_rate max 90deg/s
  }
  
  task move(speed: Percent, t: ms) {
    left.power = speed
    right.power = speed
    wait(t)
    stop()
  }
  
  schedule control @ 500Hz priority HIGH {
    if dist.value < 25cm { stop() }
  }
}
```

## Project Structure

```
neurox-lang/
 ├─ compiler/          # Parser, AST, IR, codegen
 │   ├─ lexer.c/h      # Tokenizer
 │   ├─ parser.c/h     # Recursive descent parser
 │   ├─ ast.c/h        # AST structures
 │   ├─ typechecker.c/h# Type system + units
 │   ├─ ir.c/h         # Intermediate representation
 │   └─ codegen.c/h    # C code emitter
 ├─ runtime/           # Embedded runtime
 │   ├─ core/          # Scheduler, messaging, safety
 │   ├─ hal/           # Hardware abstraction layer
 │   ├─ net/           # MQTT, CoAP, TLS
 │   └─ security/      # Crypto, ACLs, OTA
 ├─ robots/            # .neuro source files
 ├─ build/
 │   ├─ gen/           # Generated C code
 │   └─ bin/           # Compiled binaries
 ├─ examples/          # Sample robots
 ├─ tests/             # Unit + integration tests
 └─ tools/             # CLI, formatter, linter
```

## Build & Run

```bash
# Build everything (compiler + runtime + package manager)
make all

# Create new project
./build/bin/neurox init my-robot
cd my-robot

# Install packages
./build/bin/neurox install motor-control
./build/bin/neurox install vision-utils

# Build project
./build/bin/neurox build

# Run
./build/bin/my-robot
```

## Package Manager

```bash
# Install packages from GitHub
neurox install motor-control                 # Latest version
neurox install vision-utils@v1.0.0          # Specific version

# Manage project
neurox init my-robot                        # Create project
neurox build                                # Build
neurox list                                 # List packages
neurox search motor                         # Search packages

# Package repository
# Organization: https://github.com/vishwakarmaindustriesabhishek-in
# Naming: neurox-package-<name>
```

## Roadmap

- **M1**: Parser + mock runtime (Linux RT, mock HAL)
- **M2**: Safety + events + command handler
- **M3**: Embedded bring-up (ESP32/STM32)
- **M4**: Security hardening (TLS, ACLs, OTA)
- **M5**: Edge gateway + dashboards
- **M6**: Advanced features (sensor fusion, multi-robot coordination)

## Testing

```bash
make test              # Run all tests
make test-parser       # Parser unit tests
make test-runtime      # Runtime integration tests
make test-timing       # Jitter/deadline validation
```

## License

MIT (or your preferred license)
