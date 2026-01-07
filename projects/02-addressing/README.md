# Assignment 02: Network Utilities Library

**Estimated Time:** 1–2 hours 
**Grading:** Automatic via doctest + manual code review  

## Overview

Build a **NetworkUtils C++ class** implementing IP addressing, subnet calculations, CIDR operations, IPv6 validation, and FQDN parsing. This assignment directly reinforces **Week 02 lecture topics** and provides a reusable networking utility library for future assignments.

## Project Structure

```
projects/02-addressing/
├── CMakeLists.txt          # Build configuration
├── src/
│   └── main.cpp            # Demo executable (9 scenarios)
├── tests/
│   └── tests.cpp           # Comprehensive doctest suite
└── README.md               # This file
../../lib/
    ├── NetworkUtils.h      # Header with function declarations
    └── NetworkUtils.cpp    # Your implementation goes here
```

- **Implementation location:** `lib/NetworkUtils.h` and `lib/NetworkUtils.cpp`  
- **Header already provided** — your task is to implement the functions.

## Tasks & Functions to Implement

### Task 1: IPv4 Address Validation & Conversion (30 min)

**Functions:**

- `bool is_valid_ipv4(const std::string& ip_str)`
    - Validate dotted decimal format (4 octets, each 0-255)
    - Return `false` for "256.1.1.1", "192.168.1", etc.
    - Use regex pattern: `^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$`
- `std::optional<uint32_t> ipv4_to_uint32(const std::string& ip_str)`
    - Convert "192.168.1.100" to `0xC0A80164`
    - Shift octets: `(octet1 << 24) | (octet2 << 16) | (octet3 << 8) | octet4`
    - Return `std::nullopt` if invalid
- `std::string uint32_to_ipv4(uint32_t ip_int)`
    - Reverse operation: `0xC0A80164` becomes "192.168.1.100"
    - Extract octets using shifts and masks: `(ip >> 24) & 0xFF`
- `std::string octet_to_binary(uint8_t octet)`
    - Convert 192 to "11000000", 255 to "11111111"
    - Loop through bits and build string: `for (int i = 7; i >= 0; --i)`

### Task 2: CIDR & Subnet Mask Conversion (20 min)

**Functions:**

- `std::string cidr_to_subnet_mask(uint8_t prefix)`
    - Convert CIDR prefix to dotted decimal: `/24` becomes "255.255.255.0"
    - Build 32-bit mask: prefix ones, then zeros
    - Formula: `mask = (prefix == 0) ? 0 : (0xFFFFFFFFU << (32 - prefix))`
    - Convert mask to dotted decimal
- `std::optional<uint8_t> subnet_mask_to_cidr(const std::string& mask)`
    - Reverse: "255.255.255.192" becomes `/26`
    - Count leading ones, verify contiguous (no gaps)
    - Return `std::nullopt` if invalid mask pattern

### Task 3: Subnet Calculations (45 min)

**Functions:**

- `std::string get_network_address(const std::string& ip_str, uint8_t prefix)`
    - Apply subnet mask with bitwise AND: `ip & mask`
    - From lecture: "192.168.1.100"/24 gives "192.168.1.0"
- `std::string get_broadcast_address(const std::string& ip_str, uint8_t prefix)`
    - Set all host bits to 1
    - Formula: `broadcast = network | (~mask)`
    - Example: "192.168.1.0"/24 gives "192.168.1.255"
- `std::string get_first_host(const std::string& ip_str, uint8_t prefix)`
    - Network address + 1
    - Special case: `/31` (point-to-point) has 2 usable hosts
- `std::string get_last_host(const std::string& ip_str, uint8_t prefix)`
    - Broadcast address - 1
    - Special case: `/31` returns broadcast address
- `uint32_t count_usable_hosts(uint8_t prefix)`
    - Formula: `2^(32 - prefix) - 2` (exclude network & broadcast)
    - Special cases: `/32` returns 0, `/31` returns 2 (RFC 3021)
    - From lecture: `/24` returns 254, `/26` returns 62
- `bool is_in_subnet(const std::string& ip_str, const std::string& network_str, uint8_t prefix)`
    - Check if IP belongs to subnet: `(ip & mask) == (network & mask)`
- `SubnetInfo analyze_subnet(const std::string& ip_str, uint8_t prefix)`
    - Return struct with: network, first_host, last_host, broadcast
    - Lecture example: "192.168.100.50"/26 returns ("192.168.100.0", "192.168.100.1", "192.168.100.62", "192.168.100.63")

### Task 4: IPv6 Address Handling (25 min) **EXTRA CREDIT**

**Functions:**

- `bool is_valid_ipv6(const std::string& ip_str)`
    - Accept formats: "::1", "2001:db8:85a3::8a2e:370:7334", "::ffff:192.0.2.1"
    - Reject: "::1::2" (multiple ::), "gggg::1" (invalid hex)
    - Check for valid hex characters and :: rules
- `std::string expand_ipv6(const std::string& ip_str)`
    - Expand "::1" to "0000:0000:0000:0000:0000:0000:0000:0001"
    - Expand "2001:db8::1" to "2001:0db8:0000:0000:0000:0000:0000:0001"
    - Pad each group to 4 characters with leading zeros

