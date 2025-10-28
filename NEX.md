# NeuroX Extension Format (.nex)

**Complete specification and guide for the NeuroX package format**

---

## 📦 What is .nex?

`.nex` (NeuroX Extension) is the standard package format for distributing NeuroX libraries, drivers, and modules. It's a self-contained archive that includes:

- Compiled code (static/shared libraries)
- Header files (public API)
- Source code (optional)
- Documentation
- Metadata (dependencies, version, license)
- Examples

---

## 🏗️ Package Structure

### Complete .nex Archive Layout

```
package-name.nex
├── manifest.json              # Package metadata
├── compiled/                  # Compiled binaries
│   ├── linux-x64/
│   │   ├── libpackage.a      # Static library
│   │   └── libpackage.so     # Shared library
│   ├── linux-arm64/
│   ├── esp32/
│   ├── stm32/
│   └── rp2040/
├── headers/                   # Public API headers
│   ├── package.h             # Main header
│   └── types.h               # Type definitions
├── source/                    # Source code (optional)
│   ├── lib.neuro             # NeuroX source
│   ├── impl.c                # C implementation
│   └── impl.h                # Private headers
├── docs/                      # Documentation
│   ├── README.md             # Package documentation
│   ├── API.md                # API reference
│   └── CHANGELOG.md          # Version history
├── examples/                  # Usage examples
│   ├── basic.neuro           # Basic example
│   └── advanced.neuro        # Advanced example
├── tests/                     # Unit tests
│   └── test.neuro
└── LICENSE                    # License file
```

---

## 📋 Manifest Format (manifest.json)

```json
{
  "package": {
    "name": "motor-control",
    "version": "2.1.0",
    "description": "Advanced motor control library with PID",
    "authors": [
      "John Doe <john@example.com>"
    ],
    "license": "MIT",
    "repository": "https://github.com/vishwakarmaindustriesabhishek-in/neurox-package-motor-control",
    "homepage": "https://neurox-motor-control.dev",
    "keywords": ["motor", "pid", "control", "robotics"],
    "categories": ["hardware", "control"]
  },
  
  "dependencies": {
    "math-utils": "^1.0.0",
    "sensor-fusion": "2.3.1"
  },
  
  "dev-dependencies": {
    "test-utils": "1.0.0"
  },
  
  "platforms": {
    "supported": ["linux-x64", "linux-arm64", "esp32", "stm32", "rp2040"],
    "tested": ["linux-x64", "esp32"]
  },
  
  "neurox": {
    "min-version": "0.1.0",
    "max-version": "1.0.0"
  },
  
  "exports": {
    "tasks": ["pid_control", "motor_init"],
    "functions": ["configure_motor", "set_limits"],
    "types": ["MotorConfig", "PIDParams"]
  },
  
  "build": {
    "compiler": "gcc",
    "flags": ["-O2", "-Wall"],
    "libraries": ["m"]
  },
  
  "checksum": {
    "algorithm": "sha256",
    "hash": "abc123def456..."
  },
  
  "signature": {
    "algorithm": "ed25519",
    "public-key": "...",
    "signature": "..."
  }
}
```

---

## 🔨 Creating a .nex Package

### Method 1: Using NeuroX CLI

```bash
# Initialize package
neurox init my-package --template library

# Edit package files
cd my-package
# ... write code ...

# Build package
neurox build-package

# Output: dist/my-package-1.0.0.nex
```

### Method 2: Using NEX Builder GUI

```bash
# Launch GUI builder
./build/bin/nex-builder

# Or with existing project
./build/bin/nex-builder --project ./my-package
```

### Method 3: Manual Creation

```bash
# Create directory structure
mkdir -p my-package/{compiled,headers,source,docs,examples}

# Create manifest
cat > my-package/manifest.json << EOF
{
  "package": {
    "name": "my-package",
    "version": "1.0.0",
    ...
  }
}
EOF

# Build libraries
neurox build --release

# Package everything
tar -czf my-package-1.0.0.nex -C my-package .
```

---

## 📥 Installing .nex Packages

### From GitHub
```bash
neurox install motor-control
neurox install motor-control@v2.1.0
```

### From Local File
```bash
neurox install ./motor-control-2.1.0.nex
```

### From URL
```bash
neurox install https://github.com/.../releases/download/v2.1.0/motor-control.nex
```

---

## 💻 Using .nex Packages in Code

