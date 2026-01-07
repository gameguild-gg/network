#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

/**
 * @class NetworkUtils
 * @brief Utilities for IP addressing, subnetting, CIDR operations, and DNS hostname parsing.
 *
 * This class provides static methods for:
 * - IPv4 address validation and conversion
 * - Subnet calculations (network, broadcast, host range)
 * - CIDR notation operations
 * - IPv6 address validation and expansion
 * - FQDN parsing and validation
 *
 */
class NetworkUtils {
public:
    // ============ IPv4 Address Validation & Conversion ============

    /**
     * Validate IPv4 address format (dotted decimal notation).
     * Valid: "192.168.1.100", "127.0.0.1", "0.0.0.0"
     * Invalid: "256.1.1.1", "192.168.1", "abc.def.ghi.jkl"
     *
     * @param ip_str The IPv4 address as dotted decimal string
     * @return true if valid, false otherwise
     */
    static bool is_valid_ipv4(const std::string& ip_str);

    /**
     * Convert dotted decimal IPv4 string to 32-bit unsigned integer.
     * Example: "192.168.1.100" → 0xC0A80164 (3232235876)
     *
     * @param ip_str Dotted decimal IPv4 address
     * @return 32-bit integer representation, or std::nullopt if invalid
     */
    static std::optional<uint32_t> ipv4_to_uint32(const std::string& ip_str);

    /**
     * Convert 32-bit unsigned integer to dotted decimal IPv4 string.
     * Example: 3232235876 → "192.168.1.100"
     *
     * @param ip_int 32-bit integer representation
     * @return Dotted decimal string representation
     */
    static std::string uint32_to_ipv4(uint32_t ip_int);

    /**
     * Convert single octet (0-255) to 8-bit binary string.
     * Example: 192 → "11000000", 255 → "11111111"
     *
     * @param octet Value 0-255
     * @return 8-character binary string
     */
    static std::string octet_to_binary(uint8_t octet);

    // ============ Subnet Calculations ============

    /**
     * Convert CIDR prefix length to dotted decimal subnet mask.
     * Example: 24 → "255.255.255.0", 26 → "255.255.255.192"
     *
     * @param prefix CIDR prefix length (0-32)
     * @return Dotted decimal subnet mask string
     */
    static std::string cidr_to_subnet_mask(uint8_t prefix);

    /**
     * Convert dotted decimal subnet mask to CIDR prefix length.
     * Example: "255.255.255.0" → 24, "255.255.255.192" → 26
     *
     * @param mask Dotted decimal subnet mask
     * @return CIDR prefix (0-32), or std::nullopt if invalid mask
     */
    static std::optional<uint8_t> subnet_mask_to_cidr(const std::string& mask);

    /**
     * Calculate network address given IP and CIDR prefix.
     * Example: ("192.168.1.100", 24) → "192.168.1.0"
     *
     * @param ip_str IPv4 address string
     * @param prefix CIDR prefix length
     * @return Network address string, or empty string if invalid
     */
    static std::string get_network_address(const std::string& ip_str, uint8_t prefix);

    /**
     * Calculate broadcast address given IP and CIDR prefix.
     * Example: ("192.168.1.0", 24) → "192.168.1.255"
     *
     * @param ip_str IPv4 address string
     * @param prefix CIDR prefix length
     * @return Broadcast address string, or empty string if invalid
     */
    static std::string get_broadcast_address(const std::string& ip_str, uint8_t prefix);

    /**
     * Calculate first usable host address in a subnet.
     * Example: ("192.168.1.0", 24) → "192.168.1.1"
     * (Network address + 1)
     *
     * @param ip_str IPv4 address in subnet
     * @param prefix CIDR prefix length
     * @return First host address, or empty string if invalid
     */
    static std::string get_first_host(const std::string& ip_str, uint8_t prefix);

    /**
     * Calculate last usable host address in a subnet.
     * Example: ("192.168.1.0", 24) → "192.168.1.254"
     * (Broadcast address - 1)
     *
     * @param ip_str IPv4 address in subnet
     * @param prefix CIDR prefix length
     * @return Last host address, or empty string if invalid
     */
    static std::string get_last_host(const std::string& ip_str, uint8_t prefix);

    /**
     * Calculate number of usable host addresses in a subnet.
     * Formula: 2^(32 - prefix) - 2
     * Examples: /24 → 254 hosts, /25 → 126 hosts, /30 → 2 hosts
     *
     * @param prefix CIDR prefix length
     * @return Number of usable hosts, or 0 for invalid input
     */
    static uint32_t count_usable_hosts(uint8_t prefix);