### Task 5: FQDN Parsing & DNS (30 min)

**Functions:**

- `bool is_valid_hostname(const std::string& hostname)`
    - RFC 1123 rules:
        - Each label: alphanumeric + hyphens (not start/end)
        - Max 63 chars per label, 253 chars total
        - Not purely numeric (distinguish from IPs)
    - Invalid: "256.1.1.1" (looks like IP), "-game.com" (starts with hyphen), "a"*64".com" (too long)
- `std::vector<std::string> parse_fqdn(const std::string& fqdn)`
    - Split by '.': "game.example.com" returns ["game", "example", "com"]
    - From lecture: "api.servers.example.org" returns ["api", "servers", "example", "org"]
    - Return empty vector if invalid
- `std::string get_tld(const std::string& fqdn)`
    - Return last label: "game.example.com" returns "com"
- `std::string get_domain(const std::string& fqdn)`
    - Return second-to-last label: "game.example.com" returns "example"

### Task 6: IP Classification (20 min)

**Functions:**

- `bool is_private_ip(const std::string& ip_str)`
    - RFC 1918 private ranges:
        - 10.0.0.0/8: `(ip & 0xFF000000) == 0x0A000000`
        - 172.16.0.0/12: `(ip & 0xFFF00000) == 0xAC100000`
        - 192.168.0.0/16: `(ip & 0xFFFF0000) == 0xC0A80000`
    - Also include:
        - 127.0.0.0/8 (loopback)
        - 169.254.0.0/16 (link-local)
- `bool is_reserved_ip(const std::string& ip_str)`
    - 0.0.0.0/8, 255.255.255.255, 240.0.0.0/4 (future use)

## Testing & Grading

### Automatic Tests

- **IPv4 Validation** (valid/invalid addresses)
- **Subnet Calculations** (lecture examples: 192.168.100.50/26, 10.0.0.130/25)
- **CIDR Operations** (all common prefixes: /8, /16, /24, /25, /26, /28, /30, /32)
- **FQDN Parsing** (lecture examples: game.example.com, api.servers.example.org)
- **IP Classification** (private, reserved, public ranges)
- **Quiz Question Scenarios** (Questions 7, 8, 9, 12, 25, 28)
- **Edge Cases** (boundary values, invalid inputs)

### Grading Rubric (10 points)

| Component | Points | Test Cases | Points per Test |
|-----------|--------|------------|----------------|
| IPv4 Validation & Conversion | 2.0 | 5 | 0.4 |
| CIDR & Subnet Masks | 0.8 | 2 | 0.4 |
| Subnet Calculations | 4.0 | 10 | 0.4 |
| FQDN & DNS | 1.6 | 4 | 0.4 |
| IP Classification | 1.6 | 4 | 0.4 |
| **Total** | **10** | **25** | **0.4** |
| IPv6 Handling (Extra Credit) | +1.2 | 3 | +0.4 |

## Demo Executable

The `main.cpp` includes **9 networking scenarios**:

1. **Server Configuration** — Validate bind addresses (0.0.0.0, 127.0.0.1, 192.168.x.x)
2. **LAN Planning** — Design subnet for 50-computer studio (growth planning)
3. **Public Subnet Analysis** — Lecture example breakdown (192.168.100.50/26)
4. **CIDR Operations** — Common network subnets reference table
5. **IPv6 Support** — Check readiness for IPv6 deployment
6. **DNS Configuration** — Validate server hostnames
7. **Network Classification** — Decide which IPs suitable for public servers
8. **Player IP Validation** — Check if player IP in region-locked subnet
9. **Binary Representation** — Understand octet-to-binary for subnetting math

## Implementation Tips

1. **Start with IPv4 (Tasks 1-3)** — These are the most used and map directly to quiz questions
2. **Use bitwise operations efficiently:**
   ```cpp
   // Create mask with 'prefix' ones: (0xFFFFFFFFU << (32 - prefix))
   // Apply mask: (ip & mask)
   // Set all host bits: (network | ~mask)
   ```
3. **String manipulation with `std::istringstream` and `std::sstream`:**
   ```cpp
   std::istringstream iss("192.168.1.100");
   int octet1, octet2, octet3, octet4;
   char dot;
   iss >> octet1 >> dot >> octet2 >> dot >> octet3 >> dot >> octet4;
   ```
4. **Use `std::optional<T>`** to signal failure gracefully
5. **Validate edge cases:** `/0`, `/32`, `/31` (point-to-point), invalid CIDR values
6. **Test against lecture examples first** before running full test suite

## References

- **Lecture:** Week 02 — IP Addressing and DNS
- **Textbook:** Beej's Guide Ch. 6-8 (IPv4/IPv6), Ch. 17 (Subnets)
- **RFCs:**
  - RFC 791: IPv4 (32-bit address space)
  - RFC 8200: IPv6 (128-bit address space)
  - RFC 1918: Private address ranges (10.x, 172.16-31.x, 192.168.x)
  - RFC 1123: Hostname requirements
  - RFC 3021: /31 subnet (point-to-point)

