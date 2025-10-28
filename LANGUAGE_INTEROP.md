# NeuroX-lang Language Interoperability

**Seamless integration with all major programming languages**

---

## 🌐 Overview

NeuroX-lang provides **native interoperability** with:
- **Python** (via C API, ctypes, CFFI)
- **C/C++** (native compilation target)
- **Rust** (via FFI)
- **JavaScript/TypeScript** (via WebAssembly, Node.js)
- **Go** (via cgo)
- **Java/Kotlin** (via JNI)
- **C#/.NET** (via P/Invoke)
- **Julia** (via ccall)
- **MATLAB** (via MEX)
- **R** (via Rcpp)

---

## 🔧 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    NeuroX-lang Core                         │
│              (Compiled to C/LLVM IR)                        │
└─────────────────┬───────────────────────────────────────────┘
                  │
┌─────────────────┴───────────────────────────────────────────┐
│              Language Bridges & FFI                         │
└─┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────────────┘
  │    │    │    │    │    │    │    │    │    │
┌─▼─┐┌─▼─┐┌─▼─┐┌─▼─┐┌─▼─┐┌─▼─┐┌─▼─┐┌─▼─┐┌─▼─┐┌─▼─┐
│Py ││C++││Rust││JS ││Go ││Java││C# ││Julia││MATLAB││R│
└───┘└───┘└───┘└───┘└───┘└───┘└───┘└───┘└───┘└───┘
```

---

## 1️⃣ Python Integration

### C API Bridge
```c
// runtime/bindings/python/neurox_python.h
#include <Python.h>

PyObject* neurox_py_init(PyObject* self, PyObject* args);
PyObject* neurox_py_run_task(PyObject* self, PyObject* args);
PyObject* neurox_py_get_sensor(PyObject* self, PyObject* args);
```

### Python Module
```python
# neurox/__init__.py
import _neurox

class Robot:
    def __init__(self, config_file):
        self.handle = _neurox.init(config_file)
    
    def run_task(self, task_name, **kwargs):
        return _neurox.run_task(self.handle, task_name, kwargs)
    
    def get_sensor(self, sensor_name):
        return _neurox.get_sensor(self.handle, sensor_name)

# Usage
robot = Robot("myrobot.neuro")
robot.run_task("move", speed=50, duration=1000)
distance = robot.get_sensor("lidar")
```

### NeuroX calling Python
```neuro
robot MyBot {
  python {
    module "my_algorithms"
    
    function path_planner {
      signature "plan_path(start, goal) -> path"
    }
  }
  
  task navigate() {
    let start = get_position()
    let goal = {x: 10, y: 20}
    let path = python.path_planner(start, goal)
    follow_path(path)
  }
}
```

---

## 2️⃣ C/C++ Integration

### Direct Compilation
```neuro
// NeuroX compiles to C
robot MyBot {
  task move(speed: Percent) {
    motor.power = speed
  }
}

// Generated C code
void task_move(float speed) {
    nrx_motor_set_power(&motor, speed);
}
```

### C++ Wrapper
```cpp
// neurox.hpp
namespace neurox {
    class Robot {
    public:
        Robot(const std::string& config);
        void runTask(const std::string& name);
        template<typename T>
        T getSensor(const std::string& name);
    };
}

// Usage
neurox::Robot robot("myrobot.neuro");
robot.runTask("move");
auto distance = robot.getSensor<float>("lidar");
```

### Calling C/C++ from NeuroX
```neuro
robot MyBot {
  extern "C" {
    function calculate_trajectory(
      start: Vec3,
      end: Vec3,
      obstacles: Array<Vec3>
    ) -> Array<Vec3>
  }
  
  task navigate() {
    let path = calculate_trajectory(pos, goal, obstacles)
    follow_path(path)
  }
}
```

---

## 3️⃣ Rust Integration

### FFI Bridge
```rust
// neurox-rs/src/lib.rs
use std::ffi::{CStr, CString};
use std::os::raw::c_char;

#[repr(C)]
pub struct NeuroxRobot {
    handle: *mut std::ffi::c_void,
}

extern "C" {
    fn neurox_init(config: *const c_char) -> *mut std::ffi::c_void;
    fn neurox_run_task(handle: *mut std::ffi::c_void, task: *const c_char);
}

impl NeuroxRobot {
    pub fn new(config: &str) -> Self {
        let c_config = CString::new(config).unwrap();
        let handle = unsafe { neurox_init(c_config.as_ptr()) };
        NeuroxRobot { handle }
    }
    
