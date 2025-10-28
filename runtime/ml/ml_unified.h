#ifndef NEUROX_ML_UNIFIED_H
#define NEUROX_ML_UNIFIED_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Unified ML framework interface for NeuroX-lang
// Supports: TensorFlow Lite, ONNX, PyTorch (LibTorch), OpenCV DNN

// ML Framework types
typedef enum {
    NRX_ML_TFLITE,          // TensorFlow Lite (primary for embedded)
    NRX_ML_ONNX,            // ONNX Runtime (cross-platform)
    NRX_ML_PYTORCH,         // LibTorch (training & inference)
    NRX_ML_OPENCV_DNN,      // OpenCV DNN module
    NRX_ML_JAX,             // JAX (via Python bridge)
    NRX_ML_TRANSFORMERS,    // Hugging Face (via Python bridge)
} nrx_ml_framework_t;

// Hardware accelerator types
typedef enum {
    NRX_ACCEL_CPU,
    NRX_ACCEL_GPU,          // CUDA, OpenCL, Metal
    NRX_ACCEL_NPU,          // Neural Processing Unit
    NRX_ACCEL_DSP,          // Digital Signal Processor
    NRX_ACCEL_TPU,          // Tensor Processing Unit (Coral)
    NRX_ACCEL_TENSORRT,     // NVIDIA TensorRT
    NRX_ACCEL_OPENVINO,     // Intel OpenVINO
} nrx_ml_accelerator_t;

// Tensor data types
typedef enum {
    NRX_DTYPE_FLOAT32,
    NRX_DTYPE_FLOAT16,
    NRX_DTYPE_INT8,
    NRX_DTYPE_UINT8,
    NRX_DTYPE_INT32,
    NRX_DTYPE_INT64,
} nrx_dtype_t;

// Tensor structure
typedef struct {
    void *data;
    uint32_t *shape;
    size_t ndim;
    nrx_dtype_t dtype;
    size_t size;
    size_t stride[8];
} nrx_tensor_t;

// Model handle (opaque)
typedef struct nrx_ml_model_t nrx_ml_model_t;

// Model configuration
typedef struct {
    nrx_ml_framework_t framework;
    nrx_ml_accelerator_t accelerator;
    
    // Performance constraints
    uint32_t max_latency_ms;
    uint32_t min_fps;
    
    // Optimization
    bool use_quantization;
    bool use_pruning;
    int num_threads;
    
    // Memory
    size_t max_memory_mb;
    bool allow_fp16;
} nrx_ml_config_t;

// ============================================================================
// UNIFIED MODEL API
// ============================================================================

// Load model from file
nrx_ml_model_t *nrx_ml_load_model(const char *model_path, nrx_ml_config_t *config);

// Get model info
nrx_ml_framework_t nrx_ml_get_framework(nrx_ml_model_t *model);
size_t nrx_ml_get_input_count(nrx_ml_model_t *model);
size_t nrx_ml_get_output_count(nrx_ml_model_t *model);
void nrx_ml_get_input_shape(nrx_ml_model_t *model, size_t index, uint32_t *shape, size_t *ndim);
void nrx_ml_get_output_shape(nrx_ml_model_t *model, size_t index, uint32_t *shape, size_t *ndim);

// Inference
int nrx_ml_predict(nrx_ml_model_t *model, nrx_tensor_t *input, nrx_tensor_t *output);
int nrx_ml_predict_batch(nrx_ml_model_t *model, nrx_tensor_t **inputs, 
                         nrx_tensor_t **outputs, size_t batch_size);

// Async inference (non-blocking)
typedef void (*nrx_ml_callback_t)(nrx_tensor_t *output, void *user_data);
int nrx_ml_predict_async(nrx_ml_model_t *model, nrx_tensor_t *input, 
                         nrx_ml_callback_t callback, void *user_data);

// Cleanup
void nrx_ml_free_model(nrx_ml_model_t *model);

// ============================================================================
// TENSORFLOW LITE SPECIFIC
// ============================================================================

typedef struct nrx_tflite_model_t nrx_tflite_model_t;

