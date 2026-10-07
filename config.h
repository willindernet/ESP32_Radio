#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// Start of configuration
// ============================================================

// seriell debuging
#define DEBUG_SERIAL 1

// Debug-Ausgaben zentral steuern.
// Bei DEBUG_SERIAL 0 werden die Debug-Ausgaben vom Compiler entfernt.
#if DEBUG_SERIAL
    #define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
#endif

// Digital I/O used
#define I2S_DOUT      25  // DIN connection
#define I2S_BCLK      33  // Bit clock
#define I2S_LRC       26  // Left Right Clock
#define MUTE_PIN      32  // Output Pin for Muting
#define WIFI_PIN      2   // blink 100/100 ms in AP mode, on when connected

// Access Point for WIFI-Configuration
static const char* const AP_SSID = "ESP32-Radio-Setup";

// Network hostname and mDNS name
static const char* const DEVICE_HOSTNAME = "ESP32-Radio";

// Time to try connecting to Wi-Fi
static const unsigned long WIFI_CONNECT_TIMEOUT = 15;       // seconds

// URL radio station - Factory Settings
static const char* const WEB_RADIO_URL = "https://streams.fluxfm.de/live/mp3-320/streams.fluxfm.de";

// ============================================================
// End of configuration
// ============================================================

#endif
