#include <ArduinoJson.h>
#include <WiFi.h>
#include <serial/serialManager.h>

#include <string>

// Setup Neon Timing
const byte serialBufferSize = 201;
char serialBuffer[serialBufferSize];

const char *deviceID = "1234";
const char *deviceName = "Example Device";
const std::vector<std::string> supportedEvents = {"*"};

void onSerialEvent(NTEventType type);
void onEventCommand(JsonDocument &messageDoc);

void setup() {
  NTSerialManager.begin();
  NTSerialClient = NTSerialManager.getClient();
  NTSerialClient->onCommandReceived("event", onEventCommand);
  NTSerialClient->onEvent(onSerialEvent);
}

void loop() { NTSerialManager.loop(); }

void onSerialEvent(NTEventType type) {
  switch (type) {
    case NT_CLIENT_MESSAGE_ERROR:
      // failed to process message
      break;
    case NT_CLIENT_DISCONNECTED:
      // client has disconnected
      break;
    case NT_CLIENT_CONNECTED:
      // client has connected
      break;
  }
}

void onEventCommand(JsonDocument &messageDoc) {
  if (!NTSerialClient->isConnected()) {
    NTSerialClient->sendLogMessage("Error processing message: device not initialized");
    return;
  }

  // Process the command: messageDoc["type"]
}
