#ifndef NT_SERIAL_CLIENT_H

#define NT_SERIAL_CLIENT_H

#include "neon-timing/client/client.h"

class NeonTimingSerialClient : public NeonTimingClient {
 private:
 public:
  NeonTimingSerialClient(const std::string &deviceId, const std::string &deviceName,
                         const std::vector<std::string> &supportedEvents);
  void sendMessage(const char *message);
};

#endif
