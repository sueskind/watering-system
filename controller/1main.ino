#include <WiFi.h>

#define UPDATE_INTERVAL 200

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


    server.on("/", HTTP_GET, get);
    server.onNotFound(notFound);

    server.begin();
    Serial.println("HTTP server started\n");
}

long lastUpdate = 0;
void loop() {
    if (millis() - lastUpdate > UPDATE_INTERVAL) {
        server.handleClient();
        lastUpdate = millis();
    }
}