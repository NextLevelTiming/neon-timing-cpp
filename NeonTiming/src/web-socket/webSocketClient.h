#ifndef NT_WEB_SOCKET_CLIENT_H

#define NT_WEB_SOCKET_CLIENT_H

#include <WebSocketsServer.h>

#include "client/client.h"

class NeonTimingWebSocketClient : public NeonTimingClient {
 private:
  WebSocketsServer *server;
  uint8_t clientId;

 public:
  NeonTimingWebSocketClient(const std::string &deviceId, const std::string &deviceName,
                            const std::vector<std::string> &supportedEvents, WebSocketsServer *server,
                            uint8_t clientId);
  void sendMessage(const char *message);
};

#endif
