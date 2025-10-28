#include "hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linux mock implementation for testing

nrx_platform_t nrx_hal_get_platform(void) {
    return NRX_PLATFORM_LINUX;
}

// GPIO mock
static uint8_t gpio_states[256] = {0};

void nrx_gpio_init(uint8_t pin, nrx_gpio_mode_t mode) {
    printf("[HAL] GPIO init: pin=%d, mode=%d\n", pin, mode);
}

void nrx_gpio_write(uint8_t pin, nrx_gpio_state_t state) {
    gpio_states[pin] = state;
    printf("[HAL] GPIO write: pin=%d, state=%d\n", pin, state);
}

nrx_gpio_state_t nrx_gpio_read(uint8_t pin) {
    return gpio_states[pin];
}

void nrx_gpio_toggle(uint8_t pin) {
    gpio_states[pin] = !gpio_states[pin];
    printf("[HAL] GPIO toggle: pin=%d, new_state=%d\n", pin, gpio_states[pin]);
}

// PWM mock
void nrx_pwm_init(uint8_t pin, uint32_t frequency_hz) {
    printf("[HAL] PWM init: pin=%d, freq=%u Hz\n", pin, frequency_hz);
}

void nrx_pwm_set_duty(uint8_t pin, float duty_percent) {
    printf("[HAL] PWM set duty: pin=%d, duty=%.1f%%\n", pin, duty_percent);
}

void nrx_pwm_stop(uint8_t pin) {
    printf("[HAL] PWM stop: pin=%d\n", pin);
}

// ADC mock
void nrx_adc_init(uint8_t pin) {
    printf("[HAL] ADC init: pin=%d\n", pin);
}

uint16_t nrx_adc_read(uint8_t pin) {
    return 2048; // Mock 12-bit value
}

float nrx_adc_read_voltage(uint8_t pin) {
    return 1.65f; // Mock voltage (3.3V / 2)
}

// UART mock
struct nrx_uart_t {
    uint8_t port;
    uint32_t baud_rate;
};

nrx_uart_t *nrx_uart_init(uint8_t port, uint32_t baud_rate) {
    nrx_uart_t *uart = malloc(sizeof(nrx_uart_t));
    uart->port = port;
    uart->baud_rate = baud_rate;
    printf("[HAL] UART init: port=%d, baud=%u\n", port, baud_rate);
    return uart;
}

void nrx_uart_deinit(nrx_uart_t *uart) {
    if (uart) {
        printf("[HAL] UART deinit: port=%d\n", uart->port);
        free(uart);
    }
}

int nrx_uart_write(nrx_uart_t *uart, const uint8_t *data, size_t len) {
    printf("[HAL] UART write: port=%d, len=%zu\n", uart->port, len);
    return (int)len;
}

int nrx_uart_read(nrx_uart_t *uart, uint8_t *buffer, size_t len) {
    return 0; // No data available
}

int nrx_uart_available(nrx_uart_t *uart) {
    return 0;
}

// I2C mock
struct nrx_i2c_t {
    uint8_t port;
    uint32_t frequency_hz;
};

nrx_i2c_t *nrx_i2c_init(uint8_t port, uint32_t frequency_hz) {
    nrx_i2c_t *i2c = malloc(sizeof(nrx_i2c_t));
    i2c->port = port;
    i2c->frequency_hz = frequency_hz;
    printf("[HAL] I2C init: port=%d, freq=%u Hz\n", port, frequency_hz);
    return i2c;
}

void nrx_i2c_deinit(nrx_i2c_t *i2c) {
    if (i2c) {
        printf("[HAL] I2C deinit: port=%d\n", i2c->port);
        free(i2c);
    }
}

int nrx_i2c_write(nrx_i2c_t *i2c, uint8_t addr, const uint8_t *data, size_t len) {
    printf("[HAL] I2C write: port=%d, addr=0x%02X, len=%zu\n", i2c->port, addr, len);
    return (int)len;
}

int nrx_i2c_read(nrx_i2c_t *i2c, uint8_t addr, uint8_t *buffer, size_t len) {
    printf("[HAL] I2C read: port=%d, addr=0x%02X, len=%zu\n", i2c->port, addr, len);
    return 0;
}

// SPI mock
struct nrx_spi_t {
    uint8_t port;
    uint32_t frequency_hz;
};

nrx_spi_t *nrx_spi_init(uint8_t port, uint32_t frequency_hz) {
    nrx_spi_t *spi = malloc(sizeof(nrx_spi_t));
    spi->port = port;
    spi->frequency_hz = frequency_hz;
    printf("[HAL] SPI init: port=%d, freq=%u Hz\n", port, frequency_hz);
    return spi;
}

