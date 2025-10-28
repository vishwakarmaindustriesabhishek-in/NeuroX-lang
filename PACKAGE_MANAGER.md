# NeuroX Package Manager

**Complete package management system for NeuroX-lang**

---

## 🚀 Overview

The **NeuroX Package Manager** (`neurox` CLI) provides a unified system for:
- Installing packages from registry or GitHub
- Managing dependencies
- Building and publishing libraries
- Version control and updates
- Cross-platform compatibility

---

## 📦 Package Format: `.nex`

### NeuroX Extension Format (.nex)
`.nex` files are compiled NeuroX libraries that can be imported and reused.

**Structure:**
```
package.nex
├── metadata.json       # Package info, dependencies, version
├── compiled/          # Compiled C code
│   ├── lib.a         # Static library
│   └── lib.so        # Shared library
├── headers/          # Public API headers
│   └── package.h
├── source/           # Original .neuro source (optional)
│   └── package.neuro
└── docs/             # Documentation
    └── README.md
```

---

## 🔧 CLI Commands

### Installation

All packages are hosted on GitHub at: `https://github.com/vishwakarmaindustriesabhishek-in`

#### Install from GitHub Organization
```bash
neurox install <package-name>                    # Latest from main branch
neurox install motor-control                     # From neurox-package-motor-control repo
neurox install vision-utils@v1.2.0              # Specific version/tag
neurox install sensor-fusion@main               # Specific branch
neurox install slam@develop                     # Development branch
```

**Repository Naming Convention:**
- Package name: `motor-control`
- GitHub repo: `neurox-package-motor-control`
- Full URL: `https://github.com/vishwakarmaindustriesabhishek-in/neurox-package-motor-control`

#### Install Multiple Packages
```bash
neurox install motor-control vision-utils sensor-fusion
```

#### Install with Options
```bash
neurox install package-name --save          # Add to dependencies
neurox install package-name --save-dev      # Add to dev dependencies
neurox install package-name --global        # Install globally
neurox install package-name --force         # Force reinstall
```

### Uninstallation

```bash
neurox uninstall <package-name>
neurox uninstall motor-control
neurox uninstall motor-control vision-utils
neurox uninstall --all                      # Remove all packages
```

### Building

#### Build Current Project
```bash
neurox build                                # Build project
neurox build --release                      # Release build (optimized)
neurox build --debug                        # Debug build
neurox build --target esp32                 # Target specific platform
neurox build --verbose                      # Verbose output
```

#### Build Package
```bash
neurox build-package                        # Create .nex package
neurox build-package --output dist/         # Specify output directory
```

### Project Management

#### Initialize New Project
```bash
neurox init                                 # Interactive setup
neurox init my-robot                        # Create new project
neurox init --template basic                # Use template
neurox init --template advanced             # Advanced template
```

#### Update Dependencies
```bash
neurox update                               # Update all packages
neurox update <package-name>                # Update specific package
neurox update --check                       # Check for updates
```

#### List Packages
```bash
neurox list                                 # List installed packages
neurox list --global                        # List global packages
neurox list --tree                          # Show dependency tree
```

#### Search Packages
```bash
neurox search motor                         # Search registry
neurox search vision --tag opencv           # Search with tags
```

#### Package Info
```bash
neurox info <package-name>                  # Show package details
neurox info motor-control --versions        # Show all versions
```

### Publishing

```bash
neurox login                                # Login to registry
neurox publish                              # Publish package
neurox publish --tag beta                   # Publish with tag
neurox unpublish <package-name>@<version>   # Remove version
```

### Configuration

```bash
neurox config set registry https://registry.neurox.dev
neurox config get registry
neurox config list
neurox config delete <key>
```

### Cleaning

```bash
neurox clean                                # Clean build artifacts
neurox clean --cache                        # Clean package cache
neurox clean --all                          # Clean everything
```

### Version Management

```bash
neurox version                              # Show current version
neurox version patch                        # Bump patch version
neurox version minor                        # Bump minor version
neurox version major                        # Bump major version
```

---

## 📋 Project Configuration: `neurox.toml`

```toml
[package]
name = "my-robot"
version = "1.0.0"
description = "My awesome robot"
authors = ["Your Name <you@example.com>"]
license = "MIT"
repository = "https://github.com/user/my-robot"
keywords = ["robotics", "autonomous", "vision"]
categories = ["robotics", "embedded"]

[dependencies]
motor-control = "^2.1.0"
vision-utils = "1.5.2"
sensor-fusion = { version = "3.0.0", features = ["kalman", "ekf"] }
slam = { git = "https://github.com/user/slam.git", tag = "v2.0.0" }

[dev-dependencies]
test-utils = "1.0.0"
mock-hardware = "0.5.0"

[build]
target = "esp32"
optimization = "release"
features = ["wifi", "bluetooth"]

[profile.release]
opt-level = 3
lto = true
strip = true

[profile.debug]
opt-level = 0
debug = true
```

