#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <M5Unified.h>
#include <PubSubClient.h>
#include <WiFi.h>

// MQTT connection status
enum MQTTStatus {
    MQTT_STATUS_DISCONNECTED = 0,
    MQTT_STATUS_CONNECTING = 1,
    MQTT_STATUS_CONNECTED = 2,
    MQTT_STATUS_FAILED = 3
};

// UI Colors for MQTT status (same as WiFi)
const uint16_t MQTT_STATUS_CONNECTING_COLOR = 0x7BEF;  // Gray (blinking)
const uint16_t MQTT_STATUS_CONNECTED_COLOR = 0x07FF;   // Cyan
const uint16_t MQTT_STATUS_FAILED_COLOR = 0xF800;      // Red

/**
 * @brief Initialize MQTT manager
 */
void initMQTTManager();

/**
 * @brief Start MQTT connection attempt (only call when WiFi is connected)
 */
void startMQTTConnection();

/**
 * @brief Stop MQTT and disconnect
 */
void stopMQTT();

/**
 * @brief Update MQTT connection state (call in loop)
 */
void updateMQTT();

/**
 * @brief Get current MQTT status
 * @return Current MQTT status
 */
MQTTStatus getMQTTStatus();

/**
 * @brief Check if MQTT is connected
 * @return true if connected, false otherwise
 */
bool isMQTTConnected();

/**
 * @brief Get color for MQTT status indicator
 * @return RGB565 color based on current MQTT status
 */
uint16_t getMQTTStatusColor();

/**
 * @brief Publish a message to an MQTT topic
 * @param topic Topic to publish to
 * @param payload Payload to send (can be NULL for empty payload)
 * @param length Length of payload (0 for empty)
 * @return true if published successfully, false otherwise
 */
bool publishMQTT(const char* topic, const uint8_t* payload, unsigned int length);

#endif // MQTT_MANAGER_H
