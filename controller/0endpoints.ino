#include <WebServer.h>

WebServer server(80); // port

void api_measure() {
    server.send(200, "text/html", "Measure successful");
    Serial.println("HTTP 200 GET /measure");
}

void api_water() {
    server.send(200, "text/html", "Water successful");
    Serial.println("HTTP 200 GET /water");
}

void api_notFound() {
    server.send(404, "text/plain", "Not found");
    Serial.println("HTTP 404 GET ?");
}