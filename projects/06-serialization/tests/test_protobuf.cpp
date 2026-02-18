/**
 * Tests for Section 6: Protobuf Wire Format Encoding (10 points)
 *
 * DO NOT MODIFY THIS FILE — your code must pass these tests as-is.
 */
#include <doctest/doctest.h>
#include "../src/protobuf.h"

#include <cstdint>
#include <cstring>

TEST_SUITE("Protobuf Tag Encoding") {
    TEST_CASE("make_tag computes (field << 3) | wire_type") {
        CHECK(make_tag(1, WireType::VARINT) == 0x08); // (1<<3)|0 = 8
        CHECK(make_tag(1, WireType::I32)    == 0x0D); // (1<<3)|5 = 13
        CHECK(make_tag(1, WireType::LEN)    == 0x0A); // (1<<3)|2 = 10
        CHECK(make_tag(2, WireType::VARINT) == 0x10); // (2<<3)|0 = 16
        CHECK(make_tag(2, WireType::LEN)    == 0x12); // (2<<3)|2 = 18
        CHECK(make_tag(3, WireType::I32)    == 0x1D); // (3<<3)|5 = 29
    }

    TEST_CASE("parse_tag extracts field number and wire type") {
        uint32_t field;
        WireType wt;

        parse_tag(0x08, field, wt);
        CHECK(field == 1);
        CHECK(wt == WireType::VARINT);

        parse_tag(0x12, field, wt);
        CHECK(field == 2);
        CHECK(wt == WireType::LEN);

        parse_tag(0x1D, field, wt);
        CHECK(field == 3);
        CHECK(wt == WireType::I32);
    }

    TEST_CASE("make_tag / parse_tag round-trip") {
        for (uint32_t fn = 1; fn <= 10; ++fn) {
            for (auto wt : {WireType::VARINT, WireType::I64, WireType::LEN, WireType::I32}) {
                uint32_t tag = make_tag(fn, wt);
                uint32_t parsed_fn;
                WireType parsed_wt;
                parse_tag(tag, parsed_fn, parsed_wt);
                CHECK(parsed_fn == fn);
                CHECK(parsed_wt == wt);
            }
        }
    }
}

TEST_SUITE("Protobuf GameObject Encoding") {
    TEST_CASE("basic round-trip") {
        GameObject original;
        original.id = 42;
        original.position = {1.5f, 2.5f, 3.5f};

        uint8_t buf[128];
        size_t written = encode_proto_game_object(original, buf);
        CHECK(written > 0);

        GameObject decoded;
        size_t consumed = decode_proto_game_object(buf, decoded);
        CHECK(consumed == written);

        CHECK(decoded.id == 42);
        CHECK(decoded.position.x == 1.5f);
        CHECK(decoded.position.y == 2.5f);
        CHECK(decoded.position.z == 3.5f);
    }

    TEST_CASE("zero id and zero position") {
        GameObject original;
        original.id = 0;
        original.position = {0.0f, 0.0f, 0.0f};

        uint8_t buf[128];
        size_t written = encode_proto_game_object(original, buf);
        CHECK(written > 0);

        GameObject decoded;
        decode_proto_game_object(buf, decoded);

        CHECK(decoded.id == 0);
        CHECK(decoded.position.x == 0.0f);
        CHECK(decoded.position.y == 0.0f);
        CHECK(decoded.position.z == 0.0f);
    }

    TEST_CASE("large id") {
        GameObject original;
        original.id = 100000;
        original.position = {-1.0f, 0.0f, 1.0f};

        uint8_t buf[128];
        size_t written = encode_proto_game_object(original, buf);

        GameObject decoded;
        size_t consumed = decode_proto_game_object(buf, decoded);
        CHECK(consumed == written);

        CHECK(decoded.id == 100000);
        CHECK(decoded.position.x == -1.0f);
        CHECK(decoded.position.y == 0.0f);
        CHECK(decoded.position.z == 1.0f);
    }

    TEST_CASE("negative floats preserved exactly") {
        // No quantization — raw float bytes, so exact equality is expected
        GameObject original;
        original.id = 1;
        original.position = {-123.456f, 789.012f, -0.001f};

        uint8_t buf[128];
        size_t written = encode_proto_game_object(original, buf);

        GameObject decoded;
        decode_proto_game_object(buf, decoded);

        CHECK(decoded.position.x == original.position.x);
        CHECK(decoded.position.y == original.position.y);
        CHECK(decoded.position.z == original.position.z);
    }

    TEST_CASE("first byte is tag for field 1 VARINT") {
        // The first byte on the wire should be the tag for field 1, VARINT
        // tag = (1 << 3) | 0 = 0x08
        GameObject original;
        original.id = 42;
        original.position = {0, 0, 0};

        uint8_t buf[128];
        encode_proto_game_object(original, buf);

        CHECK(buf[0] == 0x08); // tag: field 1, VARINT
    }

    TEST_CASE("nested Position uses LEN wire type") {
        // After the id field, the next tag should be field 2, LEN
        // tag = (2 << 3) | 2 = 0x12
        GameObject original;
        original.id = 1;  // varint(1) = 1 byte
        original.position = {0, 0, 0};

        uint8_t buf[128];
        encode_proto_game_object(original, buf);

        // buf[0] = 0x08 (tag field 1)
        // buf[1] = 0x01 (varint 1)
        // buf[2] = 0x12 (tag field 2, LEN)
        CHECK(buf[2] == 0x12);
    }
}
