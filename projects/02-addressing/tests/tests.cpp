#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "NetworkUtils.h"
#include <sstream>

// ============ IPv4 Validation Tests ============

TEST_SUITE("IPv4 Validation") {
    TEST_CASE("Valid IPv4 addresses") {
        CHECK(NetworkUtils::is_valid_ipv4("127.0.0.1") == true);
        CHECK(NetworkUtils::is_valid_ipv4("192.168.1.100") == true);
        CHECK(NetworkUtils::is_valid_ipv4("10.0.0.0") == true);
        CHECK(NetworkUtils::is_valid_ipv4("255.255.255.255") == true);
        CHECK(NetworkUtils::is_valid_ipv4("0.0.0.0") == true);
        CHECK(NetworkUtils::is_valid_ipv4("203.0.113.50") == true);
    }

    TEST_CASE("Invalid IPv4 addresses") {
        CHECK(NetworkUtils::is_valid_ipv4("256.1.1.1") == false);
        CHECK(NetworkUtils::is_valid_ipv4("192.168.1") == false);
        CHECK(NetworkUtils::is_valid_ipv4("abc.def.ghi.jkl") == false);
        CHECK(NetworkUtils::is_valid_ipv4("192.168.1.1.1") == false);
        CHECK(NetworkUtils::is_valid_ipv4("") == false);
        CHECK(NetworkUtils::is_valid_ipv4("192.168.-1.1") == false);
    }
}

// ============ IPv4 Conversion Tests ============

TEST_SUITE("IPv4 Conversion") {
    TEST_CASE("IPv4 to uint32") {
        auto result = NetworkUtils::ipv4_to_uint32("192.168.1.100");
        CHECK(result.has_value() == true);
        CHECK(result.value() == 0xC0A80164);

        auto localhost = NetworkUtils::ipv4_to_uint32("127.0.0.1");
        CHECK(localhost.has_value() == true);
        CHECK(localhost.value() == 0x7F000001);

        auto broadcast = NetworkUtils::ipv4_to_uint32("255.255.255.255");
        CHECK(broadcast.has_value() == true);
        CHECK(broadcast.value() == 0xFFFFFFFF);

        auto invalid = NetworkUtils::ipv4_to_uint32("256.1.1.1");
        CHECK(invalid.has_value() == false);
    }

    TEST_CASE("uint32 to IPv4") {
        CHECK(NetworkUtils::uint32_to_ipv4(0xC0A80164) == "192.168.1.100");
        CHECK(NetworkUtils::uint32_to_ipv4(0x7F000001) == "127.0.0.1");
        CHECK(NetworkUtils::uint32_to_ipv4(0xFFFFFFFF) == "255.255.255.255");
        CHECK(NetworkUtils::uint32_to_ipv4(0x00000000) == "0.0.0.0");
    }

    TEST_CASE("Octet to binary") {
        CHECK(NetworkUtils::octet_to_binary(192) == "11000000");
        CHECK(NetworkUtils::octet_to_binary(255) == "11111111");
        CHECK(NetworkUtils::octet_to_binary(1) == "00000001");
        CHECK(NetworkUtils::octet_to_binary(0) == "00000000");
        CHECK(NetworkUtils::octet_to_binary(128) == "10000000");
    }
}

// ============ CIDR & Subnet Mask Tests ============

TEST_SUITE("CIDR & Subnet Masks") {
    TEST_CASE("CIDR to subnet mask") {
        CHECK(NetworkUtils::cidr_to_subnet_mask(8) == "255.0.0.0");
        CHECK(NetworkUtils::cidr_to_subnet_mask(16) == "255.255.0.0");
        CHECK(NetworkUtils::cidr_to_subnet_mask(24) == "255.255.255.0");
        CHECK(NetworkUtils::cidr_to_subnet_mask(26) == "255.255.255.192");
        CHECK(NetworkUtils::cidr_to_subnet_mask(30) == "255.255.255.252");
        CHECK(NetworkUtils::cidr_to_subnet_mask(32) == "255.255.255.255");
        CHECK(NetworkUtils::cidr_to_subnet_mask(0) == "0.0.0.0");
    }

    TEST_CASE("Subnet mask to CIDR") {
        auto mask8 = NetworkUtils::subnet_mask_to_cidr("255.0.0.0");
        CHECK(mask8.has_value() == true);
        CHECK(mask8.value() == 8);

        auto mask24 = NetworkUtils::subnet_mask_to_cidr("255.255.255.0");
        CHECK(mask24.has_value() == true);
        CHECK(mask24.value() == 24);

        auto mask26 = NetworkUtils::subnet_mask_to_cidr("255.255.255.192");
        CHECK(mask26.has_value() == true);
        CHECK(mask26.value() == 26);

        auto invalid = NetworkUtils::subnet_mask_to_cidr("255.255.0.255");
        CHECK(invalid.has_value() == false);
    }
}

