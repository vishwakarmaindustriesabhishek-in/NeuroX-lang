# NeuroX-lang Quickstart Guide

Get started with NeuroX-lang in 5 minutes.

## Build the Compiler

```bash
make
```

This builds:
- `build/bin/neuroxc` - The NeuroX compiler
- `build/bin/libneurox_runtime.a` - Runtime library

## Your First Robot

Create `my_robot.neuro`:

```neuro
robot MyFirstBot {
  motor m1 on M1
  
  task blink() {
    m1.power = 50%
    wait(1000ms)
    m1.power = 0%
    wait(1000ms)
  }
  
  schedule main @ 1Hz priority HIGH {
    blink()
  }
}
```

## Compile & Run

```bash
# Parse and check syntax
./build/bin/neuroxc parse my_robot.neuro

# Generate C code
./build/bin/neuroxc emit-c my_robot.neuro -o build/gen/my_robot.c

# Compile to executable (requires linking runtime)
gcc -o build/bin/my_robot build/gen/my_robot.c \
    build/bin/libneurox_runtime.a -lm -lpthread

# Run on Linux
./build/bin/my_robot
```

## Run Tests

```bash
make test
```

## Try Examples

```bash
# Parse the simple bot example
./build/bin/neuroxc parse examples/simple_bot.neuro

# Tokenize minimal example
./build/bin/neuroxc lex examples/minimal.neuro
```

## Language Features

### Hardware Declarations

```neuro
motor left on M1
motor right on M2
sensor distance on UART0 type Distance
gpio led on GPIO13 mode Output
```

### Tasks (Functions)

```neuro
task move(speed: Percent, duration: ms) {
  left.power = speed
  right.power = speed
  wait(duration)
  stop()
}
```

### Schedules (Periodic Execution)

```neuro
schedule control @ 100Hz priority HIGH {
  // Runs 100 times per second
  avoid_obstacles()
}

schedule telemetry @ 1Hz priority LOW {
  // Runs once per second
  send_status()
}
```

### Safety Limits

```neuro
limits {
  speed max 80%
  turn_rate max 120deg/s
}
```

### IoT/MQTT

```neuro
net mqtt broker "mqtts://broker.local:8883" client_id "bot-01"
topic cmd "robots/bot01/cmd"
topic telem "robots/bot01/telemetry"

on message cmd as msg {
  if msg.type == "move" {
    move(msg.speed%, msg.duration_ms)
  }
}
```

### Events

```neuro
gpio estop on GPIO5 mode InputPullup

when estop reads LOW {
  estop()  // Emergency stop
}
```

## Next Steps

1. **Read the docs**: See `README.md` for full language reference
2. **Explore examples**: Check `examples/` directory
3. **Write your robot**: Start with `examples/minimal.neuro` as a template
4. **Target embedded**: Port to ESP32/STM32 (see runtime/hal/)

## Project Structure

```
neurox-lang/
├── compiler/          # Lexer, parser, AST, codegen
├── runtime/           # Scheduler, safety, HAL, IoT
│   ├── core/         # Scheduler + safety
│   ├── hal/          # Hardware abstraction
│   └── net/          # MQTT/IoT stack
├── examples/         # Sample .neuro files
├── tests/            # Unit tests
└── tools/            # CLI (neuroxc)
```

## Troubleshooting

**Build fails?**
- Ensure you have `gcc`, `make` installed
- Check C11 support: `gcc --version`

**Parser errors?**
- Use `neuroxc lex <file>` to check tokens
- Use `neuroxc parse <file>` to see AST

**Runtime issues?**
- Check HAL implementation for your platform
- Enable debug output in runtime/core/

## Get Help

- Check `README.md` for detailed documentation
- Review examples in `examples/`
- Run tests: `make test`