---

## 🏗️ Creating a Package

### 1. Project Structure
```
my-package/
├── neurox.toml           # Package manifest
├── src/
│   ├── main.neuro       # Main source
│   └── lib.neuro        # Library code
├── include/
│   └── my-package.h     # Public headers
├── examples/
│   └── example.neuro    # Usage examples
├── tests/
│   └── test.neuro       # Unit tests
├── docs/
│   └── README.md        # Documentation
└── LICENSE
```

### 2. Package Manifest
```toml
# neurox.toml
[package]
name = "motor-control"
version = "2.1.0"
description = "Advanced motor control library"
authors = ["John Doe <john@example.com>"]
license = "MIT"
repository = "https://github.com/neurox/motor-control"

[dependencies]
math-utils = "1.0.0"

[lib]
name = "motor_control"
path = "src/lib.neuro"
```

### 3. Library Code
```neuro
// src/lib.neuro
library MotorControl {
  export task pid_control(
    target: float,
    current: float,
    kp: float,
    ki: float,
    kd: float
  ) -> float {
    // PID implementation
    let error = target - current
    let output = kp * error
    return output
  }
  
  export struct MotorConfig {
    max_speed: Percent
    acceleration: float
    deadband: float
  }
  
  export function configure_motor(
    motor: Motor,
    config: MotorConfig
  ) {
    motor.max_speed = config.max_speed
    // Configuration logic
  }
}
```

### 4. Build Package
```bash
neurox build-package
# Creates: dist/motor-control-2.1.0.nex
```

### 5. Publish
```bash
neurox publish
# Published to: https://registry.neurox.dev/motor-control
```

---

## 📥 Using Packages

### Import in NeuroX Code
```neuro
// Import entire package
import motor-control

// Import specific items
import { pid_control, MotorConfig } from motor-control

// Import with alias
import motor-control as mc

// Usage
robot MyRobot {
  motor main on M1
  
  task control_loop() {
    let output = motor-control.pid_control(
      target: 100,
      current: main.speed,
      kp: 1.0,
      ki: 0.1,
      kd: 0.01
    )
    main.power = output
  }
}
```

---

## 🌐 Package Repository

### GitHub Organization
```
https://github.com/vishwakarmaindustriesabhishek-in
```

### Repository Structure
```
github.com/vishwakarmaindustriesabhishek-in/
├── neurox-package-motor-control/
│   ├── neurox.toml
│   ├── src/
│   ├── README.md
│   └── releases/
│       ├── v1.0.0
│       └── v2.0.0
├── neurox-package-vision-utils/
├── neurox-package-sensor-fusion/
└── neurox-package-slam/
```

### Package Categories
- **Core**: Essential utilities
- **Hardware**: Motor, servo, sensor drivers
- **Vision**: Computer vision algorithms
- **ML**: Machine learning models
- **Navigation**: SLAM, path planning
- **Swarm**: Multi-robot coordination
- **Communication**: MQTT, ROS bridges
- **Security**: Crypto, authentication

---

## 📦 Popular Packages

### Core Packages
```bash
neurox install @neurox/core              # Core utilities
neurox install @neurox/math              # Math functions
neurox install @neurox/time              # Time utilities
```

### Hardware Drivers
```bash
neurox install motor-control             # Motor control
neurox install servo-driver              # Servo control
neurox install sensor-fusion             # Sensor fusion
neurox install imu-driver                # IMU interface
neurox install lidar-driver              # LiDAR interface
```

### Vision & ML
```bash
neurox install opencv-bridge             # OpenCV integration
neurox install yolo-detector             # YOLO object detection
neurox install pose-estimation           # Pose estimation
neurox install visual-slam               # Visual SLAM
```

### Navigation
```bash
neurox install path-planning             # Path planning
neurox install obstacle-avoidance        # Obstacle avoidance
neurox install localization              # Localization
neurox install mapping                   # Mapping
```

### Communication
```bash
neurox install mqtt-client               # MQTT client
neurox install ros2-bridge               # ROS 2 bridge
neurox install websocket                 # WebSocket
neurox install http-client               # HTTP client
```

---

## 🔐 Package Security

### Verification
```bash
neurox verify <package-name>             # Verify package integrity
neurox audit                             # Security audit
neurox audit fix                         # Fix vulnerabilities
```

