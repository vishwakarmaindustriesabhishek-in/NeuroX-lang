#ifndef NEUROX_HAL_H
#define NEUROX_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Platform identification
typedef enum {
    NRX_PLATFORM_LINUX,
    NRX_PLATFORM_ESP32,
    NRX_PLATFORM_STM32,
    NRX_PLATFORM_RP2040,
    NRX_PLATFORM_FREERTOS,
} nrx_platform_t;

nrx_platform_t nrx_hal_get_platform(void);

// GPIO
typedef enum {
    NRX_GPIO_MODE_INPUT,
    NRX_GPIO_MODE_OUTPUT,
    NRX_GPIO_MODE_INPUT_PULLUP,
    NRX_GPIO_MODE_INPUT_PULLDOWN,
} nrx_gpio_mode_t;

typedef enum {
    NRX_GPIO_LOW = 0,
    NRX_GPIO_HIGH = 1,
} nrx_gpio_state_t;

void nrx_gpio_init(uint8_t pin, nrx_gpio_mode_t mode);
void nrx_gpio_write(uint8_t pin, nrx_gpio_state_t state);
nrx_gpio_state_t nrx_gpio_read(uint8_t pin);
void nrx_gpio_toggle(uint8_t pin);

// PWM (for motors, servos)
void nrx_pwm_init(uint8_t pin, uint32_t frequency_hz);
void nrx_pwm_set_duty(uint8_t pin, float duty_percent);
void nrx_pwm_stop(uint8_t pin);

// ADC (for analog sensors)
void nrx_adc_init(uint8_t pin);
uint16_t nrx_adc_read(uint8_t pin);
float nrx_adc_read_voltage(uint8_t pin);

// UART
typedef struct nrx_uart_t nrx_uart_t;

nrx_uart_t *nrx_uart_init(uint8_t port, uint32_t baud_rate);
void nrx_uart_deinit(nrx_uart_t *uart);
int nrx_uart_write(nrx_uart_t *uart, const uint8_t *data, size_t len);
int nrx_uart_read(nrx_uart_t *uart, uint8_t *buffer, size_t len);
int nrx_uart_available(nrx_uart_t *uart);

// I2C
typedef struct nrx_i2c_t nrx_i2c_t;

nrx_i2c_t *nrx_i2c_init(uint8_t port, uint32_t frequency_hz);
void nrx_i2c_deinit(nrx_i2c_t *i2c);
int nrx_i2c_write(nrx_i2c_t *i2c, uint8_t addr, const uint8_t *data, size_t len);
int nrx_i2c_read(nrx_i2c_t *i2c, uint8_t addr, uint8_t *buffer, size_t len);

// SPI
typedef struct nrx_spi_t nrx_spi_t;

nrx_spi_t *nrx_spi_init(uint8_t port, uint32_t frequency_hz);
void nrx_spi_deinit(nrx_spi_t *spi);
int nrx_spi_transfer(nrx_spi_t *spi, const uint8_t *tx_data, uint8_t *rx_data, size_t len);

// Motor control
typedef struct {
    uint8_t pin_pwm;
    uint8_t pin_dir1;
    uint8_t pin_dir2;
    float power;
    bool reversed;
} nrx_motor_t;

void nrx_motor_init(nrx_motor_t *motor, uint8_t pin_pwm, uint8_t pin_dir1, uint8_t pin_dir2);
void nrx_motor_set_power(nrx_motor_t *motor, float power_percent);
void nrx_motor_stop(nrx_motor_t *motor);
void nrx_motor_brake(nrx_motor_t *motor);

// Servo control
typedef struct {
    uint8_t pin;
    float angle;
    float min_pulse_us;
    float max_pulse_us;
} nrx_servo_t;

void nrx_servo_init(nrx_servo_t *servo, uint8_t pin);
void nrx_servo_set_angle(nrx_servo_t *servo, float angle_deg);
void nrx_servo_set_pulse(nrx_servo_t *servo, float pulse_us);

// Sensor abstractions
typedef struct {
    void *context;
    float (*read_fn)(void *context);
} nrx_sensor_t;

void nrx_sensor_init(nrx_sensor_t *sensor, void *context, float (*read_fn)(void *));
float nrx_sensor_read(nrx_sensor_t *sensor);

#endif // NEUROX_HAL_H
