/**
 * @file    cloud_uploader.h
 * @brief   HTTP JSON uploader for ESP8266
 *
 * Packages a parsed sensor frame into a JSON payload and POSTs it to
 * the cloud backend REST endpoint defined in config.h.
 *
 * The JSON payload sent to the server:
 * {
 *   "device_id": "solar-charger-001",
 *   "location":  "Lab-A",
 *   "v_solar":   12450,
 *   "v_battery": 13200,
 *   "v_supercap":8500,
 *   "i_input":   1200,
 *   "i_output":  950,
 *   "p_input":   14940,
 *   "p_output":  12540,
 *   "e_input":   45200,
 *   "e_output":  37900,
 *   "efficiency":83,
 *   "sc_soc":    72,
 *   "sc_energy": 18170,
 *   "session_s": 38
 * }
 */

#ifndef CLOUD_UPLOADER_H
#define CLOUD_UPLOADER_H

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "config.h"

/* -------------------------------------------------------------------------
 * Parsed sensor data structure (matches STM32 JSON frame fields)
 * ---------------------------------------------------------------------- */
struct SensorFrame {
    int  v_solar;
    int  v_battery;
    int  v_supercap;
    int  i_input;
    int  i_output;
    long p_input;
    long p_output;
    long e_input;
    long e_output;
    int  efficiency;
    int  sc_soc;
    long sc_energy;
    long session_s;
    bool valid;   /* set to false if parse failed */
};

/* -------------------------------------------------------------------------
 * CloudUploader class
 * ---------------------------------------------------------------------- */
class CloudUploader {
public:
    /** Upload a parsed sensor frame to the backend.
     *  @return HTTP response code (200/201 = success), or negative on error. */
    static int upload(const SensorFrame &frame) {
        if (!frame.valid) {
            return -1;
        }

        /* Build JSON payload */
        char payload[HTTP_TX_BUF_LEN];
        snprintf(payload, sizeof(payload),
            "{"
            "\"device_id\":\"%s\","
            "\"location\":\"%s\","
            "\"v_solar\":%d,"
            "\"v_battery\":%d,"
            "\"v_supercap\":%d,"
            "\"i_input\":%d,"
            "\"i_output\":%d,"
            "\"p_input\":%ld,"
            "\"p_output\":%ld,"
            "\"e_input\":%ld,"
            "\"e_output\":%ld,"
            "\"efficiency\":%d,"
            "\"sc_soc\":%d,"
            "\"sc_energy\":%ld,"
            "\"session_s\":%ld"
            "}",
            DEVICE_ID,
            DEVICE_LOCATION,
            frame.v_solar,
            frame.v_battery,
            frame.v_supercap,
            frame.i_input,
            frame.i_output,
            frame.p_input,
            frame.p_output,
            frame.e_input,
            frame.e_output,
            frame.efficiency,
            frame.sc_soc,
            frame.sc_energy,
            frame.session_s);

        /* HTTP POST */
        WiFiClient client;
        HTTPClient http;

        String url = String("http://") + CLOUD_HOST + ":" + CLOUD_PORT + CLOUD_API_PATH;
        http.begin(client, url);
        http.addHeader("Content-Type", "application/json");

        int httpCode = http.POST(payload);

        if (httpCode > 0) {
            Serial.printf("[Cloud] POST %d → %s\n", httpCode, url.c_str());
        } else {
            Serial.printf("[Cloud] POST failed: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        return httpCode;
    }

    /** Parse the compact JSON frame received from STM32 via UART.
     *  Returns a SensorFrame with valid=true on success. */
    static SensorFrame parseFrame(const String &raw) {
        SensorFrame f = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false};

        /* Simple key:value extraction without a JSON library
         * (Arduino JSON library can be added for robustness) */
        f.v_solar    = extractInt(raw, "\"vs\":");
        f.v_battery  = extractInt(raw, "\"vb\":");
        f.v_supercap = extractInt(raw, "\"vc\":");
        f.i_input    = extractInt(raw, "\"ii\":");
        f.i_output   = extractInt(raw, "\"io\":");
        f.p_input    = extractLong(raw, "\"pi\":");
        f.p_output   = extractLong(raw, "\"po\":");
        f.e_input    = extractLong(raw, "\"ei\":");
        f.e_output   = extractLong(raw, "\"eo\":");
        f.efficiency = extractInt(raw, "\"ef\":");
        f.sc_soc     = extractInt(raw, "\"sc_soc\":");
        f.sc_energy  = extractLong(raw, "\"sc_e\":");
        f.session_s  = extractLong(raw, "\"ts\":");
        f.valid      = (raw.indexOf('{') >= 0 && raw.indexOf('}') >= 0);

        return f;
    }

private:
    static int extractInt(const String &s, const char *key) {
        int idx = s.indexOf(key);
        if (idx < 0) return 0;
        return s.substring(idx + strlen(key)).toInt();
    }
    static long extractLong(const String &s, const char *key) {
        int idx = s.indexOf(key);
        if (idx < 0) return 0L;
        return s.substring(idx + strlen(key)).toInt();
    }
};

#endif /* CLOUD_UPLOADER_H */
