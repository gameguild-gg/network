#include "NetworkUtils.h"
#include <iostream>
#include <iomanip>

/**
 * 02-addressing: Network Utilities Demo
 *
 * This program demonstrates practical usage of IPv4, IPv6, subnetting, and DNS utilities
 * in real-world game networking scenarios.
 *
 * Learning Outcomes:
 * - IPv4/IPv6 address parsing and validation
 * - Subnet calculations for network planning
 * - CIDR notation and binary bit operations
 * - Hostname validation and FQDN parsing
 * - IP classification (private, reserved, etc)
 */

void print_header(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << title << "\n";
    std::cout << std::string(70, '=') << "\n";
}

void print_subnet_info(const std::string& title, const std::string& ip, uint8_t prefix) {
    print_header(title);
    std::cout << "Input: " << ip << "/" << static_cast<int>(prefix) << "\n\n";

    auto info = NetworkUtils::analyze_subnet(ip, prefix);
    std::cout << "Network Address:  " << info.network << "\n";
    std::cout << "First Host:       " << info.first_host << "\n";
    std::cout << "Last Host:        " << info.last_host << "\n";
    std::cout << "Broadcast Address:" << info.broadcast << "\n";
    std::cout << "Usable Hosts:     " << NetworkUtils::count_usable_hosts(prefix) << "\n";
    std::cout << "Subnet Mask:      " << NetworkUtils::cidr_to_subnet_mask(prefix) << "\n";
}

void demo_ipv4_basics() {
    print_header("SCENARIO 1: Game Server Configuration");
    std::cout << "You're setting up a game server. Let's verify the server binding address.\n\n";

    std::string server_ip = "0.0.0.0";
    std::string localhost = "127.0.0.1";
    std::string internal_ip = "192.168.1.50";

    std::cout << "Server bind address (0.0.0.0): " << (NetworkUtils::is_valid_ipv4(server_ip) ? "✓ Valid" : "✗ Invalid") << "\n";
    std::cout << "Localhost (127.0.0.1):        " << (NetworkUtils::is_valid_ipv4(localhost) ? "✓ Valid" : "✗ Invalid") << "\n";
    std::cout << "Internal IP (192.168.1.50):   " << (NetworkUtils::is_valid_ipv4(internal_ip) ? "✓ Valid" : "✗ Invalid") << "\n";

    std::cout << "\nIP Classification:\n";
    std::cout << "Is 0.0.0.0 private?         " << (NetworkUtils::is_private_ip(server_ip) ? "Yes" : "No") << "\n";
    std::cout << "Is 127.0.0.1 private?       " << (NetworkUtils::is_private_ip(localhost) ? "Yes" : "No") << "\n";
    std::cout << "Is 192.168.1.50 private?    " << (NetworkUtils::is_private_ip(internal_ip) ? "Yes" : "No") << "\n";
}

void demo_subnet_planning() {
    print_subnet_info("SCENARIO 2: Game Studio LAN Planning (50 Computers)", "192.168.1.1", 25);

    std::cout << "\nGrowth Plan: Using /25 gives you 126 usable addresses.\n";
    std::cout << "Current need: 50 computers\n";
    std::cout << "Available buffer: " << (126 - 50) << " additional addresses for future growth\n";
}

void demo_subnet_analysis() {
    print_subnet_info("SCENARIO 3: Public Game Server Subnet (from Lecture)", "192.168.100.50", 26);

    std::cout << "\nThis matches the lecture example:\n";
    std::cout << "- /26 provides 62 usable hosts\n";
    std::cout << "- Suitable for a small regional game server cluster\n";
}

void demo_cidr_operations() {
    print_header("SCENARIO 4: CIDR Notation Conversion");

    std::cout << "Common CIDR subnets used in game networks:\n\n";

    std::vector<uint8_t> prefixes = {8, 16, 24, 25, 26, 28, 30, 32};
    std::cout << std::left << std::setw(8) << "CIDR"
              << std::setw(20) << "Subnet Mask"
              << std::setw(20) << "Usable Hosts\n";
    std::cout << std::string(48, '-') << "\n";

    for (uint8_t prefix : prefixes) {
        auto mask = NetworkUtils::cidr_to_subnet_mask(prefix);
        auto hosts = NetworkUtils::count_usable_hosts(prefix);
        std::cout << std::left << std::setw(8) << ("/" + std::to_string(prefix))
                  << std::setw(20) << mask
                  << std::setw(20) << hosts << "\n";
    }
}

void demo_ipv6() {
    print_header("SCENARIO 5: IPv6 Support Check");

    std::vector<std::string> ipv6_addresses = {
        "::1",
        "2001:db8:85a3::8a2e:370:7334",
        "fe80::1",
        "::ffff:192.0.2.1"
    };

    std::cout << "Checking IPv6 addresses for game server deployment:\n\n";

    for (const auto& addr : ipv6_addresses) {
        bool valid = NetworkUtils::is_valid_ipv6(addr);
        std::cout << std::left << std::setw(40) << addr
                  << (valid ? "✓ Valid" : "✗ Invalid") << "\n";
    }
}

