#include "mqtt_manager.h"
#include "config.h"

// MQTT Configuration constants
const char* MQTT_SERVER = "192.168.1.5";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_NAME = "M5-remote";

// WiFi client for MQTT
WiFiClient wifiClient;

// MQTT client
PubSubClient mqttClient(wifiClient);

// MQTT connection state
MQTTStatus mqttStatus = MQTT_STATUS_DISCONNECTED;

// Connection timeout and retry settings
const unsigned long MQTT_CONNECT_TIMEOUT = 5000;  // 5 seconds
unsigned long mqttConnectStartTime = 0;

// Blink state for connecting indicator
unsigned long lastMqttBlinkTime = 0;
const unsigned long MQTT_BLINK_INTERVAL = 500;  // 500ms blink interval
bool mqttBlinkState = false;

void initMQTTManager() {
    mqttStatus = MQTT_STATUS_DISCONNECTED;
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void startMQTTConnection() {
    if (mqttStatus == MQTT_STATUS_CONNECTING || mqttStatus == MQTT_STATUS_CONNECTED) {
        return;  // Already connecting or connected
    }
    
    mqttStatus = MQTT_STATUS_CONNECTING;
    mqttConnectStartTime = millis();
    mqttBlinkState = false;
    lastMqttBlinkTime = millis();
    
    // Attempt to connect
    mqttClient.connect(MQTT_CLIENT_NAME);
}

void stopMQTT() {
    if (mqttClient.connected()) {
        mqttClient.disconnect();
    }
    mqttStatus = MQTT_STATUS_DISCONNECTED;
}

void updateMQTT() {
    if (mqttStatus == MQTT_STATUS_CONNECTING) {
        // Check if connected
        if (mqttClient.connected()) {
            mqttStatus = MQTT_STATUS_CONNECTED;
            return;
        }
        
        // Check for timeout
        unsigned long elapsed = millis() - mqttConnectStartTime;
        if (elapsed > MQTT_CONNECT_TIMEOUT) {
            mqttStatus = MQTT_STATUS_FAILED;
            return;
        }
        
        // Handle blinking for connecting state
        unsigned long currentTime = millis();
        if (currentTime - lastMqttBlinkTime >= MQTT_BLINK_INTERVAL) {
            mqttBlinkState = !mqttBlinkState;
            lastMqttBlinkTime = currentTime;
        }
        
        // Try to connect if not already attempting
        if (!mqttClient.connected() && mqttClient.state() != MQTT_STATUS_CONNECTING) {
            mqttClient.connect(MQTT_CLIENT_NAME);
        }
    } else if (mqttStatus == MQTT_STATUS_CONNECTED) {
        // Maintain connection
        mqttClient.loop();
        
        // Check if connection was lost
        if (!mqttClient.connected()) {
            mqttStatus = MQTT_STATUS_DISCONNECTED;
        }
    }
}

MQTTStatus getMQTTStatus() {
    return mqttStatus;
}

bool isMQTTConnected() {
    return mqttStatus == MQTT_STATUS_CONNECTED;
}

uint16_t getMQTTStatusColor() {
    const uint16_t INACTIVE_GRAY = 0x7BEF;  // Gray color for inactive state
    
    switch (mqttStatus) {
        case MQTT_STATUS_CONNECTING:
            // Blink between gray and cyan
            return mqttBlinkState ? MQTT_STATUS_CONNECTED_COLOR : MQTT_STATUS_CONNECTING_COLOR;
        case MQTT_STATUS_CONNECTED:
            return MQTT_STATUS_CONNECTED_COLOR;
        case MQTT_STATUS_FAILED:
            return MQTT_STATUS_FAILED_COLOR;
        case MQTT_STATUS_DISCONNECTED:
        default:
            return INACTIVE_GRAY;
    }
}

bool publishMQTT(const char* topic, const uint8_t* payload, unsigned int length) {
    // Only publish if connected
    if (!isMQTTConnected()) {
        return false;
    }
    
    // Publish the message
    return mqttClient.publish(topic, payload, length);
}
