#include "serialManager.h"

NeonTimingSerialManager::NeonTimingSerialManager(const std::string &deviceId, const std::string &deviceName,
                                                 const std::vector<std::string> &supportedEvents, char *serialBuffer,
                                                 size_t maxSerialBuffer)
    : serialBuffer(serialBuffer),
      maxSerialBuffer(maxSerialBuffer),
      client(new NeonTimingSerialClient(std::move(deviceId), std::move(deviceName), supportedEvents)){};

void NeonTimingSerialManager::begin() {
  Serial.setRxBufferSize(10240);
  Serial.begin(115200);
  Serial.println();
  client->begin();
}

void NeonTimingSerialManager::loop() {
  client->loop();

  if (Serial.available() <= 0) {
    return;
  }

  Serial.readBytesUntil('\n', serialBuffer, maxSerialBuffer - 1);

  client->processCommand(serialBuffer);

  // Reset buffer
  memset(serialBuffer, 0, maxSerialBuffer);
}

NeonTimingSerialClient *NeonTimingSerialManager::getClient() { return client; }
