/**
 * Varint + ZigZag Encoding
 *
 * Assignment 06: Serialization Library
 *
 * Implements:
 * - bits_required(min, max): minimum bits to represent a range
 * - encode_varint / decode_varint: unsigned variable-length integer encoding
 * - zigzag_encode / zigzag_decode: signed-to-unsigned mapping for small magnitudes
 * - encode_signed_varint / decode_signed_varint: convenience wrappers
 *
 * References:
 * - https://protobuf.dev/programming-guides/encoding/#varints
 * - https://en.wikipedia.org/wiki/Variable-length_quantity
 *
 * TODO: Implement all functions below.
 */

#ifndef VARINT_H
#define VARINT_H

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <concepts>
#include <bit>
#include <type_traits>

// ---------------------------------------------------------------------------
// bits_required: compute the minimum number of bits to represent [min, max]
// ---------------------------------------------------------------------------
// Formula: ceil(log2(max - min + 1)), with edge case min == max -> 1
//
// Hint: std::bit_width(x) returns the number of bits needed to represent x.
//       std::bit_width(0) == 0, std::bit_width(1) == 1, std::bit_width(7) == 3
//       Think about how (max - min) relates to the number of distinct values.
//
// TODO: Implement this function.
constexpr int bits_required(std::unsigned_integral auto min,
                            std::unsigned_integral auto max) {
    // YOUR CODE HERE
    return 0; // placeholder
}

// ---------------------------------------------------------------------------
// encode_varint: encode an unsigned integer as a varint into buffer
// ---------------------------------------------------------------------------
// Each byte uses 7 data bits + 1 continuation bit (MSB).
// If MSB is set, more bytes follow. If MSB is clear, this is the last byte.
//
// Example: value = 300 (0b100101100)
//   Byte 0: 0b10101100 (low 7 bits = 0101100, MSB=1 means "more")
//   Byte 1: 0b00000010 (next 7 bits = 0000010, MSB=0 means "done")
//
// ---------------------------------------------------------------------------
// encode_varint_core: core varint encoding logic (single implementation)
// ---------------------------------------------------------------------------
// Each byte uses 7 data bits + 1 continuation bit (MSB).
// If MSB is set, more bytes follow. If MSB is clear, this is the last byte.
//
// Takes a callable `write_byte` that writes a single uint8_t to any destination
// (buffer, stream, etc.). This avoids duplicating the encoding loop.
//
// WriteByteFn signature: void write_byte(uint8_t b)
//
// Example: value = 300 (0b100101100)
//   Byte 0: 0b10101100 (low 7 bits = 0101100, MSB=1 means "more")
//   Byte 1: 0b00000010 (next 7 bits = 0000010, MSB=0 means "done")
//
// Returns: number of bytes written.
//
// TODO: Implement this function.
template <typename WriteByteFn>
size_t encode_varint_core(std::unsigned_integral auto value, WriteByteFn write_byte) {
    size_t i = 0;
    // your code goes here
    return i;
}

// ---------------------------------------------------------------------------
// encode_varint (buffer overload): encode to a uint8_t* buffer
// ---------------------------------------------------------------------------
// Wraps encode_varint_core with a lambda that writes to buffer[i++].
size_t encode_varint(std::unsigned_integral auto value, uint8_t* buffer) {
    size_t i = 0;
    return encode_varint_core(value, [&](uint8_t b) { buffer[i++] = b; });
}

// ---------------------------------------------------------------------------
// encode_varint (stream overload): encode to any stream with write_bytes()
// ---------------------------------------------------------------------------
// Wraps encode_varint_core with a lambda that writes one byte via stream.
template <typename Stream>
    requires requires(Stream& s) { s.write_bytes(static_cast<const uint8_t*>(nullptr), size_t(0)); }
size_t encode_varint(std::unsigned_integral auto value, Stream& stream) {
    return encode_varint_core(value, [&](uint8_t b) { stream.write_bytes(&b, 1); });
}