// ============ Subnet Calculation Tests (Lecture Examples) ============

TEST_SUITE("Subnet Calculations - Lecture Examples") {
    TEST_CASE("Practice Activity 2: CIDR to mask") {
        // From lecture: cidr_to_mask tests
        CHECK(NetworkUtils::cidr_to_subnet_mask(8) == "255.0.0.0");
        CHECK(NetworkUtils::cidr_to_subnet_mask(16) == "255.255.0.0");
        CHECK(NetworkUtils::cidr_to_subnet_mask(24) == "255.255.255.0");
        CHECK(NetworkUtils::cidr_to_subnet_mask(26) == "255.255.255.192");
    }

    TEST_CASE("Practice Activity 3: Network address calculation") {
        // From lecture: get_network_address tests
        CHECK(NetworkUtils::get_network_address("192.168.1.100", 24) == "192.168.1.0");
        CHECK(NetworkUtils::get_network_address("192.168.1.100", 26) == "192.168.1.64");
        CHECK(NetworkUtils::get_network_address("10.20.30.40", 16) == "10.20.0.0");
    }

    TEST_CASE("Practice Activity 4: Count usable hosts") {
        // From lecture: count_usable_hosts tests
        CHECK(NetworkUtils::count_usable_hosts(24) == 254);
        CHECK(NetworkUtils::count_usable_hosts(26) == 62);
        CHECK(NetworkUtils::count_usable_hosts(30) == 2);
        CHECK(NetworkUtils::count_usable_hosts(32) == 0);
    }

    TEST_CASE("Practice Activity 5: Complete subnet analysis") {
        // From lecture: analyze_subnet tests
        auto info1 = NetworkUtils::analyze_subnet("192.168.100.50", 26);
        CHECK(info1.network == "192.168.100.0");
        CHECK(info1.first_host == "192.168.100.1");
        CHECK(info1.last_host == "192.168.100.62");
        CHECK(info1.broadcast == "192.168.100.63");

        auto info2 = NetworkUtils::analyze_subnet("10.0.0.130", 25);
        CHECK(info2.network == "10.0.0.128");
        CHECK(info2.first_host == "10.0.0.129");
        CHECK(info2.last_host == "10.0.0.254");
        CHECK(info2.broadcast == "10.0.0.255");
    }
}

// ============ Detailed Subnet Tests ============

TEST_SUITE("Subnet Operations") {
    TEST_CASE("Network address calculation") {
        CHECK(NetworkUtils::get_network_address("192.168.100.50", 24) == "192.168.100.0");
        CHECK(NetworkUtils::get_network_address("10.0.0.1", 8) == "10.0.0.0");
        CHECK(NetworkUtils::get_network_address("172.16.0.50", 12) == "172.16.0.0");
    }

    TEST_CASE("Broadcast address calculation") {
        CHECK(NetworkUtils::get_broadcast_address("192.168.1.0", 24) == "192.168.1.255");
        CHECK(NetworkUtils::get_broadcast_address("192.168.100.0", 26) == "192.168.100.63");
        CHECK(NetworkUtils::get_broadcast_address("10.0.0.0", 8) == "10.255.255.255");
    }

    TEST_CASE("First and last host calculation") {
        CHECK(NetworkUtils::get_first_host("192.168.1.0", 24) == "192.168.1.1");
        CHECK(NetworkUtils::get_last_host("192.168.1.0", 24) == "192.168.1.254");

        CHECK(NetworkUtils::get_first_host("192.168.100.0", 26) == "192.168.100.1");
        CHECK(NetworkUtils::get_last_host("192.168.100.0", 26) == "192.168.100.62");
    }

    TEST_CASE("Usable hosts count") {
        CHECK(NetworkUtils::count_usable_hosts(8) == (16777216 - 2));  // /8: 16,777,214
        CHECK(NetworkUtils::count_usable_hosts(16) == (65536 - 2));    // /16: 65,534
        CHECK(NetworkUtils::count_usable_hosts(24) == 254);            // /24: 254
        CHECK(NetworkUtils::count_usable_hosts(25) == 126);            // /25: 126
        CHECK(NetworkUtils::count_usable_hosts(27) == 30);             // /27: 30
        CHECK(NetworkUtils::count_usable_hosts(28) == 14);             // /28: 14
        
    }

    TEST_CASE("Special cases (/31 and /32)") {
        CHECK(NetworkUtils::count_usable_hosts(31) == 2);              // /31: 2 RFC 3021. /31 subnets have 2 usable addresses.
        CHECK(NetworkUtils::count_usable_hosts(32) == 0);
    }

    TEST_CASE("IP in subnet check") {
        CHECK(NetworkUtils::is_in_subnet("192.168.1.50", "192.168.1.0", 24) == true);
        CHECK(NetworkUtils::is_in_subnet("192.168.2.50", "192.168.1.0", 24) == false);
        CHECK(NetworkUtils::is_in_subnet("192.168.100.50", "192.168.100.0", 26) == true);
        CHECK(NetworkUtils::is_in_subnet("192.168.100.100", "192.168.100.0", 26) == false);
    }
}

