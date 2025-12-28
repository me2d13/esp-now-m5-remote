#include "config.h"
#include "battery.h"
#include "sleep_manager.h"
#include "state_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "command_selector.h"
#include <M5Unified.h>

// Button B long press detection
const unsigned long LONG_PRESS_DURATION = 1000;  // 1 second in milliseconds

// Track previous WiFi status to detect state changes
WiFiStatus prevWiFiStatus = WIFI_DISCONNECTED;

// Track button B press time for long press detection
unsigned long btnBPressTime = 0;
bool btnBLongPressHandled = false;

void setup() {
    auto cfg = M5.config();
    
    // Enable external button (Button C on GPIO35)
    cfg.external_spk = false;  // Disable speaker to free up GPIO
    
    M5.begin(cfg); // Initializes display, buttons, and power management (GPIO4)

    M5.Display.setRotation(1);
    M5.Display.setTextSize(2);
    //M5.Display.println("M5StickC PLUS2");
    //M5.Display.println("Unified Library!");
    
    // Initialize state manager (loads persisted state)
    initStateManager();
    
    // Initialize WiFi manager
    initWiFiManager();
    
    // Initialize MQTT manager
    initMQTTManager();
    
    // Initialize command selector
    initCommandSelector();
    
    // If in WiFi mode, start connection
    if (getDeviceMode() == MODE_WIFI) {
        startWiFiConnection();
    }
    
    // Draw mode indicators in top left corner
    drawModeIndicators();
    
    // Display battery level in top right corner
    displayBatteryLevel();
    
    // Draw command selector widget
    drawCommandSelector();
    
    // Initialize sleep manager
    initSleepManager();
}

void loop() {
    M5.update(); // Required for button and power state updates

    // Button A - Send MQTT command
    if (M5.BtnA.wasPressed()) {
        // Reset sleep timer on button press
        resetSleepTimer();
        
        // Send MQTT message with selected command (only in WiFi mode)
        if (getDeviceMode() == MODE_WIFI && isMQTTConnected()) {
            int selectedCmd = getSelectedCommand();
            char topic[32];
            snprintf(topic, sizeof(topic), "/cml/%d", selectedCmd);
            publishMQTT(topic, NULL, 0);
        }
    }
    
    // Button B - Short press: scroll left, Long press: switch modes
    if (M5.BtnB.wasPressed()) {
        btnBPressTime = millis();
        btnBLongPressHandled = false;
        // Reset sleep timer on button press
        resetSleepTimer();
    }
    
    if (M5.BtnB.isPressed() && !btnBLongPressHandled) {
        unsigned long pressDuration = millis() - btnBPressTime;
        if (pressDuration >= LONG_PRESS_DURATION) {
            // Long press - switch modes
            btnBLongPressHandled = true;
            
            // Get current mode before toggle
            DeviceMode oldMode = getDeviceMode();
            
            // Toggle mode on long press
            toggleDeviceMode();
            
            // Handle WiFi and MQTT based on new mode
            DeviceMode newMode = getDeviceMode();
            if (newMode == MODE_WIFI && oldMode != MODE_WIFI) {
                // Switched to WiFi mode - start connection
                startWiFiConnection();
            } else if (newMode != MODE_WIFI && oldMode == MODE_WIFI) {
                // Switched away from WiFi mode - stop WiFi and MQTT
                stopMQTT();
                stopWiFi();
            }
            
            // Wait for button release to avoid multiple toggles
            while (M5.BtnB.isPressed()) {
                M5.update();
                delay(10);
            }
        }
    }
    
    if (M5.BtnB.wasReleased() && !btnBLongPressHandled) {
        // Short press - scroll command left
        scrollCommandLeft();
        drawCommandSelector();
    }
    
    // Button C - Scroll command right
    if (M5.BtnC.wasPressed()) {
        // Reset sleep timer on button press
        resetSleepTimer();
        scrollCommandRight();
        drawCommandSelector();
    }
    
    // Update WiFi state (handles connection, timeout, blinking)
    if (getDeviceMode() == MODE_WIFI) {
        updateWiFi();
        
        // Check if WiFi status changed
        WiFiStatus currentWiFiStatus = getWiFiStatus();
        if (currentWiFiStatus != prevWiFiStatus) {
            // WiFi status changed
            if (currentWiFiStatus == WIFI_CONNECTED && prevWiFiStatus != WIFI_CONNECTED) {
                // WiFi just connected - start MQTT
                startMQTTConnection();
            } else if (currentWiFiStatus != WIFI_CONNECTED && prevWiFiStatus == WIFI_CONNECTED) {
                // WiFi disconnected - stop MQTT
                stopMQTT();
            }
            prevWiFiStatus = currentWiFiStatus;
        }
        
        // Update MQTT if WiFi is connected
        if (isWiFiConnected()) {
            updateMQTT();
        }
        
        // Redraw mode indicators to show WiFi and MQTT status changes (blinking, etc.)
        drawModeIndicators();
    }
    
    // Update sleep manager (checks timeout and draws indicator)
    updateSleepManager();
    
    delay(100);  // Small delay to prevent excessive updates
}