// ---------------------------------------------------------------------------
// decode_varint_core: core varint decoding logic (single implementation)
// ---------------------------------------------------------------------------
// Reverse of encode_varint. Read bytes while MSB is set, accumulating
// 7 bits at a time into out_value.
//
// Takes a callable `read_byte` that returns the next uint8_t from any source
// (buffer, stream, etc.). This avoids duplicating the decoding loop.
//
// ReadByteFn signature: uint8_t read_byte()  — returns the next byte
//
// Hint: decltype(ref_param) includes &, so use:
//   using T = std::remove_reference_t<decltype(out_value)>;
// before casting with static_cast<T>(...).
//
// Returns: number of bytes consumed.
//
// TODO: Implement this function.
template <typename ReadByteFn>
size_t decode_varint_core(ReadByteFn read_byte, std::unsigned_integral auto& out_value) {
    using T = std::remove_reference_t<decltype(out_value)>;
    T result = 0;
    size_t i = 0;
    int shift = 0;
    uint8_t byte;
    // your code goes here
    return i;
}

// ---------------------------------------------------------------------------
// decode_varint (buffer overload): decode from a uint8_t* buffer
// ---------------------------------------------------------------------------
// Wraps decode_varint_core with a lambda that reads from buffer[i++].
size_t decode_varint(const uint8_t* buffer, std::unsigned_integral auto& out_value) {
    size_t i = 0;
    return decode_varint_core([&]() -> uint8_t { return buffer[i++]; }, out_value);
}

// ---------------------------------------------------------------------------
// decode_varint (stream overload): decode from any stream with read_bytes()
// ---------------------------------------------------------------------------
// Wraps decode_varint_core with a lambda that reads one byte via stream.
template <typename Stream>
    requires requires(Stream& s) { s.read_bytes(static_cast<uint8_t*>(nullptr), size_t(0)); }
size_t decode_varint(Stream& stream, std::unsigned_integral auto& out_value) {
    return decode_varint_core([&]() -> uint8_t {
        uint8_t byte;
        stream.read_bytes(&byte, 1);
        return byte;
    }, out_value);
}

// ---------------------------------------------------------------------------
// zigzag_encode: map signed integers to unsigned (small magnitude -> small value)
// ---------------------------------------------------------------------------
// Formula (32-bit): (n << 1) ^ (n >> 31)
//   -1 -> 1, 1 -> 2, -2 -> 3, 2 -> 4, ...
//
// Generic shift:  constexpr auto shift = sizeof(decltype(value)) * 8 - 1;
// Return type:    -> std::make_unsigned_t<decltype(value)>
// Cast result:    static_cast<std::make_unsigned_t<decltype(value)>>(...)
//
// The right shift MUST be arithmetic (sign-extending). Using int32_t ensures this
// on all major compilers.
//
// TODO: Implement this function. Make it work for both 32-bit and 64-bit signed types.
auto zigzag_encode(std::signed_integral auto value) -> std::make_unsigned_t<decltype(value)> {
    // YOUR CODE HERE
    return 0; // placeholder
}

// ---------------------------------------------------------------------------
// zigzag_decode: map unsigned back to signed
// ---------------------------------------------------------------------------
// Formula (32-bit): (value >> 1) ^ -(value & 1)
//   1 -> -1, 2 -> 1, 3 -> -2, 4 -> 2, ...
//
// Return type: -> std::make_signed_t<decltype(value)>
//
// TODO: Implement this function.
auto zigzag_decode(std::unsigned_integral auto value) -> std::make_signed_t<decltype(value)> {
    // YOUR CODE HERE
    return 0; // placeholder
}

// ---------------------------------------------------------------------------
// Convenience: encode/decode signed varint (ZigZag + varint combined)
// Hint: chain traits for ref params:
//   using U = std::make_unsigned_t<std::remove_reference_t<decltype(out_value)>>;
// ---------------------------------------------------------------------------

// TODO: Implement using zigzag_encode + encode_varint
size_t encode_signed_varint(std::signed_integral auto value, uint8_t* buffer) {
    // YOUR CODE HERE
    return 0; // placeholder
}

// TODO: Implement using decode_varint + zigzag_decode
size_t decode_signed_varint(const uint8_t* buffer, std::signed_integral auto& out_value) {
    using U = std::make_unsigned_t<std::remove_reference_t<decltype(out_value)>>;
    U temp = 0;

    // YOUR CODE HERE

    return 0; // placeholder
}

#endif // VARINT_H