### Import Package
```neuro
// Import entire package
import motor-control

// Import specific items
import { pid_control, MotorConfig } from motor-control

// Import with alias
import motor-control as mc

robot MyRobot {
  motor m1 on M1
  
  task control_loop() {
    // Use imported functions
    let output = motor-control.pid_control(
      target: 100,
      current: m1.speed,
      kp: 1.0,
      ki: 0.1,
      kd: 0.01
    )
    m1.power = output
  }
}
```

---

## 🔍 Package Metadata

### Version Specification
- **Semantic Versioning**: MAJOR.MINOR.PATCH
- **Version Ranges**:
  - `^1.2.3` = `>=1.2.3 <2.0.0` (compatible)
  - `~1.2.3` = `>=1.2.3 <1.3.0` (patch updates)
  - `1.2.3` = exact version
  - `>=1.2.3` = minimum version
  - `*` = any version (not recommended)

### Platform Identifiers
- `linux-x64` - Linux x86_64
- `linux-arm64` - Linux ARM64
- `linux-armv7` - Linux ARMv7
- `esp32` - ESP32 microcontroller
- `stm32` - STM32 microcontroller
- `rp2040` - Raspberry Pi Pico
- `macos-x64` - macOS Intel
- `macos-arm64` - macOS Apple Silicon
- `windows-x64` - Windows 64-bit

---

## 🛠️ NEX Builder GUI

### Features
- **Visual Package Editor**
- **Dependency Manager**
- **Multi-Platform Builder**
- **Metadata Editor**
- **File Browser**
- **Build Progress**
- **Package Validator**
- **Digital Signing**

### Interface Sections

#### 1. Project Info
- Package name
- Version
- Description
- Authors
- License

#### 2. Dependencies
- Add/remove dependencies
- Version constraints
- Dependency tree view

#### 3. Files
- Source files browser
- Include/exclude patterns
- File organization

#### 4. Build Settings
- Target platforms
- Compiler options
- Optimization level
- Debug symbols

#### 5. Documentation
- README editor
- API documentation
- Examples

#### 6. Build & Package
- Build progress
- Error/warning display
- Package validation
- Output location

---

## 📦 Package Types

### Library Package
```json
{
  "package": {
    "type": "library",
    "name": "motor-control"
  }
}
```

### Driver Package
```json
{
  "package": {
    "type": "driver",
    "name": "lidar-driver",
    "hardware": ["RPLIDAR-A1", "RPLIDAR-A2"]
  }
}
```

### Template Package
```json
{
  "package": {
    "type": "template",
    "name": "autonomous-robot-template"
  }
}
```

### Plugin Package
```json
{
  "package": {
    "type": "plugin",
    "name": "custom-optimizer"
  }
}
```

---

## 🔐 Security

### Package Signing
```bash
# Generate signing key
neurox keygen --output signing-key.pem

# Sign package
neurox sign my-package.nex --key signing-key.pem

# Verify signature
neurox verify my-package.nex --public-key public-key.pem
```

### Checksum Verification
```bash
# Calculate checksum
sha256sum my-package.nex

# Verify during installation
neurox install my-package.nex --verify-checksum
```

---

## 📊 Package Quality

### Quality Checklist
- ✅ Complete manifest.json
- ✅ README.md with usage examples
- ✅ API documentation
- ✅ At least one example
- ✅ Unit tests
- ✅ License file
- ✅ Changelog
- ✅ Multi-platform support
- ✅ Signed package
- ✅ Version tagged in Git

### Validation
```bash
# Validate package structure
neurox validate my-package.nex

# Check for common issues
neurox lint my-package.nex

# Test installation
neurox test-install my-package.nex
```

---

## 🎯 Best Practices

### Naming Conventions
- **Package names**: lowercase, hyphen-separated
  - ✅ `motor-control`
  - ✅ `vision-utils`
  - ❌ `MotorControl`
  - ❌ `motor_control`

- **Repository names**: `neurox-package-<name>`
  - ✅ `neurox-package-motor-control`
  - ✅ `neurox-package-vision-utils`

### Versioning
- Start at `0.1.0` for initial development
- Use `1.0.0` for first stable release
- Increment MAJOR for breaking changes
- Increment MINOR for new features
- Increment PATCH for bug fixes

### Documentation
- Write clear README with:
  - Installation instructions
  - Quick start example
  - API reference
  - License information
- Include code examples
- Document all public APIs
- Keep changelog updated

### Dependencies
- Minimize dependencies
- Pin versions in applications
- Use ranges in libraries
- Test with minimum supported versions

