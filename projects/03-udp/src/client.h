/**
 * UDP Echo Client with Broadcast Discovery
 *
 * Assignment 03: UDP and Datagram Sockets
 *
 * A simple UDP echo client that:
 * 1. Broadcasts "DISCOVER" to find a server on the LAN
 * 2. Waits for the echo response from a server
 * 3. Sends messages and receives echoes
 *
 * TODO: Implement the client methods following the pseudocode in README.md
 */

#ifndef UDP_ECHO_CLIENT_H
#define UDP_ECHO_CLIENT_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <array>
#include <optional>

#include "server.h"

using boost::asio::ip::udp;

/**
 * UDP Echo Client class
 *
 * Example usage:
 *   boost::asio::io_context io;
 *   UdpEchoClient client(io);
 *
 *   // Discover a server on LAN (blocks until found)
 *   auto server = client.discover(9999);
 *   client.connect(server);
 *   auto echo = client.send_and_receive("Hello!");
 */
class UdpEchoClient {
public:
    /**
     * Construct client
     * @param io_context The Boost.Asio io_context
     */
    explicit UdpEchoClient(boost::asio::io_context& io_context)
        : m_socket(io_context)
    {
        // TODO: Open socket and enable broadcast option
    }

    /**
     * Discover a server on the LAN via broadcast
     * Blocks until a server responds
     * @param port Port to broadcast to
     * @return The server endpoint that responded
     */
    udp::endpoint discover([[maybe_unused]] uint16_t port = 9999) {
        // TODO: Broadcast DISCOVER_MESSAGE to 255.255.255.255:port
        //       Wait for response, disable broadcast, return server endpoint
        // Use: address_v4::broadcast(), send_to(), receive_from()

        return {};  // Placeholder - implement this!
    }

    /**
     * Set the server endpoint to communicate with
     * @param server_endpoint The server's endpoint
     */
    void connect(const udp::endpoint& server_endpoint) {
        m_server_endpoint = server_endpoint;
    }

    /**
     * Check if connected to a server
     */
    bool is_connected() const {
        return m_server_endpoint.port() != 0;
    }

    /**
     * Get the server endpoint
     */
    udp::endpoint server_endpoint() const {
        return m_server_endpoint;
    }

    /**
     * Send a message and receive the echo
     * Blocks until response is received
     * @param message The message to send
     * @return The echoed message, or nullopt if not connected
     */
    std::optional<std::string> send_and_receive([[maybe_unused]] const std::string& message) {
        if (!is_connected()) {
            return std::nullopt;
        }

        // TODO: Send message to server, receive and return the echo
        // Use: send_to(), receive_from(), boost::asio::buffer()

        return std::nullopt;  // Placeholder - implement this!
    }

    /**
     * Get the underlying socket (for testing)
     */
    udp::socket& socket() {
        return m_socket;
    }

private:
    udp::socket m_socket;
    udp::endpoint m_server_endpoint;
};

// ============================================================================
// Main entry point function - DO NOT MODIFY
// ============================================================================

/**
 * Run the UDP echo client (called from main)
 * @param port Server port to discover/connect to
 * @return Exit code (0 = success)
 */
inline int run_echo_client(uint16_t port) {
    try {
        boost::asio::io_context io_context;
        UdpEchoClient client(io_context);

        // ==================== DISCOVERY PHASE ====================

        std::cout << "Searching for servers on LAN (port " << port << ")...\n";
        std::cout << "(Waiting for server response...)\n";

        auto server = client.discover(port);
        client.connect(server);

        std::cout << "Found server at " << client.server_endpoint() << "\n";

        // ==================== INTERACTIVE PHASE ====================

        std::cout << "\nConnected! Type messages (or 'quit' to exit):\n\n";

        std::string line;
        while (true) {
            std::cout << "> " << std::flush;

            if (!std::getline(std::cin, line)) {
                break;
            }

            if (line == "quit" || line == "exit") {
                std::cout << "Goodbye!\n";
                break;
            }

            if (line.empty()) {
                continue;
            }

            auto echo = client.send_and_receive(line);

            if (echo) {
                std::cout << "Echo: " << *echo << "\n\n";
            } else {
                std::cerr << "Error: not connected to server\n\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

#endif // UDP_ECHO_CLIENT_H
