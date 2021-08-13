#include <WebServer.h>

WebServer server(80); // port

void get() {
    server.send(200, "text/html", "Get successful");
    Serial.println("HTTP 200 GET /");
}

void notFound() {
    server.send(404, "text/plain", "Not found");
    Serial.println("HTTP 404 GET ?");
}