### Testing
- Include unit tests
- Test on all supported platforms
- Verify examples work
- Check for memory leaks

---

## 🔄 Package Lifecycle

### Development
```bash
neurox init my-package --template library
cd my-package
# ... develop ...
neurox test
```

### Building
```bash
neurox build-package --platforms linux-x64,esp32
# Output: dist/my-package-1.0.0.nex
```

### Publishing
```bash
# Create GitHub release
git tag v1.0.0
git push origin v1.0.0

# Upload .nex file to release
# Users can now: neurox install my-package@v1.0.0
```

### Updating
```bash
# Bump version
neurox version patch  # 1.0.0 -> 1.0.1
neurox version minor  # 1.0.1 -> 1.1.0
neurox version major  # 1.1.0 -> 2.0.0

# Rebuild and republish
neurox build-package
```

---

## 📚 Example Packages

### Simple Library
```
motor-control.nex
├── manifest.json
├── compiled/
│   └── linux-x64/
│       └── libmotor.a
├── headers/
│   └── motor.h
├── source/
│   └── motor.neuro
└── docs/
    └── README.md
```

### Complex Package
```
vision-slam.nex
├── manifest.json
├── compiled/
│   ├── linux-x64/
│   ├── linux-arm64/
│   └── esp32/
├── headers/
│   ├── slam.h
│   ├── map.h
│   └── types.h
├── source/
│   ├── slam.neuro
│   ├── orb.c
│   └── optimizer.cpp
├── docs/
│   ├── README.md
│   ├── API.md
│   └── TUTORIAL.md
├── examples/
│   ├── basic-slam.neuro
│   └── advanced-mapping.neuro
└── tests/
    └── test-slam.neuro
```

---

## 🔧 Advanced Features

### Conditional Dependencies
```json
{
  "dependencies": {
    "opencv": {
      "version": "^4.5.0",
      "platforms": ["linux-x64", "linux-arm64"],
      "optional": true
    }
  }
}
```

### Build Scripts
```json
{
  "scripts": {
    "prebuild": "python generate_tables.py",
    "build": "make all",
    "postbuild": "strip libpackage.so",
    "test": "neurox test"
  }
}
```

### Feature Flags
```json
{
  "features": {
    "opencv": {
      "description": "OpenCV integration",
      "dependencies": ["opencv"]
    },
    "cuda": {
      "description": "CUDA acceleration",
      "dependencies": ["cuda-toolkit"]
    }
  }
}
```

Install with features:
```bash
neurox install vision-utils --features opencv,cuda
```

---

## 🎨 NEX Builder GUI Usage

### Launch
```bash
./build/bin/nex-builder
```

### Workflow
1. **New Project** → Enter package details
2. **Add Files** → Select source files
3. **Configure** → Set build options
4. **Add Dependencies** → Search and add
5. **Build** → Compile for platforms
6. **Package** → Create .nex file
7. **Sign** → Add digital signature
8. **Publish** → Upload to GitHub

### Keyboard Shortcuts
- `Ctrl+N` - New project
- `Ctrl+O` - Open project
- `Ctrl+S` - Save project
- `Ctrl+B` - Build package
- `Ctrl+T` - Run tests
- `F5` - Build and package
- `F9` - Validate package

---

## 📖 Complete Example

### Creating motor-control.nex

#### 1. Initialize
```bash
neurox init motor-control --template library
cd motor-control
```

#### 2. Write Code (src/lib.neuro)
```neuro
library MotorControl {
  export struct PIDParams {
    kp: float
    ki: float
    kd: float
  }
  
  export task pid_control(
    target: float,
    current: float,
    params: PIDParams
  ) -> float {
    let error = target - current
    return params.kp * error
  }
}
```

#### 3. Create manifest (neurox.toml)
```toml
[package]
name = "motor-control"
version = "1.0.0"
description = "PID motor control"
authors = ["You <you@example.com>"]
license = "MIT"

[dependencies]
math-utils = "1.0.0"
```

#### 4. Build
```bash
neurox build-package
# Creates: dist/motor-control-1.0.0.nex
```

#### 5. Test Locally
```bash
neurox install ./dist/motor-control-1.0.0.nex
```

#### 6. Publish
```bash
git tag v1.0.0
git push origin v1.0.0
# Upload .nex to GitHub release
```

---

**NEX Format: The standard for NeuroX package distribution. 📦🚀**
