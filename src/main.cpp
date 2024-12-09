#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

#include "WiFiS3.h"
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#define WIFI_SSID "Nick's Pixel XL"
#define WIFI_PASS "4bb27c43ef22"

SocketIOclient socketIO;
 
// void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {

//   switch (type) {
//     case WStype_DISCONNECTED:
//       Serial.println("[WSc] Disconnected!");
//       break;
//     case WStype_CONNECTED:
//       Serial.println("[WSc] Connected!");

//       // send message to server when Connected
//       webSocket.sendTXT("Connected");
//       break;
//     case WStype_TEXT:
//       Serial.print("[WSc] get text:");
//       Serial.println((char *)payload);

//       // send message to server
//       // webSocket.sendTXT("message here");
//       break;
//     case WStype_BIN:
//       // send data to server
//       // webSocket.sendBIN(payload, length);
//       break;
//     case WStype_ERROR:
//     case WStype_FRAGMENT_TEXT_START:
//     case WStype_FRAGMENT_BIN_START:
//     case WStype_FRAGMENT:
//     case WStype_FRAGMENT_FIN:
//       break;
//   }
// }

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case sIOtype_DISCONNECT:
      Serial.println("[IOc] Disconnected!");
      break;
    case sIOtype_CONNECT:
      Serial.println("[IOc] Connected!");

      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT:
    case sIOtype_ACK:
    case sIOtype_ERROR:
    case sIOtype_BINARY_EVENT:
    case sIOtype_BINARY_ACK:
      break;
  }
}

void setup() {
  Serial.begin(9600);

  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.println("[SETUP] BOOT WAIT ...");
    Serial.flush();
    delay(1000);
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  Serial.println("[Wifi]: Connecting");

  int status = WL_IDLE_STATUS;

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("[Wifi]: Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);

    delay(1000);
  }

  Serial.println("Connected!");

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Try connecting via socketIO
  socketIO.begin("129.21.148.69", 3000);
  socketIO.onEvent(socketIOEvent);

  socketIO.setReconnectInterval(5000);

  // // server address, port and URL
  // webSocket.beginSocketIO("129.21.148.69", 3000);

  // // event handler
  // webSocket.onEvent(webSocketEvent);

  // // try ever 5000 again if connection has failed
  // webSocket.setReconnectInterval(5000);
}

void loop() {
  socketIO.loop();
}