#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define UPDATE_INTERVAL 100
#define SENSORS_COUNT 4

byte sensorPins[] = {34, 35, 32, 33};
byte pumpPins[] = {19, 18, 5, 17};

WebServer server(80); // port
long lastUpdate = 0;


// endpoints
void api_measure() {
    StaticJsonDocument<64> json;

    for (byte i = 0; i < SENSORS_COUNT; i++) {
        json.add(analogRead(sensorPins[i]));
    }

    String body;
    serializeJson(json, body);

    server.send(200, "application/json", body);
    Serial.println("HTTP 200 GET /measure");
}

void api_water() {
    server.send(200);
    Serial.println("HTTP 200 GET /water");
}

void api_notFound() {
    server.send(404, "text/plain", "Not found");
    Serial.println("HTTP 404 GET ?");
}


void setup() {
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
    if (millis() - lastUpdate > UPDATE_INTERVAL) {
        server.handleClient();
        lastUpdate = millis();
    }

}