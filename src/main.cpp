#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);
// 看你的继电器是连接那个io，默认gpio2
const int led = 2;
// const int led = 0;
// 开关的状态表示
const int on = 1;
const int off = 0;

void handleRoot() {
  digitalWrite(led, on);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, off);
}

void handleNotFound(){
  digitalWrite(led, on);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, off);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  // Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  WiFi.beginSmartConfig();
  // Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }
  // Serial.println("");
  // Serial.print("Connected to ");
  // Serial.println(ssid);
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    // Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  // Serial.println("HTTP server started");
  MDNS.addService("http", "tcp", 80);
}

void loop(void){
  MDNS.update();
  server.handleClient();
}
