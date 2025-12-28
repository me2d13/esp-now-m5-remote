# Agent Context for ESP-NOW M5 Remote Project

## Project Overview
This is a PlatformIO project for the **M5StickC PLUS2** device, creating a remote control that can operate in two modes:
- **WiFi Mode**: Connects to WiFi and MQTT broker to send commands
- **ESP-NOW Mode**: Will use ESP-NOW protocol for direct device communication (not yet implemented)

## Hardware Details
- **Device**: M5StickC PLUS2
- **MCU**: ESP32-PICO-V3-02
- **Display**: 240x135 LCD (landscape orientation, rotation 1)
- **Flash**: 8MB
- **PSRAM**: 2MB
- **Buttons**:
  - Button A (GPIO37): Front button - sends MQTT commands
  - Button B (GPIO39): Side button - short press scrolls left, long press (1s) switches modes
  - Button C (GPIO35): External button connector - scroll right (currently not working, needs physical button)

## PlatformIO Configuration
- **Platform**: espressif32
- **Board**: m5stick-c (with PLUS2 overrides)
- **Framework**: Arduino
- **Upload Port**: COM6
- **Monitor Port**: COM6
- **Upload Speed**: 1500000
- **Monitor Speed**: 115200

### Libraries Used
```ini
lib_deps =
    m5stack/M5Unified @ latest
    m5stack/M5GFX @ latest
    knolleary/PubSubClient @ ^2.8
```

### Build & Upload Commands
```bash
# Build only
pio run

# Build and upload
pio run --target upload

# Monitor serial output
pio device monitor
```

## Project Structure

### Core Modules

#### 1. **Battery Monitor** (`battery.h/cpp`)
- Reads battery voltage using M5.Power.getBatteryVoltage()
- Calculates percentage (3300mV-4200mV range, linear interpolation)
- Displays battery icon and percentage in **top right corner**
- Colors: Green (>50%), Yellow (20-50%), Red (<20%)
- Text size: 2

#### 2. **Sleep Manager** (`sleep_manager.h/cpp`)
- Auto deep sleep after configurable timeout (60 seconds in config.h)
- Visual countdown: cyan line at **bottom of display** shrinks from right to left
- Optimized drawing: only updates changed pixels to avoid flickering
- Wake source: Button A (GPIO37)
- Resets timer on any button press

#### 3. **State Manager** (`state_manager.h/cpp`)
- Manages device operating mode (WiFi/ESP-NOW)
- Persists mode to NVS using Preferences library
- Namespace: "device", Key: "mode"
- Default mode: WiFi

#### 4. **WiFi Manager** (`wifi_manager.h/cpp`)
- Manages WiFi connection
- Credentials from `secrets.h` (WIFI_SSID, WIFI_PASSWORD)
- Connection timeout: 10 seconds
- Status states: DISCONNECTED, CONNECTING, CONNECTED, FAILED
- Blink interval during connection: 500ms

#### 5. **MQTT Manager** (`mqtt_manager.h/cpp`)
- Uses PubSubClient library
- Configuration in `config.h`:
  - Server: 192.168.1.5
  - Port: 1883
  - Client Name: "M5-remote"
  - No authentication
- Connection timeout: 5 seconds
- Auto-connects when WiFi connects
- Auto-disconnects when WiFi disconnects
- Status states: MQTT_STATUS_DISCONNECTED, MQTT_STATUS_CONNECTING, MQTT_STATUS_CONNECTED, MQTT_STATUS_FAILED

#### 6. **Command Selector** (`command_selector.h/cpp`)
- Scrollable command list widget
- Commands defined in `config.h`: {2, 5, 90, 700}
- Circular scrolling enabled (configurable)
- Display layout:
  - Center: Large white text (current selection, text size 4)
  - Left: Small gray text (previous command, text size 2)
  - Right: Small gray text (next command, text size 2)
- Position: Center of screen, slightly below middle
- Sends MQTT to topic: `/cml/[selected_number]`

### Display Layout

```
WiFi  Mq  Now                    [battery] 100%
                                 (top bar)

                5    90    700
                (command selector)


━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                                 (sleep indicator)
```

#### Top Bar Indicators (left to right):
1. **WiFi** - WiFi connection status
2. **Mq** - MQTT connection status
3. **Now** - ESP-NOW mode indicator