nrx_tflite_model_t *nrx_tflite_load(const char *model_path);
void nrx_tflite_set_delegate(nrx_tflite_model_t *model, nrx_ml_accelerator_t accel);
int nrx_tflite_invoke(nrx_tflite_model_t *model, nrx_tensor_t *input, nrx_tensor_t *output);
void nrx_tflite_free(nrx_tflite_model_t *model);

// Quantization
int nrx_tflite_quantize_model(const char *input_path, const char *output_path, 
                              nrx_dtype_t target_dtype);

// ============================================================================
// ONNX RUNTIME SPECIFIC
// ============================================================================

typedef struct nrx_onnx_model_t nrx_onnx_model_t;

nrx_onnx_model_t *nrx_onnx_load(const char *model_path);
void nrx_onnx_set_execution_provider(nrx_onnx_model_t *model, nrx_ml_accelerator_t accel);
int nrx_onnx_run(nrx_onnx_model_t *model, nrx_tensor_t *input, nrx_tensor_t *output);
void nrx_onnx_free(nrx_onnx_model_t *model);

// Graph optimization
typedef enum {
    NRX_ONNX_OPT_NONE,
    NRX_ONNX_OPT_BASIC,
    NRX_ONNX_OPT_EXTENDED,
    NRX_ONNX_OPT_ALL,
} nrx_onnx_opt_level_t;

void nrx_onnx_set_optimization(nrx_onnx_model_t *model, nrx_onnx_opt_level_t level);

// ============================================================================
// PYTORCH (LIBTORCH) SPECIFIC
// ============================================================================

typedef struct nrx_pytorch_model_t nrx_pytorch_model_t;

nrx_pytorch_model_t *nrx_pytorch_load(const char *model_path);
nrx_pytorch_model_t *nrx_pytorch_load_jit(const char *torchscript_path);
int nrx_pytorch_forward(nrx_pytorch_model_t *model, nrx_tensor_t *input, nrx_tensor_t *output);
void nrx_pytorch_free(nrx_pytorch_model_t *model);

// Training mode
void nrx_pytorch_set_training(nrx_pytorch_model_t *model, bool training);
int nrx_pytorch_backward(nrx_pytorch_model_t *model, nrx_tensor_t *loss);
void nrx_pytorch_zero_grad(nrx_pytorch_model_t *model);

// Export
int nrx_pytorch_export_onnx(nrx_pytorch_model_t *model, const char *output_path);
int nrx_pytorch_export_torchscript(nrx_pytorch_model_t *model, const char *output_path);

// ============================================================================
// OPENCV DNN SPECIFIC
// ============================================================================

typedef struct nrx_opencv_dnn_t nrx_opencv_dnn_t;

nrx_opencv_dnn_t *nrx_opencv_dnn_load(const char *model_path, const char *config_path);
void nrx_opencv_dnn_set_backend(nrx_opencv_dnn_t *net, nrx_ml_accelerator_t backend);
int nrx_opencv_dnn_forward(nrx_opencv_dnn_t *net, nrx_tensor_t *input, nrx_tensor_t *output);
void nrx_opencv_dnn_free(nrx_opencv_dnn_t *net);

// Preprocessing
void nrx_opencv_blob_from_image(nrx_tensor_t *image, nrx_tensor_t *blob,
                                float scale, int width, int height,
                                float mean[3], bool swap_rb);

// ============================================================================
// HIGH-LEVEL TASK APIs
// ============================================================================

// Object Detection
typedef struct {
    float x, y, width, height;  // Bounding box
    uint32_t class_id;
    float confidence;
    char label[64];
} nrx_detection_t;

int nrx_ml_detect_objects(nrx_ml_model_t *model, nrx_tensor_t *image,
                          nrx_detection_t *detections, size_t max_detections,
                          float confidence_threshold);

// Image Classification
typedef struct {
    uint32_t class_id;
    float confidence;
    char label[64];
} nrx_classification_t;

int nrx_ml_classify(nrx_ml_model_t *model, nrx_tensor_t *image,
                   nrx_classification_t *results, size_t top_k);

// Pose Estimation
typedef struct {
    float x, y;
    float confidence;
} nrx_keypoint_t;

