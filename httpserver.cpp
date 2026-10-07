#include "httpserver.h"
#include "radio.h"
#include "config.h"
#include "version.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

static WebServer httpServer(80);
static TaskHandle_t httpServerTaskHandle = nullptr;


//---------------------------------------------------------------------------
// String htmlEscape(const String &value)
//---------------------------------------------------------------------------
// Description     | HTML Utility: escaping HTML characters
// Parameter       | String value: character to escape
// Return value    | String: escaped character
//---------------------------------------------------------------------------
static String htmlEscape(const String &value)
{
    String result = value;

    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    result.replace("\"", "&quot;");

    return result;
}


//---------------------------------------------------------------------------
// String makePage()
//---------------------------------------------------------------------------
// Description     | generate HTML-Page
// Parameter       | None
// Return value    | String: generated HTML-Page
//---------------------------------------------------------------------------
static String makePage()
{
    String server;

    loadStationConfiguration(server);

    String html;

    html += "<!DOCTYPE html>";
    html += "<html lang='de'>";
    html += "<head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>ESP32-Radio</title>";

    html += "<style>";
    html += "body{font-family:Arial,sans-serif;max-width:700px;margin:40px auto;padding:20px;}";
    html += "h1{font-size:24px;}";
    html += "h2{margin-top:30px;}";
    html += "label{display:block;margin-top:15px;}";
    html += "input,select{width:100%;padding:10px;margin-top:5px;box-sizing:border-box;}";
    html += "button{margin-top:20px;padding:12px 20px;font-size:16px;}";
    html += ".status{padding:15px;background:#eee;margin:20px 0;}";
    html += ".section{border-top:1px solid #ccc;margin-top:30px;padding-top:10px;}";
    html += ".hint{font-size:14px;color:#555;}";
    html += "</style>";

    html += "</head>";
    html += "<body>";

    html += "<h1>ESP32 Radio</h1>";

    html += "<div class='status'>";

    if (isAccessPointMode())
    {
        html += "<b>WLAN-Modus:</b> Access Point<br>";
        html += "<b>SSID:</b> ";
        html += htmlEscape(AP_SSID);
        html += "<br>";
        html += "<b>IP-Adresse:</b> ";
        html += WiFi.softAPIP().toString();
        html += "<br>";
        html += "<b>Version:</b> ";
        html += PROJECT_VERSION;
    }
    else if (isWiFiConnected())
    {
        html += "<b>WLAN-Modus:</b> verbunden<br>";
        html += "<b>SSID:</b> ";
        html += htmlEscape(WiFi.SSID());
        html += "<br>";
        html += "<b>IP-Adresse:</b> ";
        html += WiFi.localIP().toString();
        html += "<br>";
        html += "<b>Version:</b> ";
        html += PROJECT_VERSION;
        html += "<br>";
        html += "<b>Signalstärke:</b> ";
        html += String(WiFi.RSSI());
        html += " dBm";
    }
    else
    {
        html += "<b>WLAN-Modus:</b> nicht verbunden";
    }

    html += "</div>";

    // --------------------------------------------------------
    // WLAN
    // --------------------------------------------------------

    html += "<h2>WLAN konfigurieren</h2>";

    html += "<form action='/save' method='POST'>";

    html += "<label>SSID</label>";
    html += "<input type='text' name='ssid' required>";

    html += "<label>Passwort</label>";
    html += "<input type='password' name='password' required>";

    html += "<button type='submit'>WLAN speichern und Neustart</button>";

    html += "</form>";

    html += "<form action='/clear' method='POST'>";
    html += "<button type='submit'>WLAN-Konfiguration löschen</button>";
    html += "</form>";


    if (isWiFiConnected())
    {
        // --------------------------------------------------------
        // Radiostation
        // --------------------------------------------------------

        html += "<div class='section'>";
        html += "<h2>Radiostation</h2>";

        html += "<form action='/stationsave' method='POST'>";

        html += "<label>URL</label>";
        html += "<input type='text' name='radio_url' value='";
        html += htmlEscape(server);
        html += "' required>";

        html += "<button type='submit'>Radiostation speichern und Neustart</button>";

        html += "</form>";

        html += "<form action='/stationreset' method='POST'>";
        html += "<button type='submit'>Werkseinstellungen für Radiostation laden</button>";
        html += "</form>";
        html += "</div>";
    }

    html += "</body>";
    html += "</html>";

    return html;
}


//---------------------------------------------------------------------------
// void handleRoot()
//---------------------------------------------------------------------------
// Description     | generate Homepage
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
static void handleRoot()
{
    httpServer.send(
        200,
        "text/html; charset=utf-8",
        makePage()
    );
}


//---------------------------------------------------------------------------
// void handleSave()
//---------------------------------------------------------------------------
// Description     | Save WIFI
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
static void handleSave()
{
    if (!httpServer.hasArg("ssid") || !httpServer.hasArg("password"))
    {
        httpServer.send(
            400,
            "text/plain; charset=utf-8",
            "SSID und Passwort muessen eingegeben werden."
        );

        return;
    }

    String ssid = httpServer.arg("ssid");
    String password = httpServer.arg("password");

    ssid.trim();

    if (ssid.length() == 0 || password.length() == 0)
    {
        httpServer.send(
            400,
            "text/html; charset=utf-8",
            "<!DOCTYPE html>"
            "<html lang='de'>"
            "<head>"
            "<meta charset='UTF-8'>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<title>ESP32-Radio</title>"
            "</head>"
            "<body>"
            "<h1>Fehler</h1>"
            "<p>SSID und Passwort duerfen nicht leer sein.</p>"
            "<p><a href='/'>Zurueck zur WLAN-Konfiguration</a></p>"
            "</body>"
            "</html>"
        );

        return;
    }

    saveWiFiCredentials(ssid, password);

    httpServer.send(
        200,
        "text/html; charset=utf-8",
        "<!DOCTYPE html>"
        "<html lang='de'>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>ESP32-Radio</title>"
        "</head>"
        "<body>"
        "<h1>WLAN gespeichert</h1>"
        "<p>Die neuen WLAN-Zugangsdaten wurden gespeichert.</p>"
        "<p>Der ESP32 startet jetzt neu.</p>"
        "</body>"
        "</html>"
    );

    delay(1000);

    ESP.restart();
}


