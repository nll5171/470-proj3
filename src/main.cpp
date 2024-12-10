#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

#include "WiFiS3.h"
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <ArduinoJson.h>

#include <ezButton.h>

#define WIFI_SSID "Nick's Pixel XL"
#define WIFI_PASS "4bb27c43ef22"

#define joyX A0  // Joystick x-axis
#define joyY A1  // Joystick y-axis
#define SW_PIN 2 // Joystick button

ezButton button(SW_PIN);

SocketIOclient socketIO;

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
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

void setup()
{
  Serial.begin(9600);

  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();
  Serial.println();
  Serial.println();

  // Configure joystick button input
  button.setDebounceTime(50); // set debounce time to 50 milliseconds

  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.println("[SETUP] BOOT WAIT ...");
    Serial.flush();
    delay(1000);
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  Serial.println("[Wifi]: Connecting");

  int status = WL_IDLE_STATUS;

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
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
  socketIO.begin("129.21.148.69", 3000, "/socket.io/?EIO=4");
  socketIO.onEvent(socketIOEvent);

  socketIO.setReconnectInterval(5000);
}

void loop()
{
  socketIO.loop();
  button.loop();

  // Gather user input and attempt to send it
  // int xValue = analogRead(joyX);
  // int yValue = analogRead(joyY);
  int xValue = map(analogRead(joyX), 0, 1013, -5, 5); // Prevent default value from being -1
  int yValue = map(analogRead(joyY), 0, 1023, -5, 5);

  int bValue = button.getState();

  // Convert input to JSON data and send it to server
  // create JSON message for Socket.IO (event)
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();

  // add event name
  // ie. socket.on('event_name)
  array.add("user input");

  // add payload (parameters) for the event
  JsonObject input = array.createNestedObject();
  input["xVal"] = xValue;
  input["yVal"] = yValue;
  input["pressed"] = button.isPressed();

  // JSON to String (serializion)
  String output;
  serializeJson(doc, output);

  // Send event
  socketIO.sendEVENT(output);

  // Print JSON for debugging
  Serial.println(output);
}