void nrx_spi_deinit(nrx_spi_t *spi) {
    if (spi) {
        printf("[HAL] SPI deinit: port=%d\n", spi->port);
        free(spi);
    }
}

int nrx_spi_transfer(nrx_spi_t *spi, const uint8_t *tx_data, uint8_t *rx_data, size_t len) {
    printf("[HAL] SPI transfer: port=%d, len=%zu\n", spi->port, len);
    return (int)len;
}

// Motor control
void nrx_motor_init(nrx_motor_t *motor, uint8_t pin_pwm, uint8_t pin_dir1, uint8_t pin_dir2) {
    motor->pin_pwm = pin_pwm;
    motor->pin_dir1 = pin_dir1;
    motor->pin_dir2 = pin_dir2;
    motor->power = 0.0f;
    motor->reversed = false;
    
    nrx_pwm_init(pin_pwm, 1000);
    nrx_gpio_init(pin_dir1, NRX_GPIO_MODE_OUTPUT);
    nrx_gpio_init(pin_dir2, NRX_GPIO_MODE_OUTPUT);
    
    printf("[HAL] Motor init: pwm=%d, dir1=%d, dir2=%d\n", pin_pwm, pin_dir1, pin_dir2);
}

void nrx_motor_set_power(nrx_motor_t *motor, float power_percent) {
    motor->power = power_percent;
    
    if (power_percent > 0) {
        nrx_gpio_write(motor->pin_dir1, motor->reversed ? NRX_GPIO_LOW : NRX_GPIO_HIGH);
        nrx_gpio_write(motor->pin_dir2, motor->reversed ? NRX_GPIO_HIGH : NRX_GPIO_LOW);
        nrx_pwm_set_duty(motor->pin_pwm, power_percent);
    } else if (power_percent < 0) {
        nrx_gpio_write(motor->pin_dir1, motor->reversed ? NRX_GPIO_HIGH : NRX_GPIO_LOW);
        nrx_gpio_write(motor->pin_dir2, motor->reversed ? NRX_GPIO_LOW : NRX_GPIO_HIGH);
        nrx_pwm_set_duty(motor->pin_pwm, -power_percent);
    } else {
        nrx_motor_stop(motor);
    }
    
    printf("[HAL] Motor set power: %.1f%%\n", power_percent);
}

void nrx_motor_stop(nrx_motor_t *motor) {
    nrx_gpio_write(motor->pin_dir1, NRX_GPIO_LOW);
    nrx_gpio_write(motor->pin_dir2, NRX_GPIO_LOW);
    nrx_pwm_set_duty(motor->pin_pwm, 0);
    motor->power = 0;
    printf("[HAL] Motor stop\n");
}

void nrx_motor_brake(nrx_motor_t *motor) {
    nrx_gpio_write(motor->pin_dir1, NRX_GPIO_HIGH);
    nrx_gpio_write(motor->pin_dir2, NRX_GPIO_HIGH);
    nrx_pwm_set_duty(motor->pin_pwm, 100);
    motor->power = 0;
    printf("[HAL] Motor brake\n");
}

// Servo control
void nrx_servo_init(nrx_servo_t *servo, uint8_t pin) {
    servo->pin = pin;
    servo->angle = 90.0f;
    servo->min_pulse_us = 1000.0f;
    servo->max_pulse_us = 2000.0f;
    
    nrx_pwm_init(pin, 50); // 50 Hz for servos
    printf("[HAL] Servo init: pin=%d\n", pin);
}

void nrx_servo_set_angle(nrx_servo_t *servo, float angle_deg) {
    servo->angle = angle_deg;
    
    // Map angle to pulse width
    float pulse_us = servo->min_pulse_us + 
                     (angle_deg / 180.0f) * (servo->max_pulse_us - servo->min_pulse_us);
    
    nrx_servo_set_pulse(servo, pulse_us);
    printf("[HAL] Servo set angle: %.1f deg (pulse: %.1f us)\n", angle_deg, pulse_us);
}

void nrx_servo_set_pulse(nrx_servo_t *servo, float pulse_us) {
    // Convert pulse width to duty cycle (50 Hz = 20ms period)
    float duty = (pulse_us / 20000.0f) * 100.0f;
    nrx_pwm_set_duty(servo->pin, duty);
}

// Sensor abstractions
void nrx_sensor_init(nrx_sensor_t *sensor, void *context, float (*read_fn)(void *)) {
    sensor->context = context;
    sensor->read_fn = read_fn;
}

float nrx_sensor_read(nrx_sensor_t *sensor) {
    if (sensor && sensor->read_fn) {
        return sensor->read_fn(sensor->context);
    }
    return 0.0f;
}
