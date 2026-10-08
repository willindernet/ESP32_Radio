#ifndef RADIOSTATION_H
#define RADIOSTATION_H

#include <Arduino.h>

//---------------------------------------------------------------------------
// bool connectWiFi()
//---------------------------------------------------------------------------
// Description     | Connect to WIFI
// Parameter       | None
// Return value    | bool: 1 -> OK
//                 | bool: 0 -> NOK
//---------------------------------------------------------------------------
bool connectWiFi();


//---------------------------------------------------------------------------
// bool isWiFiConnected()
//---------------------------------------------------------------------------
// Description     | WIFI conection status
// Parameter       | None
// Return value    | bool: 1 -> connected
//                 | bool: 0 -> not connected
//---------------------------------------------------------------------------
bool isWiFiConnected();


//---------------------------------------------------------------------------
// bool isAccessPointMode()
//---------------------------------------------------------------------------
// Description     | check access point mode
// Parameter       | None
// Return value    | bool: 1 -> access point mode
//                 | bool: 0 -> not access point mode
//---------------------------------------------------------------------------
bool isAccessPointMode();


//---------------------------------------------------------------------------
// void updateWiFiLed()
//---------------------------------------------------------------------------
// Description     | Let LED blink 100/100 ms in AP mode, on when connected
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
void updateWiFiLed();


//---------------------------------------------------------------------------
// bool hasWiFiCredentials()
//---------------------------------------------------------------------------
// Description     | check for WIFI credentials available
// Parameter       | None
// Return value    | bool: 1 -> credentials available
//                 | bool: 0 -> no credentials available
//---------------------------------------------------------------------------
bool hasWiFiCredentials();


//---------------------------------------------------------------------------
// void saveWiFiCredentials(const String &ssid, const String &password)
//---------------------------------------------------------------------------
// Description     | Save WIFI login information
// Parameter       | String ssid: WIFI ssid
// Parameter       | String password: WIFI password
// Return value    | void
//---------------------------------------------------------------------------
void saveWiFiCredentials(const String &ssid, const String &password);


//---------------------------------------------------------------------------
// void clearWiFiCredentials()
//---------------------------------------------------------------------------
// Description     | Delete WIFI login information
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
void clearWiFiCredentials();


//---------------------------------------------------------------------------
// void loadStationConfiguration()
//---------------------------------------------------------------------------
// Description     | Load Station configuration
// Parameter       | String server: Address of Radio Station
// Return value    | void
//---------------------------------------------------------------------------
void loadStationConfiguration(String &server);


//---------------------------------------------------------------------------
// void saveStationConfiguration(const String &server)
//---------------------------------------------------------------------------
// Description     | Save Station Configuration to NVS
// Parameter       | String server: Address of Radio Station
// Return value    | The void saveStationConfiguratio result of the operation.
//---------------------------------------------------------------------------
void saveStationConfiguration(const String &server);


//---------------------------------------------------------------------------
// void resetStationConfiguration()
//---------------------------------------------------------------------------
// Description     | Reset the Station settings to factory defaults
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
void resetStationConfiguration();

#endif
