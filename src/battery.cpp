#include "battery.h"

// Battery voltage thresholds for M5StickC PLUS2 (in mV)
// These values are typical for a single-cell LiPo battery
const int BATTERY_MAX_VOLTAGE = 4200;  // Fully charged
const int BATTERY_MIN_VOLTAGE = 3300;  // Empty (safe cutoff)

void initBattery() {
    // M5Unified handles battery initialization automatically
    // This function is here for future expansion if needed
}

int getBatteryVoltage() {
    // M5.Power.getBatteryVoltage() returns voltage in millivolts
    return M5.Power.getBatteryVoltage();
}

int getBatteryPercentage() {
    int voltage = getBatteryVoltage();
    
    // Handle edge cases
    if (voltage >= BATTERY_MAX_VOLTAGE) {
        return 100;
    }
    if (voltage <= BATTERY_MIN_VOLTAGE) {
        return 0;
    }
    
    // Linear interpolation between min and max voltage
    // This is a simplified approach; real LiPo discharge curves are non-linear
    int percentage = ((voltage - BATTERY_MIN_VOLTAGE) * 100) / 
                     (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE);
    
    // Clamp to 0-100 range
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    
    return percentage;
}

void displayBatteryLevel() {
    int batteryPercent = getBatteryPercentage();
    int batteryVoltage = getBatteryVoltage();
    
    // Set text properties for battery display - larger size for readability
    M5.Display.setTextSize(2);
    
    // Choose color based on battery level using RGB565 format
    uint16_t batteryColor;
    if (batteryPercent > 50) {
        batteryColor = 0x07E0;  // Green (RGB565: 0, 255, 0)
    } else if (batteryPercent > 20) {
        batteryColor = 0xFFE0;  // Yellow (RGB565: 255, 255, 0)
    } else {
        batteryColor = 0xF800;  // Red (RGB565: 255, 0, 0)
    }
    // Set text color with black background to ensure proper color display
    M5.Display.setTextColor(batteryColor, 0x0000);  // 0x0000 = Black
    
    // Position in top right corner
    // M5StickC PLUS2 display is 240x135 in landscape (rotation 1)
    int displayWidth = M5.Display.width();
    int displayHeight = M5.Display.height();
    
    // Format: "100%" or "50%" - calculate width for right alignment
    char batteryText[8];
    snprintf(batteryText, sizeof(batteryText), "%d%%", batteryPercent);
    
    // Calculate text width for right alignment (text size 2 = 12 pixels per char)
    int textWidth = strlen(batteryText) * 12;  // Approximate width for text size 2
    int xPos = displayWidth - textWidth - 4;  // 4 pixels padding from right edge
    int yPos = 4;  // 4 pixels padding from top
    
    // Draw battery percentage
    M5.Display.setCursor(xPos, yPos);
    M5.Display.print(batteryText);
    
    // Draw a larger battery icon (simple rectangle representation)
    int iconX = xPos - 30;  // More space for larger icon
    int iconY = yPos + 2;
    int iconWidth = 24;     // Larger icon
    int iconHeight = 10;    // Larger icon
    
    // Draw battery outline with thicker border (white = 0xFFFF)
    M5.Display.drawRect(iconX, iconY, iconWidth, iconHeight, 0xFFFF);
    M5.Display.drawRect(iconX + 1, iconY + 1, iconWidth - 2, iconHeight - 2, 0xFFFF);
    
    // Draw battery terminal (small rectangle on the left)
    M5.Display.fillRect(iconX - 3, iconY + 3, 3, 4, 0xFFFF);
    
    // Fill battery based on percentage
    int fillWidth = (iconWidth - 4) * batteryPercent / 100;
    if (fillWidth > 0) {
        M5.Display.fillRect(iconX + 2, iconY + 2, fillWidth, iconHeight - 4, batteryColor);
    }
}
