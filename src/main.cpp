#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
const int httpPort = 80;
String deviceName = "esp8266 Switch";
String version = "1.0";
ESP8266WebServer server(httpPort);
// 看你的继电器是连接那个io，默认gpio0
const int ledPin = 0;
// 开关的状态表示
const int on = 1;
const int off = 0;
// 开关的当前状态
String ledStatus = "off";
// digitalWrite(led, on);

// web服务器的根目录
void handleRoot() {
  server.send(200, "text/html", "<h1>this is index page from esp8266!</h1>");
}
// 操作LED开关状态的API
void handleLedStatusChange(){
  String message = "{\"code\":0,\"message\":\"success\"}";
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i)=="status")
    {
      if (server.arg(i)=="on")
      {
        digitalWrite(ledPin, on);
        ledStatus = "on";
      }else if (server.arg(i)=="off")
      {
        digitalWrite(ledPin, off);
        ledStatus = "off";
      }
    }
  }
  server.send(200, "application/json", message);
}
// 设备改名的API
void handleDeviceRename(){
  String message = "{\"code\":0,\"message\":\"success\"}";
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i)=="name")
    {
      deviceName = server.arg(i);
    }
  }
  server.send(200, "application/json", message);
}
// 当前的LED开关状态API
void handleCurrentledStatus(){
  String message;
  message = "{\"led\":\""+ledStatus+"\",\"code\":0,\"message\":\"success\"}";
  server.send(200, "application/json", message);
}
// 设备信息
void handleDeviceInfo(){
  String message;
  message = "{\n";
  message += "\"name\":\""+deviceName +"\",\n";
  message += "\"model\":\"com.iotserv.devices.one-key-switch\",\n";
  message += "\"mac\":\""+WiFi.macAddress()+"\",\n";
  message += "\"id\":\""+String(ESP.getFlashChipId())+"\",\n";
  message += "\"ui-support\":[\"web\",\"native\"],\n";
  message += "\"ui-first\":\"native\",\n";
  message += "\"author\":\"Farry\",\n";
  message += "\"email\":\"newfarry@126.com\",\n";
  message += "\"home-page\":\"https://github.com/iotdevice\",\n";
  message += "\"firmware-respository\":\"https://github.com/iotdevice/esp8266-switch\",\n";
  message += "\"firmware-version\":\""+version+"\"\n";
  message +="}";
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
//  WiFi.softAP("ESP-Switch");

   WiFi.mode(WIFI_STA);
  // 选取一种连接路由器的方式 
//   WiFi.begin(ssid, password);
   WiFi.beginSmartConfig();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, off);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }

  if (MDNS.begin("esp-switch-"+String(ESP.getFlashChipId()))) {
    // Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/led", handleLedStatusChange);
  server.on("/rename", handleDeviceRename);
  server.on("/status", handleCurrentledStatus);
  // about this device
  server.on("/info", handleDeviceInfo);
  server.onNotFound(handleNotFound);

  server.begin();
  // Serial.println("HTTP server started");
  MDNS.addService("iotdevice", "tcp", httpPort);
}

void loop(void){
  MDNS.update();
  server.handleClient();
}
