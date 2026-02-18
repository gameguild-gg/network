/**
 * Tests for Section 4: String Serialization (10 points)
 *
 * DO NOT MODIFY THIS FILE — your code must pass these tests as-is.
 */
#include <doctest/doctest.h>
#include "../src/serialize.h"

#include <string>
#include <cstdint>

TEST_SUITE("String Serialization") {
    TEST_CASE("empty string round-trip") {
        std::string original;

        BitWriter w;
        CHECK(serialize_string(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        std::string decoded;
        CHECK(serialize_string(r, decoded));
        CHECK(decoded.empty());
        CHECK(decoded == original);
    }

    TEST_CASE("short string round-trip") {
        std::string original = "Hello";

        BitWriter w;
        CHECK(serialize_string(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        std::string decoded;
        CHECK(serialize_string(r, decoded));
        CHECK(decoded == "Hello");
    }

    TEST_CASE("string with special characters") {
        std::string original = "Hello, World! @#$%^&*()";

        BitWriter w;
        CHECK(serialize_string(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        std::string decoded;
        CHECK(serialize_string(r, decoded));
        CHECK(decoded == original);
    }

    TEST_CASE("max length string (255 bytes)") {
        std::string original(255, 'X');

        BitWriter w;
        CHECK(serialize_string(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        std::string decoded;
        CHECK(serialize_string(r, decoded));
        CHECK(decoded == original);
        CHECK(decoded.size() == 255);
    }

    TEST_CASE("string exceeding 255 bytes is rejected") {
        std::string too_long(256, 'Y');

        BitWriter w;
        CHECK_FALSE(serialize_string(w, too_long));
    }

    TEST_CASE("multiple strings round-trip") {
        std::string s1 = "Alice";
        std::string s2 = "Bob";
        std::string s3 = "";

        BitWriter w;
        CHECK(serialize_string(w, s1));
        CHECK(serialize_string(w, s2));
        CHECK(serialize_string(w, s3));
        w.flush();

        BitReader r(w.data(), w.size());
        std::string d1, d2, d3;
        CHECK(serialize_string(r, d1));
        CHECK(serialize_string(r, d2));
        CHECK(serialize_string(r, d3));
        CHECK(d1 == "Alice");
        CHECK(d2 == "Bob");
        CHECK(d3 == "");
    }

    TEST_CASE("string length is varint-encoded") {
        // A 5-char string: varint(5) = 1 byte, + 5 data bytes = 6 bytes total
        std::string original = "Hello";

        BitWriter w;
        CHECK(serialize_string(w, original));
        w.flush();

        // Should be: 1 byte varint length + 5 bytes data = 6 bytes
        CHECK(w.size() == 6);
    }
}
