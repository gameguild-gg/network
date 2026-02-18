/**
 * Protobuf Wire Format Encoding
 *
 * Assignment 06: Serialization Library
 *
 * Implements protobuf-style tag-value encoding using your varint functions.
 * This connects your varint work to the real Protocol Buffers wire format.
 *
 * Wire format recap:
 *   Each field is preceded by a TAG, which is varint-encoded:
 *     tag = (field_number << 3) | wire_type
 *
 *   Wire types:
 *     0 (VARINT) — int32, uint32, bool, enum
 *     1 (I64)    — fixed64, double
 *     2 (LEN)    — string, bytes, nested messages
 *     5 (I32)    — fixed32, float
 *
 * Schema for this assignment:
 *   message Position {
 *       float x = 1;  // wire type I32
 *       float y = 2;  // wire type I32
 *       float z = 3;  // wire type I32
 *   }
 *   message GameObject {
 *       uint32 id       = 1;  // wire type VARINT
 *       Position position = 2;  // wire type LEN (nested message)
 *   }
 *
 * TODO: Implement all functions below.
 *
 * References:
 * - https://protobuf.dev/programming-guides/encoding/
 */

#ifndef PROTOBUF_H
#define PROTOBUF_H

#include "varint.h"
#include "gameobject.h"

#include <cstdint>
#include <cstddef>
#include <bit>

// =========================================================================
// Wire types from the protobuf specification
// =========================================================================
enum class WireType : uint8_t {
    VARINT = 0,  // int32, uint32, bool, enum
    I64    = 1,  // fixed64, double
    LEN    = 2,  // string, bytes, nested messages
    I32    = 5,  // fixed32, float
};

// =========================================================================
// make_tag — encode a field tag
// =========================================================================
// Formula: (field_number << 3) | uint8_t(wire_type)
//
// Example: field 2, wire type LEN(2) -> (2 << 3) | 2 = 18
//
// TODO: Implement this function (one line).
inline uint32_t make_tag(uint32_t field_number, WireType wire_type) {
    // YOUR CODE HERE
    return 0; // placeholder
}

// =========================================================================
// parse_tag — decode a tag into field number and wire type
// =========================================================================
// field_number = tag >> 3
// wire_type    = tag & 0x07
//
// TODO: Implement this function.
inline void parse_tag(uint32_t tag, uint32_t& field_number, WireType& wire_type) {
    // YOUR CODE HERE
}

// =========================================================================
// encode_proto_game_object — encode a GameObject in protobuf wire format
// =========================================================================
//
// Wire layout:
//   [tag: field 1, VARINT] [varint: id]
//   [tag: field 2, LEN]    [varint: nested_length] [nested Position bytes]
//
// Nested Position layout:
//   [tag: field 1, I32] [4 bytes: float x as uint32_t]
//   [tag: field 2, I32] [4 bytes: float y as uint32_t]
//   [tag: field 3, I32] [4 bytes: float z as uint32_t]
//
// Approach:
//   1. Encode Position into a temporary buffer first (so you know its byte length).
//   2. Write the outer message: field 1 (id) + field 2 (LEN with position bytes).
//
// Hint for I32 fields: use std::bit_cast<uint32_t>(float_val) instead of memcpy.
//
// Returns: total bytes written to buffer.
//
// TODO: Implement this function.
//
inline size_t encode_proto_game_object(const GameObject& obj, uint8_t* buffer) {
    // YOUR CODE HERE
    return 0; // placeholder
}

// =========================================================================
// decode_proto_game_object — decode a protobuf-encoded GameObject
// =========================================================================
//
// Read tag-value pairs until all bytes are consumed.
// For each tag:
//   - Parse field_number and wire_type
//   - Based on wire_type, read the appropriate payload:
//       VARINT: decode_varint
//       I32: read 4 bytes, memcpy to float
//       LEN: read varint length, then recursively decode nested fields
//
// Returns: bytes consumed.
//
// TODO: Implement this function.
//
inline size_t decode_proto_game_object(const uint8_t* buffer, GameObject& obj) {
    // YOUR CODE HERE
    return 0; // placeholder
}

#endif // PROTOBUF_H
