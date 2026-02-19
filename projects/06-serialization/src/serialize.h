/**
 * Unified Serialization — PlayerState + String Serialization
 *
 * Assignment 06: Serialization Library
 *
 * Implements Glenn Fiedler's unified serialize pattern:
 *   - ONE template function handles both writing and reading
 *   - Constrained with C++20 concepts (IsStream)
 *   - Uses bits_required() for all bit widths (no magic numbers)
 *
 * Also implements length-prefixed string serialization with varint length.
 *
 * TODO: Implement serialize_string and serialize_player.
 *
 * References:
 * - https://gafferongames.com/post/serialization_strategies/
 */

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "bitstream.h"
#include "varint.h"

#include <string>
#include <cstdint>
#include <cstring>

// =========================================================================
// PlayerState — game state for one player
// =========================================================================
struct PlayerState {
    uint16_t    x       = 0;    // 0–1023  (10 bits via bits_required)
    uint16_t    y       = 0;    // 0–1023  (10 bits)
    uint16_t    z       = 0;    // 0–1023  (10 bits)
    uint8_t     health  = 0;    // 0–100   (7 bits)
    uint16_t    heading = 0;    // 0–359   (9 bits)
    uint8_t     team    = 0;    // 0–3     (2 bits)
    bool        alive   = false; //        (1 bit)
    std::string name;           // length-prefixed string
    // Total bitpacked fields: 49 bits = 7 bytes + name string
};

// =========================================================================
// serialize_string — length-prefixed string read/write
// =========================================================================
//
// Wire format:
//   [varint length] [raw UTF-8 bytes, no null terminator]
//
// Writing (IsWriter):
//   1. Reject strings longer than 255 bytes (return false).
//   2. Encode the string length as a varint directly to the stream
//      using encode_varint(value, stream).
//   3. Write the raw string bytes using write_bytes().
//
// Reading (IsReader):
//   1. Decode the varint length directly from the stream using
//      decode_varint(stream, len).
//   2. Read that many raw bytes for the string content.
//
// TODO: Implement this function template.
//
template <typename Stream>
    requires IsStream<Stream>
bool serialize_string(Stream& stream, std::string& str) {
    if constexpr (IsWriter<Stream>) {
        // TODO: Write the string (length-prefixed with varint)
        // YOUR CODE HERE
        return true;
    } else {
        // TODO: Read the string (decode varint length, then read bytes)
        // YOUR CODE HERE
        return true;
    }
}

// =========================================================================
// serialize_player — unified serialize function for PlayerState
// =========================================================================
//
// This single function handles BOTH serialization and deserialization.
// Use `if constexpr (IsWriter<Stream>)` to branch where needed.
//
// For each numeric field, use bits_required() to compute the bit width:
//   stream.write_bits(player.x, bits_required(0u, 1023u))   // writing
//   player.x = stream.read_bits(bits_required(0u, 1023u))   // reading
//
// Field order (MUST match for read/write):
//   1. x       — bits_required(0u, 1023u)
//   2. y       — bits_required(0u, 1023u)
//   3. z       — bits_required(0u, 1023u)
//   4. health  — bits_required(0u, 100u)
//   5. heading — bits_required(0u, 359u)
//   6. team    — bits_required(0u, 3u)
//   7. alive   — 1 bit (write_bool / read_bool)
//   8. name    — serialize_string (auto byte-aligns)
//
// After writing, call stream.flush().
//
// TODO: Implement this function template.
//
template <typename Stream>
    requires IsStream<Stream>
bool serialize_player(Stream& stream, PlayerState& player) {
    if constexpr (IsWriter<Stream>) {
        // TODO: Write all fields using write_bits / write_bool / serialize_string
        // YOUR CODE HERE

        stream.flush();
        return true;
    } else {
        // TODO: Read all fields using read_bits / read_bool / serialize_string
        // YOUR CODE HERE

        return true;
    }
}

#endif // SERIALIZE_H