    pub fn run_task(&self, task: &str) {
        let c_task = CString::new(task).unwrap();
        unsafe { neurox_run_task(self.handle, c_task.as_ptr()) };
    }
}

// Usage
let robot = NeuroxRobot::new("myrobot.neuro");
robot.run_task("move");
```

### Calling Rust from NeuroX
```neuro
robot MyBot {
  rust {
    crate "my_algorithms"
    
    function optimize_path {
      signature "optimize_path(path: &[Vec3]) -> Vec<Vec3>"
    }
  }
  
  task navigate() {
    let raw_path = plan_path()
    let optimized = rust.optimize_path(raw_path)
    follow_path(optimized)
  }
}
```

---

## 4️⃣ JavaScript/TypeScript Integration

### Node.js Addon
```javascript
// neurox.node (C++ addon)
const neurox = require('bindings')('neurox');

class Robot {
    constructor(configFile) {
        this.handle = neurox.init(configFile);
    }
    
    runTask(taskName, params = {}) {
        return neurox.runTask(this.handle, taskName, params);
    }
    
    getSensor(sensorName) {
        return neurox.getSensor(this.handle, sensorName);
    }
}

// Usage
const robot = new Robot('myrobot.neuro');
await robot.runTask('move', { speed: 50, duration: 1000 });
const distance = robot.getSensor('lidar');
```

### WebAssembly
```javascript
// neurox.wasm
import init, { Robot } from './neurox_wasm.js';

await init();
const robot = new Robot('myrobot.neuro');
robot.runTask('move');
```

### Calling JS from NeuroX
```neuro
robot MyBot {
  javascript {
    module "visualization"
    
    function update_dashboard {
      signature "updateDashboard(data: Object) -> void"
    }
  }
  
  task telemetry() {
    let data = collect_telemetry()
    javascript.update_dashboard(data)
  }
}
```

---

## 5️⃣ Go Integration

### cgo Bridge
```go
// neurox.go
package neurox

/*
#cgo LDFLAGS: -lneurox_runtime
#include "neurox.h"
*/
import "C"
import "unsafe"

type Robot struct {
    handle unsafe.Pointer
}

func NewRobot(config string) *Robot {
    cConfig := C.CString(config)
    defer C.free(unsafe.Pointer(cConfig))
    
    handle := C.neurox_init(cConfig)
    return &Robot{handle: handle}
}

func (r *Robot) RunTask(task string) {
    cTask := C.CString(task)
    defer C.free(unsafe.Pointer(cTask))
    C.neurox_run_task(r.handle, cTask)
}

// Usage
robot := neurox.NewRobot("myrobot.neuro")
robot.RunTask("move")
```

---

## 6️⃣ Java/Kotlin Integration

### JNI Bridge
```java
// Neurox.java
public class Neurox {
    static {
        System.loadLibrary("neurox_jni");
    }
    
    private long handle;
    
    public Neurox(String configFile) {
        this.handle = nativeInit(configFile);
    }
    
    public void runTask(String taskName) {
        nativeRunTask(handle, taskName);
    }
    
    private native long nativeInit(String config);
    private native void nativeRunTask(long handle, String task);
}

// Usage
Neurox robot = new Neurox("myrobot.neuro");
robot.runTask("move");
```

### Kotlin
```kotlin
// Neurox.kt
class Neurox(configFile: String) {
    private val handle: Long = nativeInit(configFile)
    
    fun runTask(taskName: String) {
        nativeRunTask(handle, taskName)
    }
    
    private external fun nativeInit(config: String): Long
    private external fun nativeRunTask(handle: Long, task: String)
    
    companion object {
        init {
            System.loadLibrary("neurox_jni")
        }
    }
}

// Usage
val robot = Neurox("myrobot.neuro")
robot.runTask("move")
```

---

## 7️⃣ C#/.NET Integration

### P/Invoke
```csharp
// Neurox.cs
using System;
using System.Runtime.InteropServices;

public class Neurox {
    [DllImport("neurox_runtime")]
    private static extern IntPtr neurox_init(string config);
    
    [DllImport("neurox_runtime")]
    private static extern void neurox_run_task(IntPtr handle, string task);
    
    private IntPtr handle;
    
    public Neurox(string configFile) {
        handle = neurox_init(configFile);
    }
    
    public void RunTask(string taskName) {
        neurox_run_task(handle, taskName);
    }
}

// Usage
var robot = new Neurox("myrobot.neuro");
robot.RunTask("move");
```

---

## 8️⃣ Julia Integration

### ccall
```julia
# Neurox.jl
module Neurox

