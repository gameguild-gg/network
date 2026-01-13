#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

// Simple header-only PCAP/PCAPNG reader with zero dependencies
// Supports both classic PCAP and modern PCAPNG file formats

namespace pcap {

// Protocol type identifiers (IANA assigned numbers)
// See: https://www.iana.org/assignments/protocol-numbers/
//      https://www.iana.org/assignments/ieee-802-numbers/
enum ProtocolType : uint16_t {
    Unknown = 0,
    Ethernet = 1,         // Layer 2: IEEE 802.3 Ethernet
    ARP = 0x0806,         // Layer 3: Address Resolution Protocol (RFC 826)
    IPv4 = 0x0800,        // Layer 3: Internet Protocol version 4 (RFC 791)
    IPv6 = 0x86DD,        // Layer 3: Internet Protocol version 6 (RFC 2460)
    TCP = 6 | 0x1000,     // Layer 4: Transmission Control Protocol (RFC 793) - offset to avoid collision with ICMP
    UDP = 17 | 0x1000,    // Layer 4: User Datagram Protocol (RFC 768)
    ICMP = 1 | 0x1000,    // Layer 4: Internet Control Message Protocol (RFC 792)
    ICMPv6 = 58 | 0x1000  // Layer 4: ICMP for IPv6 (RFC 4443)
};

// Raw packet data container
class RawPacket {
public:
    std::vector<uint8_t> data;
    uint32_t timestamp_sec = 0;
    uint32_t timestamp_usec = 0;
    
    const uint8_t* getData() const { return data.data(); }
    size_t getDataLen() const { return data.size(); }
};

// Parsed packet with protocol detection
class Packet {
private:
    const RawPacket* rawPacket_;
    bool hasEthernet_ = false;
    bool hasIPv4_ = false;
    bool hasIPv6_ = false;
    bool hasARP_ = false;
    bool hasTCP_ = false;
    bool hasUDP_ = false;
    bool hasICMP_ = false;
    
    void parsePacket() {
        // Minimum Ethernet frame size: 14 bytes (6 dst MAC + 6 src MAC + 2 EtherType)
        if (!rawPacket_ || rawPacket_->getDataLen() < 14) return;
        
        const uint8_t* data = rawPacket_->getData();
        size_t len = rawPacket_->getDataLen();
        size_t offset = 0;
        
        // Layer 2: Ethernet Frame (IEEE 802.3)
        // Structure: [Dst MAC: 6][Src MAC: 6][EtherType: 2][Payload][FCS: 4]
        if (len >= 14) {
            hasEthernet_ = true;
            // EtherType at bytes 12-13 (big-endian)
            uint16_t etherType = (data[12] << 8) | data[13];
            offset = 14;
            
            // Handle 802.1Q VLAN tags (EtherType 0x8100)
            // VLAN structure: [TPID: 0x8100][TCI: 2][EtherType: 2]
            while (etherType == 0x8100 && offset + 4 <= len) {
                etherType = (data[offset + 2] << 8) | data[offset + 3];
                offset += 4;  // Skip VLAN tag (4 bytes)
            }
            
            // Layer 3: Network Layer Protocols
            if (etherType == 0x0800 && offset + 20 <= len) {
                // IPv4 (RFC 791) - EtherType 0x0800
                // Min header size: 20 bytes
                hasIPv4_ = true;
                
                // Protocol field at byte 9 of IPv4 header
                uint8_t protocol = data[offset + 9];
                
                // IHL (Internet Header Length) at lower 4 bits of byte 0
                // Value in 32-bit words, so multiply by 4 for bytes
                uint8_t ihl = (data[offset] & 0x0F) * 4;
                offset += ihl;
                
                // Layer 4: Transport/Control Protocols (IANA protocol numbers)
                if (protocol == 6 && offset + 20 <= len) {
                    // TCP (RFC 793) - Protocol number 6, min header 20 bytes
                    hasTCP_ = true;
                } else if (protocol == 17 && offset + 8 <= len) {
                    // UDP (RFC 768) - Protocol number 17, header 8 bytes
                    hasUDP_ = true;
                } else if (protocol == 1) {
                    // ICMP (RFC 792) - Protocol number 1
                    hasICMP_ = true;
                }
            } else if (etherType == 0x86DD && offset + 40 <= len) {
                // IPv6 (RFC 2460) - EtherType 0x86DD
                // Fixed header size: 40 bytes
                hasIPv6_ = true;
                
                // Next Header field at byte 6 of IPv6 header
                uint8_t nextHeader = data[offset + 6];
                offset += 40;  // Skip fixed IPv6 header
                
                // Process IPv6 extension headers and find transport protocol
                // See RFC 2460 Section 4 for extension header format
                while (offset < len) {
                    if (nextHeader == 6 && offset + 20 <= len) {
                        // TCP - Protocol 6
                        hasTCP_ = true;
                        break;
                    } else if (nextHeader == 17 && offset + 8 <= len) {
                        // UDP - Protocol 17
                        hasUDP_ = true;
                        break;
                    } else if (nextHeader == 58) {
                        // ICMPv6 (RFC 4443) - Protocol 58
                        hasICMP_ = true;
                        break;
                    } else if (nextHeader == 0 ||   // Hop-by-Hop Options (RFC 2460)
                               nextHeader == 43 ||  // Routing Header (RFC 2460)
                               nextHeader == 44 ||  // Fragment Header (RFC 2460)
                               nextHeader == 51 ||  // Authentication Header / ESP (RFC 4303)
                               nextHeader == 60) {  // Destination Options (RFC 2460)
                        // Extension header format: [Next Header: 1][Hdr Ext Len: 1][Options: variable]
                        // Hdr Ext Len is in 8-byte units, NOT including first 8 bytes
                        if (offset + 2 > len) break;
                        uint8_t extLen = data[offset + 1];
                        nextHeader = data[offset];
                        offset += (extLen + 1) * 8;  // Length field + 1, in 8-byte units
                    } else {
                        // Unknown or unsupported next header
                        break;
                    }
                }
            } else if (etherType == 0x0806) {
                // ARP (RFC 826) - EtherType 0x0806
                hasARP_ = true;
            }
        }
    }
    
public:
    Packet(const RawPacket* rawPacket) : rawPacket_(rawPacket) {
        parsePacket();
    }
    
