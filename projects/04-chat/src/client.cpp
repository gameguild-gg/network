/**
 * TCP Chat Client - Main Executable
 *
 * Assignment 04: TCP Chatroom
 *
 * Implement your client code in client.h
 *
 * Usage: ./04-chat-client [host] [port]
 */

#include "client.h"

int main(int argc, char* argv[]) {
    std::string host = (argc > 1) ? argv[1] : "127.0.0.1";
    uint16_t port = (argc > 2) ? static_cast<uint16_t>(std::stoi(argv[2])) : 9999;
    return run_chat_client(host, port);
}