**Indicator Colors**:
- Gray (0x7BEF): Inactive or disconnected
- Cyan (0x07FF): Active/connected
- Red (0xF800): Failed
- Blinking gray/cyan: Connecting (500ms interval)

All indicators use text size 2.

### Configuration Files

#### `include/config.h`
```cpp
const int SLEEP_TIMEOUT_SECONDS = 60;
const int COMMAND_LIST[] = {2, 5, 90, 700};
const int COMMAND_LIST_SIZE = sizeof(COMMAND_LIST) / sizeof(COMMAND_LIST[0]);
const bool COMMAND_CIRCULAR_SCROLL = true;

// MQTT config (defined in mqtt_manager.cpp)
extern const char* MQTT_SERVER;      // "192.168.1.5"
extern const int MQTT_PORT;          // 1883
extern const char* MQTT_CLIENT_NAME; // "M5-remote"
```

#### `include/secrets.h` (gitignored)
Contains WiFi credentials:
```cpp
const char* WIFI_SSID = "your-ssid";
const char* WIFI_PASSWORD = "your-password";
```

### Color Definitions (RGB565)
All colors use explicit RGB565 values to avoid library conflicts:
- Black: 0x0000
- White: 0xFFFF
- Red: 0xF800
- Green: 0x07E0
- Yellow: 0xFFE0
- Cyan: 0x07FF
- Gray: 0x7BEF

## Button Behavior

### Button A (Front)
- **Action**: Send MQTT command
- **Condition**: Only works in WiFi mode when MQTT is connected
- **Message**: Empty payload to `/cml/[selected_command]`
- **Side effect**: Resets sleep timer

### Button B (Side)
- **Short Press**: Scroll command left (select previous)
- **Long Press (1s)**: Toggle WiFi ↔ ESP-NOW modes
- **Side effect**: Resets sleep timer
- **Implementation**: Manual timing check (not using M5.BtnB.pressedFor() for short press)

### Button C (External GPIO35)
- **Action**: Scroll command right (select next)
- **Status**: Currently not working - requires physical button on external connector
- **Side effect**: Resets sleep timer

## Important Implementation Notes

1. **Color Setting**: Always use `setTextColor(color, 0x0000)` with background color to ensure proper display

2. **TFT Constants**: Don't use TFT_* constants (TFT_GREEN, etc.) - they conflict with library definitions. Use explicit RGB565 values.

3. **MQTT Enum Naming**: PubSubClient defines MQTT_DISCONNECTED, etc. as macros. Use MQTT_STATUS_* prefix for custom enums.

4. **Circular Dependencies**: Avoid including state_manager.h in wifi_manager.cpp or mqtt_manager.cpp. Use direct color values instead.

5. **Sleep Indicator Optimization**: Track last drawn width to only update changed pixels, preventing flicker.

6. **Button B Long Press**: Custom implementation using millis() timing because we need both short and long press detection.

7. **Extern Constants**: MQTT configuration constants are declared extern in config.h and defined in mqtt_manager.cpp to avoid multiple definition linker errors.

## Current Limitations & TODOs

### Known Issues
- **Button C not working**: Requires physical button on external GPIO35 connector
- **ESP-NOW mode**: Not yet implemented, only UI framework exists

### Future Enhancements
- Implement ESP-NOW functionality
- Add more sophisticated command management
- Add visual feedback for MQTT publish success/failure
- Implement battery refresh (currently only updates on startup)
- Add retry logic for failed MQTT connections
- Consider alternative button mapping if external button not available

## Testing & Debugging

### MQTT Testing
Monitor MQTT messages on broker:
```bash
mosquitto_sub -h 192.168.1.5 -t "/cml/#" -v
```

### Serial Monitor
Use PlatformIO serial monitor to see debug output:
```bash
pio device monitor
```

### Common Issues
1. **Compilation errors about TFT_* constants**: Use RGB565 values instead
2. **Multiple definition linker errors**: Check for const definitions in headers
3. **Button not responding**: Ensure M5.update() is called in loop
4. **Display flickering**: Minimize full-screen redraws, update only changed areas

## Development Workflow
1. Make code changes
2. Build: `pio run`
3. Upload: `pio run --target upload`
4. Test on device
5. Monitor serial output if needed

## Git Configuration
- `.gitignore` includes `include/secrets.h` to protect WiFi credentials
- PlatformIO build artifacts in `.pio/` are ignored

---
**Last Updated**: 2025-12-28
**Device Firmware Version**: Initial implementation with WiFi/MQTT support