    bool isPacketOfType(ProtocolType type) const {
        switch (type) {
            case Ethernet: return hasEthernet_;
            case IPv4: return hasIPv4_;
            case IPv6: return hasIPv6_;
            case ARP: return hasARP_;
            case TCP: return hasTCP_;
            case UDP: return hasUDP_;
            case ICMP: return hasICMP_;
            default: return false;
        }
    }
    
    // Compatibility methods (return nullptr as we don't parse full layers)
    template<typename T> T* getLayerOfType() { return nullptr; }
};

// PCAP/PCAPNG file reader
class IFileReaderDevice {
private:
    std::ifstream file_;
    bool isPcapNG_ = false;
    uint32_t linkType_ = 1; // Default: Ethernet
    bool swapBytes_ = false;
    
    uint16_t swap16(uint16_t val) const {
        return swapBytes_ ? ((val << 8) | (val >> 8)) : val;
    }
    
    uint32_t swap32(uint32_t val) const {
        if (!swapBytes_) return val;
        return ((val << 24) | ((val << 8) & 0x00FF0000) | 
                ((val >> 8) & 0x0000FF00) | (val >> 24));
    }
    
    bool readPcapNGBlock(RawPacket& packet) {
        // PCAPNG Block Structure (all blocks):
        // [Block Type: 4][Block Total Length: 4][Block Body: variable][Block Total Length: 4]
        
        uint32_t blockType, blockLen;
        file_.read(reinterpret_cast<char*>(&blockType), 4);
        file_.read(reinterpret_cast<char*>(&blockLen), 4);
        
        // Minimum block size: 12 bytes (type + length + trailing length)
        if (!file_ || blockLen < 12) return false;
        
        blockType = swap32(blockType);
        blockLen = swap32(blockLen);
        
        // PCAPNG Block Types (see: https://www.ietf.org/archive/id/draft-tuexen-opsawg-pcapng-02.html)
        if (blockType == 0x00000006) {
            // Enhanced Packet Block (EPB) - most common packet storage block
            // Structure: [Interface ID: 4][Timestamp High: 4][Timestamp Low: 4][Captured Len: 4]
            //            [Packet Len: 4][Packet Data: variable][Padding: 0-3][Options: variable]
            
            uint32_t interfaceId, capturedLen, packetLen;
            uint64_t timestamp;
            
            file_.read(reinterpret_cast<char*>(&interfaceId), 4);
            file_.read(reinterpret_cast<char*>(&timestamp), 8);       // 64-bit timestamp (microseconds)
            file_.read(reinterpret_cast<char*>(&capturedLen), 4);     // Actual captured bytes
            file_.read(reinterpret_cast<char*>(&packetLen), 4);       // Original packet length
            
            capturedLen = swap32(capturedLen);
            packet.data.resize(capturedLen);
            file_.read(reinterpret_cast<char*>(packet.data.data()), capturedLen);
            
            // PCAPNG requires 32-bit alignment, so calculate padding bytes
            uint32_t padding = (4 - (capturedLen % 4)) % 4;
            
            // Total block length includes: type(4) + len(4) + interface(4) + timestamp(8) +
            //                              captured_len(4) + packet_len(4) + data + padding + options + trailing_len(4)
            // = 28 + capturedLen + padding + options + 4
            uint32_t remainingBytes = blockLen - 28 - capturedLen - padding;
            file_.seekg(padding + remainingBytes, std::ios::cur);
            
            return file_.good();
        } else if (blockType == 0x00000002) {
            // Simple Packet Block (SPB) - deprecated, skip to next block
            // We skip 8 bytes already read (type + length)
            file_.seekg(blockLen - 8, std::ios::cur);
            return readPcapNGBlock(packet);  // Recursively read next block
        } else {
            // Other block types we don't process:
            // 0x0A0D0D0A = Section Header Block (file metadata)
            // 0x00000001 = Interface Description Block (interface metadata)
            // 0x00000003 = Name Resolution Block (DNS/hostname info)
            // 0x00000004 = Interface Statistics Block (interface stats)
            // Skip these and continue to next block
            file_.seekg(blockLen - 8, std::ios::cur);
            return readPcapNGBlock(packet);  // Recursively read next block
        }
    }
    
public:
    static IFileReaderDevice* getReader(const std::string& filename) {
        auto* reader = new IFileReaderDevice();
        reader->file_.open(filename, std::ios::binary);
        if (!reader->file_) {
            delete reader;
            return nullptr;
        }
        return reader;
    }
    
