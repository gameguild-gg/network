/**
 * TCP Chat Server - Main Executable
 *
 * Assignment 04: TCP Chatroom
 *
 * Implement your server code in server.h
 *
 * Usage: ./04-chat-server [port]
 */

#include "server.h"

int main(int argc, char* argv[]) {
    uint16_t port = (argc > 1) ? static_cast<uint16_t>(std::stoi(argv[1])) : 9999;
    return run_chat_server(port);
}
