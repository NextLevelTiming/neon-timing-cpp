#include "webSocketServer.h"

NeonTimingWebSocketServer::NeonTimingWebSocketServer(const std::string &deviceId, const std::string &deviceName,
                                                     const std::vector<std::string> &supportedEvents, int serverPort)
    : deviceId(std::move(deviceId)),
      deviceName(std::move(deviceName)),
      supportedEvents(supportedEvents),
      server(WebSocketsServer(serverPort)){};

void NeonTimingWebSocketServer::begin() {
  server.begin();
  server.onEvent([&](uint8_t clientId, WStype_t type, uint8_t *payload, size_t length) {
    webSocketEvent(clientId, type, payload, length);
  });
  server.enableHeartbeat(10000, 3000, 2);
}

void NeonTimingWebSocketServer::loop() {
  server.loop();
  for (auto &client : clients) {
    client.second.loop();
  }
}

void NeonTimingWebSocketServer::onEvent(eventHandlerMethodType handler) { eventHandler = handler; }

void NeonTimingWebSocketServer::webSocketEvent(uint8_t clientId, WStype_t type, uint8_t *payload, size_t length) {
  char str[201];
  auto client = clients.find(clientId);
  switch (type) {
    case WStype_TEXT:
      if (length == 0) {
        break;
      }

      if (client == clients.end()) {
        return;
      }

      if (!client->second.processCommand((char *)payload)) {
        eventHandler(NT_CLIENT_MESSAGE_ERROR, clientId);
      }

      break;
    case WStype_DISCONNECTED:
      eventHandler(NT_CLIENT_DISCONNECTED, clientId);
      clients.erase(clientId);
      break;
    case WStype_CONNECTED:
      auto ret = clients.emplace(std::piecewise_construct, std::forward_as_tuple(clientId),
                                 std::forward_as_tuple(deviceId, deviceName, supportedEvents, &server, clientId));
      if (!ret.second) {
        return;
      }

      auto &client = *ret.first;
      client.second.begin();
      eventHandler(NT_CLIENT_CONNECTED, clientId);
      break;
  }
}

int NeonTimingWebSocketServer::connectedClients() { return server.connectedClients(); }

void NeonTimingWebSocketServer::sendMessage(char *message, int clientId) {
  auto client = clients.find(clientId);
  if (client != clients.end()) {
    client->second.sendMessage(message);
  }
}

void NeonTimingWebSocketServer::broadcastMessage(char *message) {
  for (auto &client : clients) {
    client.second.sendMessage(message);
  }
}

void NeonTimingWebSocketServer::broadcastJsonCommandMessage(const char *cmd, JsonDocument &messageDoc) {
  messageDoc["cmd"] = cmd;
  messageDoc["time"] = millis();
  messageDoc["did"] = deviceId.c_str();
  messageDoc["protocol"] = "NT1";

  char str[201];
  serializeJson(messageDoc, str);

  for (auto &client : clients) {
    client.second.sendMessage(str);
  }
}

NeonTimingWebSocketClient *NeonTimingWebSocketServer::getClient(uint8_t clientId) {
  auto client = clients.find(clientId);
  if (client != clients.end()) {
    return &client->second;
  }

  return nullptr;
}
