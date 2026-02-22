/**
 * RPC Packet System — Header, Framing, and Dispatch
 *
 * Assignment 06: Serialization Library
 *
 * Implements an RPC-style packet system that frames serialized payloads
 * with a message type and length. This ties together all previous sections:
 *   - PLAYER_UPDATE uses bitpacked serialization (serialize_player)
 *   - CHAT_MESSAGE uses string serialization (serialize_string)
 *   - OBJECT_UPDATE uses protobuf wire format (encode/decode_proto_game_object)
 *   - PING has no payload
 *
 * Packet wire format:
 *   [1 byte: MessageType] [2 bytes: payload_len (big-endian)] [payload bytes...]
 *
 * Use Boost.Endian for the 2-byte length field:
 *   boost::endian::native_to_big(value)  — convert to big-endian for writing
 *   boost::endian::big_to_native(value)  — convert from big-endian when reading
 *
 * TODO: Implement write_packet, read_packet_header, serialize_chat, dispatch_packet.
 *
 * References:
 * - https://www.boost.org/doc/libs/release/libs/endian/
 */

#ifndef PACKET_H
#define PACKET_H

#include "serialize.h"
#include "protobuf.h"

#include <boost/endian/conversion.hpp>

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

// =========================================================================
// Message types — each maps to a different RPC call
// =========================================================================
enum class MessageType : uint8_t {
    PLAYER_UPDATE  = 1,  // RPC: UpdatePlayer(PlayerState)       — bitpacked
    CHAT_MESSAGE   = 2,  // RPC: SendChat(sender, text)          — string-based
    OBJECT_UPDATE  = 3,  // RPC: UpdateObject(GameObject)        — protobuf wire format
    PING           = 4,  // RPC: Ping()                          — no payload
};

// =========================================================================
// PacketHeader — 3-byte header preceding every packet
// =========================================================================
struct PacketHeader {
    MessageType type;        ///< 1 byte
    uint16_t    payload_len; ///< 2 bytes, big-endian on the wire
    // Total header: 3 bytes
};

// =========================================================================
// ChatMessage — sender + text
// =========================================================================
struct ChatMessage {
    std::string sender;  ///< length-prefixed string
    std::string text;    ///< length-prefixed string
};

// =========================================================================
// serialize_chat — serialize/deserialize a ChatMessage
// =========================================================================
//
// Uses serialize_string for both sender and text fields.
//
// TODO: Implement this function template.
//
template <typename Stream>
    requires IsStream<Stream>
bool serialize_chat(Stream& stream, ChatMessage& msg) {
    if constexpr (IsWriter<Stream>) {
        // TODO: serialize sender, then text
        // YOUR CODE HERE
        return true;
    } else {
        // TODO: deserialize sender, then text
        // YOUR CODE HERE
        return true;
    }
}

// =========================================================================
// write_packet — write a complete packet (header + payload) to a buffer
// =========================================================================
//
// Format:
//   buffer[0]   = uint8_t(type)
//   buffer[1-2] = payload_len in big-endian (high byte first, then low byte)
//   buffer[3..] = payload bytes
//
// Hint: write big-endian manually:
//   buffer.push_back(static_cast<uint8_t>((len16 >> 8) & 0xFF));  // high
//   buffer.push_back(static_cast<uint8_t>(len16 & 0xFF));          // low
//
// TODO: Implement this function.
//
inline void write_packet(std::vector<uint8_t>& buffer,
                         MessageType type,
                         const uint8_t* payload, size_t payload_len) {
    // YOUR CODE HERE
}

// =========================================================================
// read_packet_header — parse a 3-byte header from a buffer
// =========================================================================
//
// Reads:
//   header.type        = MessageType(buffer[0])
//   header.payload_len = big_to_native( *(uint16_t*)(buffer+1) )
//
// Returns: 3 (bytes consumed for the header).
//
// TODO: Implement this function.
//
inline size_t read_packet_header(const uint8_t* buffer, PacketHeader& header) {
    // YOUR CODE HERE
    return 3;
}

// =========================================================================
// dispatch_packet — read header, then deserialize the appropriate message
// =========================================================================
//
// This function is the "receiver" side of the RPC system.
// It reads the 3-byte header, figures out which message type arrived,
// then deserializes the payload and prints the result.
//
// Step 1 — Parse the header (3 bytes):
//
//   PacketHeader header;
//   size_t header_size = read_packet_header(buffer, header);
//
//   After this, header.type tells you which message arrived and
//   header.payload_len is the number of payload bytes that follow.
//
// Step 2 — Point to the payload (everything after the header):
//
//   const uint8_t* payload     = buffer + header_size;
//   size_t         payload_len = header.payload_len;
//
// Step 3 — Switch on header.type and deserialize accordingly:
//
//   switch (header.type) {
//
//   ---- PLAYER_UPDATE ----
//   Payload was written with a BitWriter/serialize_player.
//   Deserialize by constructing a BitReader over the payload, then calling
//   serialize_player with an empty PlayerState:
//
//     case MessageType::PLAYER_UPDATE: {
//         BitReader reader(payload, payload_len);
//         PlayerState player;
//         serialize_player(reader, player);
//         std::cout << "PLAYER_UPDATE: name=" << player.name
//                   << " x=" << player.x << " y=" << player.y
//                   << " z=" << player.z << " hp=" << (int)player.health << "\n";
//         break;
//     }
//
//   ---- CHAT_MESSAGE ----
//   Same approach — BitReader over payload, then serialize_chat:
//
//     case MessageType::CHAT_MESSAGE: {
//         BitReader reader(payload, payload_len);
//         ChatMessage msg;
//         serialize_chat(reader, msg);
//         std::cout << "CHAT_MESSAGE: [" << msg.sender << "] " << msg.text << "\n";
//         break;
//     }
//
//   ---- OBJECT_UPDATE ----
//   The payload is raw protobuf wire format — no BitReader needed.
//   Pass the payload pointer directly to decode_proto_game_object:
//
//     case MessageType::OBJECT_UPDATE: {
//         GameObject obj;
//         decode_proto_game_object(payload, obj);
//         std::cout << "OBJECT_UPDATE: id=" << obj.id
//                   << " pos=(" << obj.position.x << ","
//                   << obj.position.y << "," << obj.position.z << ")\n";
//         break;
//     }
//
//   ---- PING ----
//   No payload; just acknowledge receipt:
//
//     case MessageType::PING:
//         std::cout << "PING received\n";
//         break;
//   }
//
// NOTE: buffer_len is provided for bounds-checking but is not strictly
// required if you trust write_packet to produce well-formed packets.
//
// TODO: Implement this function.
//
inline void dispatch_packet(const uint8_t* buffer, size_t buffer_len) {
    // YOUR CODE HERE
}

#endif // PACKET_H
