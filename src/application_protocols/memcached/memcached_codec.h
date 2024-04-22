#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/common/logger.h"
#include "src/meta_protocol_proxy/codec/codec.h"
#include "protocol.h"  // Include the Memcached protocol header

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

class MemcachedCodec : public MetaProtocolProxy::Codec,
                       public Logger::Loggable<Logger::Id::misc> {
public:
    MemcachedCodec();
    ~MemcachedCodec() override = default;

    // Decode incoming data from the buffer and populate metadata
    MetaProtocolProxy::DecodeStatus decode(Buffer::Instance& buffer,
                                           MetaProtocolProxy::Metadata& metadata) override;

    // Encode data into the buffer based on metadata and mutation
    void encode(const MetaProtocolProxy::Metadata& metadata,
                const MetaProtocolProxy::Mutation& mutation,
                Buffer::Instance& buffer) override;

    // Handle errors and encode appropriate responses
    void onError(const MetaProtocolProxy::Metadata& metadata,
                 const MetaProtocolProxy::Error& error,
                 Buffer::Instance& buffer) override;

private:
    // Method to handle decoding of GET commands
    MetaProtocolProxy::DecodeStatus decodeGetCommand(Buffer::Instance& buffer,
                                                     const MemcachedRequestHeader& header,
                                                     MetaProtocolProxy::Metadata& metadata);

    // Method to handle decoding of SET commands
    MetaProtocolProxy::DecodeStatus decodeSetCommand(Buffer::Instance& buffer,
                                                     const MemcachedRequestHeader& header,
                                                     MetaProtocolProxy::Metadata& metadata);

    // Additional private methods and members as necessary
};


static std::string MagicByteToString(uint8_t magic) {
    switch (magic) {
        case 0x80: return "Request";
        case 0x81: return "Response";
        default: return "Unknown";
    }
}

static std::string ResponseStatusToString(uint16_t status) {
    switch (status) {
        case 0x0000: return "No error";
        case 0x0001: return "Key not found";
        case 0x0002: return "Key exists";
        case 0x0003: return "Value too large";
        case 0x0004: return "Invalid arguments";
        case 0x0005: return "Item not stored";
        case 0x0006: return "Incr/Decr on non-numeric value";
        case 0x0081: return "Unknown command";
        case 0x0082: return "Out of memory";
        default: return "Other error";
    }
}

static std::string OpcodeToString(uint8_t opcode) {
    switch (opcode) {
        case 0x00: return "Get";
        case 0x01: return "Set";
        case 0x02: return "Add";
        case 0x03: return "Replace";
        case 0x04: return "Delete";
        case 0x05: return "Increment";
        case 0x06: return "Decrement";
        case 0x07: return "Quit";
        case 0x08: return "Flush";
        case 0x09: return "GetQ";
        case 0x0A: return "No-op";
        case 0x0B: return "Version";
        case 0x0C: return "GetK";
        case 0x0D: return "GetKQ";
        case 0x0E: return "Append";
        case 0x0F: return "Prepend";
        case 0x10: return "Stat";
        case 0x11: return "SetQ";
        case 0x12: return "AddQ";
        case 0x13: return "ReplaceQ";
        case 0x14: return "DeleteQ";
        case 0x15: return "IncrementQ";
        case 0x16: return "DecrementQ";
        case 0x17: return "QuitQ";
        case 0x18: return "FlushQ";
        case 0x19: return "AppendQ";
        case 0x1A: return "PrependQ";
        default: return "Unknown Opcode";
    }
}

} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