const libneurox = "libneurox_runtime"

mutable struct Robot
    handle::Ptr{Cvoid}
    
    function Robot(config::String)
        handle = ccall((:neurox_init, libneurox), Ptr{Cvoid}, (Cstring,), config)
        new(handle)
    end
end

function run_task(robot::Robot, task::String)
    ccall((:neurox_run_task, libneurox), Cvoid, (Ptr{Cvoid}, Cstring), robot.handle, task)
end

end # module

# Usage
using Neurox
robot = Robot("myrobot.neuro")
run_task(robot, "move")
```

---

## 9️⃣ MATLAB Integration

### MEX Function
```matlab
% neurox_init.m
function handle = neurox_init(config_file)
    handle = calllib('neurox_runtime', 'neurox_init', config_file);
end

% neurox_run_task.m
function neurox_run_task(handle, task_name)
    calllib('neurox_runtime', 'neurox_run_task', handle, task_name);
end

% Usage
robot = neurox_init('myrobot.neuro');
neurox_run_task(robot, 'move');
```

---

## 🔟 R Integration

### Rcpp
```r
# neurox.R
library(Rcpp)

sourceCpp(code = '
#include <Rcpp.h>
extern "C" {
    void* neurox_init(const char* config);
    void neurox_run_task(void* handle, const char* task);
}

// [[Rcpp::export]]
SEXP neurox_init_r(std::string config) {
    void* handle = neurox_init(config.c_str());
    return Rcpp::XPtr<void>(handle);
}

// [[Rcpp::export]]
void neurox_run_task_r(SEXP handle, std::string task) {
    void* h = Rcpp::XPtr<void>(handle);
    neurox_run_task(h, task.c_str());
}
')

# Usage
robot <- neurox_init_r("myrobot.neuro")
neurox_run_task_r(robot, "move")
```

---

## 🔄 Universal FFI Interface

```c
// runtime/bindings/ffi/neurox_ffi.h
// Universal C ABI for all languages

typedef void* neurox_handle_t;

// Initialization
neurox_handle_t neurox_init(const char* config_file);
void neurox_free(neurox_handle_t handle);

// Task execution
int neurox_run_task(neurox_handle_t handle, const char* task_name);
int neurox_run_task_with_params(neurox_handle_t handle, const char* task_name, 
                                const char* json_params);

// Sensor access
float neurox_get_sensor_float(neurox_handle_t handle, const char* sensor_name);
int neurox_get_sensor_int(neurox_handle_t handle, const char* sensor_name);
const char* neurox_get_sensor_string(neurox_handle_t handle, const char* sensor_name);

// Motor control
void neurox_set_motor_power(neurox_handle_t handle, const char* motor_name, float power);

// Callbacks
typedef void (*neurox_callback_t)(const char* event, const char* data, void* user_data);
void neurox_register_callback(neurox_handle_t handle, neurox_callback_t callback, void* user_data);
```

---

## 📦 Package Distribution

### Python (PyPI)
```bash
pip install neurox-lang
```

### Rust (crates.io)
```toml
[dependencies]
neurox = "0.1.0"
```

### JavaScript (npm)
```bash
npm install neurox-lang
```

### Go (go get)
```bash
go get github.com/neurox-lang/neurox-go
```

### Java (Maven)
```xml
<dependency>
    <groupId>org.neurox</groupId>
    <artifactId>neurox-java</artifactId>
    <version>0.1.0</version>
</dependency>
```

---

## 🎯 Complete Example

### NeuroX Robot
```neuro
robot MultiLangBot {
  motor left on M1
  sensor lidar on UART0
  
  // Python for ML
  python {
    module "ml_models"
    function predict_obstacle from "predict_obstacle(distance) -> bool"
  }
  
  // Rust for performance
  rust {
    crate "path_optimizer"
    function optimize from "optimize_path(path: &[Vec3]) -> Vec<Vec3>"
  }
  
  // JavaScript for UI
  javascript {
    module "dashboard"
    function update from "updateDashboard(data: Object) -> void"
  }
  
  task navigate() {
    let dist = lidar.distance
    let obstacle = python.predict_obstacle(dist)
    
    if obstacle {
      let new_path = rust.optimize(current_path)
      follow_path(new_path)
    }
    
    javascript.update({distance: dist, obstacle: obstacle})
  }
}
```

---

**NeuroX-lang: Universal language interoperability for robotics. 🌐🤖**
