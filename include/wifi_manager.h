#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <M5Unified.h>
#include <WiFi.h>

// WiFi connection status
enum WiFiStatus {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTING = 1,
    WIFI_CONNECTED = 2,
    WIFI_FAILED = 3
};

// UI Colors for WiFi status
const uint16_t WIFI_STATUS_CONNECTING_COLOR = 0x7BEF;  // Gray (blinking)
const uint16_t WIFI_STATUS_CONNECTED_COLOR = 0x07FF;   // Cyan
const uint16_t WIFI_STATUS_FAILED_COLOR = 0xF800;      // Red

/**
 * @brief Initialize WiFi manager
 */
void initWiFiManager();

/**
 * @brief Start WiFi connection attempt
 */
void startWiFiConnection();

/**
 * @brief Stop WiFi and disconnect
 */
void stopWiFi();

/**
 * @brief Update WiFi connection state (call in loop)
 */
void updateWiFi();

/**
 * @brief Get current WiFi status
 * @return Current WiFi status
 */
WiFiStatus getWiFiStatus();

/**
 * @brief Check if WiFi is connected
 * @return true if connected, false otherwise
 */
bool isWiFiConnected();

/**
 * @brief Get color for WiFi status indicator
 * @return RGB565 color based on current WiFi status
 */
uint16_t getWiFiStatusColor();

#endif // WIFI_MANAGER_H
