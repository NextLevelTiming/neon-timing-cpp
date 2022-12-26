#include <ArduinoJson.h>
#include <NeonTiming/serial/serialManager.h>
#include <NeonTiming/web-socket/webSocketServer.h>
#include <WiFi.h>
#include <helpers/helpers.h>

#include <string>

// Setup Neon Timing
const byte serialBufferSize = 201;
char serialBuffer[serialBufferSize];

const char *deviceName = "Race Hub";
const std::vector<std::string> supportedEvents = {"*"};

int WS_PORT = 3001;
NeonTimingWebSocketServer NTWSServer(Helpers::getDeviceID(), deviceName, supportedEvents, WS_PORT);
NeonTimingSerialManager NTSerialManager(Helpers::getDeviceID(), deviceName, supportedEvents, serialBuffer,
                                        serialBufferSize);
NeonTimingSerialClient *NTSerialClient;

// WiFi AP
const int AP_CHANNEL = 1;
const boolean AP_HIDDEN = false;
const byte MAX_CLIENTS = 10;

void onSerialEvent(NTEventType type);
void onSerialCommand(JsonDocument &messageDoc);
void onWSCommand(JsonDocument &messageDoc);
void onWSEvent(NTEventType type, uint8_t clientId);

void setup() {
  NTSerialManager.begin();
  NTSerialClient = NTSerialManager.getClient();
  NTSerialClient->onCommandReceived("event", onSerialCommand);
  NTSerialClient->onEvent(onSerialEvent);

  // setup wifi
  std::string apSSID = WIFI_SSID_PREFIX;
  apSSID.append(Helpers::getDeviceID());

  char str[201];
  snprintf(str, sizeof(str), "\"evt\":\"log\",\"message\":\"Setting up WiFi AP\",\"ssid\":\"%s\"", apSSID.c_str());
  NTSerialClient->sendCommandMessage("event", str);

  boolean result = WiFi.softAP(apSSID.c_str(), WIFI_PASS, AP_CHANNEL, AP_HIDDEN, MAX_CLIENTS);
  if (result == true) {
    NTSerialClient->sendLogMessage("WiFi AP is available");
  } else {
    NTSerialClient->sendLogMessage("Critical error: WiFi AP setup failed");
  }

  // setup websocket server
  NTSerialClient->sendLogMessage("Setting up WebSocket server");
  NTWSServer.begin();
  NTWSServer.onEvent(onWSEvent);

  snprintf(str, sizeof(str),
           "\"evt\":\"log\",\"message\":\"WebSocket server "
           "available\",\"ip\":\"%s\",\"port\":\"%d\"",
           WiFi.softAPIP().toString().c_str(), WS_PORT);
  NTSerialClient->sendCommandMessage("event", str);
}

void loop() {
  NTSerialManager.loop();
  NTWSServer.loop();
}

void onSerialEvent(NTEventType type) {
  switch (type) {
    case NT_CLIENT_MESSAGE_ERROR:
      break;
    case NT_CLIENT_DISCONNECTED:
      break;
    case NT_CLIENT_CONNECTED:
      break;
  }
}

void onSerialCommand(JsonDocument &messageDoc) {
  if (!NTSerialClient->isConnected()) {
    NTSerialClient->sendLogMessage("Error processing message: device not initialized");
    return;
  }

  NTWSServer.broadcastJsonCommandMessage(messageDoc["cmd"], messageDoc);
  messagesSentCount += NTWSServer.connectedClients();
}

void onWSEvent(NTEventType type, uint8_t clientId) {
  char str[201];
  switch (type) {
    case NT_CLIENT_MESSAGE_ERROR:
      break;
    case NT_CLIENT_DISCONNECTED:
      sprintf(str, "\"client_id\":\"%u\"", clientId);
      NTSerialClient->sendCommandMessage("client_disconnected", str);
      break;
    case NT_CLIENT_CONNECTED:
      sprintf(str, "\"client_id\":\"%u\"", clientId);
      auto client = NTWSServer.getClient(clientId);
      if (client != nullptr) {
        client->onCommandReceived("event", onWSCommand);
      }
      NTSerialClient->sendCommandMessage("client_connected", str);
      break;
  }
}

void onWSCommand(JsonDocument &messageDoc) {
  if (!NTSerialClient->isConnected()) {
    NTSerialClient->sendLogMessage("Error processing message: device not initialized");
    return;
  }

  NTSerialClient->sendJsonCommandMessage(messageDoc["cmd"], messageDoc);
  messagesSentCount++;
}
