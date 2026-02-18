/**
 * Tests for Section 5: Nested GameObject Serialization (10 points)
 *
 * DO NOT MODIFY THIS FILE — your code must pass these tests as-is.
 */
#include <doctest/doctest.h>
#include "../src/gameobject.h"

#include <cstdint>
#include <cmath>

TEST_SUITE("GameObject Serialization") {
    TEST_CASE("Position round-trip (raw bytes)") {
        Position original{1.5f, 2.5f, 3.5f};

        BitWriter w;
        CHECK(serialize_position(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        Position decoded;
        CHECK(serialize_position(r, decoded));

        // If using raw byte serialization, exact equality is expected
        CHECK(decoded.x == doctest::Approx(1.5f).epsilon(0.1));
        CHECK(decoded.y == doctest::Approx(2.5f).epsilon(0.1));
        CHECK(decoded.z == doctest::Approx(3.5f).epsilon(0.1));
    }

    TEST_CASE("Position with negative values") {
        Position original{-10.5f, 0.0f, 99.9f};

        BitWriter w;
        CHECK(serialize_position(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        Position decoded;
        CHECK(serialize_position(r, decoded));

        CHECK(decoded.x == doctest::Approx(-10.5f).epsilon(0.1));
        CHECK(decoded.y == doctest::Approx(0.0f).epsilon(0.1));
        CHECK(decoded.z == doctest::Approx(99.9f).epsilon(0.1));
    }

    TEST_CASE("Position with zero values") {
        Position original{0.0f, 0.0f, 0.0f};

        BitWriter w;
        CHECK(serialize_position(w, original));
        w.flush();

        BitReader r(w.data(), w.size());
        Position decoded;
        CHECK(serialize_position(r, decoded));

        CHECK(decoded.x == doctest::Approx(0.0f).epsilon(0.1));
        CHECK(decoded.y == doctest::Approx(0.0f).epsilon(0.1));
        CHECK(decoded.z == doctest::Approx(0.0f).epsilon(0.1));
    }

    TEST_CASE("GameObject round-trip") {
        GameObject original;
        original.id = 42;
        original.position = {1.5f, 2.5f, 3.5f};

        BitWriter w;
        CHECK(serialize_game_object(w, original));

        BitReader r(w.data(), w.size());
        GameObject decoded;
        CHECK(serialize_game_object(r, decoded));

        CHECK(decoded.id == 42);
        CHECK(decoded.position.x == doctest::Approx(1.5f).epsilon(0.1));
        CHECK(decoded.position.y == doctest::Approx(2.5f).epsilon(0.1));
        CHECK(decoded.position.z == doctest::Approx(3.5f).epsilon(0.1));
    }

    TEST_CASE("GameObject with large ID") {
        GameObject original;
        original.id = 0xDEADBEEF;
        original.position = {100.0f, 200.0f, 300.0f};

        BitWriter w;
        CHECK(serialize_game_object(w, original));

        BitReader r(w.data(), w.size());
        GameObject decoded;
        CHECK(serialize_game_object(r, decoded));

        CHECK(decoded.id == 0xDEADBEEF);
        CHECK(decoded.position.x == doctest::Approx(100.0f).epsilon(0.1));
        CHECK(decoded.position.y == doctest::Approx(200.0f).epsilon(0.1));
        CHECK(decoded.position.z == doctest::Approx(300.0f).epsilon(0.1));
    }

    TEST_CASE("GameObject with zero ID and position") {
        GameObject original;
        // defaults: id=0, position={0,0,0}

        BitWriter w;
        CHECK(serialize_game_object(w, original));

        BitReader r(w.data(), w.size());
        GameObject decoded;
        CHECK(serialize_game_object(r, decoded));

        CHECK(decoded.id == 0);
        CHECK(decoded.position.x == doctest::Approx(0.0f).epsilon(0.1));
        CHECK(decoded.position.y == doctest::Approx(0.0f).epsilon(0.1));
        CHECK(decoded.position.z == doctest::Approx(0.0f).epsilon(0.1));
    }
}
