/**
 * Tests for Section 3: Unified Serialization + PlayerState (15 points)
 *
 * DO NOT MODIFY THIS FILE — your code must pass these tests as-is.
 */
#include <doctest/doctest.h>
#include "../src/serialize.h"

#include <cstdint>
#include <string>

TEST_SUITE("PlayerState Serialization") {
    TEST_CASE("basic round-trip") {
        PlayerState original;
        original.x       = 500;
        original.y       = 300;
        original.z       = 100;
        original.health  = 85;
        original.heading = 270;
        original.team    = 2;
        original.alive   = true;
        original.name    = "Alice";

        BitWriter w;
        CHECK(serialize_player(w, original));

        BitReader r(w.data(), w.size());
        PlayerState decoded;
        CHECK(serialize_player(r, decoded));

        CHECK(decoded.x       == 500);
        CHECK(decoded.y       == 300);
        CHECK(decoded.z       == 100);
        CHECK(decoded.health  == 85);
        CHECK(decoded.heading == 270);
        CHECK(decoded.team    == 2);
        CHECK(decoded.alive   == true);
        CHECK(decoded.name    == "Alice");
    }

    TEST_CASE("zero values round-trip") {
        PlayerState original;
        // All fields default to 0/false/empty

        BitWriter w;
        CHECK(serialize_player(w, original));

        BitReader r(w.data(), w.size());
        PlayerState decoded;
        CHECK(serialize_player(r, decoded));

        CHECK(decoded.x       == 0);
        CHECK(decoded.y       == 0);
        CHECK(decoded.z       == 0);
        CHECK(decoded.health  == 0);
        CHECK(decoded.heading == 0);
        CHECK(decoded.team    == 0);
        CHECK(decoded.alive   == false);
        CHECK(decoded.name    == "");
    }

    TEST_CASE("max values round-trip") {
        PlayerState original;
        original.x       = 1023;
        original.y       = 1023;
        original.z       = 1023;
        original.health  = 100;
        original.heading = 359;
        original.team    = 3;
        original.alive   = true;
        original.name    = "MaxPlayer";

        BitWriter w;
        CHECK(serialize_player(w, original));

        BitReader r(w.data(), w.size());
        PlayerState decoded;
        CHECK(serialize_player(r, decoded));

        CHECK(decoded.x       == 1023);
        CHECK(decoded.y       == 1023);
        CHECK(decoded.z       == 1023);
        CHECK(decoded.health  == 100);
        CHECK(decoded.heading == 359);
        CHECK(decoded.team    == 3);
        CHECK(decoded.alive   == true);
        CHECK(decoded.name    == "MaxPlayer");
    }

    TEST_CASE("empty name round-trip") {
        PlayerState original;
        original.x       = 42;
        original.y       = 99;
        original.z       = 7;
        original.health  = 50;
        original.heading = 180;
        original.team    = 1;
        original.alive   = true;
        original.name    = "";

        BitWriter w;
        CHECK(serialize_player(w, original));

        BitReader r(w.data(), w.size());
        PlayerState decoded;
        CHECK(serialize_player(r, decoded));

        CHECK(decoded.x       == 42);
        CHECK(decoded.y       == 99);
        CHECK(decoded.z       == 7);
        CHECK(decoded.health  == 50);
        CHECK(decoded.heading == 180);
        CHECK(decoded.team    == 1);
        CHECK(decoded.alive   == true);
        CHECK(decoded.name    == "");
    }

    TEST_CASE("bitpacked size is compact") {
        // 49 bits = 7 bytes for bitpacked fields
        // + varint(0) + 0 string bytes = 1 byte for empty name
        // Total should be 8 bytes with empty name
        PlayerState original;
        original.x = 500; original.y = 300; original.z = 100;
        original.health = 85; original.heading = 270;
        original.team = 2; original.alive = true;
        original.name = "";

        BitWriter w;
        serialize_player(w, original);

        // 49 bits of bitpacked data = 7 bytes (after flush, padded)
        // + 1 byte varint length (0) for empty string
        // = 8 bytes total
        CHECK(w.size() == 8);
    }

    TEST_CASE("uses bits_required (not hardcoded widths)") {
        // This test verifies the bit widths are correct:
        // x,y,z: bits_required(0,1023) = 10
        // health: bits_required(0,100) = 7
        // heading: bits_required(0,359) = 9
        // team: bits_required(0,3) = 2
        // alive: 1
        // Total: 10+10+10+7+9+2+1 = 49 bits
        CHECK(bits_required(0u, 1023u) == 10);
        CHECK(bits_required(0u, 100u)  == 7);
        CHECK(bits_required(0u, 359u)  == 9);
        CHECK(bits_required(0u, 3u)    == 2);
    }
}
