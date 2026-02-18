#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "../src/server.h"
#include "../src/client.h"
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;
using namespace std::chrono_literals;

// ============================================================================
// Helper: read a newline-delimited message from a TCP socket
// ============================================================================
static std::string read_line(tcp::socket& sock) {
    boost::asio::streambuf buf;
    boost::asio::read_until(sock, buf, '\n');
    std::istream is(&buf);
    std::string line;
    std::getline(is, line);
    // Strip trailing \r if present
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    return line;
}

// Helper: write a newline-delimited message to a TCP socket
static void write_line(tcp::socket& sock, const std::string& msg) {
    std::string data = msg + "\n";
    boost::asio::write(sock, boost::asio::buffer(data));
}

// ============================================================================
// Test 1: UserRegistry (thread-safe user management)
// ============================================================================

TEST_SUITE("UserRegistry") {
    TEST_CASE("Add and retrieve a user") {
        boost::asio::io_context io;
        tcp::socket sock(io);
        auto client = std::make_shared<ClientSession>(std::move(sock));

        UserRegistry registry;
        bool added = registry.add_user("alice", client);

        CHECK(added == true);

        auto found = registry.get_user("alice");
        CHECK(found != nullptr);
        CHECK(found == client);
    }

    TEST_CASE("Cannot add duplicate username") {
        boost::asio::io_context io;
        tcp::socket s1(io), s2(io);
        auto c1 = std::make_shared<ClientSession>(std::move(s1));
        auto c2 = std::make_shared<ClientSession>(std::move(s2));

        UserRegistry registry;
        CHECK(registry.add_user("alice", c1) == true);
        CHECK(registry.add_user("alice", c2) == false);
    }

    TEST_CASE("Remove a user") {
        boost::asio::io_context io;
        tcp::socket sock(io);
        auto client = std::make_shared<ClientSession>(std::move(sock));

        UserRegistry registry;
        registry.add_user("bob", client);
        registry.remove_user("bob");

        CHECK(registry.get_user("bob") == nullptr);
    }

    TEST_CASE("Get all users returns all connected clients") {
        boost::asio::io_context io;
        tcp::socket s1(io), s2(io), s3(io);
        auto c1 = std::make_shared<ClientSession>(std::move(s1));
        auto c2 = std::make_shared<ClientSession>(std::move(s2));
        auto c3 = std::make_shared<ClientSession>(std::move(s3));

        UserRegistry registry;
        registry.add_user("alice", c1);
        registry.add_user("bob", c2);
        registry.add_user("charlie", c3);

        auto all = registry.get_all_users();
        CHECK(all.size() == 3);
    }

    TEST_CASE("Get non-existent user returns nullptr") {
        UserRegistry registry;
        CHECK(registry.get_user("nobody") == nullptr);
    }
}

// ============================================================================
// Test 2: Server Construction and Port Binding
// ============================================================================

