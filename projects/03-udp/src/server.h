/**
 * UDP Echo Server
 *
 * Assignment 03: UDP and Datagram Sockets
 *
 * A simple UDP echo server that:
 * 1. Binds to a UDP port
 * 2. Receives datagrams from clients
 * 3. Echoes back the received message (including DISCOVER requests)
 *
 * TODO: Implement the server methods following the pseudocode in README.md
 */

#ifndef UDP_ECHO_SERVER_H
#define UDP_ECHO_SERVER_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <array>

using boost::asio::ip::udp;

constexpr size_t MAX_UDP_PAYLOAD = 1200;
constexpr std::string_view DISCOVER_MESSAGE = "DISCOVER";

/**
 * UDP Echo Server class
 *
 * Example usage:
 *   boost::asio::io_context io;
 *   UdpEchoServer server(io, 9999);
 *   while (true) {
 *       server.process_one();
 *   }
 */
class UdpEchoServer {
public:
    /**
     * Construct server bound to specified port
     * @param io_context The Boost.Asio io_context
     * @param port Port to listen on (default: 9999)
     */
    UdpEchoServer(boost::asio::io_context& io_context, uint16_t port = 9999)
        : m_socket(io_context)
        , m_port(port)
    {
        // TODO: Open socket and bind to port
    }

    /**
     * Get the port the server is bound to
     * This is useful when binding to port 0 (ephemeral port)
     */
    uint16_t port() const {
        // TODO: Return the actual bound port (use local_endpoint())
        return m_port;
    }

    /**
     * Process one message: receive, print, and echo back
     * This function blocks until a message is received
     * @return The message that was received
     */
    std::string process_one() {
        // TODO: Receive a message, print it, echo it back, return it
        // Use: receive_from(), send_to(), boost::asio::buffer()

        return "";  // Placeholder - implement this!
    }

private:
    udp::socket m_socket;
    uint16_t m_port;
};

// ============================================================================
// Main entry point function - DO NOT MODIFY
// ============================================================================

/**
 * Run the UDP echo server (called from main)
 * @param port Port to listen on
 * @return Exit code (0 = success)
 */
inline int run_echo_server(uint16_t port) {
    try {
        boost::asio::io_context io_context;
        UdpEchoServer server(io_context, port);

        std::cout << "UDP Echo Server listening on port " << server.port() << "...\n";
        std::cout << "Press Ctrl+C to stop.\n\n";

        while (true) {
            server.process_one();
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

#endif // UDP_ECHO_SERVER_H