void demo_dns() {
    print_header("SCENARIO 6: Game Server DNS Configuration");

    std::vector<std::string> hostnames = {
        "game.example.com",
        "api.v2.servers.example.org",
        "auth-service.example.net",
        "256.1.1.1",  // Looks like IP
        "-invalid.com"  // Invalid format
    };

    std::cout << "Validating game server hostnames:\n\n";

    for (const auto& hostname : hostnames) {
        bool valid = NetworkUtils::is_valid_hostname(hostname);
        std::cout << std::left << std::setw(40) << hostname
                  << (valid ? "✓ Valid" : "✗ Invalid");

        if (valid) {
            auto labels = NetworkUtils::parse_fqdn(hostname);
            auto tld = NetworkUtils::get_tld(hostname);
            auto domain = NetworkUtils::get_domain(hostname);
            std::cout << " | TLD: " << tld << " | Domain: " << domain;
        }
        std::cout << "\n";
    }
}

void demo_ip_classification() {
    print_header("SCENARIO 7: Network Architecture Decision");

    std::vector<std::string> test_ips = {
        "10.0.0.1",        // Private (corporate)
        "172.16.0.1",      // Private (corporate)
        "192.168.1.1",     // Private (home network)
        "127.0.0.1",       // Loopback
        "8.8.8.8",         // Public (Google DNS)
        "203.0.113.50",    // Public (game server)
        "255.255.255.255", // Broadcast
        "0.0.0.0"          // Any/Reserved
    };

    std::cout << "Classifying IPs for network deployment decision:\n\n";
    std::cout << std::left << std::setw(20) << "IP Address"
              << std::setw(15) << "Private?"
              << std::setw(15) << "Reserved?"
              << "Suitable for Public Game Server?\n";
    std::cout << std::string(65, '-') << "\n";

    for (const auto& ip : test_ips) {
        bool is_private = NetworkUtils::is_private_ip(ip);
        bool is_reserved = NetworkUtils::is_reserved_ip(ip);
        bool suitable = !is_private && !is_reserved && ip != "127.0.0.1";

        std::cout << std::left << std::setw(20) << ip
                  << std::setw(15) << (is_private ? "Yes" : "No")
                  << std::setw(15) << (is_reserved ? "Yes" : "No")
                  << (suitable ? "Yes ✓" : "No") << "\n";
    }
}

void demo_subnet_containment() {
    print_header("SCENARIO 8: Player IP Validation for Region Lock");

    std::string region_network = "192.168.1.0";
    uint8_t region_prefix = 24;

    std::vector<std::string> player_ips = {
        "192.168.1.50",
        "192.168.1.255",
        "192.168.2.1",
        "192.168.1.0",
        "192.168.1.1"
    };

    std::cout << "Checking if player IPs belong to region subnet: " << region_network << "/" << static_cast<int>(region_prefix) << "\n\n";

    for (const auto& player_ip : player_ips) {
        bool in_region = NetworkUtils::is_in_subnet(player_ip, region_network, region_prefix);
        std::cout << std::left << std::setw(20) << player_ip
                  << (in_region ? "✓ In region" : "✗ Not in region") << "\n";
    }
}

void demo_binary_representation() {
    print_header("SCENARIO 9: Understanding Binary IP Representation");

    std::cout << "Converting octets to binary (important for subnetting):\n\n";
    std::cout << std::left << std::setw(10) << "Decimal"
              << std::setw(15) << "Binary"
              << "Description\n";
    std::cout << std::string(60, '-') << "\n";

    std::vector<std::tuple<uint8_t, std::string>> values = {
        {0, "All zeros (network portion)"},
        {128, "Single bit set (subnet boundary)"},
        {192, "Two bits set (/26 subnet)"},
        {255, "All ones (broadcast/mask)"},
        {1, "Loopback"},
    };

    for (const auto& [val, desc] : values) {
        std::cout << std::left << std::setw(10) << static_cast<int>(val)
                  << std::setw(15) << NetworkUtils::octet_to_binary(val)
                  << desc << "\n";
    }
}

int main() {
    demo_ipv4_basics();
    demo_subnet_planning();
    demo_subnet_analysis();
    demo_cidr_operations();
    demo_ipv6();
    demo_dns();
    demo_ip_classification();
    demo_subnet_containment();
    demo_binary_representation();

    print_header("Demo Complete!");
    std::cout << "To run the comprehensive test suite:\n";
    std::cout << "  - CTest: 'ctest' or 'make test' (from build directory)\n";
    std::cout << "  - Direct: './02-addressing-tests' (runs doctest directly)\n\n";

    return 0;
}
