/**
 * Serialization Demo — main.cpp
 *
 * Assignment 06: Serialization Library
 *
 * Demonstrates all serialization components with hex dumps.
 * This file is provided as a starting point; students should extend it
 * to showcase their implementations.
 */

#include "varint.h"
#include "bitstream.h"
#include "serialize.h"
#include "gameobject.h"
#include "protobuf.h"
#include "packet.h"

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <vector>

// =========================================================================
// Hex dump utility — prints a byte buffer in hex + ASCII
// =========================================================================
void hex_dump(const uint8_t* data, size_t size, const char* label = nullptr) {
    if (label) std::cout << "=== " << label << " ===\n";
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << ' ';
        if ((i + 1) % 16 == 0) std::cout << '\n';
    }
    if (size % 16 != 0) std::cout << '\n';
    std::cout << std::dec << "(" << size << " bytes)\n\n";
}

int main() {
    std::cout << "===================================================\n";
    std::cout << "  Assignment 06: Serialization Library Demo\n";
    std::cout << "===================================================\n\n";

    // -----------------------------------------------------------------
    // 1. Varint demo
    // -----------------------------------------------------------------
    std::cout << "--- Varint Encoding ---\n";
    {
        uint8_t buf[10];
        uint32_t values[] = {0, 1, 127, 128, 300, 16384, 0xFFFFFFFF};
        for (auto v : values) {
            size_t n = encode_varint(v, buf);
            std::cout << "  " << v << " -> ";
            for (size_t i = 0; i < n; ++i)
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                          << (int)buf[i] << ' ';
            std::cout << std::dec << "(" << n << " bytes)\n";
        }
    }
    std::cout << '\n';

    // -----------------------------------------------------------------
    // 2. ZigZag demo
    // -----------------------------------------------------------------
    std::cout << "--- ZigZag Encoding ---\n";
    {
        int32_t values[] = {0, -1, 1, -2, 2, -64, 64};
        for (auto v : values) {
            auto encoded = zigzag_encode(v);
            std::cout << "  " << v << " -> " << encoded << '\n';
        }
    }
    std::cout << '\n';

    // -----------------------------------------------------------------
    // 3. bits_required demo
    // -----------------------------------------------------------------
    std::cout << "--- bits_required ---\n";
    std::cout << "  bits_required(0, 1023) = " << bits_required(0u, 1023u) << '\n';
    std::cout << "  bits_required(0, 100)  = " << bits_required(0u, 100u) << '\n';
    std::cout << "  bits_required(0, 359)  = " << bits_required(0u, 359u) << '\n';
    std::cout << "  bits_required(0, 3)    = " << bits_required(0u, 3u) << '\n';
    std::cout << '\n';

    // -----------------------------------------------------------------
    // 4. PlayerState round-trip
    // -----------------------------------------------------------------
    std::cout << "--- PlayerState Round-trip ---\n";
    {
        PlayerState original;
        original.x       = 500;
        original.y       = 300;
        original.z       = 100;
        original.health  = 85;
        original.heading = 270;
        original.team    = 2;
        original.alive   = true;
        original.name    = "Alice";

        // Serialize
        BitWriter writer;
        serialize_player(writer, original);
        hex_dump(writer.data(), writer.size(), "PlayerState serialized");

        // Deserialize
        BitReader reader(writer.data(), writer.size());
        PlayerState deserialized;
        serialize_player(reader, deserialized);

        std::cout << "  Original:     x=" << original.x << " y=" << original.y
                  << " z=" << original.z << " health=" << (int)original.health
                  << " heading=" << original.heading << " team=" << (int)original.team
                  << " alive=" << original.alive << " name=" << original.name << '\n';
        std::cout << "  Deserialized: x=" << deserialized.x << " y=" << deserialized.y
                  << " z=" << deserialized.z << " health=" << (int)deserialized.health
                  << " heading=" << deserialized.heading << " team=" << (int)deserialized.team
                  << " alive=" << deserialized.alive << " name=" << deserialized.name << '\n';
    }
    std::cout << '\n';

    // -----------------------------------------------------------------
    // 5. Protobuf GameObject round-trip
    // -----------------------------------------------------------------
    std::cout << "--- Protobuf GameObject Round-trip ---\n";
    {
        GameObject original;
        original.id = 42;
        original.position = {1.5f, 2.5f, 3.5f};

        uint8_t buf[128];
        size_t n = encode_proto_game_object(original, buf);
        hex_dump(buf, n, "Protobuf-encoded GameObject");

        GameObject decoded;
        decode_proto_game_object(buf, decoded);

        std::cout << "  Original: id=" << original.id
                  << " pos=(" << original.position.x << ", "
                  << original.position.y << ", " << original.position.z << ")\n";
        std::cout << "  Decoded:  id=" << decoded.id
                  << " pos=(" << decoded.position.x << ", "
                  << decoded.position.y << ", " << decoded.position.z << ")\n";
    }
    std::cout << '\n';

    // -----------------------------------------------------------------
    // 6. RPC Packet round-trip
    // -----------------------------------------------------------------
    std::cout << "--- RPC Packet Round-trip ---\n";
    {
        // Build a PLAYER_UPDATE packet
        PlayerState player;
        player.x = 500; player.y = 300; player.z = 100;
        player.health = 85; player.heading = 270;
        player.team = 2; player.alive = true;
        player.name = "Bob";

        BitWriter w;
        serialize_player(w, player);

        std::vector<uint8_t> packet;
        write_packet(packet, MessageType::PLAYER_UPDATE, w.data(), w.size());
        hex_dump(packet.data(), packet.size(), "PLAYER_UPDATE packet");

        std::cout << "Dispatching packet:\n";
        dispatch_packet(packet.data(), packet.size());
    }
    std::cout << '\n';

    std::cout << "===================================================\n";
    std::cout << "  Demo complete!\n";
    std::cout << "===================================================\n";

    return 0;
}
