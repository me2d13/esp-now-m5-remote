#include "sleep_manager.h"
#include "config.h"

// Track last activity time
unsigned long lastActivityTime = 0;

// Track last drawn indicator width to avoid flickering
int lastIndicatorWidth = -1;

// Sleep indicator properties
const int INDICATOR_HEIGHT = 3;
const uint16_t INDICATOR_COLOR = 0x07FF;  // Cyan (RGB565: 0, 255, 255)

void initSleepManager() {
    // Initialize the sleep timer
    lastActivityTime = millis();
    
    // Configure wake-up source - wake on button C/PWR press (GPIO35)
    // M5StickC PLUS2 Button C is the power button, most intuitive for wake-up
    // Note: ext1 with ESP_EXT1_WAKEUP_ALL_LOW requires ALL pins to be LOW simultaneously
    // ext1 doesn't support "ANY_LOW", only "ANY_HIGH" or "ALL_LOW"
    // Since buttons pull pins LOW when pressed, we use ext0 for single button wake
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, LOW);  // Wake when PWR button is pressed (LOW)
    
    // Reset last indicator width
    lastIndicatorWidth = -1;
}

void resetSleepTimer() {
    lastActivityTime = millis();
    // Force full redraw on next update
    lastIndicatorWidth = -1;
}

void drawSleepIndicator() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastActivityTime;
    unsigned long timeoutMs = SLEEP_TIMEOUT_SECONDS * 1000;
    
    // Calculate remaining time as percentage
    float remainingPercent = 1.0f - ((float)elapsedTime / (float)timeoutMs);
    if (remainingPercent < 0.0f) remainingPercent = 0.0f;
    if (remainingPercent > 1.0f) remainingPercent = 1.0f;
    
    // Get display dimensions
    int displayWidth = M5.Display.width();
    int displayHeight = M5.Display.height();
    
    // Calculate line width based on remaining time
    int lineWidth = (int)(displayWidth * remainingPercent);
    
    // Position at bottom of display
    int yPos = displayHeight - INDICATOR_HEIGHT;
    
    // Only update if width has changed
    if (lineWidth != lastIndicatorWidth) {
        if (lastIndicatorWidth == -1) {
            // First draw or full redraw - draw the entire line
            M5.Display.fillRect(0, yPos, lineWidth, INDICATOR_HEIGHT, INDICATOR_COLOR);
            if (lineWidth < displayWidth) {
                // Fill the rest with black
                M5.Display.fillRect(lineWidth, yPos, displayWidth - lineWidth, INDICATOR_HEIGHT, 0x0000);
            }
        } else if (lineWidth < lastIndicatorWidth) {
            // Line is shrinking - erase the portion that disappeared (from right side)
            M5.Display.fillRect(lineWidth, yPos, lastIndicatorWidth - lineWidth, INDICATOR_HEIGHT, 0x0000);
        } else {
            // Line is growing (after button press) - draw the new portion
            M5.Display.fillRect(lastIndicatorWidth, yPos, lineWidth - lastIndicatorWidth, INDICATOR_HEIGHT, INDICATOR_COLOR);
        }
        
        lastIndicatorWidth = lineWidth;
    }
}

void enterDeepSleep() {
    // Clear display before sleeping
    M5.Display.fillScreen(0x0000);  // Black
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(0xFFFF, 0x0000);  // White text on black background
    M5.Display.setCursor(40, 60);
    M5.Display.println("Sleeping...");
    
    delay(1000);  // Show message briefly
    
    // Turn off display
    M5.Display.sleep();
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

void updateSleepManager() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastActivityTime;
    unsigned long timeoutMs = SLEEP_TIMEOUT_SECONDS * 1000;
    
    // Check if timeout has been reached
    if (elapsedTime >= timeoutMs) {
        enterDeepSleep();
    }
    
    // Draw the sleep indicator
    drawSleepIndicator();
}
