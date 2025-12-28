#include "command_selector.h"
#include "config.h"

// Current selected command index
int selectedCommandIndex = 0;

void initCommandSelector() {
    // Start with first command selected
    selectedCommandIndex = 0;
}

void scrollCommandLeft() {
    if (COMMAND_LIST_SIZE == 0) return;
    
    selectedCommandIndex--;
    
    if (selectedCommandIndex < 0) {
        if (COMMAND_CIRCULAR_SCROLL) {
            selectedCommandIndex = COMMAND_LIST_SIZE - 1;
        } else {
            selectedCommandIndex = 0;  // Stay at first
        }
    }
}

void scrollCommandRight() {
    if (COMMAND_LIST_SIZE == 0) return;
    
    selectedCommandIndex++;
    
    if (selectedCommandIndex >= COMMAND_LIST_SIZE) {
        if (COMMAND_CIRCULAR_SCROLL) {
            selectedCommandIndex = 0;
        } else {
            selectedCommandIndex = COMMAND_LIST_SIZE - 1;  // Stay at last
        }
    }
}

int getSelectedCommand() {
    if (COMMAND_LIST_SIZE == 0) return 0;
    return COMMAND_LIST[selectedCommandIndex];
}

void drawCommandSelector() {
    if (COMMAND_LIST_SIZE == 0) return;
    
    // Get display dimensions
    int displayWidth = M5.Display.width();
    int displayHeight = M5.Display.height();
    
    // Calculate vertical center position (below status bar)
    int centerY = displayHeight / 2 + 10;  // Slightly below center
    int centerX = displayWidth / 2;
    
    // Clear the command selector area
    M5.Display.fillRect(0, centerY - 30, displayWidth, 60, 0x0000);  // Black background
    
    // Get current, previous, and next commands
    int currentCmd = COMMAND_LIST[selectedCommandIndex];
    
    // Draw current command (large, center)
    M5.Display.setTextSize(4);
    M5.Display.setTextColor(0xFFFF, 0x0000);  // White text
    
    // Calculate text width for centering (approximate)
    char currentText[16];
    snprintf(currentText, sizeof(currentText), "%d", currentCmd);
    int textWidth = strlen(currentText) * 24;  // Approximate width for text size 4
    int currentX = centerX - (textWidth / 2);
    
    M5.Display.setCursor(currentX, centerY - 16);
    M5.Display.print(currentCmd);
    
    // Draw previous command (smaller, left) if exists
    if (COMMAND_LIST_SIZE > 1) {
        int prevIndex = selectedCommandIndex - 1;
        if (prevIndex < 0) {
            if (COMMAND_CIRCULAR_SCROLL) {
                prevIndex = COMMAND_LIST_SIZE - 1;
            } else {
                prevIndex = -1;  // Don't show
            }
        }
        
        if (prevIndex >= 0) {
            int prevCmd = COMMAND_LIST[prevIndex];
            M5.Display.setTextSize(2);
            M5.Display.setTextColor(0x7BEF, 0x0000);  // Gray text
            M5.Display.setCursor(10, centerY - 8);
            M5.Display.printf("%d", prevCmd);
        }
    }
    
    // Draw next command (smaller, right) if exists
    if (COMMAND_LIST_SIZE > 1) {
        int nextIndex = selectedCommandIndex + 1;
        if (nextIndex >= COMMAND_LIST_SIZE) {
            if (COMMAND_CIRCULAR_SCROLL) {
                nextIndex = 0;
            } else {
                nextIndex = -1;  // Don't show
            }
        }
        
        if (nextIndex >= 0) {
            int nextCmd = COMMAND_LIST[nextIndex];
            M5.Display.setTextSize(2);
            M5.Display.setTextColor(0x7BEF, 0x0000);  // Gray text
            
            // Calculate position for right side
            char nextText[16];
            snprintf(nextText, sizeof(nextText), "%d", nextCmd);
            int nextTextWidth = strlen(nextText) * 12;  // Approximate width for text size 2
            int nextX = displayWidth - nextTextWidth - 10;
            
            M5.Display.setCursor(nextX, centerY - 8);
            M5.Display.print(nextText);
        }
    }
}
