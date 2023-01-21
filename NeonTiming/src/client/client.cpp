#include "client.h"

NeonTimingClient::NeonTimingClient(const std::string &deviceId, const std::string &deviceName,
                                   const std::vector<std::string> &supportedEventsVector)
    : deviceId(std::move(deviceId)),
      deviceName(std::move(deviceName)),
      supportedEventsVector(std::move(supportedEventsVector)) {
  supportedEvents = "";
  for (const auto &event : supportedEventsVector) {
    supportedEvents += "\"";
    supportedEvents += event;
    supportedEvents += "\",";
  }
  if (supportedEventsVector.size() > 0) {
    supportedEvents.pop_back();
  }
};

NeonTimingClient::~NeonTimingClient() {
  if (isConnected()) {
    setConnected(false);
  }
}

void NeonTimingClient::begin() {
  commandPreHandlers.emplace("handshake_init", &NeonTimingClient::onHandshakeInitCommand);
  commandPreHandlers.emplace("handshake_ack", &NeonTimingClient::onHandshakeAckCommand);
  sendHandshakeInitCommand();
};

void NeonTimingClient::loop() {
  if (isConnected()) {
    // If heartbeat timeout occurs then disconnection
    if (millis() - connectionLastHeartbeatTime > connectionHeartbeatTimeout) {
      setConnected(false);
    }
    // If heartbeat timeout
    else if (millis() - connectionLastHeartbeatTime > connectionHeartbeatInterval &&
             millis() - connectionLastHeartbeatPingTime > connectionHeartbeatInterval) {
      sendHandshakeInitCommand();
      connectionLastHeartbeatPingTime = millis();
    }
  }
}

void NeonTimingClient::onEvent(eventHandlerMethodType handler) { eventHandler = handler; }

boolean NeonTimingClient::processCommand(char *msg) {
  connectionLastHeartbeatTime = millis();

  StaticJsonDocument<512> messageDoc;
  DeserializationError error = deserializeJson(messageDoc, msg);

  if (error) {
    eventHandler(NT_CLIENT_MESSAGE_ERROR);
    sendLogMessage("Error processing message: invalid json");
    return false;
  }

  if (!messageDoc.is<JsonObject>()) {
    eventHandler(NT_CLIENT_MESSAGE_ERROR);
    sendLogMessage("Error processing message: message must be an object");
    return false;
  }

  if (!messageDoc["cmd"].is<const char *>()) {
    eventHandler(NT_CLIENT_MESSAGE_ERROR);
    sendLogMessage("Error processing message: cmd must be a string");
    return false;
  }

  char *cmd = NULL;
  heapStr(&cmd, messageDoc["cmd"]);

  auto preHandlerMap = commandPreHandlers.find(cmd);
  if (preHandlerMap != commandPreHandlers.end()) {
    commandPreHandlerMethod f = commandPreHandlers[cmd];
    (*this.*f)(messageDoc);
  }

  if (commandHandlers.find(cmd) == commandHandlers.end()) {
    if (preHandlerMap == commandPreHandlers.end()) {
      sendLogMessage("Unsupported command");
    }
    return true;
  }

  commandHandlerMethod f = commandHandlers[cmd];
  (*f)(messageDoc);
  return true;
};

void NeonTimingClient::onCommandReceived(const std::string &cmd, commandHandlerMethod method) {
  commandHandlers.emplace(cmd, method);
};

void NeonTimingClient::sendJsonCommandMessage(const char *cmd, JsonDocument &messageDoc) {
  messageDoc["cmd"] = cmd;
  messageDoc["time"] = millis();
  messageDoc["did"] = deviceId.c_str();
  messageDoc["protocol"] = "NT1";

  char str[201];
  serializeJson(messageDoc, str);
  sendMessage(str);
}

void NeonTimingClient::sendCommandMessage(const char *cmd, const char *properties) {
  char str[201];
  snprintf(str, sizeof(str), "{\"cmd\":\"%s\",%s,\"protocol\":\"NT1\",\"time\":%lu,\"did\":\"%s\"}", cmd, properties,
           millis(), deviceId.c_str());
  sendMessage(str);
}

void NeonTimingClient::sendLogMessage(const char *message) {
  char str[201];
  snprintf(str, sizeof(str), "\"evt\":\"log\",\"message\":\"%s\"", message);
  sendCommandMessage("event", str);
}

/**
 * @brief to be implemented by each client
 *
 * @param message
 */
void NeonTimingClient::sendMessage(const char *message) {}

boolean NeonTimingClient::isConnected() { return connected; }

void NeonTimingClient::setConnected(boolean state) {
  connected = state;

  if (!connected) {
    eventHandler(NT_CLIENT_DISCONNECTED);
    allowedEvents.clear();
  } else {
    eventHandler(NT_CLIENT_CONNECTED);
  }
}

void NeonTimingClient::onHandshakeInitCommand(JsonDocument &messageDoc) {
  if (strcmp(messageDoc["protocol"].as<const char *>(), "NT1") != 0) {
    sendLogMessage("Error processing message: protocol must be NT1");
    return;
  }

  if (!messageDoc["events"].is<JsonArray>()) {
    sendLogMessage("Error processing message: events must be an array");
    return;
  }

  char str[201];
  snprintf(str, sizeof(str), "\"device\":\"%s\",\"events\":[%s]", deviceName.c_str(), supportedEvents.c_str());
  sendCommandMessage("handshake_ack", str);

  if (isConnected()) {
    return;
  }

  setConnected(true);

  // todo: we may want to continue updating this later
  for (JsonVariant value : messageDoc["events"].as<JsonArray>()) {
    allowedEvents.emplace(value, true);
  }
}

void NeonTimingClient::onHandshakeAckCommand(JsonDocument &messageDoc) {
  if (strcmp(messageDoc["protocol"].as<const char *>(), "NT1") != 0) {
    sendLogMessage("Error processing message: protocol must be NT1");
    return;
  }
}

void NeonTimingClient::sendHandshakeInitCommand() {
  char str[201];
  snprintf(str, sizeof(str), "\"device\":\"%s\",\"events\":[%s]", deviceName.c_str(), supportedEvents.c_str());
  sendCommandMessage("handshake_init", str);
}
