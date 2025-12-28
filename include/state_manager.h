#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <M5Unified.h>
#include <Preferences.h>

// Device operating modes
enum DeviceMode {
    MODE_WIFI = 0,
    MODE_ESPNOW = 1
};

// UI Colors for mode indicators
const uint16_t MODE_INACTIVE_COLOR = 0x7BEF;  // Gray (RGB565: 128, 128, 128)
const uint16_t MODE_ACTIVE_COLOR = 0x07FF;    // Cyan (RGB565: 0, 255, 255)

// Device state structure
struct DeviceState {
    DeviceMode mode;  // Current operating mode (persistable)
    // Add more state variables here as needed
};

/**
 * @brief Initialize state manager and load persisted state
 */
void initStateManager();

/**
 * @brief Get current device state
 * @return Reference to current state
 */
DeviceState& getDeviceState();

/**
 * @brief Get current device mode
 * @return Current mode
 */
DeviceMode getDeviceMode();

/**
 * @brief Set device mode and persist it
 * @param mode New mode to set
 */
void setDeviceMode(DeviceMode mode);

/**
 * @brief Toggle between WiFi and ESP-NOW modes
 */
void toggleDeviceMode();

/**
 * @brief Draw mode indicators in top left corner
 */
void drawModeIndicators();

/**
 * @brief Save current state to persistent storage
 */
void saveState();

/**
 * @brief Load state from persistent storage
 */
void loadState();

#endif // STATE_MANAGER_H
