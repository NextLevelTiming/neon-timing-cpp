#ifndef NT_SERIAL_MANAGER_H

#define NT_SERIAL_MANAGER_H

#include "serial/serialClient.h"

class NeonTimingSerialManager {
 private:
  size_t maxSerialBuffer;
  char* serialBuffer;
  NeonTimingSerialClient* client;

 public:
  NeonTimingSerialManager(const std::string& deviceId, const std::string& deviceName,
                          const std::vector<std::string>& supportedEvents, char* serialBuffer, size_t maxSerialBuffer);
  void begin();
  void loop();
  NeonTimingSerialClient* getClient();
};

#endif
