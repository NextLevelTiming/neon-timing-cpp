#include "serialClient.h"

NeonTimingSerialClient::NeonTimingSerialClient(const std::string &deviceId, const std::string &deviceName,
                                               const std::vector<std::string> &supportedEvents)
    : NeonTimingClient(deviceId, deviceName, supportedEvents){};

void NeonTimingSerialClient::sendMessage(const char *message) { Serial.println(message); }