// ============ IPv6 Tests ============

TEST_SUITE("IPv6 Handling") {
    TEST_CASE("IPv6 validation") {
        CHECK(NetworkUtils::is_valid_ipv6("::1") == true);
        CHECK(NetworkUtils::is_valid_ipv6("2001:db8:85a3::8a2e:370:7334") == true);
        CHECK(NetworkUtils::is_valid_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334") == true);
        CHECK(NetworkUtils::is_valid_ipv6("::ffff:192.0.2.1") == true);
    }

    TEST_CASE("IPv6 validation - invalid") {
        CHECK(NetworkUtils::is_valid_ipv6("gggg::1") == false);
        CHECK(NetworkUtils::is_valid_ipv6("::1::2") == false);
        CHECK(NetworkUtils::is_valid_ipv6("") == false);
    }

    TEST_CASE("IPv6 expansion") {
        auto expanded = NetworkUtils::expand_ipv6("::1");
        CHECK(expanded.empty() == false);
        // Should expand to full form with zeros

        auto expanded2 = NetworkUtils::expand_ipv6("2001:db8::1");
        CHECK(expanded2.empty() == false);
    }
}

// ============ FQDN & DNS Tests ============

TEST_SUITE("FQDN & DNS") {
    TEST_CASE("Hostname validation") {
        CHECK(NetworkUtils::is_valid_hostname("game.example.com") == true);
        CHECK(NetworkUtils::is_valid_hostname("api-v2.servers.example.org") == true);
        CHECK(NetworkUtils::is_valid_hostname("localhost") == true);
        CHECK(NetworkUtils::is_valid_hostname("a") == true);
    }

    TEST_CASE("Hostname validation - invalid") {
        CHECK(NetworkUtils::is_valid_hostname("256.1.1.1") == false);      // Looks like IP
        CHECK(NetworkUtils::is_valid_hostname("-game.com") == false);      // Starts with hyphen
        CHECK(NetworkUtils::is_valid_hostname("game-.com") == false);      // Label ends with hyphen
        CHECK(NetworkUtils::is_valid_hostname("") == false);              // Empty
        CHECK(NetworkUtils::is_valid_hostname(std::string(254, 'a')) == false);  // Too long
    }

    TEST_CASE("Practice Activity 6: FQDN parsing") {
        // From lecture: parse_fqdn tests
        auto labels1 = NetworkUtils::parse_fqdn("game.example.com");
        CHECK(labels1.size() == 3);
        CHECK(labels1[0] == "game");
        CHECK(labels1[1] == "example");
        CHECK(labels1[2] == "com");

        auto labels2 = NetworkUtils::parse_fqdn("auth.api.example.org");
        CHECK(labels2.size() == 4);
        CHECK(labels2[0] == "auth");
        CHECK(labels2[1] == "api");
        CHECK(labels2[2] == "example");
        CHECK(labels2[3] == "org");
    }

    TEST_CASE("Get TLD") {
        CHECK(NetworkUtils::get_tld("game.example.com") == "com");
        CHECK(NetworkUtils::get_tld("api.servers.example.org") == "org");
        CHECK(NetworkUtils::get_tld("localhost") == "localhost");
    }

    TEST_CASE("Get domain") {
        CHECK(NetworkUtils::get_domain("game.example.com") == "example");
        CHECK(NetworkUtils::get_domain("api.servers.example.org") == "example");
    }
}

// ============ IP Classification Tests ============

TEST_SUITE("IP Classification") {
    TEST_CASE("Private IP detection (RFC 1918)") {
        // 10.0.0.0/8
        CHECK(NetworkUtils::is_private_ip("10.0.0.0") == true);
        CHECK(NetworkUtils::is_private_ip("10.255.255.255") == true);
        CHECK(NetworkUtils::is_private_ip("10.20.30.40") == true);

        // 172.16.0.0/12
        CHECK(NetworkUtils::is_private_ip("172.16.0.0") == true);
        CHECK(NetworkUtils::is_private_ip("172.31.255.255") == true);
        CHECK(NetworkUtils::is_private_ip("172.20.0.1") == true);

        // 192.168.0.0/16
        CHECK(NetworkUtils::is_private_ip("192.168.0.0") == true);
        CHECK(NetworkUtils::is_private_ip("192.168.255.255") == true);
        CHECK(NetworkUtils::is_private_ip("192.168.1.100") == true);

        // Loopback 127.0.0.0/8
        CHECK(NetworkUtils::is_private_ip("127.0.0.1") == true);
        CHECK(NetworkUtils::is_private_ip("127.255.255.255") == true);

        // Link-local 169.254.0.0/16
        CHECK(NetworkUtils::is_private_ip("169.254.1.1") == true);
    }

    TEST_CASE("Private IP detection - public") {
        CHECK(NetworkUtils::is_private_ip("8.8.8.8") == false);
        CHECK(NetworkUtils::is_private_ip("1.1.1.1") == false);
        CHECK(NetworkUtils::is_private_ip("203.0.113.50") == false);
    }

    TEST_CASE("Reserved IP detection") {
        CHECK(NetworkUtils::is_reserved_ip("0.0.0.0") == true);
        CHECK(NetworkUtils::is_reserved_ip("255.255.255.255") == true);
        CHECK(NetworkUtils::is_reserved_ip("240.0.0.1") == true);  // 240.0.0.0/4
    }

    TEST_CASE("Reserved IP detection - not reserved") {
        CHECK(NetworkUtils::is_reserved_ip("192.168.1.1") == false);
        CHECK(NetworkUtils::is_reserved_ip("8.8.8.8") == false);
    }
}

// ============ Quiz Question Scenarios ============

TEST_SUITE("Quiz Question Scenarios") {
    TEST_CASE("Question 7: Network address with /24") {
        // Given 192.168.100.50 with mask 255.255.255.0, find network
        auto network = NetworkUtils::get_network_address("192.168.100.50", 24);
        CHECK(network == "192.168.100.0");
    }

    TEST_CASE("Question 8: Broadcast address for 192.168.1.0/24") {
        auto broadcast = NetworkUtils::get_broadcast_address("192.168.1.0", 24);
        CHECK(broadcast == "192.168.1.255");
    }

    TEST_CASE("Question 9: Usable hosts in /25") {
        auto count = NetworkUtils::count_usable_hosts(25);
        CHECK(count == 126);
    }

    TEST_CASE("Question 12: /26 to subnet mask") {
        auto mask = NetworkUtils::cidr_to_subnet_mask(26);
        CHECK(mask == "255.255.255.192");
    }

    TEST_CASE("Question 25: Game server bind address") {
        // Server binds to 0.0.0.0 to accept on any interface
        CHECK(NetworkUtils::is_valid_ipv4("0.0.0.0") == true);
    }

    TEST_CASE("Question 28: Subnet planning for 50 computers") {
        // 50 computers need at least /25 (126 hosts) or /26 (62 hosts)
        auto hosts_25 = NetworkUtils::count_usable_hosts(25);
        auto hosts_26 = NetworkUtils::count_usable_hosts(26);
        CHECK(hosts_25 == 126);  // Sufficient for 50
        CHECK(hosts_26 == 62);   // Sufficient for 50
    }
}

// ============ Edge Cases & Error Handling ============

TEST_SUITE("Edge Cases") {
    TEST_CASE("Invalid input handling") {
        CHECK(NetworkUtils::get_network_address("invalid", 24) == "");
        CHECK(NetworkUtils::get_broadcast_address("invalid", 24) == "");
        CHECK(NetworkUtils::parse_fqdn("256.1.1.1").empty() == true);
    }

    TEST_CASE("Boundary CIDR values") {
        auto mask0 = NetworkUtils::cidr_to_subnet_mask(0);
        CHECK(mask0 == "0.0.0.0");

        auto mask32 = NetworkUtils::cidr_to_subnet_mask(32);
        CHECK(mask32 == "255.255.255.255");
    }

    TEST_CASE("Special addresses") {
        CHECK(NetworkUtils::is_valid_ipv4("127.0.0.1") == true);      // Loopback
        CHECK(NetworkUtils::is_valid_ipv4("0.0.0.0") == true);        // Any
        CHECK(NetworkUtils::is_valid_ipv4("255.255.255.255") == true); // Broadcast
    }
}