//---------------------------------------------------------------------------
// void handleClear()
//---------------------------------------------------------------------------
// Description     | Delete WiFi configuration
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
static void handleClear()
{
    clearWiFiCredentials();

    httpServer.send(
        200,
        "text/html; charset=utf-8",
        "<!DOCTYPE html>"
        "<html lang='de'>"
        "<head><meta charset='UTF-8'></head>"
        "<body>"
        "<h1>WLAN-Konfiguration gelöscht</h1>"
        "<p>Der ESP32 startet neu und öffnet anschließend den "
        "Setup-Access-Point.</p>"
        "</body>"
        "</html>"
    );

    delay(1000);

    ESP.restart();
}


//---------------------------------------------------------------------------
// void handleStationSave()
//---------------------------------------------------------------------------
// Description     | Save Radio Station
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
static void handleStationSave()
{
    if (!httpServer.hasArg("radio_url"))
    {
        httpServer.send(
            400,
            "text/plain; charset=utf-8",
            "Radiostation fehlt."
        );

        return;
    }

    String server = httpServer.arg("radio_url");

    server.trim();

    if (!server.startsWith("http://") &&
    !server.startsWith("https://"))
    {
        httpServer.send(
            400,
            "text/html; charset=utf-8",
            "<!DOCTYPE html>"
            "<html lang='de'>"
            "<head><meta charset='UTF-8'>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<title>ESP32-Radio</title></head>"
            "<body>"
            "<h1>Fehler</h1>"
            "<p>Die Radiostation muss mit http:// oder https:// beginnen.</p>"
            "<p><a href='/'>Zurueck</a></p>"
            "</body>"
            "</html>"
        );

        return;
    }

    saveStationConfiguration(server);

    httpServer.send(
        200,
        "text/html; charset=utf-8",
        "<!DOCTYPE html>"
        "<html lang='de'>"
        "<head><meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>ESP32-Radio</title></head>"
        "<body>"
        "<h1>Radiostation gespeichert</h1>"
        "<p>Die neue Radiostation wurde gespeichert.</p>"
        "<p>Der ESP32 startet jetzt neu.</p>"
        "</body>"
        "</html>"
    );

    delay(1000);

    ESP.restart();
}


//---------------------------------------------------------------------------
// void handleStationReset()
//---------------------------------------------------------------------------
// Description     | Reset Radio Station to factory settings
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
static void handleStationReset()
{
    resetStationConfiguration();

    httpServer.send(
        200,
        "text/html; charset=utf-8",
        "<!DOCTYPE html>"
        "<html lang='de'>"
        "<head><meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>ESP32-Radio</title></head>"
        "<body>"
        "<h1>Werkseinstellungen geladen</h1>"
        "<p>Radiostation wurde auf den Wert aus "
        "config.h zurückgesetzt.</p>"
        "<p>Der ESP32 startet jetzt neu.</p>"
        "</body>"
        "</html>"
    );

    delay(1000);

    ESP.restart();
}


//---------------------------------------------------------------------------
// void httpServerTask(void *parameter)
//---------------------------------------------------------------------------
// Description     | HTTP server task
// Parameter       | parameter: void *
// Return value    | void
//---------------------------------------------------------------------------
static void httpServerTask(void *parameter)
{
    for (;;)
    {
        httpServer.handleClient();

        vTaskDelay(pdMS_TO_TICKS(2));
    }
}


//---------------------------------------------------------------------------
// void initHttpServer()
//---------------------------------------------------------------------------
// Description     | Initialize the HTTP server
// Parameter       | None
// Return value    | void
//---------------------------------------------------------------------------
void initHttpServer()
{
    httpServer.on("/", HTTP_GET, handleRoot);
    httpServer.on("/save", HTTP_POST, handleSave);
    httpServer.on("/clear", HTTP_POST, handleClear);

    httpServer.on("/stationsave", HTTP_POST, handleStationSave);
    httpServer.on("/stationreset", HTTP_POST, handleStationReset);

    httpServer.onNotFound([]()
    {
        httpServer.send(
            404,
            "text/plain; charset=utf-8",
            "Seite nicht gefunden."
        );
    });

    httpServer.begin();

    DEBUG_PRINTLN("HTTP-Server gestartet.");

    if (isAccessPointMode())
    {
        DEBUG_PRINT("Setup-Adresse: http://");
        DEBUG_PRINTLN(WiFi.softAPIP());
    }
    else if (isWiFiConnected())
    {
        DEBUG_PRINT("Webseite: http://");
        DEBUG_PRINTLN(WiFi.localIP());
    }

    xTaskCreatePinnedToCore(
        httpServerTask,
        "HttpServer",
        8192,
        nullptr,
        1,
        &httpServerTaskHandle,
        0
    );
}