typedef struct {
    nrx_keypoint_t keypoints[17];  // COCO format
    float bbox[4];
    float confidence;
} nrx_pose_t;

int nrx_ml_estimate_pose(nrx_ml_model_t *model, nrx_tensor_t *image,
                         nrx_pose_t *poses, size_t max_poses);

// Semantic Segmentation
int nrx_ml_segment(nrx_ml_model_t *model, nrx_tensor_t *image,
                  nrx_tensor_t *mask);

// Reinforcement Learning
typedef struct {
    uint32_t action;
    float value;
    float *action_probs;
    size_t num_actions;
} nrx_rl_output_t;

int nrx_ml_rl_select_action(nrx_ml_model_t *policy, nrx_tensor_t *state,
                            nrx_rl_output_t *output);

// ============================================================================
// TENSOR OPERATIONS
// ============================================================================

nrx_tensor_t *nrx_tensor_create(uint32_t *shape, size_t ndim, nrx_dtype_t dtype);
void nrx_tensor_free(nrx_tensor_t *tensor);
void nrx_tensor_fill(nrx_tensor_t *tensor, float value);
void nrx_tensor_copy(nrx_tensor_t *src, nrx_tensor_t *dst);
void nrx_tensor_reshape(nrx_tensor_t *tensor, uint32_t *new_shape, size_t new_ndim);

// Preprocessing
void nrx_tensor_normalize(nrx_tensor_t *tensor, float mean, float std);
void nrx_tensor_normalize_per_channel(nrx_tensor_t *tensor, float *means, float *stds);
void nrx_tensor_resize(nrx_tensor_t *src, nrx_tensor_t *dst);

// Post-processing
int nrx_tensor_argmax(nrx_tensor_t *tensor);
void nrx_tensor_softmax(nrx_tensor_t *tensor);
void nrx_tensor_sigmoid(nrx_tensor_t *tensor);

// ============================================================================
// PERFORMANCE & PROFILING
// ============================================================================

typedef struct {
    float inference_time_ms;
    float preprocess_time_ms;
    float postprocess_time_ms;
    size_t memory_usage_bytes;
    uint32_t inference_count;
    float avg_fps;
} nrx_ml_stats_t;

void nrx_ml_get_stats(nrx_ml_model_t *model, nrx_ml_stats_t *stats);
void nrx_ml_reset_stats(nrx_ml_model_t *model);

// Benchmarking
typedef struct {
    float min_latency_ms;
    float max_latency_ms;
    float avg_latency_ms;
    float p50_latency_ms;
    float p95_latency_ms;
    float p99_latency_ms;
} nrx_ml_benchmark_t;

int nrx_ml_benchmark(nrx_ml_model_t *model, nrx_tensor_t *input,
                    int num_iterations, nrx_ml_benchmark_t *results);

// ============================================================================
// MODEL OPTIMIZATION
// ============================================================================

// Quantization
typedef enum {
    NRX_QUANT_DYNAMIC,      // Dynamic range quantization
    NRX_QUANT_INT8,         // Full integer quantization
    NRX_QUANT_FLOAT16,      // Half precision
} nrx_quant_type_t;

int nrx_ml_quantize(const char *input_model, const char *output_model,
                   nrx_quant_type_t quant_type, nrx_tensor_t **calibration_data,
                   size_t calibration_size);

// Pruning
int nrx_ml_prune(const char *input_model, const char *output_model,
                float sparsity);

// Knowledge Distillation
int nrx_ml_distill(nrx_ml_model_t *teacher, nrx_ml_model_t *student,
                  nrx_tensor_t **training_data, size_t data_size);

// ============================================================================
// UTILITIES
// ============================================================================

// Convert between frameworks
int nrx_ml_convert_pytorch_to_onnx(const char *pytorch_path, const char *onnx_path);
int nrx_ml_convert_onnx_to_tflite(const char *onnx_path, const char *tflite_path);

// Model validation
bool nrx_ml_validate_model(const char *model_path, nrx_ml_framework_t framework);

// Get supported accelerators
int nrx_ml_get_available_accelerators(nrx_ml_accelerator_t *accelerators, size_t max_count);

#endif // NEUROX_ML_UNIFIED_H
