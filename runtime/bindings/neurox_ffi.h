#ifndef NEUROX_FFI_H
#define NEUROX_FFI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Universal FFI for all language bindings
// Provides C ABI for Python, Rust, Go, Java, C#, Julia, MATLAB, R, JavaScript

// ============================================================================
// CORE API
// ============================================================================

typedef void* neurox_handle_t;
typedef void* neurox_task_handle_t;
typedef void* neurox_sensor_handle_t;

// Initialization
neurox_handle_t neurox_init(const char* config_file);
neurox_handle_t neurox_init_from_string(const char* config_string);
void neurox_free(neurox_handle_t handle);

// Robot control
int neurox_start(neurox_handle_t handle);
int neurox_stop(neurox_handle_t handle);
int neurox_pause(neurox_handle_t handle);
int neurox_resume(neurox_handle_t handle);

// ============================================================================
// TASK API
// ============================================================================

// Task execution
int neurox_run_task(neurox_handle_t handle, const char* task_name);
int neurox_run_task_with_params(neurox_handle_t handle, const char* task_name, 
                                const char* json_params);
int neurox_run_task_async(neurox_handle_t handle, const char* task_name,
                          void (*callback)(int status, void* user_data),
                          void* user_data);

// Task info
int neurox_get_task_count(neurox_handle_t handle);
const char* neurox_get_task_name(neurox_handle_t handle, int index);
const char* neurox_get_task_params(neurox_handle_t handle, const char* task_name);

// ============================================================================
// SENSOR API
// ============================================================================

// Generic sensor access
float neurox_get_sensor_float(neurox_handle_t handle, const char* sensor_name);
int neurox_get_sensor_int(neurox_handle_t handle, const char* sensor_name);
bool neurox_get_sensor_bool(neurox_handle_t handle, const char* sensor_name);
const char* neurox_get_sensor_string(neurox_handle_t handle, const char* sensor_name);

// Array sensors
int neurox_get_sensor_array_float(neurox_handle_t handle, const char* sensor_name,
                                  float* buffer, size_t buffer_size);
int neurox_get_sensor_array_int(neurox_handle_t handle, const char* sensor_name,
                                int* buffer, size_t buffer_size);

// Sensor info
int neurox_get_sensor_count(neurox_handle_t handle);
const char* neurox_get_sensor_name(neurox_handle_t handle, int index);
const char* neurox_get_sensor_type(neurox_handle_t handle, const char* sensor_name);

// ============================================================================
// ACTUATOR API
// ============================================================================

// Motor control
int neurox_set_motor_power(neurox_handle_t handle, const char* motor_name, float power);
float neurox_get_motor_power(neurox_handle_t handle, const char* motor_name);
int neurox_stop_motor(neurox_handle_t handle, const char* motor_name);
int neurox_stop_all_motors(neurox_handle_t handle);

// Servo control
int neurox_set_servo_angle(neurox_handle_t handle, const char* servo_name, float angle);
float neurox_get_servo_angle(neurox_handle_t handle, const char* servo_name);

// GPIO
int neurox_set_gpio(neurox_handle_t handle, const char* gpio_name, bool value);
bool neurox_get_gpio(neurox_handle_t handle, const char* gpio_name);

// ============================================================================
// CALLBACK API
// ============================================================================

// Event callbacks
typedef void (*neurox_event_callback_t)(const char* event_name, 
                                        const char* event_data, 
                                        void* user_data);

int neurox_register_event_callback(neurox_handle_t handle, 
                                   const char* event_name,
                                   neurox_event_callback_t callback,
                                   void* user_data);

int neurox_unregister_event_callback(neurox_handle_t handle, const char* event_name);

// Sensor callbacks (triggered on value change)
typedef void (*neurox_sensor_callback_t)(const char* sensor_name,
                                         float value,
                                         void* user_data);

int neurox_register_sensor_callback(neurox_handle_t handle,
                                    const char* sensor_name,
                                    neurox_sensor_callback_t callback,
                                    void* user_data);

// ============================================================================
// STATE API
// ============================================================================

// Robot state
typedef enum {
    NEUROX_STATE_IDLE,
    NEUROX_STATE_RUNNING,
    NEUROX_STATE_PAUSED,
    NEUROX_STATE_ERROR,
    NEUROX_STATE_ESTOP,
} neurox_state_t;

neurox_state_t neurox_get_state(neurox_handle_t handle);
const char* neurox_get_state_string(neurox_handle_t handle);

// Position/pose
typedef struct {
    float x, y, z;
    float roll, pitch, yaw;
} neurox_pose_t;

int neurox_get_pose(neurox_handle_t handle, neurox_pose_t* pose);
int neurox_set_pose(neurox_handle_t handle, const neurox_pose_t* pose);

// ============================================================================
// TELEMETRY API
// ============================================================================

// Get telemetry as JSON
const char* neurox_get_telemetry_json(neurox_handle_t handle);

