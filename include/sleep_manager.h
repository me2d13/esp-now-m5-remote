#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <M5Unified.h>

/**
 * @brief Initialize sleep manager
 */
void initSleepManager();

/**
 * @brief Reset the sleep timer (call when user activity is detected)
 */
void resetSleepTimer();

/**
 * @brief Update sleep manager - checks timeout and draws indicator
 * Call this in loop()
 */
void updateSleepManager();

/**
 * @brief Draw the sleep timeout indicator line at the bottom of the display
 */
void drawSleepIndicator();

/**
 * @brief Enter deep sleep mode
 */
void enterDeepSleep();

#endif // SLEEP_MANAGER_H
