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
// Steps:
//   1. Read the header with read_packet_header.
//   2. Based on header.type, deserialize the payload:
//      - PLAYER_UPDATE: create BitReader over payload, call serialize_player
//      - CHAT_MESSAGE:  create BitReader over payload, call serialize_chat
//      - OBJECT_UPDATE: call decode_proto_game_object on payload
//      - PING:          no payload to read
//   3. Print what was received (see examples in README).
//
// TODO: Implement this function.
//
inline void dispatch_packet(const uint8_t* buffer, size_t buffer_len) {
    // YOUR CODE HERE
}

#endif // PACKET_H
