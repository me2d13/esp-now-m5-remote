#include "state_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

// Global device state
DeviceState deviceState;

// Preferences object for persistent storage
Preferences preferences;

// Namespace for preferences storage
const char* PREFS_NAMESPACE = "device";
const char* PREFS_MODE_KEY = "mode";

void initStateManager() {
    // Load persisted state
    loadState();
}

DeviceState& getDeviceState() {
    return deviceState;
}

DeviceMode getDeviceMode() {
    return deviceState.mode;
}

void setDeviceMode(DeviceMode mode) {
    if (deviceState.mode != mode) {
        deviceState.mode = mode;
        saveState();
        drawModeIndicators();  // Update display immediately
    }
}

void toggleDeviceMode() {
    if (deviceState.mode == MODE_WIFI) {
        setDeviceMode(MODE_ESPNOW);
    } else {
        setDeviceMode(MODE_WIFI);
    }
}

void drawModeIndicators() {
    // Position in top left corner
    int xPos = 4;
    int yPos = 4;
    
    // Set text size for indicators (same as battery indicator)
    M5.Display.setTextSize(2);
    
    // Draw WiFi indicator with status-based color
    uint16_t wifiColor;
    if (deviceState.mode == MODE_WIFI) {
        // In WiFi mode, use WiFi status color (connecting/connected/failed)
        wifiColor = getWiFiStatusColor();
    } else {
        // In ESP-NOW mode, WiFi is inactive (gray)
        wifiColor = MODE_INACTIVE_COLOR;
    }
    M5.Display.setTextColor(wifiColor, 0x0000);  // Text color with black background
    M5.Display.setCursor(xPos, yPos);
    M5.Display.print("WiFi");
    
    // Calculate position for MQTT indicator (after WiFi text + spacing)
    int wifiWidth = 4 * 12;  // "WiFi" = 4 chars * 12 pixels per char at text size 2
    int spacing = 12;  // Spacing between indicators
    int mqttXPos = xPos + wifiWidth + spacing;
    
    // Draw MQTT indicator with status-based color (only relevant in WiFi mode)
    uint16_t mqttColor;
    if (deviceState.mode == MODE_WIFI) {
        mqttColor = getMQTTStatusColor();
    } else {
        mqttColor = MODE_INACTIVE_COLOR;
    }
    M5.Display.setTextColor(mqttColor, 0x0000);
    M5.Display.setCursor(mqttXPos, yPos);
    M5.Display.print("Mq");
    
    // Calculate position for ESP-NOW indicator (after MQTT text + spacing)
    int mqttWidth = 2 * 12;  // "Mq" = 2 chars * 12 pixels per char at text size 2
    int espnowXPos = mqttXPos + mqttWidth + spacing;
    
    // Draw ESP-NOW indicator (shortened to "Now")
    uint16_t espnowColor = (deviceState.mode == MODE_ESPNOW) ? MODE_ACTIVE_COLOR : MODE_INACTIVE_COLOR;
    M5.Display.setTextColor(espnowColor, 0x0000);
    M5.Display.setCursor(espnowXPos, yPos);
    M5.Display.print("Now");
}

void saveState() {
    preferences.begin(PREFS_NAMESPACE, false);  // false = read/write mode
    preferences.putUChar(PREFS_MODE_KEY, (uint8_t)deviceState.mode);
    preferences.end();
}

void loadState() {
    preferences.begin(PREFS_NAMESPACE, true);  // true = read-only mode
    
    // Load mode with default to WiFi if not found
    deviceState.mode = (DeviceMode)preferences.getUChar(PREFS_MODE_KEY, (uint8_t)MODE_WIFI);
    
    preferences.end();
    
    // Validate loaded mode
    if (deviceState.mode != MODE_WIFI && deviceState.mode != MODE_ESPNOW) {
        deviceState.mode = MODE_WIFI;  // Fallback to default if invalid
    }
}
