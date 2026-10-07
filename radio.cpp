#include "radio.h"
#include "config.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <Preferences.h>

static Preferences preferences;

static bool wifiConnected = false;
static bool accessPointMode = false;


//---------------------------------------------------------------------------
// bool loadWiFiCredentials(String &ssid, String &password)
//---------------------------------------------------------------------------
// Description     | Load WIFI login credentials from NVS
// Parameter       | String ssid: WIFI ssid
// Parameter       | String password: WIFI password
// Return value    | bool: 1 -> OK
//                 | bool: 0 -> NOK
//---------------------------------------------------------------------------
static bool loadWiFiCredentials(String &ssid, String &password)
{
    preferences.begin("wifi", true);

    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");

    preferences.end();

    return ssid.length() > 0;
}


//---------------------------------------------------------------------------
// void saveWiFiCredentials(const String &ssid, const String &password)
//---------------------------------------------------------------------------
// Description     | Save WIFI login information
// Parameter       | String ssid: WIFI ssid
// Parameter       | String password: WIFI password
// Return value    | void
//---------------------------------------------------------------------------
void saveWiFiCredentials(const String &ssid, const String &password)
{
    preferences.begin("wifi", false);

    preferences.putString("ssid", ssid);
    preferences.putString("password", password);

    preferences.end();
}


//---------------------------------------------------------------------------
// void clearWiFiCredentials()
//---------------------------------------------------------------------------
// Description     | Delete WIFI login information
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
void clearWiFiCredentials()
{
    preferences.begin("wifi", false);

    preferences.clear();

    preferences.end();
}


//---------------------------------------------------------------------------
// void startAccessPoint()
//---------------------------------------------------------------------------
// Description     | Start the access point
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
static void startAccessPoint()
{
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Keine WLAN-Verbindung möglich.");
    DEBUG_PRINTLN("Starte Access Point...");

    WiFi.mode(WIFI_AP);

    if (WiFi.softAP(AP_SSID))
    {
        accessPointMode = true;
        wifiConnected = false;

        DEBUG_PRINTLN();
        DEBUG_PRINTLN("Access Point gestartet");
        DEBUG_PRINT("SSID: ");
        DEBUG_PRINTLN(AP_SSID);

        DEBUG_PRINT("IP-Adresse: ");
        DEBUG_PRINTLN(WiFi.softAPIP());
    }
    else
    {
        DEBUG_PRINTLN("Fehler beim Start des Access Points!");
    }
}


//---------------------------------------------------------------------------
// bool connectWiFi()
//---------------------------------------------------------------------------
// Description     | Connect to WIFI
// Parameter       | None
// Return value    | bool: 1 -> OK
//                 | bool: 0 -> NOK
//---------------------------------------------------------------------------
bool connectWiFi()
{
    String ssid;
    String password;

    if (!loadWiFiCredentials(ssid, password))
    {
        DEBUG_PRINTLN("Keine WLAN-Zugangsdaten gespeichert.");

        startAccessPoint();

        return false;
    }

    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Gespeicherte WLAN-Konfiguration gefunden.");
    DEBUG_PRINT("SSID: ");
    DEBUG_PRINTLN(ssid);

    WiFi.setHostname(DEVICE_HOSTNAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    DEBUG_PRINT("Verbinde mit WLAN");

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - startTime < WIFI_CONNECT_TIMEOUT * 1000UL)
    {
        delay(500);
        DEBUG_PRINT(".");
    }

    DEBUG_PRINTLN();

    if (WiFi.status() == WL_CONNECTED)
    {
        wifiConnected = true;
        accessPointMode = false;

        DEBUG_PRINTLN("WLAN verbunden");

        if (MDNS.begin(DEVICE_HOSTNAME))
        {
            DEBUG_PRINT("mDNS erreichbar unter: http://");
            DEBUG_PRINT(DEVICE_HOSTNAME);
            DEBUG_PRINTLN(".local");
        }
        else
        {
            DEBUG_PRINTLN("mDNS konnte nicht gestartet werden.");
        }

        DEBUG_PRINT("IP-Adresse: ");
        DEBUG_PRINTLN(WiFi.localIP());

        DEBUG_PRINT("Signalstärke: ");
        DEBUG_PRINT(WiFi.RSSI());
        DEBUG_PRINTLN(" dBm");

        return true;
    }

    DEBUG_PRINTLN("WLAN-Verbindung fehlgeschlagen.");

    WiFi.disconnect(true);

    startAccessPoint();

    return false;
}


