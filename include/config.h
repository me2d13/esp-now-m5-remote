#ifndef CONFIG_H
#define CONFIG_H

// create secrets.h file with your WiFi credentials by copying secrets_template.h to secrets.h and filling in your credentials
#include "secrets.h"

// Sleep timeout in seconds - device will enter deep sleep after this period of inactivity
const int SLEEP_TIMEOUT_SECONDS = 60;

// Command list configuration
const int COMMAND_LIST[] = {2, 5, 90, 700};
const int COMMAND_LIST_SIZE = sizeof(COMMAND_LIST) / sizeof(COMMAND_LIST[0]);
const bool COMMAND_CIRCULAR_SCROLL = true;  // Enable circular scrolling

// MQTT Configuration (defined in mqtt_manager.cpp)
extern const char* MQTT_SERVER;
extern const int MQTT_PORT;
extern const char* MQTT_CLIENT_NAME;

#endif // CONFIG_H
