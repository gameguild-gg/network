/**
 * TCP Chat Client
 *
 * Assignment 04: TCP Chatroom
 *
 * A TCP chat client that:
 * 1. Connects to a chat server
 * 2. Sends and receives messages
 * 3. Supports /quit command to disconnect gracefully
 *
 * TODO: Implement the client methods following the requirements in README.md
 */

#ifndef TCP_CHAT_CLIENT_H
#define TCP_CHAT_CLIENT_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <atomic>

using boost::asio::ip::tcp;

constexpr size_t CLIENT_BUFFER_SIZE = 1200;

/**
 * TCP Chat Client class
 *
 * Example usage:
 *   boost::asio::io_context io;
 *   TcpChatClient client(io);
 *   client.connect("127.0.0.1", 9999);
 *   client.set_username("alice");
 *   client.run();  // Blocking - handles send/receive
 */
class TcpChatClient {
public:
    /**
     * Construct client
     * @param io_context The Boost.Asio io_context
     */
    explicit TcpChatClient(boost::asio::io_context& io_context)
        : m_io_context(io_context)
        , m_socket(io_context)
        , m_running(false)
    {
    }

    /**
     * Connect to a chat server
     * @param host Server hostname or IP address
     * @param port Server port
     * @return true if connection successful
     */
    bool connect([[maybe_unused]] const std::string& host, 
                 [[maybe_unused]] uint16_t port) {
        // TODO: Resolve hostname and connect to server
        // Use: tcp::resolver, resolver.resolve(), boost::asio::connect()
        // Set m_connected = true on success
        return false;
    }

    /**
     * Set the username and send it to the server
     * @param username The desired username
     */
    void set_username(const std::string& username) {
        m_username = username;
        // TODO: Send username to server as first message
    }

    /**
     * Check if connected to server
     */
    bool is_connected() const {
        return m_socket.is_open();
    }

    /**
     * Get the username
     */
    const std::string& username() const {
        return m_username;
    }

    /**
     * Run the client - blocks until disconnected
     * Handles both sending user input and receiving messages
     */
    void run() {
        if (!is_connected()) {
            std::cerr << "[Client] Not connected to server\n";
            return;
        }

        m_running = true;

        // TODO: Implement the main client loop
        // You need to handle BOTH:
        // 1. Reading user input from console and sending to server
        // 2. Receiving messages from server and displaying them
        //
        // Options:
        // A) Two threads: one for reading stdin, one for reading socket
        // B) Async I/O with Boost.Asio
        // C) Select/poll on both stdin and socket
        //
        // Recommended: Use std::jthread for simplicity
        //
        // Example with threads:
        //   std::jthread receiver([this]() { receive_loop(); });
        //   send_loop();  // Runs on main thread
        //   receiver.join();

        // Placeholder - implement your solution here
        std::cout << "[Client] TODO: Implement run() method\n";
    }

    /**
     * Disconnect from server gracefully
     */
    void disconnect() {
        // TODO: Send /quit command if connected
        // Use proper TCP shutdown: shutdown() then close()
        m_running = false;
    }

private:
    /**
     * Send a message to the server
     * @param message The message to send
     * @return true if sent successfully
     */
    bool send_message([[maybe_unused]] const std::string& message) {
        // TODO: Send message to server
        // Use: boost::asio::write()
        // Add newline if not present
        return false;
    }

    /**
     * Receive a message from the server
     * @return The received message, or empty string on error/disconnect
     */
    std::string receive_message() {
        // TODO: Receive message from server
        // Use: boost::asio::read_until() with '\n' delimiter
        // Or: boost::asio::read() with fixed buffer
        return "";
    }

    /**
     * Loop to receive messages from server (run in separate thread)
     */
    void receive_loop() {
        // TODO: Continuously receive and display messages
        // Exit when connection closes or m_running becomes false
    }

    /**
     * Loop to send user input to server (run on main thread)
     */
    void send_loop() {
        // TODO: Read user input and send to server
        // Handle /quit command to exit
        // Use std::getline(std::cin, input)
    }

    boost::asio::io_context& m_io_context;
    tcp::socket m_socket;
    std::string m_username;
    std::atomic<bool> m_running;
};

// ============================================================================
// Main entry point function
// ============================================================================

/**
 * Run the TCP chat client (called from main)
 * @param host Server hostname
 * @param port Server port
 * @return Exit code (0 = success)
 */
inline int run_chat_client(const std::string& host, uint16_t port) {
    try {
        boost::asio::io_context io_context;
        TcpChatClient client(io_context);

        std::cout << "[Client] TCP Chat Client\n";
        std::cout << "[Client] Connecting to " << host << ":" << port << "...\n";

        if (!client.connect(host, port)) {
            std::cerr << "[Client] Failed to connect to server\n";
            return 1;
        }

        std::cout << "[Client] Connected!\n";

        // Get username from user
        std::string username;
        std::cout << "Enter your username: ";
        std::getline(std::cin, username);

        if (username.empty()) {
            username = "anonymous";
        }

        client.set_username(username);
        std::cout << "[Client] Joined as '" << username << "'\n";
        std::cout << "[Client] Type messages and press Enter to send.\n";
        std::cout << "[Client] Type /quit to exit.\n\n";

        client.run();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Client] Error: " << e.what() << "\n";
        return 1;
    }
}

#endif // TCP_CHAT_CLIENT_H
