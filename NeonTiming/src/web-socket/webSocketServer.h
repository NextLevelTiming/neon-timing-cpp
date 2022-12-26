#ifndef NT_WEB_SOCKET_SERVER_H

#define NT_WEB_SOCKET_SERVER_H

#include <WebSocketsServer.h>

#include <string>
#include <unordered_map>

#include "WebSocketClient.h"

class NeonTimingWebSocketServer {
 private:
  std::string deviceId;
  std::string deviceName;
  std::vector<std::string> supportedEvents;
  std::unordered_map<int, NeonTimingWebSocketClient> clients;
  WebSocketsServer server;
  typedef void (*eventHandlerMethodType)(NTEventType eventType, uint8_t clientId);
  eventHandlerMethodType eventHandler;

 public:
  NeonTimingWebSocketServer(const std::string &deviceId, const std::string &deviceName,
                            const std::vector<std::string> &supportedEvents, int serverPort);
  void webSocketEvent(uint8_t clientId, WStype_t type, uint8_t *payload, size_t length);
  void begin();
  void loop();
  void onEvent(eventHandlerMethodType handler);
  int connectedClients();
  void broadcastMessage(char *message);
  void sendMessage(char *message, int clientId);
  void broadcastJsonCommandMessage(const char *cmd, JsonDocument &messageDoc);
  NeonTimingWebSocketClient *getClient(uint8_t clientId);
};

#endif
