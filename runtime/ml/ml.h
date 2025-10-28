#ifndef NEUROX_ML_H
#define NEUROX_ML_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Tensor data types
typedef enum {
    NRX_DTYPE_FLOAT32,
    NRX_DTYPE_FLOAT16,
    NRX_DTYPE_INT8,
    NRX_DTYPE_UINT8,
    NRX_DTYPE_INT32,
} nrx_dtype_t;

// Tensor structure
typedef struct {
    void *data;
    uint32_t *shape;
    size_t ndim;
    nrx_dtype_t dtype;
    size_t size;
} nrx_tensor_t;

// ML Model handle
typedef struct nrx_model_t nrx_model_t;

// Model loading
nrx_model_t *nrx_model_load(const char *path);
nrx_model_t *nrx_model_load_tflite(const char *path);
nrx_model_t *nrx_model_load_onnx(const char *path);
void nrx_model_free(nrx_model_t *model);

// Model inference
int nrx_model_predict(nrx_model_t *model, nrx_tensor_t *input, nrx_tensor_t *output);
int nrx_model_predict_batch(nrx_model_t *model, nrx_tensor_t **inputs, 
                             nrx_tensor_t **outputs, size_t batch_size);

// Model info
size_t nrx_model_get_input_count(nrx_model_t *model);
size_t nrx_model_get_output_count(nrx_model_t *model);
void nrx_model_get_input_shape(nrx_model_t *model, size_t index, uint32_t *shape, size_t *ndim);
void nrx_model_get_output_shape(nrx_model_t *model, size_t index, uint32_t *shape, size_t *ndim);

// Tensor operations
nrx_tensor_t *nrx_tensor_create(uint32_t *shape, size_t ndim, nrx_dtype_t dtype);
void nrx_tensor_free(nrx_tensor_t *tensor);
void nrx_tensor_fill(nrx_tensor_t *tensor, float value);
void nrx_tensor_copy(nrx_tensor_t *src, nrx_tensor_t *dst);

// Pre-processing
void nrx_tensor_normalize(nrx_tensor_t *tensor, float mean, float std);
void nrx_tensor_resize(nrx_tensor_t *src, nrx_tensor_t *dst);

// Post-processing
int nrx_tensor_argmax(nrx_tensor_t *tensor);
void nrx_tensor_softmax(nrx_tensor_t *tensor);
void nrx_tensor_sigmoid(nrx_tensor_t *tensor);

// Classification result
typedef struct {
    uint32_t class_id;
    float confidence;
    char label[64];
} nrx_classification_t;

int nrx_classify(nrx_model_t *model, nrx_tensor_t *input, 
                 nrx_classification_t *results, size_t max_results);

// Regression
float nrx_regress(nrx_model_t *model, nrx_tensor_t *input);

// Anomaly detection
bool nrx_detect_anomaly(nrx_model_t *model, nrx_tensor_t *input, float threshold);

// Time series prediction
int nrx_predict_timeseries(nrx_model_t *model, float *history, size_t history_len,
                           float *predictions, size_t prediction_len);

// Reinforcement learning (policy network)
typedef struct {
    uint32_t action;
    float value;
    float *action_probs;
    size_t num_actions;
} nrx_rl_output_t;

int nrx_rl_select_action(nrx_model_t *policy, nrx_tensor_t *state, nrx_rl_output_t *output);

// Model quantization (for embedded deployment)
int nrx_model_quantize(nrx_model_t *model, nrx_dtype_t target_dtype);

// Hardware acceleration
typedef enum {
    NRX_ACCEL_CPU,
    NRX_ACCEL_GPU,
    NRX_ACCEL_NPU,
    NRX_ACCEL_DSP,
} nrx_accelerator_t;

int nrx_model_set_accelerator(nrx_model_t *model, nrx_accelerator_t accel);

// Performance profiling
typedef struct {
    float inference_time_ms;
    float preprocess_time_ms;
    float postprocess_time_ms;
    size_t memory_usage_bytes;
} nrx_ml_stats_t;

void nrx_model_get_stats(nrx_model_t *model, nrx_ml_stats_t *stats);

#endif // NEUROX_ML_H
