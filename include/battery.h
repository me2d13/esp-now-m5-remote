#ifndef BATTERY_H
#define BATTERY_H

#include <M5Unified.h>

/**
 * @brief Initialize battery monitoring
 */
void initBattery();

/**
 * @brief Get the current battery voltage in millivolts
 * @return Battery voltage in mV
 */
int getBatteryVoltage();

/**
 * @brief Get the battery level as a percentage (0-100)
 * @return Battery percentage (0-100)
 */
int getBatteryPercentage();

/**
 * @brief Display battery percentage in the top right corner of the display
 */
void displayBatteryLevel();

#endif // BATTERY_H
