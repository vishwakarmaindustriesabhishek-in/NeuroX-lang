#ifndef NEUROX_MQTT_H
#define NEUROX_MQTT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// QoS levels
typedef enum {
    NRX_MQTT_QOS_0 = 0,  // At most once
    NRX_MQTT_QOS_1 = 1,  // At least once
    NRX_MQTT_QOS_2 = 2,  // Exactly once
} nrx_mqtt_qos_t;

// Connection state
typedef enum {
    NRX_MQTT_DISCONNECTED,
    NRX_MQTT_CONNECTING,
    NRX_MQTT_CONNECTED,
    NRX_MQTT_ERROR,
} nrx_mqtt_state_t;

// Message structure
typedef struct {
    const char *topic;
    const uint8_t *payload;
    size_t payload_len;
    nrx_mqtt_qos_t qos;
    bool retained;
} nrx_mqtt_message_t;

// Message callback
typedef void (*nrx_mqtt_message_cb_t)(const nrx_mqtt_message_t *message, void *user_data);

// Connection configuration
typedef struct {
    const char *broker_url;      // e.g., "mqtts://broker.local:8883"
    const char *client_id;
    const char *username;
    const char *password;
    
    bool use_tls;
    const char *ca_cert_path;
    const char *client_cert_path;
    const char *client_key_path;
    
    uint16_t keepalive_sec;
    bool clean_session;
    
    nrx_mqtt_message_cb_t message_callback;
    void *user_data;
} nrx_mqtt_config_t;

// MQTT client handle
typedef struct nrx_mqtt_client_t nrx_mqtt_client_t;

// MQTT API
nrx_mqtt_client_t *nrx_mqtt_create(nrx_mqtt_config_t *config);
void nrx_mqtt_destroy(nrx_mqtt_client_t *client);

int nrx_mqtt_connect(nrx_mqtt_client_t *client);
int nrx_mqtt_disconnect(nrx_mqtt_client_t *client);
nrx_mqtt_state_t nrx_mqtt_get_state(nrx_mqtt_client_t *client);

int nrx_mqtt_publish(nrx_mqtt_client_t *client, const char *topic,
                     const uint8_t *payload, size_t len, nrx_mqtt_qos_t qos);
int nrx_mqtt_subscribe(nrx_mqtt_client_t *client, const char *topic, nrx_mqtt_qos_t qos);
int nrx_mqtt_unsubscribe(nrx_mqtt_client_t *client, const char *topic);

void nrx_mqtt_loop(nrx_mqtt_client_t *client);

// Statistics
typedef struct {
    uint32_t messages_sent;
    uint32_t messages_received;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    uint32_t connection_errors;
    uint64_t last_message_time_us;
} nrx_mqtt_stats_t;

void nrx_mqtt_get_stats(nrx_mqtt_client_t *client, nrx_mqtt_stats_t *stats);

#endif // NEUROX_MQTT_H
