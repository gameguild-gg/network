/**
 * Nested GameObject Serialization
 *
 * Assignment 06: Serialization Library
 *
 * Implements serialization for a nested struct: GameObject contains a Position
 * with three float fields and an integer ID.
 *
 * You are free to choose your serialization strategy:
 *   - Raw bytes (std::bit_cast<uint32_t>(float) -> write_bits 32)
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
#include <bit>
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

// Helper: reinterpret float <-> uint32_t using std::bit_cast (C++20).
//   uint32_t bits = std::bit_cast<uint32_t>(f);
//   float back    = std::bit_cast<float>(bits);

// serialize_position: write/read each float as 32 raw bits via bit_cast.
//   Write: stream.write_bits(std::bit_cast<uint32_t>(pos.x), 32)
//   Read:  pos.x = std::bit_cast<float>(stream.read_bits(32))
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
