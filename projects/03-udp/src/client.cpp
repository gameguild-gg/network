/**
 * UDP Echo Client - Main Executable
 *
 * Assignment 03: UDP and Datagram Sockets
 *
 * DO NOT MODIFY THIS FILE - implement your code in client.h
 *
 * Usage: ./03-udp-client [port]
 */

#include "client.h"

int main(int argc, char* argv[]) {
    uint16_t port = (argc > 1) ? static_cast<uint16_t>(std::stoi(argv[1])) : 9999;
    return run_echo_client(port);
}
