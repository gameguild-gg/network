/**
 * Bitpacking Stream — BitWriter / BitReader
 *
 * Assignment 06: Serialization Library
 *
 * Implements a bit-level read/write stream using a scratch-register technique
 * (inspired by Glenn Fiedler's packet serialization articles).
 *
 * The key idea:
 *   - A 64-bit "scratch" register accumulates bits.
 *   - When scratch has >= 8 bits, whole bytes are flushed to a byte buffer.
 *   - This avoids bit-shifting across byte boundaries in user code.
 *
 * TODO: Fill in the write_bits() and read_bits() methods marked with TODO.
 *
 * References:
 * - https://gafferongames.com/post/reading_and_writing_packets/
 * - https://gafferongames.com/post/serialization_strategies/
 */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <cassert>
#include <concepts>
#include <type_traits>

// =========================================================================
// BitWriter — packs bits into a byte buffer
// =========================================================================
class BitWriter {
public:
    /**
     * Write `bits` number of bits from `value` into the stream.
     *
     * @param value  The value to write (only the lowest `bits` bits are used)
     * @param bits   Number of bits to write (1–32)
     *
     * Algorithm (scratch-register technique):
     *   1. Mask `value` to keep only the lowest `bits` bits.
     *   2. Shift the masked value LEFT by `m_scratch_bits` and OR it into `m_scratch`.
     *   3. Add `bits` to `m_scratch_bits`.
     *   4. While `m_scratch_bits` >= 8:
     *        a. Push the lowest 8 bits of `m_scratch` to `m_buffer`.
     *        b. Right-shift `m_scratch` by 8.
     *        c. Subtract 8 from `m_scratch_bits`.
     *
     * TODO: Implement this method.
     */
    void write_bits(uint32_t value, int bits) {
        assert(bits >= 1 && bits <= 32);

        // YOUR CODE HERE
    }

    /**
     * Write a single boolean as 1 bit.
     */
    void write_bool(bool value) {
        write_bits(value ? 1 : 0, 1);
    }

    /**
     * Flush any remaining bits in the scratch register to the buffer.
     * The final byte is zero-padded on the high bits.
     * Must be called after all write_bits() calls before reading data()/size().
     */
    void flush() {
        while (m_scratch_bits > 0) {
            m_buffer.push_back(static_cast<uint8_t>(m_scratch & 0xFF));
            m_scratch >>= 8;
            m_scratch_bits = (m_scratch_bits > 8) ? (m_scratch_bits - 8) : 0;
        }
    }

    /**
     * Write raw bytes directly to the buffer (for string data, etc.).
     * Flushes any pending bits first so that byte-aligned data follows.
     */
    void write_bytes(const uint8_t* data, size_t len) {
        flush(); // ensure byte alignment
        m_buffer.insert(m_buffer.end(), data, data + len);
        m_total_bits += static_cast<int>(len * 8);
    }

    /// Pointer to the serialized byte buffer (valid after flush).
    const uint8_t* data() const { return m_buffer.data(); }

    /// Number of bytes written (valid after flush).
    size_t size() const { return m_buffer.size(); }

    /// Total number of bits written so far.
    size_t bits_written() const { return m_total_bits; }

private:
    std::vector<uint8_t> m_buffer;   ///< output byte buffer
    uint64_t m_scratch = 0;          ///< scratch register (accumulates bits)
    int m_scratch_bits = 0;          ///< how many bits are currently in scratch
    int m_total_bits = 0;            ///< running count of all bits written
};

// =========================================================================
// BitReader — reads bits from a byte buffer
// =========================================================================
class BitReader {
public:
    /**
     * Construct a reader over an existing byte buffer.
     * @param data  Pointer to the serialized buffer
     * @param size  Number of bytes in the buffer
     */
    BitReader(const uint8_t* data, size_t size)
        : m_data(data), m_size(size) {}

    /**
     * Read `bits` number of bits from the stream and return them as a uint32_t.
     *
     * @param bits  Number of bits to read (1–32)
     * @return      The value read (in the lowest `bits` bits)
     *
     * Algorithm (scratch-register technique):
     *   1. While `m_scratch_bits` < `bits`:
     *        a. Read the next byte from `m_data[m_byte_offset]`.
     *        b. OR it into `m_scratch`, shifted LEFT by `m_scratch_bits`.
     *        c. Increment `m_byte_offset` and add 8 to `m_scratch_bits`.
     *   2. Mask the lowest `bits` bits of `m_scratch` — that's the result.
     *   3. Right-shift `m_scratch` by `bits` and subtract from `m_scratch_bits`.
     *   4. Add `bits` to `m_total_bits_read`.
     *   5. Return the result.
     *
     * TODO: Implement this method.
     */
    uint32_t read_bits(int bits) {
        assert(bits >= 1 && bits <= 32);

        // YOUR CODE HERE
        return 0; // placeholder
    }

    /**
     * Read a single boolean (1 bit).
     */
    bool read_bool() {
        return read_bits(1) != 0;
    }

    /**
     * Read raw bytes from the buffer (for string data, etc.).
     * Discards any remaining bits in scratch to re-align to byte boundary.
     */
    void read_bytes(uint8_t* out, size_t len) {
        // Discard remaining scratch bits to align to byte boundary
        m_scratch = 0;
        m_scratch_bits = 0;
        for (size_t i = 0; i < len && m_byte_offset < m_size; ++i) {
            out[i] = m_data[m_byte_offset++];
        }
        m_total_bits_read += static_cast<int>(len * 8);
    }

    /// Total bits consumed so far.
    size_t bits_read() const { return m_total_bits_read; }

private:
    const uint8_t* m_data;       ///< source byte buffer
    size_t m_size;               ///< total bytes in buffer
    size_t m_byte_offset = 0;    ///< current byte read position
    uint64_t m_scratch = 0;      ///< scratch register
    int m_scratch_bits = 0;      ///< bits available in scratch
    int m_total_bits_read = 0;   ///< running count of all bits read
};

// =========================================================================
// C++20 Concepts for stream type constraints
// =========================================================================

template <typename T>
concept IsWriter = requires(T& s, uint32_t v, int bits, bool b) {
    { s.write_bits(v, bits) };
    { s.write_bool(b) };
    { s.flush() };
    { s.data() } -> std::convertible_to<const uint8_t*>;
    { s.size() } -> std::convertible_to<size_t>;
};

template <typename T>
concept IsReader = requires(T& s, int bits) {
    { s.read_bits(bits) } -> std::convertible_to<uint32_t>;
    { s.read_bool() } -> std::convertible_to<bool>;
};

template <typename T>
concept IsStream = IsWriter<T> || IsReader<T>;

#endif // BITSTREAM_H
