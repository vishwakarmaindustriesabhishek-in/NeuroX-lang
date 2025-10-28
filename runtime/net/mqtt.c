#define _POSIX_C_SOURCE 200809L

#include "mqtt.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Mock MQTT implementation for testing
// In production, this would use a real MQTT library like Paho or Mosquitto

struct nrx_mqtt_client_t {
    nrx_mqtt_config_t config;
    nrx_mqtt_state_t state;
    nrx_mqtt_stats_t stats;
    
    // Subscription list
    char **subscriptions;
    size_t subscription_count;
};

nrx_mqtt_client_t *nrx_mqtt_create(nrx_mqtt_config_t *config) {
    if (!config || !config->broker_url || !config->client_id) {
        return NULL;
    }
    
    nrx_mqtt_client_t *client = malloc(sizeof(nrx_mqtt_client_t));
    if (!client) return NULL;
    
    memset(client, 0, sizeof(nrx_mqtt_client_t));
    
    // Copy configuration
    client->config = *config;
    client->config.broker_url = strdup(config->broker_url);
    client->config.client_id = strdup(config->client_id);
    
    if (config->username) {
        client->config.username = strdup(config->username);
    }
    if (config->password) {
        client->config.password = strdup(config->password);
    }
    
    client->state = NRX_MQTT_DISCONNECTED;
    
    printf("[MQTT] Client created: broker=%s, client_id=%s\n",
           config->broker_url, config->client_id);
    
    return client;
}

void nrx_mqtt_destroy(nrx_mqtt_client_t *client) {
    if (!client) return;
    
    nrx_mqtt_disconnect(client);
    
    free((void *)client->config.broker_url);
    free((void *)client->config.client_id);
    free((void *)client->config.username);
    free((void *)client->config.password);
    
    for (size_t i = 0; i < client->subscription_count; i++) {
        free(client->subscriptions[i]);
    }
    free(client->subscriptions);
    
    free(client);
    
    printf("[MQTT] Client destroyed\n");
}

int nrx_mqtt_connect(nrx_mqtt_client_t *client) {
    if (!client) return -1;
    
    if (client->state == NRX_MQTT_CONNECTED) {
        return 0; // Already connected
    }
    
    client->state = NRX_MQTT_CONNECTING;
    
    printf("[MQTT] Connecting to %s...\n", client->config.broker_url);
    
    // Mock connection - in production, use real MQTT library
    client->state = NRX_MQTT_CONNECTED;
    
    printf("[MQTT] Connected successfully\n");
    
    return 0;
}

int nrx_mqtt_disconnect(nrx_mqtt_client_t *client) {
    if (!client) return -1;
    
    if (client->state == NRX_MQTT_DISCONNECTED) {
        return 0;
    }
    
    printf("[MQTT] Disconnecting...\n");
    
    client->state = NRX_MQTT_DISCONNECTED;
    
    return 0;
}

nrx_mqtt_state_t nrx_mqtt_get_state(nrx_mqtt_client_t *client) {
    return client ? client->state : NRX_MQTT_DISCONNECTED;
}

int nrx_mqtt_publish(nrx_mqtt_client_t *client, const char *topic,
                     const uint8_t *payload, size_t len, nrx_mqtt_qos_t qos) {
    if (!client || !topic || !payload) return -1;
    
    if (client->state != NRX_MQTT_CONNECTED) {
        printf("[MQTT] Cannot publish - not connected\n");
        return -1;
    }
    
    printf("[MQTT] Publish: topic='%s', len=%zu, qos=%d\n", topic, len, qos);
    
    // Mock publish
    client->stats.messages_sent++;
    client->stats.bytes_sent += len;
    
    return 0;
}

int nrx_mqtt_subscribe(nrx_mqtt_client_t *client, const char *topic, nrx_mqtt_qos_t qos) {
    if (!client || !topic) return -1;
    
    if (client->state != NRX_MQTT_CONNECTED) {
        printf("[MQTT] Cannot subscribe - not connected\n");
        return -1;
    }
    
    printf("[MQTT] Subscribe: topic='%s', qos=%d\n", topic, qos);
    
    // Add to subscription list
    size_t new_count = client->subscription_count + 1;
    client->subscriptions = realloc(client->subscriptions, new_count * sizeof(char *));
    client->subscriptions[client->subscription_count] = strdup(topic);
    client->subscription_count = new_count;
    
    return 0;
}

int nrx_mqtt_unsubscribe(nrx_mqtt_client_t *client, const char *topic) {
    if (!client || !topic) return -1;
    
    printf("[MQTT] Unsubscribe: topic='%s'\n", topic);
    
    // Remove from subscription list
    for (size_t i = 0; i < client->subscription_count; i++) {
        if (strcmp(client->subscriptions[i], topic) == 0) {
            free(client->subscriptions[i]);
            
            // Shift remaining subscriptions
            for (size_t j = i; j < client->subscription_count - 1; j++) {
                client->subscriptions[j] = client->subscriptions[j + 1];
            }
            
            client->subscription_count--;
            break;
        }
    }
    
    return 0;
}

void nrx_mqtt_loop(nrx_mqtt_client_t *client) {
    if (!client || client->state != NRX_MQTT_CONNECTED) {
        return;
    }
    
    // Mock message processing
    // In production, this would call the MQTT library's loop function
    // and invoke the message callback when messages arrive
}

void nrx_mqtt_get_stats(nrx_mqtt_client_t *client, nrx_mqtt_stats_t *stats) {
    if (client && stats) {
        *stats = client->stats;
    }
}