// Individual telemetry values
float neurox_get_battery_level(neurox_handle_t handle);
float neurox_get_cpu_usage(neurox_handle_t handle);
float neurox_get_memory_usage(neurox_handle_t handle);
uint64_t neurox_get_uptime_ms(neurox_handle_t handle);

// ============================================================================
// NETWORK API
// ============================================================================

// MQTT
int neurox_mqtt_publish(neurox_handle_t handle, const char* topic, 
                       const char* payload, int qos);
int neurox_mqtt_subscribe(neurox_handle_t handle, const char* topic, int qos);
int neurox_mqtt_unsubscribe(neurox_handle_t handle, const char* topic);

// HTTP
const char* neurox_http_get(neurox_handle_t handle, const char* url);
const char* neurox_http_post(neurox_handle_t handle, const char* url, const char* data);

// ============================================================================
// ML API
// ============================================================================

typedef void* neurox_ml_model_t;

// Model loading
neurox_ml_model_t neurox_ml_load_model(neurox_handle_t handle, const char* model_path);
void neurox_ml_free_model(neurox_ml_model_t model);

// Inference
int neurox_ml_predict(neurox_ml_model_t model, 
                     const float* input, size_t input_size,
                     float* output, size_t output_size);

int neurox_ml_predict_image(neurox_ml_model_t model,
                            const uint8_t* image_data, int width, int height,
                            float* output, size_t output_size);

// ============================================================================
// VISION API
// ============================================================================

typedef void* neurox_camera_t;
typedef void* neurox_image_t;

// Camera
neurox_camera_t neurox_camera_init(neurox_handle_t handle, const char* camera_name);
void neurox_camera_free(neurox_camera_t camera);
neurox_image_t neurox_camera_capture(neurox_camera_t camera);

// Image
void neurox_image_free(neurox_image_t image);
int neurox_image_get_width(neurox_image_t image);
int neurox_image_get_height(neurox_image_t image);
const uint8_t* neurox_image_get_data(neurox_image_t image);

// Object detection
typedef struct {
    float x, y, width, height;
    int class_id;
    float confidence;
    char label[64];
} neurox_detection_t;

int neurox_vision_detect_objects(neurox_image_t image,
                                 neurox_detection_t* detections,
                                 size_t max_detections);

// ============================================================================
// SWARM API
// ============================================================================

// Swarm coordination
int neurox_swarm_broadcast(neurox_handle_t handle, const char* message);
int neurox_swarm_send(neurox_handle_t handle, int target_id, const char* message);
int neurox_swarm_get_neighbors(neurox_handle_t handle, int* neighbor_ids, size_t max_neighbors);
int neurox_swarm_get_neighbor_count(neurox_handle_t handle);

// Formation
int neurox_swarm_set_formation(neurox_handle_t handle, const char* formation_type, float spacing);
int neurox_swarm_get_target_pose(neurox_handle_t handle, neurox_pose_t* target);

// ============================================================================
// ERROR HANDLING
// ============================================================================

// Error codes
typedef enum {
    NEUROX_OK = 0,
    NEUROX_ERROR_INVALID_HANDLE = -1,
    NEUROX_ERROR_INVALID_PARAM = -2,
    NEUROX_ERROR_NOT_FOUND = -3,
    NEUROX_ERROR_TIMEOUT = -4,
    NEUROX_ERROR_HARDWARE = -5,
    NEUROX_ERROR_NETWORK = -6,
    NEUROX_ERROR_ML = -7,
    NEUROX_ERROR_SAFETY = -8,
} neurox_error_t;

// Error info
const char* neurox_get_last_error(neurox_handle_t handle);
int neurox_get_last_error_code(neurox_handle_t handle);
void neurox_clear_error(neurox_handle_t handle);

// ============================================================================
// LOGGING API
// ============================================================================

typedef enum {
    NEUROX_LOG_TRACE,
    NEUROX_LOG_DEBUG,
    NEUROX_LOG_INFO,
    NEUROX_LOG_WARN,
    NEUROX_LOG_ERROR,
} neurox_log_level_t;

void neurox_set_log_level(neurox_log_level_t level);
void neurox_log(neurox_log_level_t level, const char* message);

typedef void (*neurox_log_callback_t)(neurox_log_level_t level, 
                                      const char* message, 
                                      void* user_data);

void neurox_set_log_callback(neurox_log_callback_t callback, void* user_data);

// ============================================================================
// UTILITY API
// ============================================================================

// Version info
const char* neurox_get_version(void);
int neurox_get_version_major(void);
int neurox_get_version_minor(void);
int neurox_get_version_patch(void);

// Platform info
const char* neurox_get_platform(void);
const char* neurox_get_build_info(void);

// Memory management helpers
void neurox_free_string(const char* str);
void neurox_free_array(void* array);

#ifdef __cplusplus
}
#endif

#endif // NEUROX_FFI_H
