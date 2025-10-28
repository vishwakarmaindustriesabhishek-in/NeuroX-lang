#ifndef NEUROX_SWARM_H
#define NEUROX_SWARM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Robot ID and position
typedef struct {
    uint32_t id;
    float x, y, z;      // Position in meters
    float heading;      // Orientation in radians
    uint64_t timestamp_us;
} nrx_robot_pose_t;

// Swarm message types
typedef enum {
    NRX_MSG_POSE,           // Position update
    NRX_MSG_TASK_ASSIGN,    // Task assignment
    NRX_MSG_TASK_COMPLETE,  // Task completion
    NRX_MSG_DISCOVERY,      // Robot discovery
    NRX_MSG_HEARTBEAT,      // Keep-alive
    NRX_MSG_COMMAND,        // Direct command
    NRX_MSG_DATA,           // Generic data
} nrx_swarm_msg_type_t;

// Swarm message
typedef struct {
    nrx_swarm_msg_type_t type;
    uint32_t sender_id;
    uint32_t target_id;     // 0 = broadcast
    uint8_t *payload;
    size_t payload_len;
    uint64_t timestamp_us;
} nrx_swarm_message_t;

// Swarm configuration
typedef struct {
    uint32_t robot_id;
    const char *swarm_name;
    const char *discovery_topic;
    const char *command_topic;
    uint32_t heartbeat_interval_ms;
    uint32_t timeout_ms;
} nrx_swarm_config_t;

// Swarm handle
typedef struct nrx_swarm_t nrx_swarm_t;

// Message callback
typedef void (*nrx_swarm_msg_cb_t)(nrx_swarm_message_t *msg, void *user_data);

// Swarm initialization
nrx_swarm_t *nrx_swarm_init(nrx_swarm_config_t *config);
void nrx_swarm_deinit(nrx_swarm_t *swarm);

// Communication
int nrx_swarm_broadcast(nrx_swarm_t *swarm, nrx_swarm_msg_type_t type, 
                        const uint8_t *payload, size_t len);
int nrx_swarm_send(nrx_swarm_t *swarm, uint32_t target_id, nrx_swarm_msg_type_t type,
                   const uint8_t *payload, size_t len);
void nrx_swarm_set_callback(nrx_swarm_t *swarm, nrx_swarm_msg_cb_t callback, void *user_data);

// Robot discovery
int nrx_swarm_get_robots(nrx_swarm_t *swarm, uint32_t *robot_ids, size_t max_robots);
size_t nrx_swarm_get_robot_count(nrx_swarm_t *swarm);
bool nrx_swarm_is_robot_alive(nrx_swarm_t *swarm, uint32_t robot_id);

// Position sharing
int nrx_swarm_update_pose(nrx_swarm_t *swarm, nrx_robot_pose_t *pose);
int nrx_swarm_get_pose(nrx_swarm_t *swarm, uint32_t robot_id, nrx_robot_pose_t *pose);

// Formation control
typedef enum {
    NRX_FORMATION_LINE,
    NRX_FORMATION_CIRCLE,
    NRX_FORMATION_GRID,
    NRX_FORMATION_WEDGE,
    NRX_FORMATION_CUSTOM,
} nrx_formation_type_t;

typedef struct {
    nrx_formation_type_t type;
    float spacing;          // Distance between robots
    float orientation;      // Formation heading
    nrx_robot_pose_t leader_pose;
} nrx_formation_t;

int nrx_swarm_set_formation(nrx_swarm_t *swarm, nrx_formation_t *formation);
int nrx_swarm_get_target_pose(nrx_swarm_t *swarm, nrx_robot_pose_t *target);

// Task allocation
typedef struct {
    uint32_t task_id;
    uint32_t assigned_robot_id;
    float priority;
    float x, y;             // Task location
    uint8_t status;         // 0=pending, 1=assigned, 2=in_progress, 3=complete
} nrx_task_t;

int nrx_swarm_add_task(nrx_swarm_t *swarm, nrx_task_t *task);
int nrx_swarm_assign_tasks(nrx_swarm_t *swarm);  // Automatic allocation
int nrx_swarm_get_my_tasks(nrx_swarm_t *swarm, nrx_task_t *tasks, size_t max_tasks);
int nrx_swarm_complete_task(nrx_swarm_t *swarm, uint32_t task_id);

// Consensus algorithms
typedef struct {
    float value;
    uint32_t proposer_id;
    uint32_t round;
} nrx_consensus_value_t;

int nrx_swarm_consensus_propose(nrx_swarm_t *swarm, float value);
int nrx_swarm_consensus_get_result(nrx_swarm_t *swarm, float *result);

// Flocking behavior
typedef struct {
    float separation_weight;    // Avoid crowding
    float alignment_weight;     // Align with neighbors
    float cohesion_weight;      // Stay with group
    float max_speed;
    float neighbor_radius;
} nrx_flock_params_t;

int nrx_swarm_flock_update(nrx_swarm_t *swarm, nrx_flock_params_t *params,
                           float *velocity_x, float *velocity_y);

// Coverage control (area exploration)
typedef struct {
    float min_x, max_x;
    float min_y, max_y;
    float cell_size;
} nrx_coverage_area_t;

int nrx_swarm_coverage_init(nrx_swarm_t *swarm, nrx_coverage_area_t *area);
int nrx_swarm_coverage_get_target(nrx_swarm_t *swarm, float *x, float *y);
int nrx_swarm_coverage_mark_visited(nrx_swarm_t *swarm, float x, float y);
float nrx_swarm_coverage_get_progress(nrx_swarm_t *swarm);

// Collision avoidance
typedef struct {
    float min_distance;
    float max_speed_reduction;
} nrx_collision_params_t;

int nrx_swarm_avoid_collisions(nrx_swarm_t *swarm, nrx_collision_params_t *params,
                               float *velocity_x, float *velocity_y);

// Leader election
uint32_t nrx_swarm_elect_leader(nrx_swarm_t *swarm);
bool nrx_swarm_is_leader(nrx_swarm_t *swarm);

// Statistics
typedef struct {
    size_t robot_count;
    size_t messages_sent;
    size_t messages_received;
    float avg_distance_to_neighbors;
    float formation_error;
    uint32_t leader_id;
} nrx_swarm_stats_t;

void nrx_swarm_get_stats(nrx_swarm_t *swarm, nrx_swarm_stats_t *stats);

#endif // NEUROX_SWARM_H
