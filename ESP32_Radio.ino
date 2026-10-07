#include "config.h"
#include "radio.h"
#include "httpserver.h"

#include <Arduino.h>
#include <WiFi.h>
#include <Audio.h>

Audio audio;

String streamURL;

void setup() {
    pinMode(MUTE_PIN, OUTPUT);
    pinMode(WIFI_PIN,OUTPUT);
    digitalWrite(MUTE_PIN, HIGH);
    digitalWrite(WIFI_PIN, LOW);
#if DEBUG_SERIAL
    Serial.begin(115200);
#endif

    // Connect to WiFi or start access point
    connectWiFi();

    // Always start the web server
    initHttpServer();

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(20); // 0...21

    // Load configured radio station.
    // If no station is stored, WEB_RADIO_URL from config.h is used.
    loadStationConfiguration(streamURL);

    DEBUG_PRINT("Radiostation: ");
    DEBUG_PRINTLN(streamURL);

    audio.connecttohost(streamURL.c_str());

    digitalWrite(MUTE_PIN, LOW);
}


void loop()
{
    audio.loop();
    updateWiFiLed();
    vTaskDelay(1);
}

// optional
void audio_info(const char *info){
    DEBUG_PRINT("info        "); DEBUG_PRINTLN(info);
}
void audio_id3data(const char *info){  //id3 metadata
    DEBUG_PRINT("id3data     "); DEBUG_PRINTLN(info);
}
void audio_eof_mp3(const char *info){  //end of file
    DEBUG_PRINT("eof_mp3     "); DEBUG_PRINTLN(info);
}
void audio_showstation(const char *info){
    DEBUG_PRINT("station     "); DEBUG_PRINTLN(info);
}
void audio_showstreaminfo(const char *info){
    DEBUG_PRINT("streaminfo  "); DEBUG_PRINTLN(info);
}
void audio_showstreamtitle(const char *info){
    DEBUG_PRINT("streamtitle "); DEBUG_PRINTLN(info);
}
void audio_bitrate(const char *info){
    DEBUG_PRINT("bitrate     "); DEBUG_PRINTLN(info);
}
void audio_commercial(const char *info){  //duration in sec
    DEBUG_PRINT("commercial  "); DEBUG_PRINTLN(info);
}
void audio_icyurl(const char *info){  //homepage
    DEBUG_PRINT("icyurl      "); DEBUG_PRINTLN(info);
}
void audio_lasthost(const char *info){  //stream URL played
    DEBUG_PRINT("lasthost    "); DEBUG_PRINTLN(info);
}
void audio_eof_speech(const char *info){
    DEBUG_PRINT("eof_speech  "); DEBUG_PRINTLN(info);
}
