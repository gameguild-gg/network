/**
 * Tests for Section 1: Varint + ZigZag Encoding (15 points)
 *
 * DO NOT MODIFY THIS FILE — your code must pass these tests as-is.
 */
#include <doctest/doctest.h>
#include "../src/varint.h"

#include <cstdint>
#include <array>

// ============================================================================
// bits_required
// ============================================================================

TEST_SUITE("bits_required") {
    TEST_CASE("single value range returns 1") {
        // min == max edge case: still need 1 bit to represent 1 value
        CHECK(bits_required(0u, 0u) == 1);
        CHECK(bits_required(5u, 5u) == 1);
    }

    TEST_CASE("power-of-two ranges") {
        CHECK(bits_required(0u, 1u) == 1);      // 2 values -> 1 bit
        CHECK(bits_required(0u, 3u) == 2);      // 4 values -> 2 bits
        CHECK(bits_required(0u, 7u) == 3);      // 8 values -> 3 bits
        CHECK(bits_required(0u, 15u) == 4);     // 16 values -> 4 bits
        CHECK(bits_required(0u, 255u) == 8);    // 256 values -> 8 bits
    }

    TEST_CASE("non-power-of-two ranges") {
        CHECK(bits_required(0u, 100u) == 7);    // 101 values -> 7 bits
        CHECK(bits_required(0u, 359u) == 9);    // 360 values -> 9 bits
        CHECK(bits_required(0u, 1023u) == 10);  // 1024 values -> 10 bits
    }

    TEST_CASE("offset ranges") {
        // bits_required(10, 13) => 4 values => 2 bits
        CHECK(bits_required(10u, 13u) == 2);
        // bits_required(100, 200) => 101 values => 7 bits
        CHECK(bits_required(100u, 200u) == 7);
    }
}

// ============================================================================
// Varint encoding/decoding (unsigned)
// ============================================================================

TEST_SUITE("Varint Encoding") {
    TEST_CASE("zero encodes to 1 byte") {
        uint8_t buf[10] = {};
        size_t n = encode_varint(uint32_t(0), buf);
        CHECK(n == 1);
        CHECK(buf[0] == 0x00);
    }

    TEST_CASE("values 1-127 encode to 1 byte") {
        uint8_t buf[10] = {};
        size_t n = encode_varint(uint32_t(1), buf);
        CHECK(n == 1);
        CHECK(buf[0] == 0x01);

        n = encode_varint(uint32_t(127), buf);
        CHECK(n == 1);
        CHECK(buf[0] == 0x7F);
    }

    TEST_CASE("128 encodes to 2 bytes") {
        uint8_t buf[10] = {};
        size_t n = encode_varint(uint32_t(128), buf);
        CHECK(n == 2);
        CHECK(buf[0] == 0x80);
        CHECK(buf[1] == 0x01);
    }

    TEST_CASE("300 encodes to 2 bytes") {
        uint8_t buf[10] = {};
        size_t n = encode_varint(uint32_t(300), buf);
        CHECK(n == 2);
        CHECK(buf[0] == 0xAC);
        CHECK(buf[1] == 0x02);
    }

    TEST_CASE("16384 encodes to 3 bytes") {
        uint8_t buf[10] = {};
        size_t n = encode_varint(uint32_t(16384), buf);
        CHECK(n == 3);
        CHECK(buf[0] == 0x80);
        CHECK(buf[1] == 0x80);
        CHECK(buf[2] == 0x01);
    }

    TEST_CASE("max uint32 encodes to 5 bytes") {
        uint8_t buf[10] = {};
        size_t n = encode_varint(uint32_t(0xFFFFFFFF), buf);
        CHECK(n == 5);
    }

    TEST_CASE("round-trip for various values") {
        uint32_t test_values[] = {0, 1, 127, 128, 255, 256, 300, 16383, 16384,
                                   65535, 1000000, 0x7FFFFFFF, 0xFFFFFFFF};
        for (auto original : test_values) {
            uint8_t buf[10] = {};
            size_t written = encode_varint(original, buf);
            CHECK(written > 0);
            CHECK(written <= 5);

            uint32_t decoded = 0;
            size_t consumed = decode_varint(buf, decoded);
            CHECK(consumed == written);
            CHECK(decoded == original);
        }
    }
}

// ============================================================================
// ZigZag encoding/decoding
// ============================================================================

TEST_SUITE("ZigZag Encoding") {
    TEST_CASE("known ZigZag mappings (32-bit)") {
        // From the protobuf spec:
        //  0 -> 0, -1 -> 1, 1 -> 2, -2 -> 3, 2 -> 4, ...
        CHECK(zigzag_encode(int32_t(0))  == 0u);
        CHECK(zigzag_encode(int32_t(-1)) == 1u);
        CHECK(zigzag_encode(int32_t(1))  == 2u);
        CHECK(zigzag_encode(int32_t(-2)) == 3u);
        CHECK(zigzag_encode(int32_t(2))  == 4u);
    }

    TEST_CASE("ZigZag round-trip (32-bit)") {
        int32_t values[] = {0, 1, -1, 2, -2, 63, -64, 127, -128, 1000, -1000,
                            INT32_MAX, INT32_MIN};
        for (auto v : values) {
            auto encoded = zigzag_encode(v);
            auto decoded = zigzag_decode(encoded);
            CHECK(decoded == v);
        }
    }

    TEST_CASE("small negatives produce small unsigned values") {
        // -1 -> 1 (small), not 0xFFFFFFFF (huge)
        CHECK(zigzag_encode(int32_t(-1)) < 10u);
        CHECK(zigzag_encode(int32_t(-2)) < 10u);
        CHECK(zigzag_encode(int32_t(-3)) < 10u);
    }
}

// ============================================================================
// Signed varint (ZigZag + varint combined)
// ============================================================================

TEST_SUITE("Signed Varint") {
    TEST_CASE("-1 encodes to 1 byte (not 10)") {
        uint8_t buf[10] = {};
        size_t n = encode_signed_varint(int32_t(-1), buf);
        CHECK(n == 1);
        // ZigZag(-1) = 1, varint(1) = 0x01
        CHECK(buf[0] == 0x01);
    }

    TEST_CASE("small negatives encode efficiently") {
        uint8_t buf[10] = {};
        size_t n;

        n = encode_signed_varint(int32_t(-1), buf);
        CHECK(n <= 1);

        n = encode_signed_varint(int32_t(-64), buf);
        CHECK(n <= 2);
    }

    TEST_CASE("signed varint round-trip") {
        int32_t values[] = {0, 1, -1, 2, -2, 100, -100, 1000, -1000,
                            INT32_MAX, INT32_MIN};
        for (auto original : values) {
            uint8_t buf[10] = {};
            size_t written = encode_signed_varint(original, buf);
            CHECK(written > 0);

            int32_t decoded = 0;
            size_t consumed = decode_signed_varint(buf, decoded);
            CHECK(consumed == written);
            CHECK(decoded == original);
        }
    }
}
