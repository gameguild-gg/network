/**
 * Nested GameObject Serialization
 *
 * Assignment 06: Serialization Library
 *
 * Implements serialization for a nested struct: GameObject contains a Position
 * with three float fields and an integer ID.
 *
 * You are free to choose your serialization strategy:
 *   - Raw bytes (memcpy float -> uint32_t -> write_bits 32)
 *   - Quantized/compressed floats
 *   - Varint for the ID
 *   - Any combination
 *
 * The only requirement is a correct round-trip.
 *
 * TODO: Implement serialize_position and serialize_game_object.
 */

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "bitstream.h"

#include <cstdint>
#include <cstring>
#include <cmath>

// =========================================================================
// Position — 3D position with floats
// =========================================================================
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

// =========================================================================
// GameObject — has an ID and a nested Position
// =========================================================================
struct GameObject {
    uint32_t id = 0;          ///< unique object identifier
    Position position;        ///< nested struct with floats
};

// ---------------------------------------------------------------------------
// Helper: reinterpret a float as a uint32_t (and back) using memcpy
// ---------------------------------------------------------------------------
// This is the standard-compliant way to type-pun in C++.
//
//   float f = 3.14f;
//   uint32_t bits;
//   std::memcpy(&bits, &f, sizeof(float));
//   // now `bits` holds the IEEE 754 bit pattern of f
//
// To convert back:
//   std::memcpy(&f, &bits, sizeof(float));

// =========================================================================
// serialize_position — serialize/deserialize a Position
// =========================================================================
//
// Recommended approach (raw bytes):
//   Writing: memcpy each float to uint32_t, write_bits(value, 32)
//   Reading: read_bits(32), memcpy uint32_t back to float
//
// You may also quantize if you want (extra credit), but then use
// an epsilon comparison in tests instead of exact equality.
//
// TODO: Implement this function template.
//
template <typename Stream>
    requires IsStream<Stream>
bool serialize_position(Stream& stream, Position& pos) {
    if constexpr (IsWriter<Stream>) {
        // TODO: Serialize pos.x, pos.y, pos.z
        // YOUR CODE HERE
        return true;
    } else {
        // TODO: Deserialize pos.x, pos.y, pos.z
        // YOUR CODE HERE
        return true;
    }
}

// =========================================================================
// serialize_game_object — serialize/deserialize a GameObject
// =========================================================================
//
// Must serialize both the `id` and the nested `position`.
//   - id: write_bits(obj.id, 32) or any encoding you prefer
//   - position: call serialize_position(stream, obj.position)
//
// TODO: Implement this function template.
//
template <typename Stream>
    requires IsStream<Stream>
bool serialize_game_object(Stream& stream, GameObject& obj) {
    if constexpr (IsWriter<Stream>) {
        // TODO: Write id + position
        // YOUR CODE HERE

        stream.flush();
        return true;
    } else {
        // TODO: Read id + position
        // YOUR CODE HERE
        return true;
    }
}

#endif // GAMEOBJECT_H
