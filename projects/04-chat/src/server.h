/**
 * TCP Chat Server
 *
 * Assignment 04: TCP Chatroom
 *
 * A multi-client TCP chat server that:
 * 1. Accepts multiple simultaneous client connections
 * 2. Broadcasts messages to all connected clients
 * 3. Handles graceful disconnection with /quit command
 * 4. Announces when users join or leave
 *
 * TODO: Implement the server methods following the requirements in README.md
 */

#ifndef TCP_CHAT_SERVER_H
#define TCP_CHAT_SERVER_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <array>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>

using boost::asio::ip::tcp;

constexpr size_t MAX_MESSAGE_LENGTH = 1200;

/**
 * Connected client session
 * Represents a single connected client with their socket and username
 */
class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(tcp::socket socket)
        : m_socket(std::move(socket))
    {
    }

    tcp::socket& socket() { return m_socket; }
    const std::string& username() const { return m_username; }
    void set_username(const std::string& name) { m_username = name; }

private:
    tcp::socket m_socket;
    std::string m_username;
};

using ClientPtr = std::shared_ptr<ClientSession>;

/**
 * User Registry - tracks all connected clients
 * Thread-safe container for managing connected users
 */
class UserRegistry {
public:
    /**
     * Add a client to the registry
     * @param username The client's username
     * @param client Shared pointer to the client session
     * @return true if added successfully, false if username already exists
     */
    bool add_user([[maybe_unused]] const std::string& username, 
                  [[maybe_unused]] ClientPtr client) {
        // TODO: Add user to registry (thread-safe)
        // Use std::lock_guard with m_mutex
        return false;
    }

    /**
     * Remove a client from the registry
     * @param username The username to remove
     */
    void remove_user([[maybe_unused]] const std::string& username) {
        // TODO: Remove user from registry (thread-safe)
    }

    /**
     * Get a client by username
     * @param username The username to look up
     * @return The client session, or nullptr if not found
     */
    ClientPtr get_user([[maybe_unused]] const std::string& username) {
        // TODO: Look up user in registry (thread-safe)
        return nullptr;
    }

    /**
     * Get all connected clients
     * @return Vector of all client sessions
     */
    std::vector<ClientPtr> get_all_users() {
        // TODO: Return all connected clients (thread-safe)
        return {};
    }

    /**
     * Broadcast a message to all connected clients
     * @param message The message to broadcast
     * @param exclude Optional username to exclude from broadcast
     */
    void broadcast([[maybe_unused]] const std::string& message,
                   [[maybe_unused]] const std::string& exclude = "") {
        // TODO: Send message to all clients except the excluded one
        // Use boost::asio::write() for each client
    }

private:
    std::unordered_map<std::string, ClientPtr> m_users;
    std::mutex m_mutex;
};

/**
 * TCP Chat Server class
 *
 * Example usage:
 *   boost::asio::io_context io;
 *   TcpChatServer server(io, 9999);
 *   io.run();  // Or use threads for non-blocking
 */
class TcpChatServer {
public:
    /**
     * Construct server on specified port
     * @param io_context The Boost.Asio io_context
     * @param port Port to listen on (default: 9999)
     */
    TcpChatServer(boost::asio::io_context& io_context, uint16_t port = 9999)
        : m_io_context(io_context)
        , m_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
        , m_port(port)
    {
        std::cout << "[Server] TCP Chat Server starting on port " << port << "...\n";
    }

    /**
     * Start accepting client connections
     * Call this before running the io_context
     */
    void start() {
        // TODO: Start accepting connections asynchronously
        // Use async_accept() or blocking accept() in a thread
        accept_connection();
    }

    /**
     * Get the port the server is bound to
     */
    uint16_t port() const {
        return m_acceptor.local_endpoint().port();
    }

    /**
     * Get the user registry
     */
    UserRegistry& registry() { return m_registry; }

private:
    /**
     * Accept a new client connection
     */
    void accept_connection() {
        // TODO: Accept incoming connections
        // For each connection:
        // 1. Create a ClientSession
        // 2. Start a thread to handle the client (or use async)
        // 3. Continue accepting more connections
    }

    /**
     * Handle a connected client
     * @param client The client session to handle
     */
    void handle_client([[maybe_unused]] ClientPtr client) {
        // TODO: Handle client communication
        // 1. Read username (first message)
        // 2. Add to registry
        // 3. Broadcast join message
        // 4. Loop: read messages, broadcast to others
        // 5. Handle /quit command
        // 6. On disconnect: remove from registry, broadcast leave message
    }

    /**
     * Process a command (messages starting with /)
     * @param client The client who sent the command
     * @param command The command string (including /)
     * @return true if client should disconnect
     */
    bool process_command([[maybe_unused]] ClientPtr client,
                         [[maybe_unused]] const std::string& command) {
        // TODO: Handle commands like /quit, /msg, /nick, /list, /help
        // Return true if client should disconnect (/quit)
        return false;
    }

    boost::asio::io_context& m_io_context;
    tcp::acceptor m_acceptor;
    uint16_t m_port;
    UserRegistry m_registry;
};

// ============================================================================
// Main entry point function
// ============================================================================

/**
 * Run the TCP chat server (called from main)
 * @param port Port to listen on
 * @return Exit code (0 = success)
 */
inline int run_chat_server(uint16_t port) {
    try {
        boost::asio::io_context io_context;
        TcpChatServer server(io_context, port);

        std::cout << "[Server] Listening on port " << server.port() << "...\n";
        std::cout << "[Server] Press Ctrl+C to stop.\n\n";

        server.start();

        // Run the io_context (or implement your own threading model)
        io_context.run();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Server] Error: " << e.what() << "\n";
        return 1;
    }
}

#endif // TCP_CHAT_SERVER_H
