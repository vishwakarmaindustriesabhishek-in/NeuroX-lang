#ifndef NEUROX_VISION_H
#define NEUROX_VISION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Image format
typedef enum {
    NRX_IMG_GRAY,
    NRX_IMG_RGB,
    NRX_IMG_BGR,
    NRX_IMG_RGBA,
} nrx_image_format_t;

// Image structure
typedef struct {
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    nrx_image_format_t format;
    size_t stride;
} nrx_image_t;

// Camera interface
typedef struct nrx_camera_t nrx_camera_t;

nrx_camera_t *nrx_camera_init(uint8_t port, uint32_t width, uint32_t height);
void nrx_camera_deinit(nrx_camera_t *cam);
bool nrx_camera_capture(nrx_camera_t *cam, nrx_image_t *img);

// Rectangle (for bounding boxes)
typedef struct {
    int32_t x, y;
    uint32_t width, height;
} nrx_rect_t;

// Point
typedef struct {
    int32_t x, y;
} nrx_point_t;

// Color
typedef struct {
    uint8_t r, g, b;
} nrx_color_t;

// Object detection result
typedef struct {
    nrx_rect_t bbox;
    uint32_t class_id;
    float confidence;
    char label[32];
} nrx_detection_t;

// Vision processing API

// Basic operations
void nrx_vision_resize(nrx_image_t *src, nrx_image_t *dst, uint32_t width, uint32_t height);
void nrx_vision_crop(nrx_image_t *src, nrx_image_t *dst, nrx_rect_t roi);
void nrx_vision_rotate(nrx_image_t *src, nrx_image_t *dst, float angle);
void nrx_vision_flip(nrx_image_t *img, bool horizontal, bool vertical);

// Filtering
void nrx_vision_blur(nrx_image_t *img, uint32_t kernel_size);
void nrx_vision_sharpen(nrx_image_t *img);
void nrx_vision_threshold(nrx_image_t *img, uint8_t threshold);
void nrx_vision_edge_detect(nrx_image_t *img);

// Color operations
void nrx_vision_rgb_to_gray(nrx_image_t *src, nrx_image_t *dst);
void nrx_vision_hsv_filter(nrx_image_t *img, nrx_color_t min, nrx_color_t max);

// Feature detection
int nrx_vision_find_contours(nrx_image_t *img, nrx_point_t **contours, size_t *count);
int nrx_vision_find_circles(nrx_image_t *img, nrx_point_t *centers, float *radii, size_t max_circles);
int nrx_vision_find_lines(nrx_image_t *img, nrx_point_t *starts, nrx_point_t *ends, size_t max_lines);

// Object detection (using TensorFlow Lite or similar)
typedef struct nrx_detector_t nrx_detector_t;

nrx_detector_t *nrx_detector_load(const char *model_path);
void nrx_detector_deinit(nrx_detector_t *detector);
int nrx_detector_detect(nrx_detector_t *detector, nrx_image_t *img, 
                        nrx_detection_t *detections, size_t max_detections);

// Face detection
int nrx_vision_detect_faces(nrx_image_t *img, nrx_rect_t *faces, size_t max_faces);

// QR/Barcode detection
typedef struct {
    char data[256];
    nrx_rect_t bbox;
} nrx_qrcode_t;

int nrx_vision_detect_qrcodes(nrx_image_t *img, nrx_qrcode_t *codes, size_t max_codes);

// Optical flow (motion tracking)
void nrx_vision_optical_flow(nrx_image_t *prev, nrx_image_t *curr, 
                              nrx_point_t *points, nrx_point_t *new_points, size_t count);

// Drawing utilities
void nrx_vision_draw_rect(nrx_image_t *img, nrx_rect_t rect, nrx_color_t color, uint32_t thickness);
void nrx_vision_draw_circle(nrx_image_t *img, nrx_point_t center, uint32_t radius, nrx_color_t color);
void nrx_vision_draw_line(nrx_image_t *img, nrx_point_t start, nrx_point_t end, nrx_color_t color);
void nrx_vision_draw_text(nrx_image_t *img, const char *text, nrx_point_t pos, nrx_color_t color);

#endif // NEUROX_VISION_H