### Signing
```bash
neurox sign <package.nex>                # Sign package
neurox verify-signature <package.nex>    # Verify signature
```

### Package Manifest Security
```toml
[security]
checksum = "sha256:abc123..."
signature = "ed25519:def456..."
verified = true
```

---

## 🚀 Advanced Features

### Workspaces
```toml
# neurox-workspace.toml
[workspace]
members = [
    "packages/motor-control",
    "packages/vision-utils",
    "packages/sensor-fusion"
]
```

```bash
neurox workspace build                   # Build all packages
neurox workspace test                    # Test all packages
neurox workspace publish                 # Publish all packages
```

### Private Registry
```bash
neurox config set registry https://private.company.com/neurox
neurox login --registry private
```

### Offline Mode
```bash
neurox install --offline                 # Use cached packages
neurox bundle                            # Create offline bundle
```

### Scripts
```toml
[scripts]
build = "neurox build --release"
test = "neurox test --all"
deploy = "neurox build && neurox flash"
```

```bash
neurox run build
neurox run test
neurox run deploy
```

---

## 📊 Package Statistics

### Usage Stats
```bash
neurox stats                             # Show package stats
neurox stats <package-name>              # Package-specific stats
```

### Dependencies
```bash
neurox deps                              # Show dependency tree
neurox deps --graph                      # Visual graph
neurox outdated                          # Check outdated packages
```

---

## 🔄 Migration & Compatibility

### Version Compatibility
```toml
[package]
neurox-version = ">=0.1.0, <2.0.0"       # Supported NeuroX versions
```

### Platform Support
```toml
[platform]
supported = ["linux", "esp32", "stm32", "rp2040"]
tested = ["linux", "esp32"]
```

---

## 🛠️ Development Tools

### Link Local Package
```bash
neurox link                              # Link current package
neurox link ../other-package             # Link external package
neurox unlink <package-name>             # Unlink package
```

### Watch Mode
```bash
neurox watch                             # Watch and rebuild
neurox watch --test                      # Watch and test
```

### Documentation
```bash
neurox doc                               # Generate docs
neurox doc --open                        # Generate and open
```

---

## 📝 Package Templates

### Available Templates
```bash
neurox init --template basic             # Basic robot
neurox init --template advanced          # Advanced features
neurox init --template library           # Library package
neurox init --template vision            # Vision-based robot
neurox init --template swarm             # Swarm robot
neurox init --template ml                # ML-enabled robot
```

### Custom Templates
```bash
neurox template create my-template
neurox template publish my-template
neurox init --template user/my-template
```

---

## 🌟 Example Workflows

### Install and Use Package
```bash
# Install package
neurox install motor-control

# Create project
neurox init my-robot

# Add to code
cat > src/main.neuro << EOF
import motor-control

robot MyRobot {
  motor m1 on M1
  
  task control() {
    motor-control.pid_control(100, m1.speed, 1.0, 0.1, 0.01)
  }
}
EOF

# Build
neurox build

# Run
./build/bin/my-robot
```

### Create and Publish Package
```bash
# Create package
neurox init my-package --template library

# Develop
cd my-package
# ... write code ...

# Test
neurox test

# Build package
neurox build-package

# Publish
neurox login
neurox publish
```

### Update Dependencies
```bash
# Check for updates
neurox outdated

# Update all
neurox update

# Update specific
neurox update motor-control

# Install specific version
neurox install motor-control@2.1.0
```

---

## 🔍 Package Discovery

### Search
```bash
neurox search motor
neurox search vision --tag opencv
neurox search --category navigation
```

### Browse
```bash
neurox browse                            # Open registry in browser
neurox browse <package-name>             # Open package page
```

---

## 📈 Package Metrics

### Downloads
```bash
neurox stats motor-control
# Downloads: 10,234
# Stars: 156
# Last updated: 2 days ago
```

### Trending
```bash
neurox trending                          # Trending packages
neurox trending --weekly                 # Weekly trending
```

---

## 🎯 Best Practices

### Versioning
- Follow [Semantic Versioning](https://semver.org/)
- MAJOR.MINOR.PATCH (e.g., 2.1.0)
- Use `^` for compatible updates: `^2.1.0` = `>=2.1.0 <3.0.0`
- Use `~` for patch updates: `~2.1.0` = `>=2.1.0 <2.2.0`

### Dependencies
- Minimize dependencies
- Pin versions in production
- Use ranges in libraries
- Regular security audits

### Documentation
- Include README.md
- Add usage examples
- Document public API
- Provide changelog

### Testing
- Write unit tests
- Test on target platforms
- CI/CD integration
- Version compatibility tests

---

**NeuroX Package Manager: Simplifying robotics development. 📦🤖**
