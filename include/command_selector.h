#ifndef COMMAND_SELECTOR_H
#define COMMAND_SELECTOR_H

#include <M5Unified.h>

/**
 * @brief Initialize command selector
 */
void initCommandSelector();

/**
 * @brief Scroll to previous command (left)
 */
void scrollCommandLeft();

/**
 * @brief Scroll to next command (right)
 */
void scrollCommandRight();

/**
 * @brief Get currently selected command number
 * @return Selected command number
 */
int getSelectedCommand();

/**
 * @brief Draw the command selector widget on display
 */
void drawCommandSelector();

#endif // COMMAND_SELECTOR_H
