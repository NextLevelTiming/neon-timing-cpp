# Neon Timing (C++)

A C++ library for the [Neon Timing Protocol](https://github.com/NextLevelTiming/neon-timing-protocol). This library enables both serial and web socket message communication.

This library implements the following classes:
- NeonTimingClient - The base client, used for protocol specific implementations
- NeonTimingWebSocketClient - A web socket client implementation
- NeonTimingWebSocketServer - A web socket server that manages multiple clients
- NeonTimingSerialClient - A serial client implementation
- NeonTimingSerialManager - A serial manager that manages a single client

## Neon Timing Serial Manager
The serial manager only supports one serial client.

The serial manager is capable of sending and receiving neon timing messages.

## Neon Timing Web Socket Server
The web socket server supports multiple clients, both sending and receiving messages.

## Examples
- [Serial Example](./NeonTiming/examples/serial.cpp)

## Installation
This library is hosted on the [platformio](https://registry.platformio.org/) registry and can be easily installed using hte pio command line interface.

```
pio pkg install --library "nextleveltiming/NeonTiming@^1.0.11"
```
