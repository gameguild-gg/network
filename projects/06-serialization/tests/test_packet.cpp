/**
 * Tests for Section 7: RPC Packet System (20 points)
 *
 * DO NOT MODIFY THIS FILE — your code must pass these tests as-is.
 */
#include <doctest/doctest.h>
#include "../src/packet.h"

#include <vector>
#include <cstdint>
#include <cstring>

// ============================================================================
// Packet header
// ============================================================================

TEST_SUITE("Packet Header") {
    TEST_CASE("write_packet creates correct header") {
        std::vector<uint8_t> packet;
        uint8_t payload[] = {0xAA, 0xBB, 0xCC};
        write_packet(packet, MessageType::PLAYER_UPDATE, payload, 3);

        // Total size: 3 header + 3 payload = 6
        CHECK(packet.size() == 6);

        // Byte 0: message type
        CHECK(packet[0] == static_cast<uint8_t>(MessageType::PLAYER_UPDATE));
        CHECK(packet[0] == 1);

        // Bytes 1-2: payload length in big-endian (3 = 0x0003)
        CHECK(packet[1] == 0x00);
        CHECK(packet[2] == 0x03);

        // Bytes 3-5: payload
        CHECK(packet[3] == 0xAA);
        CHECK(packet[4] == 0xBB);
        CHECK(packet[5] == 0xCC);
    }

    TEST_CASE("read_packet_header parses correctly") {
        std::vector<uint8_t> packet;
        uint8_t payload[] = {0x01, 0x02};
        write_packet(packet, MessageType::CHAT_MESSAGE, payload, 2);

        PacketHeader header;
        size_t consumed = read_packet_header(packet.data(), header);

        CHECK(consumed == 3);
        CHECK(header.type == MessageType::CHAT_MESSAGE);
        CHECK(header.payload_len == 2);
    }

    TEST_CASE("empty payload (PING)") {
        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::PING, nullptr, 0);

        CHECK(packet.size() == 3); // header only

        PacketHeader header;
        read_packet_header(packet.data(), header);
        CHECK(header.type == MessageType::PING);
        CHECK(header.payload_len == 0);
    }

    TEST_CASE("big-endian payload length") {
        // Payload of 300 bytes: 300 = 0x012C
        // Big-endian: 0x01, 0x2C
        std::vector<uint8_t> payload(300, 0xFF);
        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::OBJECT_UPDATE, payload.data(), payload.size());

        CHECK(packet[1] == 0x01);
        CHECK(packet[2] == 0x2C);

        PacketHeader header;
        read_packet_header(packet.data(), header);
        CHECK(header.payload_len == 300);
    }
}

// ============================================================================
// Chat message serialization
// ============================================================================

