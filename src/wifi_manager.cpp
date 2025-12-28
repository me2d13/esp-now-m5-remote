#include "wifi_manager.h"
#include "config.h"

// WiFi connection state
WiFiStatus wifiStatus = WIFI_DISCONNECTED;

// Connection timeout and retry settings
const unsigned long WIFI_CONNECT_TIMEOUT = 10000;  // 10 seconds
unsigned long wifiConnectStartTime = 0;

// Blink state for connecting indicator
unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 500;  // 500ms blink interval
bool blinkState = false;

void initWiFiManager() {
    wifiStatus = WIFI_DISCONNECTED;
    WiFi.mode(WIFI_STA);  // Station mode
}

void startWiFiConnection() {
    if (wifiStatus == WIFI_CONNECTING || wifiStatus == WIFI_CONNECTED) {
        return;  // Already connecting or connected
    }
    
    wifiStatus = WIFI_CONNECTING;
    wifiConnectStartTime = millis();
    blinkState = false;
    lastBlinkTime = millis();
    
    // Start connection attempt
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void stopWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    wifiStatus = WIFI_DISCONNECTED;
}

void updateWiFi() {
    if (wifiStatus == WIFI_CONNECTING) {
        // Check if connected
        if (WiFi.status() == WL_CONNECTED) {
            wifiStatus = WIFI_CONNECTED;
            return;
        }
        
        // Check for timeout
        unsigned long elapsed = millis() - wifiConnectStartTime;
        if (elapsed > WIFI_CONNECT_TIMEOUT) {
            wifiStatus = WIFI_FAILED;
            WiFi.disconnect(true);
            return;
        }
        
        // Handle blinking for connecting state
        unsigned long currentTime = millis();
        if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
            blinkState = !blinkState;
            lastBlinkTime = currentTime;
        }
    }
}

WiFiStatus getWiFiStatus() {
    return wifiStatus;
}

bool isWiFiConnected() {
    return wifiStatus == WIFI_CONNECTED;
}

uint16_t getWiFiStatusColor() {
    const uint16_t INACTIVE_GRAY = 0x7BEF;  // Gray color for inactive state
    
    switch (wifiStatus) {
        case WIFI_CONNECTING:
            // Blink between gray and cyan
            return blinkState ? WIFI_STATUS_CONNECTED_COLOR : WIFI_STATUS_CONNECTING_COLOR;
        case WIFI_CONNECTED:
            return WIFI_STATUS_CONNECTED_COLOR;
        case WIFI_FAILED:
            return WIFI_STATUS_FAILED_COLOR;
        case WIFI_DISCONNECTED:
        default:
            return INACTIVE_GRAY;
    }
}
