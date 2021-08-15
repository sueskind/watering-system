#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define REQUESTS_UPDATE_INTERVAL 100

WebServer server(80); // port
long lastUpdate = 0;

unsigned long pumpUntil[PUMPS_COUNT];


// endpoints
void api_measure() {
    StaticJsonDocument<64> json;

    for (int i = 0; i < SENSORS_COUNT; i++) {
        json.add(analogRead(sensorPins[i]));
    }

    String body;
    serializeJson(json, body);

    server.send(200, "application/json", body);
    Serial.println("HTTP 200 GET /measure");
}

void api_water() {
    if (!server.hasArg("pumpNumber")) {
        server.send(400, "text/plain", "Bad request: Missing parameter 'pumpNumber'");
        Serial.println("HTTP 400 GET /water");

    } else if (!server.hasArg("duration")) {
        server.send(400, "text/plain", "Bad request: Missing parameter 'duration'");
        Serial.println("HTTP 400 GET /water");

    } else {
        int pumpNumber = server.arg("pumpNumber").toInt();
        long duration = server.arg("duration").toInt();

        if (pumpNumber < 0 || pumpNumber >= PUMPS_COUNT) {
            server.send(400, "text/plain", "Bad request: Illegal parameter value: 'pumpNumber'");
            Serial.println("HTTP 400 GET /water");

        } else if (duration < 0) {
            server.send(400, "text/plain", "Bad request: Illegal parameter value: 'duration'");
            Serial.println("HTTP 400 GET /water");

        } else {
            server.send(200);
            Serial.println("HTTP 200 GET /water");

            pumpUntil[pumpNumber] = millis() + duration;
        }
    }
}

void api_notFound() {
    server.send(404, "text/plain", "Not found");
    Serial.println("HTTP 404 GET ?");
}


void setup() {

    // initialize pumps as OFF
    for (int i = 0; i < PUMPS_COUNT; i++) {
        pinMode(pumpPins[i], OUTPUT);
        digitalWrite(pumpPins[i], HIGH);
        pumpUntil[i] = 0;
    }

    Serial.begin(115200);
    while (!Serial) {;}
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/measure", HTTP_GET, api_measure);
    server.on("/water", HTTP_GET, api_water);
    server.onNotFound(api_notFound);

    server.begin();
    Serial.println("HTTP server started\n");
}


void loop() {

    // check if water needs to be pumped
    for (int i = 0; i < PUMPS_COUNT; i++) {
        if (pumpUntil[i] > millis()) {
            digitalWrite(pumpPins[i], LOW);
        } else {
            digitalWrite(pumpPins[i], HIGH);
        }
    }

    // handle webserver stuff
    if (millis() - lastUpdate > REQUESTS_UPDATE_INTERVAL) {

        // first check if connection is lost and reconnect in that case
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnecting to WiFi...");
            WiFi.disconnect();
            WiFi.reconnect();
        }

        server.handleClient();
        lastUpdate = millis();
    }
}