# ESP32 Web Radio

A web-configurable internet radio based on the ESP32, written in C++ using the Arduino framework. The radio streams audio over Wi-Fi and provides a built-in web interface for configuring network credentials and the radio station.

## Features

* **Internet radio streaming** using the [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) library.
* **Web-based Wi-Fi configuration** to enter and save Wi-Fi credentials without modifying the source code.
* **Access Point fallback:** If no Wi-Fi credentials are stored or the connection fails, the ESP32 starts its own Wi-Fi access point.
* **Web-based radio station configuration** to change the stream URL from a browser.
* **Persistent settings:** Wi-Fi credentials and the selected radio station are stored in the ESP32's non-volatile storage (NVS).
* **Default radio station:** A factory-default stream URL is provided in `config.h`.
* **Reset to default station:** Clear the stored station URL and restore the factory default.
* **mDNS support:** Access the device using its hostname on supported networks.
* **Status LED:** The Wi-Fi status pin blinks in Access Point mode and stays on when connected to Wi-Fi.
* 
## Hardware

The project uses an ESP32 board and an I2S-compatible audio DAC or amplifier (PCM5102A, CS4344, PT8211 and MAX98357A).

The following GPIO assignments are defined in `config.h`:

| Function                   | GPIO |
| -------------------------- | ---: |
| I2S Data Out (DOUT)        |   25 |
| I2S Bit Clock (BCLK)       |   33 |
| I2S Left/Right Clock (LRC) |   26 |
| Audio Mute Control         |   32 |
| Wi-Fi Status LED           |    2 |

Adjust these pins in `config.h` to match your hardware. GPIO 2 is a strapping pin on many ESP32 boards, so check your board's documentation before connecting external circuitry to it.

### Required hardware

* ESP32 development board
* I2S-compatible DAC or amplifier (PCM5102A, CS4344, PT8211 and MAX98357A)
* Speaker or audio output system

**Note:** The I2S DAC/amplifier wiring and mute circuit depend on the specific hardware used.

## Software Requirements