//---------------------------------------------------------------------------
// void loadStationConfiguration()
//---------------------------------------------------------------------------
// Description     | Load Station configuration from NVS
//                 | If no custom values have been saved yet,
//                 | the factory settings from config.h are returned.
// Parameter       | String server: Address of Radio Station
// Return value    | void
//---------------------------------------------------------------------------
void loadStationConfiguration(
    String &server)
{
    if (!preferences.begin("stationcfg", true))
    {
        server = WEB_RADIO_URL;
        return;
    }

    server = preferences.getString("radio_url", WEB_RADIO_URL);
    preferences.end();
}


//---------------------------------------------------------------------------
// void saveStationConfiguration()
//---------------------------------------------------------------------------
// Description     | Save Station configuration
// Parameter       | String server: Address of Radio Station
// Return value    | void
//---------------------------------------------------------------------------
void saveStationConfiguration(
    const String &server)
{
    preferences.begin("stationcfg", false);

    preferences.putString("radio_url", server);

    preferences.end();
}


//---------------------------------------------------------------------------
// void resetStationConfiguration()
//---------------------------------------------------------------------------
// Description     | Reset the Radio Station to factory defaults
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
void resetStationConfiguration()
{
    preferences.begin("stationcfg", false);

    preferences.clear();

    preferences.end();
}


//---------------------------------------------------------------------------
// bool isWiFiConnected()
//---------------------------------------------------------------------------
// Description     | WIFI conection status
// Parameter       | None
// Return value    | bool: 1 -> connected
//                 | bool: 0 -> not connected
//---------------------------------------------------------------------------
bool isWiFiConnected()
{
    return wifiConnected;
}


//---------------------------------------------------------------------------
// bool isAccessPointMode()
//---------------------------------------------------------------------------
// Description     | check access point mode
// Parameter       | None
// Return value    | bool: 1 -> access point mode
//                 | bool: 0 -> not access point mode
//---------------------------------------------------------------------------
bool isAccessPointMode()
{
    return accessPointMode;
}

//---------------------------------------------------------------------------
// bool hasWiFiCredentials()
//---------------------------------------------------------------------------
// Description     | check for WIFI credentials available
// Parameter       | None
// Return value    | bool: 1 -> credentials available
//                 | bool: 0 -> no credentials available
//---------------------------------------------------------------------------
bool hasWiFiCredentials()
{
    String ssid;
    String password;
    return loadWiFiCredentials(ssid, password);
}


//---------------------------------------------------------------------------
// void updateWiFiLed()
//---------------------------------------------------------------------------
// Description     | Let LED blink 100/100 ms in AP mode, on when connected
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
void updateWiFiLed()
{
    static unsigned long lastBlinkTime = 0;
    static bool ledState = false;

    if (!accessPointMode)
    {
        // LED permanently on when connected to Wi-Fi.
        if (!ledState)
        {
            digitalWrite(WIFI_PIN, HIGH);
            ledState = true;
        }
        return;
    }

    // Blink every 100 ms while the ESP32 is in AP mode.
    unsigned long now = millis();

    if (now - lastBlinkTime >= 100)
    {
        lastBlinkTime = now;
        ledState = !ledState;
        digitalWrite(WIFI_PIN, ledState ? HIGH : LOW);
    }
}
