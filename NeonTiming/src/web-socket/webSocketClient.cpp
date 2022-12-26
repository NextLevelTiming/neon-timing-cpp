#include "webSocketClient.h"

NeonTimingWebSocketClient::NeonTimingWebSocketClient(const std::string &deviceId, const std::string &deviceName,
                                                     const std::vector<std::string> &supportedEvents,
                                                     WebSocketsServer *server, uint8_t clientId)
    : NeonTimingClient(deviceId, deviceName, supportedEvents), server(server), clientId(clientId){};

void NeonTimingWebSocketClient::sendMessage(const char *message) { server->sendTXT(clientId, message); }