* [Arduino IDE](https://www.arduino.cc/en/software) Board "esp32 by Espressif Systems" version 3.2.1
* [ESP32-audioI2S library](https://github.com/schreibfaul1/ESP32-audioI2S) version 3.2.1

The project also uses ESP32 Arduino libraries for Wi-Fi, HTTP server functionality, mDNS, and NVS preferences.

Install the required ESP32 board support and audio library before compiling the project.

## Installation

1. Clone or download this repository.
2. Open `ESP32_Radio.ino` in the Arduino IDE.
3. Install the required libraries and select the appropriate ESP32 board.
4. Open `config.h` and review the GPIO assignments, hostname, Access Point SSID, and default radio stream URL.
5. Compile and upload the sketch to your ESP32.
6. Open the serial monitor at **115200 baud** if serial debugging is enabled.

## First Startup

On startup, the ESP32 attempts to connect using the saved Wi-Fi credentials.

### If Wi-Fi credentials are available

The device attempts to connect to the configured network. If successful, it starts the web server and makes the radio available on the local network.

If mDNS is supported by your network, the device can be accessed at:

`http://ESP32-Radio.local`

Otherwise, use the IP address printed to the serial monitor.

### If Wi-Fi connection fails

The ESP32 starts an Access Point for configuration:

* **SSID:** `ESP32-Radio-Setup`
* **Password:** No password is configured by default.

Connect your phone or computer to this network, then open the device's setup page in a browser. If the page does not open automatically, try the Access Point IP address, typically `192.168.4.1`.

Use the web interface to enter your Wi-Fi network name and password. The device saves the credentials and restarts to connect to the configured network.

> **Security note:** The configuration Access Point is open by default. Anyone within range may be able to connect to it. Use the device in a trusted environment, and consider adding Access Point authentication if it will be deployed in an untrusted location.

## Configuring the Radio Station

Once the device is accessible through its web interface, you can configure the radio stream URL.

1. Open the radio's configuration page.
2. Enter a stream URL beginning with `http://` or `https://`.
3. Save the configuration.
4. The device stores the URL and restarts to apply the new station.

The selected URL is stored in NVS.

### Default Station

The factory-default stream URL is defined in `config.h`:

```cpp
static const char* const WEB_RADIO_URL =
    "https://streams.fluxfm.de/live/mp3-320/streams.fluxfm.de";
```

If no custom station URL is stored, the radio uses this default.

Use the reset option in the web interface to clear the stored station URL and return to the factory default.

The stream URL must point to a compatible audio stream supported by the installed ESP32-audioI2S library.

## Configuration Reference

The main settings are located in `config.h`.

| Setting                | Description                                             |
| ---------------------- | ------------------------------------------------------- |
| `DEBUG_SERIAL`         | Enables or disables serial debug output                 |
| `I2S_DOUT`             | I2S audio data output pin                               |
| `I2S_BCLK`             | I2S bit clock pin                                       |
| `I2S_LRC`              | I2S left/right clock pin                                |
| `MUTE_PIN`             | Audio mute-control pin                                  |
| `WIFI_PIN`             | Wi-Fi status LED pin                                    |
| `AP_SSID`              | SSID of the configuration Access Point                  |
| `DEVICE_HOSTNAME`      | Hostname used for network identification and mDNS       |
| `WIFI_CONNECT_TIMEOUT` | Time allowed for a Wi-Fi connection attempt, in seconds |
| `WEB_RADIO_URL`        | Factory-default radio stream URL                        |

### Serial Debugging

Debug output is controlled by `DEBUG_SERIAL`:

```cpp
#define DEBUG_SERIAL 1
```

Set it to `0` to disable debug output:

```cpp
#define DEBUG_SERIAL 0
```

The `DEBUG_PRINT(...)` and `DEBUG_PRINTLN(...)` macros centralize serial output, including messages from the audio library callbacks.

## Wi-Fi Status LED

The status pin indicates the network state:

* **Blinking:** The ESP32 is operating in Access Point mode. The pin switches on and off at 100 ms intervals.
* **On:** The ESP32 is connected to Wi-Fi.

## Project Structure

```text
ESP32_Radio/
├── ESP32_Radio.ino   # Main application and audio initialization
├── config.h          # Hardware pins and user-configurable defaults
├── radio.cpp         # Wi-Fi connection, NVS settings and status handling
├── radio.h           # Radio and Wi-Fi function declarations
├── httpserver.cpp    # Web interface and HTTP request handlers
├── httpserver.h      # HTTP server interface
└── version.h         # Project version
```

## Troubleshooting

**The configuration Access Point does not appear**

* Verify that the ESP32 starts successfully.
* Confirm that the device has not connected to a previously saved Wi-Fi network.
* Check the serial output if debugging is enabled.

**The web interface is unreachable**

* Check that your phone or computer is connected to the correct network.
* Use the device's IP address rather than the mDNS hostname.
* Ensure that the client is on the same local network as the ESP32 when using station mode.

**The radio does not play audio**

* Check the I2S wiring and GPIO assignments.
* Verify that the station URL is valid and points to a compatible audio stream.
* Check the serial output for connection or decoding messages.
* Confirm that the DAC/amplifier is powered and configured correctly.

**The status LED does not behave as expected**

* Check the `WIFI_PIN` assignment.
* Verify the LED polarity and wiring.
* Ensure the selected GPIO is suitable for your ESP32 board.

## License

The original source code of this project is licensed under the MIT License. See the [`LICENSE`](LICENSE) file for the full license text.

This project uses third-party components provided by the ESP32 Arduino Core and its dependencies, including the [ESP32-audioI2S library](https://github.com/schreibfaul1/ESP32-audioI2S) by schreibfaul1, which is licensed under the GNU General Public License v3.0 (GPL-3.0).

These third-party components remain subject to their respective licenses and copyright notices.

See [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) for an overview of the third-party components and their applicable licenses.

The MIT License of this project applies to its original source code and does not relicense or replace the licenses of third-party components.

## Acknowledgements

* [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) by schreibfaul1
* The Arduino and ESP32 open-source communities
