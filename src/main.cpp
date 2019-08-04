#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);
// 看你的继电器是连接那个io，默认gpio0
const int led1 = 0;
const int led2 = 0;
// 如果是gpio0
// const int led = 0;
// 开关的状态表示
const int on = 1;
const int off = 0;
// 开关的当前状态
int led1status = off;
int led2status = off;
// digitalWrite(led1, on);

// web服务器的根目录
void handleRoot() {
  server.send(200, "text/plain", "this is index page from esp8266!");
}

void handleLEDStatusChange(){
  String message = "{\"code\":0,\"message\":\"success\"}";
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i)=="pin")
    {
      if (server.arg(i)=="ON1")
      {
        digitalWrite(led1, on);
        led1status = on;
      }else if (server.arg(i)=="OFF1")
      {
        digitalWrite(led1, off);
        led1status = off;
      }else if (server.arg(i)=="ON2")
      {
        digitalWrite(led2, on);
        led2status = on;
      }else if (server.arg(i)=="OFF2")
      {
        digitalWrite(led2, off);
        led2status = off;
      }
    }
  }
  server.send(200, "text/plain", message);
}

void handleCurrentLEDStatus(){
  String message;
  message = "{\"led1\":"+String(led1status)+",\"led2\":"+String(led2status)+"}";
  server.send(200, "application/json", message);
}

// 页面或者api没有找到
void handleNotFound(){
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
}

void setup(void){
  pinMode(led1, OUTPUT);
  digitalWrite(led1, off);
  pinMode(led2, OUTPUT);
  digitalWrite(led2, off);

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
  server.on("/led", handleLEDStatusChange);
  server.on("/status", handleCurrentLEDStatus);
  server.onNotFound(handleNotFound);

  server.begin();
  // Serial.println("HTTP server started");
  MDNS.addService("http", "tcp", 80);
}

void loop(void){
  MDNS.update();
  server.handleClient();
}
