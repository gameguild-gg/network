/**
 * Tests for Section 2: Bitpacking Stream (20 points)
 *
 * DO NOT MODIFY THIS FILE — your code must pass these tests as-is.
 */
#include <doctest/doctest.h>
#include "../src/bitstream.h"

#include <cstdint>
#include <cstring>

// ============================================================================
// BitWriter basics
// ============================================================================

TEST_SUITE("BitWriter") {
    TEST_CASE("write single bit") {
        BitWriter w;
        w.write_bits(1, 1);
        w.flush();
        CHECK(w.bits_written() == 1);
        CHECK(w.size() == 1);
        // Bit 0 set -> byte = 0x01
        CHECK(w.data()[0] == 0x01);
    }

    TEST_CASE("write 3-bit value") {
        BitWriter w;
        w.write_bits(5, 3);  // binary 101
        w.flush();
        CHECK(w.bits_written() == 3);
        CHECK(w.size() == 1);
        CHECK(w.data()[0] == 0x05);
    }

    TEST_CASE("write full byte") {
        BitWriter w;
        w.write_bits(0xAB, 8);
        w.flush();
        CHECK(w.size() == 1);
        CHECK(w.data()[0] == 0xAB);
    }

    TEST_CASE("write 32-bit value") {
        BitWriter w;
        w.write_bits(0xDEADBEEF, 32);
        w.flush();
        CHECK(w.size() == 4);
        // Little-endian byte order in scratch register
        CHECK(w.data()[0] == 0xEF);
        CHECK(w.data()[1] == 0xBE);
        CHECK(w.data()[2] == 0xAD);
        CHECK(w.data()[3] == 0xDE);
    }

    TEST_CASE("multiple small writes pack contiguously") {
        BitWriter w;
        w.write_bits(5, 3);   // 101
        w.write_bits(3, 2);   // 11
        w.write_bits(1, 3);   // 001
        // Combined: 001_11_101 = 0x3D (reading right-to-left in bits)
        w.flush();
        CHECK(w.size() == 1);
        CHECK(w.data()[0] == 0x3D);
        // Let's verify: 5=101 in bits 0-2, 3=11 in bits 3-4, 1=001 in bits 5-7
        // Byte: bit7..bit0 = 0 0 1 1 1 1 0 1 = 0b00111101 wait...
        // Actually: bits are packed LSB first:
        //   bits 0-2: 101 (value 5)
        //   bits 3-4: 11  (value 3)
        //   bits 5-7: 001 (value 1)
        //   byte = 0b001_11_101 = 0b00111101 = 0x3D... hmm
        // Let me recalculate: 00111101 = 0x3D
        // But maybe the scratch shifts differently. Let's just test round-trip.
    }

    TEST_CASE("write_bool works") {
        BitWriter w;
        w.write_bool(true);
        w.write_bool(false);
        w.write_bool(true);
        w.flush();
        CHECK(w.bits_written() == 3);
        // true=1, false=0, true=1 -> bits: 101 -> byte 0x05
        CHECK(w.data()[0] == 0x05);
    }
}

// ============================================================================
// BitReader basics
// ============================================================================

TEST_SUITE("BitReader") {
    TEST_CASE("read single bit") {
        uint8_t data[] = {0x01};
        BitReader r(data, 1);
        uint32_t val = r.read_bits(1);
        CHECK(val == 1);
        CHECK(r.bits_read() == 1);
    }

    TEST_CASE("read 3-bit value") {
        uint8_t data[] = {0x05};  // binary 00000101
        BitReader r(data, 1);
        uint32_t val = r.read_bits(3);
        CHECK(val == 5);
    }

    TEST_CASE("read full byte") {
        uint8_t data[] = {0xAB};
        BitReader r(data, 1);
        uint32_t val = r.read_bits(8);
        CHECK(val == 0xAB);
    }

    TEST_CASE("read_bool works") {
        uint8_t data[] = {0x05};  // bits: ...00000101
        BitReader r(data, 1);
        CHECK(r.read_bool() == true);   // bit 0 = 1
        CHECK(r.read_bool() == false);  // bit 1 = 0
        CHECK(r.read_bool() == true);   // bit 2 = 1
    }
}

// ============================================================================
// Round-trip: write then read
// ============================================================================

TEST_SUITE("BitStream Round-trip") {
    TEST_CASE("single value round-trip") {
        BitWriter w;
        w.write_bits(42, 6);
        w.flush();

        BitReader r(w.data(), w.size());
        CHECK(r.read_bits(6) == 42);
    }

    TEST_CASE("multiple fields round-trip") {
        BitWriter w;
        w.write_bits(500, 10);   // x: 0-1023
        w.write_bits(300, 10);   // y: 0-1023
        w.write_bits(85, 7);     // health: 0-100
        w.write_bits(270, 9);    // heading: 0-359
        w.write_bits(2, 2);      // team: 0-3
        w.write_bool(true);      // alive
        w.flush();

        BitReader r(w.data(), w.size());
        CHECK(r.read_bits(10) == 500);
        CHECK(r.read_bits(10) == 300);
        CHECK(r.read_bits(7) == 85);
        CHECK(r.read_bits(9) == 270);
        CHECK(r.read_bits(2) == 2);
        CHECK(r.read_bool() == true);
    }

    TEST_CASE("32-bit value round-trip") {
        BitWriter w;
        w.write_bits(0xDEADBEEF, 32);
        w.flush();

        BitReader r(w.data(), w.size());
        CHECK(r.read_bits(32) == 0xDEADBEEF);
    }

    TEST_CASE("many small values round-trip") {
        BitWriter w;
        for (int i = 0; i < 100; ++i) {
            w.write_bits(i % 8, 3);
        }
        w.flush();

        BitReader r(w.data(), w.size());
        for (int i = 0; i < 100; ++i) {
            CHECK(r.read_bits(3) == static_cast<uint32_t>(i % 8));
        }
    }

    TEST_CASE("bits_written tracks correctly") {
        BitWriter w;
        w.write_bits(0, 1);
        CHECK(w.bits_written() == 1);
        w.write_bits(0, 10);
        CHECK(w.bits_written() == 11);
        w.write_bits(0, 5);
        CHECK(w.bits_written() == 16);
    }
}

// ============================================================================
// Concepts
// ============================================================================

TEST_SUITE("Stream Concepts") {
    TEST_CASE("BitWriter satisfies IsWriter") {
        CHECK(IsWriter<BitWriter>);
        CHECK(IsStream<BitWriter>);
    }

    TEST_CASE("BitReader satisfies IsReader") {
        CHECK(IsReader<BitReader>);
        CHECK(IsStream<BitReader>);
    }

    TEST_CASE("BitWriter does NOT satisfy IsReader") {
        CHECK_FALSE(IsReader<BitWriter>);
    }

    TEST_CASE("BitReader does NOT satisfy IsWriter") {
        CHECK_FALSE(IsWriter<BitReader>);
    }
}
