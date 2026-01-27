#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "../src/server.h"
#include "../src/client.h"
#include <thread>

using boost::asio::ip::udp;

// ============================================================================
// Test 1: UDP Transmission Limit (1200 bytes)
// ============================================================================

TEST_SUITE("UDP Transmission Limit") {
    TEST_CASE("Can send and receive exactly 1200 bytes") {
        boost::asio::io_context io;

        // Create two sockets for send/receive
        udp::socket sender(io, udp::v4());
        udp::socket receiver(io, udp::endpoint(udp::v4(), 0));
        auto receiver_port = receiver.local_endpoint().port();

        udp::endpoint receiver_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            receiver_port
        );

        // Send exactly 1200 bytes
        std::string max_message(1200, 'X');
        sender.send_to(boost::asio::buffer(max_message), receiver_endpoint);

        // Receive and verify
        std::array<char, MAX_UDP_PAYLOAD> buffer;
        udp::endpoint sender_endpoint;
        size_t len = receiver.receive_from(boost::asio::buffer(buffer), sender_endpoint);

        CHECK(len == 1200);
        CHECK(std::string(buffer.data(), len) == max_message);
    }

    TEST_CASE("Messages larger than MAX_UDP_PAYLOAD are truncated") {
        // This test demonstrates WHY we use MAX_UDP_PAYLOAD = 1200
        // When a message larger than the receive buffer is sent,
        // the receiver only gets the first MAX_UDP_PAYLOAD bytes!

        boost::asio::io_context io;

        udp::socket sender(io, udp::v4());
        udp::socket receiver(io, udp::endpoint(udp::v4(), 0));
        auto receiver_port = receiver.local_endpoint().port();

        udp::endpoint receiver_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            receiver_port
        );

        // Send a message LARGER than MAX_UDP_PAYLOAD
        std::string large_message(2000, 'L');
        sender.send_to(boost::asio::buffer(large_message), receiver_endpoint);

        // Receive into a buffer of MAX_UDP_PAYLOAD size
        std::array<char, MAX_UDP_PAYLOAD> buffer;
        udp::endpoint sender_endpoint;
        size_t len = receiver.receive_from(boost::asio::buffer(buffer), sender_endpoint);

        // Only the first 1200 bytes are received - the rest is LOST!
        CHECK(len == MAX_UDP_PAYLOAD);
        CHECK(len < large_message.size());

        // Verify we got the first 1200 bytes
        std::string received(buffer.data(), len);
        CHECK(received == std::string(1200, 'L'));
    }

    TEST_CASE("Server echoes maximum payload correctly") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();

        udp::socket client(io, udp::v4());
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Send max payload in background thread
        std::string max_message(1200, 'M');
        std::thread client_thread([&]() {
            client.send_to(boost::asio::buffer(max_message), server_endpoint);
        });

        client_thread.join();

        // Server processes and echoes
        std::string received = server.process_one();
        CHECK(received.size() == 1200);

        // Client receives echo
        std::array<char, MAX_UDP_PAYLOAD> buffer;
        udp::endpoint sender;
        size_t len = client.receive_from(boost::asio::buffer(buffer), sender);

        CHECK(len == 1200);
        CHECK(std::string(buffer.data(), len) == max_message);
    }
}

// ============================================================================
// Test 2: Server Discovery Response (echoes DISCOVER back)
// ============================================================================

TEST_SUITE("Server Discovery Response") {
    TEST_CASE("Server echoes DISCOVER message") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();

        udp::socket client(io, udp::v4());
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Send DISCOVER request
        std::string discover_msg(DISCOVER_MESSAGE);
        client.send_to(boost::asio::buffer(discover_msg), server_endpoint);

        // Server processes the discovery request
        std::string received = server.process_one();
        CHECK(received == DISCOVER_MESSAGE);

        // Client receives DISCOVER echoed back
        std::array<char, MAX_UDP_PAYLOAD> buffer;
        udp::endpoint sender;
        size_t len = client.receive_from(boost::asio::buffer(buffer), sender);

        std::string response(buffer.data(), len);
        CHECK(response == DISCOVER_MESSAGE);
    }

    TEST_CASE("Server responds from correct endpoint") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();

        udp::socket client(io, udp::v4());
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Send DISCOVER
        client.send_to(boost::asio::buffer(std::string(DISCOVER_MESSAGE)), server_endpoint);
        server.process_one();

        // Verify response comes from server's port
        std::array<char, MAX_UDP_PAYLOAD> buffer;
        udp::endpoint response_sender;
        client.receive_from(boost::asio::buffer(buffer), response_sender);

        CHECK(response_sender.port() == server_port);
    }

    TEST_CASE("Server handles multiple discovery requests") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();

        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Multiple clients send DISCOVER
        udp::socket client1(io, udp::v4());
        udp::socket client2(io, udp::v4());

        client1.send_to(boost::asio::buffer(std::string(DISCOVER_MESSAGE)), server_endpoint);
        server.process_one();

        std::array<char, MAX_UDP_PAYLOAD> buffer;
        udp::endpoint sender;
        size_t len = client1.receive_from(boost::asio::buffer(buffer), sender);
        CHECK(std::string(buffer.data(), len) == DISCOVER_MESSAGE);

        client2.send_to(boost::asio::buffer(std::string(DISCOVER_MESSAGE)), server_endpoint);
        server.process_one();

        len = client2.receive_from(boost::asio::buffer(buffer), sender);
        CHECK(std::string(buffer.data(), len) == DISCOVER_MESSAGE);
    }
}

