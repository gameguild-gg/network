/**
 * UDP Echo Server - Main Executable
 *
 * Assignment 03: UDP and Datagram Sockets
 *
 * DO NOT MODIFY THIS FILE - implement your code in server.h
 *
 * Usage: ./03-udp-server [port]
 */

#include "server.h"

int main(int argc, char* argv[]) {
    uint16_t port = (argc > 1) ? static_cast<uint16_t>(std::stoi(argv[1])) : 9999;
    return run_echo_server(port);
}

