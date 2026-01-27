# Assignment 03: UDP Echo Server & Client

Build a UDP echo server and client using Boost.Asio. The client discovers the server via broadcast, then exchanges messages.

## Learning Objectives

- Use `udp::socket` for datagram communication
- Implement the echo pattern: `receive_from()` → `send_to()`
- Use UDP broadcast for LAN discovery

## Project Structure

```
projects/03-udp/
├── src/
│   ├── server.h    # UdpEchoServer class (implement TODO sections)
│   ├── client.h    # UdpEchoClient class (implement TODO sections)
│   ├── server.cpp  # Server executable
│   └── client.cpp  # Client executable
└── tests/
    └── tests.cpp   # Automated tests
```

---

## Part 1: UDP Echo Server

Implement the `UdpEchoServer` class in `src/server.h`:

- Bind to a UDP port
- Receive datagrams and echo them back to the sender

---

## Part 2: UDP Echo Client with Discovery

Implement the `UdpEchoClient` class in `src/client.h`:

- Broadcast `"DISCOVER"` to find a server
- Save the responding server's endpoint
- Send messages to the server and receive echoes

---

## Grading Rubric (10 points)

| Component | Points | Criteria |
|-----------|--------|----------|
| **Server: Basic Echo** | 4.0 | Server receives UDP datagrams and echoes them back |
| **Client: Broadcast Discovery** | 2.5 | Client sends broadcast and receives server response |
| **Client: Interactive Echo** | 2.0 | Client sends user input, displays echo response |
| **Code Quality** | 1.5 | Clean code, proper error handling, good structure |
| **Total** | **10** | |

---

## Extra Credit: UDP Chat Room (+3 points)

Implement a chat room where the server broadcasts received messages to all "connected" clients.

### Requirements

1. **Server tracks clients**: Maintain a list of client endpoints
2. **Client registration**: First message from a client adds them to the list
3. **Broadcast messages**: When server receives a message, send it to ALL registered clients
4. **Client timeout**: Remove clients that haven't sent a message in 30 seconds
5. **Username support**: First message is the username, subsequent messages are chat

### Chat Protocol

```
Client → Server: "JOIN:username"     → Server adds client, broadcasts "username joined"
Client → Server: "MSG:hello world"   → Server broadcasts "username: hello world" to all
Client → Server: "LEAVE"             → Server removes client, broadcasts "username left"
```

### Submission

- **Record a video** (1-2 minutes) demonstrating:
  - Starting the server
  - Two or more clients connecting
  - Clients exchanging messages
  - Messages appearing on all clients
- **Submit the video** to Canvas along with your code

---

## Common Pitfalls

1. **Broadcast requires opt-in**: Call `set_option(broadcast(true))` before sending to broadcast address
2. **Buffer size matters**: UDP truncates if buffer is smaller than datagram — use 1200 bytes
3. **`receive_from()` blocks**: Server must be running before client tries discovery
4. **Use `sender_endpoint`**: Server needs it to know where to echo back

---

## Submission Checklist

- [ ] `UdpEchoServer` echoes messages correctly
- [ ] `UdpEchoClient` discovers server via broadcast
- [ ] All tests pass
- [ ] (Extra Credit) Chat room video submitted