    /**
     * Check if an IP address belongs to a given subnet.
     * Example: is_in_subnet("192.168.1.50", "192.168.1.0", 24) → true
     *
     * @param ip_str The IP address to check
     * @param network_str The network address
     * @param prefix CIDR prefix length
     * @return true if IP is in subnet (including network and broadcast), false otherwise
     */
    static bool is_in_subnet(const std::string& ip_str, const std::string& network_str, uint8_t prefix);

    /**
     * Complete subnet analysis: return network, first host, last host, broadcast.
     * Example: analyze_subnet("192.168.100.50", 26) →
     *   ("192.168.100.0", "192.168.100.1", "192.168.100.62", "192.168.100.63")
     *
     * @param ip_str IPv4 address in subnet
     * @param prefix CIDR prefix length
     * @return Tuple of (network, first_host, last_host, broadcast), or empty strings if invalid
     */
    struct SubnetInfo {
        std::string network;
        std::string first_host;
        std::string last_host;
        std::string broadcast;
    };

    static SubnetInfo analyze_subnet(const std::string& ip_str, uint8_t prefix);

    // ============ IPv6 Address Handling ============

    /**
     * Validate IPv6 address format.
     * Valid: "2001:db8:85a3::8a2e:370:7334", "::1", "::ffff:192.0.2.1" (IPv4-mapped)
     * Invalid: "gggg::1", "::1::2" (multiple ::)
     *
     * @param ip_str The IPv6 address string
     * @return true if valid, false otherwise
     */
    static bool is_valid_ipv6(const std::string& ip_str);

    /**
     * Expand compressed IPv6 address (remove ::, add leading zeros).
     * Example: "::1" → "0000:0000:0000:0000:0000:0000:0000:0001"
     * Example: "2001:db8::1" → "2001:0db8:0000:0000:0000:0000:0000:0001"
     *
     * @param ip_str The IPv6 address (may be compressed)
     * @return Expanded IPv6 address, or empty string if invalid
     */
    static std::string expand_ipv6(const std::string& ip_str);

    // ============ DNS & FQDN Handling ============

    /**
     * Validate hostname according to RFC 1123.
     * Rules:
     * - Each label: alphanumeric + hyphen (not start/end)
     * - Max 63 chars per label
     * - Max 253 chars total
     * - Not purely numeric (to distinguish from IPs)
     *
     * Valid: "game.example.com", "api-v2.servers.example.org"
     * Invalid: "256.1.1.1" (IP), "-game.com" (starts with hyphen), "a"*64".com" (label too long)
     *
     * @param hostname The hostname string
     * @return true if valid, false otherwise
     */
    static bool is_valid_hostname(const std::string& hostname);

    /**
     * Parse FQDN into components (labels).
     * Example: "game.example.com" → ["game", "example", "com"]
     * Example: "api.v2.servers.example.org" → ["api", "v2", "servers", "example", "org"]
     *
     * @param fqdn Fully qualified domain name
     * @return Vector of domain labels, or empty vector if invalid
     */
    static std::vector<std::string> parse_fqdn(const std::string& fqdn);

    /**
     * Get TLD (top-level domain) from FQDN.
     * Example: "game.example.com" → "com"
     * Example: "api.servers.example.org" → "org"
     *
     * @param fqdn Fully qualified domain name
     * @return TLD string, or empty string if invalid
     */
    static std::string get_tld(const std::string& fqdn);

    /**
     * Get domain name (second-level domain).
     * Example: "game.example.com" → "example"
     * Example: "api.servers.example.org" → "example"
     *
     * @param fqdn Fully qualified domain name
     * @return Domain string, or empty string if invalid
     */
    static std::string get_domain(const std::string& fqdn);

    /**
     * Check if IP is in private ranges (RFC 1918 + link-local + loopback).
     * Private ranges: 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
     * Link-local: 169.254.0.0/16
     * Loopback: 127.0.0.0/8
     *
     * @param ip_str IPv4 address string
     * @return true if private, false otherwise
     */
    static bool is_private_ip(const std::string& ip_str);

    /**
     * Check if IP is in reserved ranges (0.0.0.0/8, 255.255.255.255, broadcast, etc).
     *
     * @param ip_str IPv4 address string
     * @return true if reserved, false otherwise
     */
    static bool is_reserved_ip(const std::string& ip_str);
};