TEST_SUITE("ChatMessage Serialization") {
    TEST_CASE("basic round-trip") {
        ChatMessage original;
        original.sender = "Alice";
        original.text   = "Hello, World!";

        BitWriter w;
        CHECK(serialize_chat(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        ChatMessage decoded;
        CHECK(serialize_chat(r, decoded));

        CHECK(decoded.sender == "Alice");
        CHECK(decoded.text   == "Hello, World!");
    }

    TEST_CASE("empty sender and text") {
        ChatMessage original;
        original.sender = "";
        original.text   = "";

        BitWriter w;
        CHECK(serialize_chat(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        ChatMessage decoded;
        CHECK(serialize_chat(r, decoded));

        CHECK(decoded.sender == "");
        CHECK(decoded.text   == "");
    }

    TEST_CASE("special characters") {
        ChatMessage original;
        original.sender = "Bob99";
        original.text   = "GG! Score: 42-17 @#$";

        BitWriter w;
        CHECK(serialize_chat(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        ChatMessage decoded;
        CHECK(serialize_chat(r, decoded));

        CHECK(decoded.sender == "Bob99");
        CHECK(decoded.text   == "GG! Score: 42-17 @#$");
    }
}

// ============================================================================
// Full RPC packet round-trips
// ============================================================================

TEST_SUITE("RPC Packet Round-trip") {
    TEST_CASE("PLAYER_UPDATE packet round-trip") {
        // Serialize a PlayerState
        PlayerState original;
        original.x = 500; original.y = 300; original.z = 100;
        original.health = 85; original.heading = 270;
        original.team = 2; original.alive = true;
        original.name = "TestPlayer";

        BitWriter w;
        serialize_player(w, original);

        // Wrap in a packet
        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::PLAYER_UPDATE, w.data(), w.size());

        // Parse header
        PacketHeader header;
        read_packet_header(packet.data(), header);
        CHECK(header.type == MessageType::PLAYER_UPDATE);
        CHECK(header.payload_len == w.size());

        // Deserialize payload
        const uint8_t* payload = packet.data() + 3;
        BitReader r(payload, header.payload_len);
        PlayerState decoded;
        serialize_player(r, decoded);

        CHECK(decoded.x       == 500);
        CHECK(decoded.y       == 300);
        CHECK(decoded.z       == 100);
        CHECK(decoded.health  == 85);
        CHECK(decoded.heading == 270);
        CHECK(decoded.team    == 2);
        CHECK(decoded.alive   == true);
        CHECK(decoded.name    == "TestPlayer");
    }

    TEST_CASE("CHAT_MESSAGE packet round-trip") {
        ChatMessage original;
        original.sender = "Alice";
        original.text   = "Hello everyone!";

        BitWriter w;
        serialize_chat(w, original);

        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::CHAT_MESSAGE, w.data(), w.size());

        PacketHeader header;
        read_packet_header(packet.data(), header);
        CHECK(header.type == MessageType::CHAT_MESSAGE);

        const uint8_t* payload = packet.data() + 3;
        BitReader r(payload, header.payload_len);
        ChatMessage decoded;
        serialize_chat(r, decoded);

        CHECK(decoded.sender == "Alice");
        CHECK(decoded.text   == "Hello everyone!");
    }

    TEST_CASE("OBJECT_UPDATE packet round-trip (protobuf)") {
        GameObject original;
        original.id = 99;
        original.position = {10.0f, 20.0f, 30.0f};

        uint8_t proto_buf[128];
        size_t proto_len = encode_proto_game_object(original, proto_buf);

        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::OBJECT_UPDATE, proto_buf, proto_len);

        PacketHeader header;
        read_packet_header(packet.data(), header);
        CHECK(header.type == MessageType::OBJECT_UPDATE);
        CHECK(header.payload_len == proto_len);

        const uint8_t* payload = packet.data() + 3;
        GameObject decoded;
        decode_proto_game_object(payload, decoded);

        CHECK(decoded.id == 99);
        CHECK(decoded.position.x == 10.0f);
        CHECK(decoded.position.y == 20.0f);
        CHECK(decoded.position.z == 30.0f);
    }

    TEST_CASE("PING packet round-trip") {
        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::PING, nullptr, 0);

        PacketHeader header;
        read_packet_header(packet.data(), header);
        CHECK(header.type == MessageType::PING);
        CHECK(header.payload_len == 0);
    }

    TEST_CASE("dispatch_packet does not crash") {
        // Build a PING packet and dispatch — should print and not crash
        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::PING, nullptr, 0);
        CHECK_NOTHROW(dispatch_packet(packet.data(), packet.size()));
    }

    TEST_CASE("dispatch PLAYER_UPDATE does not crash") {
        PlayerState player;
        player.x = 100; player.y = 200; player.z = 300;
        player.health = 50; player.heading = 90;
        player.team = 1; player.alive = true;
        player.name = "Bot";

        BitWriter w;
        serialize_player(w, player);

        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::PLAYER_UPDATE, w.data(), w.size());
        CHECK_NOTHROW(dispatch_packet(packet.data(), packet.size()));
    }
}