    bool open() {
        if (!file_.is_open()) return false;
        
        // Read magic number (first 4 bytes) to determine file format
        uint32_t magic;
        file_.read(reinterpret_cast<char*>(&magic), 4);
        
        // PCAP File Format Magic Numbers:
        if (magic == 0xA1B2C3D4) {
            // Classic PCAP format (libpcap) - native byte order
            // Magic: 0xA1B2C3D4 means timestamps in microseconds, native endian
            isPcapNG_ = false;
            swapBytes_ = false;
            
            // PCAP Header Structure (24 bytes total):
            // [Magic: 4][Major: 2][Minor: 2][ThisZone: 4][SigFigs: 4][SnapLen: 4][Network: 4]
            file_.seekg(16, std::ios::cur);  // Skip version, timezone, sigfigs, snaplen (16 bytes)
            
            // Read link type (data link type, e.g., 1 = Ethernet)
            // See: https://www.tcpdump.org/linktypes.html
            file_.read(reinterpret_cast<char*>(&linkType_), 4);
            linkType_ = swap32(linkType_);
            
        } else if (magic == 0xD4C3B2A1) {
            // Classic PCAP format - byte-swapped (different endianness)
            // Magic: 0xD4C3B2A1 means we need to swap all multi-byte fields
            isPcapNG_ = false;
            swapBytes_ = true;
            
            // Skip and read header same as above
            file_.seekg(16, std::ios::cur);
            file_.read(reinterpret_cast<char*>(&linkType_), 4);
            linkType_ = swap32(linkType_);
            
        } else if (magic == 0x0A0D0D0A) {
            // PCAPNG format (modern format, block-based)
            // Magic: 0x0A0D0D0A is a unique byte sequence that:
            // - Is unlikely to appear naturally (CR+LF+LF+CR)
            // - Helps detect text-mode file corruption
            // - Works as endianness detector in subsequent bytes
            isPcapNG_ = true;
            file_.seekg(0, std::ios::beg);  // Reset to start for block-based reading
        } else {
            // Unrecognized format
            return false;
        }
        
        return true;
    }
    
    bool getNextPacket(RawPacket& packet) {
        if (!file_) return false;
        
        if (isPcapNG_) {
            return readPcapNGBlock(packet);
        } else {
            // Classic PCAP Packet Record Structure (16 bytes header + data):
            // [Timestamp Seconds: 4][Timestamp Microseconds: 4][Captured Length: 4][Original Length: 4][Packet Data: variable]
            
            uint32_t ts_sec, ts_usec, incl_len, orig_len;
            
            file_.read(reinterpret_cast<char*>(&ts_sec), 4);     // Unix epoch timestamp (seconds)
            file_.read(reinterpret_cast<char*>(&ts_usec), 4);    // Microseconds or nanoseconds (depends on magic)
            file_.read(reinterpret_cast<char*>(&incl_len), 4);   // Number of bytes actually captured
            file_.read(reinterpret_cast<char*>(&orig_len), 4);   // Original packet length (may be truncated)
            
            if (!file_) return false;
            
            // Apply byte swapping if needed (for different endianness files)
            incl_len = swap32(incl_len);
            
            // Read the actual packet data
            packet.data.resize(incl_len);
            file_.read(reinterpret_cast<char*>(packet.data.data()), incl_len);
            
            return file_.good();
        }
    }
    
    void close() {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    ~IFileReaderDevice() {
        close();
    }
};

} // namespace pcap

namespace pcpp = pcap;