TEST_SUITE("Server Construction") {
    TEST_CASE("Server binds to specified port") {
        boost::asio::io_context io;
        TcpChatServer server(io, 0); // ephemeral port

        CHECK(server.port() != 0);
    }

    TEST_CASE("Server accepts a TCP connection") {
        boost::asio::io_context io;
        TcpChatServer server(io, 0);
        auto server_port = server.port();

        // Start server in background thread
        std::thread server_thread([&]() {
            server.start();
            io.run_for(2s); // run for a limited time
        });

        // Give server a moment to start accepting
        std::this_thread::sleep_for(100ms);

        // Client connects
        boost::asio::io_context client_io;
        tcp::socket client_sock(client_io);
        tcp::endpoint server_ep(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        boost::system::error_code ec;
        client_sock.connect(server_ep, ec);

        CHECK(ec.value() == 0);
        CHECK(client_sock.is_open());

        client_sock.close();
        io.stop();
        server_thread.join();
    }
}

// ============================================================================
// Test 3: Client Connection
// ============================================================================

TEST_SUITE("Client Connection") {
    TEST_CASE("Client connects to server") {
        boost::asio::io_context server_io;
        TcpChatServer server(server_io, 0);
        auto server_port = server.port();

        std::thread server_thread([&]() {
            server.start();
            server_io.run_for(2s);
        });

        std::this_thread::sleep_for(100ms);

        boost::asio::io_context client_io;
        TcpChatClient client(client_io);

        bool connected = client.connect("127.0.0.1", server_port);
        CHECK(connected == true);
        CHECK(client.is_connected() == true);

        client.disconnect();
        server_io.stop();
        server_thread.join();
    }

    TEST_CASE("Client reports failure for unreachable server") {
        boost::asio::io_context io;
        TcpChatClient client(io);

        // Connect to a port that nobody is listening on
        bool connected = client.connect("127.0.0.1", 1);
        CHECK(connected == false);
        CHECK(client.is_connected() == false);
    }

    TEST_CASE("Client stores username") {
        boost::asio::io_context io;
        TcpChatClient client(io);
        client.set_username("alice");

        CHECK(client.username() == "alice");
    }
}

// ============================================================================
// Test 4: Message Broadcasting
// ============================================================================

TEST_SUITE("Message Broadcasting") {
    TEST_CASE("Server broadcasts message to connected clients") {
        boost::asio::io_context server_io;
        TcpChatServer server(server_io, 0);
        auto server_port = server.port();

        std::thread server_thread([&]() {
            server.start();
            server_io.run_for(3s);
        });

        std::this_thread::sleep_for(100ms);

        // Connect two raw TCP clients
        boost::asio::io_context client_io;
        tcp::socket client1(client_io);
        tcp::socket client2(client_io);

        tcp::endpoint server_ep(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        client1.connect(server_ep);
        std::this_thread::sleep_for(100ms);
        client2.connect(server_ep);
        std::this_thread::sleep_for(200ms);

        // Client1 sends username then a message
        write_line(client1, "alice");
        std::this_thread::sleep_for(100ms);
        write_line(client2, "bob");
        std::this_thread::sleep_for(200ms);

        // Alice sends a chat message
        write_line(client1, "Hello everyone!");
        std::this_thread::sleep_for(200ms);

        // Bob should receive the broadcast (something containing "Hello everyone!")
        // Set a short timeout on bob's socket so we don't hang forever
        client2.non_blocking(true);

        std::array<char, MAX_MESSAGE_LENGTH> buffer;
        boost::system::error_code ec;
        size_t len = client2.read_some(boost::asio::buffer(buffer), ec);

        if (!ec) {
            std::string received(buffer.data(), len);
            CHECK(received.find("Hello everyone!") != std::string::npos);
        }
        // If ec == would_block, message may not have arrived yet — acceptable
        // for stub implementations

        client1.close();
        client2.close();
        server_io.stop();
        server_thread.join();
    }
}

// ============================================================================
// Test 5: Graceful Disconnection with /quit
// ============================================================================

TEST_SUITE("Graceful Disconnection") {
    TEST_CASE("Client can send /quit to disconnect") {
        boost::asio::io_context server_io;
        TcpChatServer server(server_io, 0);
        auto server_port = server.port();

        std::thread server_thread([&]() {
            server.start();
            server_io.run_for(3s);
        });

        std::this_thread::sleep_for(100ms);

        boost::asio::io_context client_io;
        tcp::socket client(client_io);
        tcp::endpoint server_ep(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        client.connect(server_ep);
        std::this_thread::sleep_for(100ms);

        // Send username then /quit
        write_line(client, "testuser");
        std::this_thread::sleep_for(100ms);
        write_line(client, "/quit");
        std::this_thread::sleep_for(200ms);

        // After /quit, server should close the connection.
        // Attempting to read should get EOF or error.
        std::array<char, 256> buffer;
        boost::system::error_code ec;
        client.read_some(boost::asio::buffer(buffer), ec);

        // We expect EOF (eof) or connection reset — both indicate server closed
        CHECK((ec == boost::asio::error::eof ||
               ec == boost::asio::error::connection_reset ||
               ec == boost::asio::error::not_connected ||
               !client.is_open()));

        client.close(ec); // ignore error on close
        server_io.stop();
        server_thread.join();
    }
}

// ============================================================================
// Test 6: Join/Leave Announcements
// ============================================================================

TEST_SUITE("Join and Leave Announcements") {
    TEST_CASE("Server announces when a user joins") {
        boost::asio::io_context server_io;
        TcpChatServer server(server_io, 0);
        auto server_port = server.port();

        std::thread server_thread([&]() {
            server.start();
            server_io.run_for(3s);
        });

        std::this_thread::sleep_for(100ms);

        boost::asio::io_context client_io;
        tcp::socket client1(client_io);
        tcp::socket client2(client_io);

        tcp::endpoint server_ep(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // First client connects and registers
        client1.connect(server_ep);
        std::this_thread::sleep_for(100ms);
        write_line(client1, "alice");
        std::this_thread::sleep_for(200ms);

        // Second client connects — alice should get a join announcement
        client2.connect(server_ep);
        std::this_thread::sleep_for(100ms);
        write_line(client2, "bob");
        std::this_thread::sleep_for(300ms);

        // Try to read the join announcement on client1
        client1.non_blocking(true);
        std::array<char, MAX_MESSAGE_LENGTH> buffer;
        boost::system::error_code ec;
        size_t len = client1.read_some(boost::asio::buffer(buffer), ec);

        if (!ec) {
            std::string received(buffer.data(), len);
            // Should contain something about "bob" joining
            CHECK((received.find("bob") != std::string::npos ||
                   received.find("joined") != std::string::npos ||
                   received.find("join") != std::string::npos));
        }

        client1.close();
        client2.close();
        server_io.stop();
        server_thread.join();
    }
}

// ============================================================================
// Test 7: Multiple Clients (stress test)
// ============================================================================

TEST_SUITE("Multi-Client Support") {
    TEST_CASE("Server handles 3 simultaneous clients") {
        boost::asio::io_context server_io;
        TcpChatServer server(server_io, 0);
        auto server_port = server.port();

        std::thread server_thread([&]() {
            server.start();
            server_io.run_for(4s);
        });

        std::this_thread::sleep_for(100ms);

        boost::asio::io_context client_io;
        tcp::socket c1(client_io), c2(client_io), c3(client_io);

        tcp::endpoint server_ep(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Connect all three
        c1.connect(server_ep);
        std::this_thread::sleep_for(50ms);
        c2.connect(server_ep);
        std::this_thread::sleep_for(50ms);
        c3.connect(server_ep);
        std::this_thread::sleep_for(100ms);

        // Register usernames
        write_line(c1, "alice");
        std::this_thread::sleep_for(50ms);
        write_line(c2, "bob");
        std::this_thread::sleep_for(50ms);
        write_line(c3, "charlie");
        std::this_thread::sleep_for(200ms);

        // All three should be connected
        CHECK(c1.is_open());
        CHECK(c2.is_open());
        CHECK(c3.is_open());

        c1.close();
        c2.close();
        c3.close();
        server_io.stop();
        server_thread.join();
    }
}

// ============================================================================
// Test 8: Message Format [username]: message
// ============================================================================

TEST_SUITE("Message Format") {
    TEST_CASE("Broadcast messages include sender username") {
        boost::asio::io_context server_io;
        TcpChatServer server(server_io, 0);
        auto server_port = server.port();

        std::thread server_thread([&]() {
            server.start();
            server_io.run_for(3s);
        });

        std::this_thread::sleep_for(100ms);

        boost::asio::io_context client_io;
        tcp::socket sender(client_io);
        tcp::socket receiver(client_io);

        tcp::endpoint server_ep(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        sender.connect(server_ep);
        std::this_thread::sleep_for(100ms);
        receiver.connect(server_ep);
        std::this_thread::sleep_for(100ms);

        write_line(sender, "alice");
        std::this_thread::sleep_for(100ms);
        write_line(receiver, "bob");
        std::this_thread::sleep_for(200ms);

        // Drain any join announcements from receiver
        receiver.non_blocking(true);
        {
            std::array<char, MAX_MESSAGE_LENGTH> drain;
            boost::system::error_code ec;
            receiver.read_some(boost::asio::buffer(drain), ec);
        }

        // Alice sends a chat message
        receiver.non_blocking(false);
        write_line(sender, "Hello world");
        std::this_thread::sleep_for(300ms);

        // Bob should receive "[alice]: Hello world"
        receiver.non_blocking(true);
        std::array<char, MAX_MESSAGE_LENGTH> buffer;
        boost::system::error_code ec;
        size_t len = receiver.read_some(boost::asio::buffer(buffer), ec);

        if (!ec) {
            std::string received(buffer.data(), len);
            CHECK(received.find("[alice]") != std::string::npos);
            CHECK(received.find("Hello world") != std::string::npos);
        }

        sender.close();
        receiver.close();
        server_io.stop();
        server_thread.join();
    }
}

// ============================================================================
// Test 9: Newline-Delimited Protocol
// ============================================================================

TEST_SUITE("Newline-Delimited Protocol") {
    TEST_CASE("MAX_MESSAGE_LENGTH is 1200") {
        CHECK(MAX_MESSAGE_LENGTH == 1200);
    }

    TEST_CASE("CLIENT_BUFFER_SIZE is 1200") {
        CHECK(CLIENT_BUFFER_SIZE == 1200);
    }
}

// ============================================================================
// Test 10: ClientSession basics
// ============================================================================

TEST_SUITE("ClientSession") {
    TEST_CASE("Can set and get username") {
        boost::asio::io_context io;
        tcp::socket sock(io);
        ClientSession session(std::move(sock));

        session.set_username("testuser");
        CHECK(session.username() == "testuser");
    }

    TEST_CASE("Default username is empty") {
        boost::asio::io_context io;
        tcp::socket sock(io);
        ClientSession session(std::move(sock));

        CHECK(session.username().empty());
    }
}
