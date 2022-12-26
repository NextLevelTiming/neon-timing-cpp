#ifndef NT_CLIENT_H

#define NT_CLIENT_H

#include <ArduinoJson.h>

#include <unordered_map>
#include <vector>

#include "strTools.h"

typedef void (*commandHandlerMethod)(JsonDocument &);

typedef enum {
  NT_CLIENT_CONNECTED,
  NT_CLIENT_DISCONNECTED,
  NT_CLIENT_MESSAGE_ERROR,
} NTEventType;

class NeonTimingClient {
 private:
  std::string deviceId;
  std::string deviceName;
  std::string supportedEvents;
  std::vector<std::string> supportedEventsVector;

  using commandPreHandlerMethod = void (NeonTimingClient::*)(JsonDocument &);
  std::unordered_map<std::string, commandPreHandlerMethod> commandPreHandlers;

  std::unordered_map<std::string, commandHandlerMethod> commandHandlers;
  std::unordered_map<std::string, boolean> allowedEvents;

  typedef void (*eventHandlerMethodType)(NTEventType eventType);
  eventHandlerMethodType eventHandler;

  boolean connected = false;
  unsigned long connectionLastHeartbeatTime = millis();
  unsigned long connectionLastHeartbeatPingTime = millis();
  unsigned long connectionHeartbeatTimeout = 10000;
  unsigned long connectionHeartbeatInterval = connectionHeartbeatTimeout / 3L;
  void setConnected(boolean state);

 public:
  NeonTimingClient(const std::string &deviceId, const std::string &deviceName,
                   const std::vector<std::string> &supportedEventsVector);
  ~NeonTimingClient();

  void begin();
  void loop();
  void onEvent(eventHandlerMethodType handler);
  boolean processCommand(char *msg);
  void onCommandReceived(const std::string &cmd, commandHandlerMethod method);
  boolean isConnected();
  void onHandshakeInitCommand(JsonDocument &messageDoc);
  void onHandshakeAckCommand(JsonDocument &messageDoc);
  void sendJsonCommandMessage(const char *cmd, JsonDocument &messageDoc);
  void sendCommandMessage(const char *cmd, const char *properties);
  void sendLogMessage(const char *message);
  virtual void sendMessage(const char *message);
  void sendHandshakeInitCommand();
};

#endif