// ============================================================================
// Test 3: Client Broadcast Discovery
// ============================================================================

TEST_SUITE("Client Broadcast Discovery") {
    TEST_CASE("Client socket has broadcast option enabled") {
        boost::asio::io_context io;
        UdpEchoClient client(io);

        // Check broadcast option is set
        boost::asio::socket_base::broadcast option;
        client.socket().get_option(option);
        CHECK(option.value() == true);
    }

    TEST_CASE("Client can send to broadcast address") {
        boost::asio::io_context io;
        UdpEchoClient client(io);

        // This should not throw - broadcast is enabled
        udp::endpoint broadcast_ep(
            boost::asio::ip::address_v4::broadcast(),
            9999
        );

        REQUIRE_NOTHROW(
            client.socket().send_to(
                boost::asio::buffer(std::string("TEST")),
                broadcast_ep
            )
        );
    }
}

// ============================================================================
// Test 4: Echo Functionality (Client -> Server -> Client)
// ============================================================================

TEST_SUITE("Echo Functionality") {
    TEST_CASE("Server echoes simple message") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();

        udp::socket client(io, udp::v4());
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Send message
        std::string message = "Hello, Server!";
        client.send_to(boost::asio::buffer(message), server_endpoint);

        // Server echoes
        std::string received = server.process_one();
        CHECK(received == message);

        // Client receives echo
        std::array<char, MAX_UDP_PAYLOAD> buffer;
        udp::endpoint sender;
        size_t len = client.receive_from(boost::asio::buffer(buffer), sender);

        CHECK(std::string(buffer.data(), len) == message);
    }

    TEST_CASE("Client send_and_receive gets echo") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        UdpEchoClient client(io);
        client.connect(server_endpoint);

        // Server thread echoes (needed because both block)
        std::thread server_thread([&]() {
            server.process_one();
        });

        // Client sends and receives
        std::string message = "Echo test message";
        auto echo = client.send_and_receive(message);

        server_thread.join();

        REQUIRE(echo.has_value());
        CHECK(*echo == message);
    }

    TEST_CASE("Echo preserves message content exactly") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        UdpEchoClient client(io);
        client.connect(server_endpoint);

        // Test various message types
        std::vector<std::string> test_messages = {
            "Simple text",
            "With numbers 123456",
            "Special !@#$%^&*()",
            std::string(100, 'A'),  // Repeated chars
        };

        for (const auto& msg : test_messages) {
            std::thread server_thread([&]() {
                server.process_one();
            });

            auto echo = client.send_and_receive(msg);
            server_thread.join();

            REQUIRE(echo.has_value());
            CHECK(*echo == msg);
        }
    }

    TEST_CASE("Echo works for multiple sequential messages") {
        boost::asio::io_context io;

        UdpEchoServer server(io, 0);
        auto server_port = server.port();
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        UdpEchoClient client(io);
        client.connect(server_endpoint);

        // Send 5 messages in sequence
        for (int i = 1; i <= 5; ++i) {
            std::string msg = "Message #" + std::to_string(i);

            std::thread server_thread([&]() {
                server.process_one();
            });

            auto echo = client.send_and_receive(msg);
            server_thread.join();

            REQUIRE(echo.has_value());
            CHECK(*echo == msg);
        }
    }
}

// ============================================================================
// Integration Test: Full Discovery + Echo Workflow
// ============================================================================

TEST_SUITE("Full Workflow Integration") {
    TEST_CASE("Discovery then echo workflow") {
        boost::asio::io_context io;

        // Create server
        UdpEchoServer server(io, 0);
        auto server_port = server.port();
        udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Create client and manually connect (simulating discovery)
        UdpEchoClient client(io);
        client.connect(server_endpoint);
        CHECK(client.is_connected());

        // Echo messages
        std::thread echo_thread([&]() {
            server.process_one();
        });

        auto echo = client.send_and_receive("After discovery!");
        echo_thread.join();

        REQUIRE(echo.has_value());
        CHECK(*echo == "After discovery!");
    }
}
