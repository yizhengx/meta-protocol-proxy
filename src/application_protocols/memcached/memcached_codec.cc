#include "memcached_codec.h"
#include "envoy/common/exception.h"
#include "common/common/assert.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

MetaProtocolProxy::DecodeStatus MemcachedCodec::decode(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {
  ENVOY_LOG(debug, "Memcached codec: {} bytes available", buffer.length());

  const size_t MemcachedHeaderSize = 24; // Size of Memcached protocol header

  // Check if the buffer has enough data for the Memcached header
  if (buffer.length() < MemcachedHeaderSize) {
    return MetaProtocolProxy::DecodeStatus::WaitForData;
  }

  // Extract the first 24 bytes from the buffer as header
  uint8_t header_bytes[MemcachedHeaderSize];
  buffer.copyOut(0, MemcachedHeaderSize, header_bytes);

  // Parse the Memcached header
  uint8_t magic = header_bytes[0];
  uint8_t opcode = header_bytes[1];
  uint16_t key_length = ntohs(*reinterpret_cast<const uint16_t*>(header_bytes + 2));
  uint8_t extras_length = header_bytes[4];
  uint8_t data_type = header_bytes[5];
  uint16_t status_or_reserved = ntohs(*reinterpret_cast<const uint16_t*>(header_bytes + 6));
  uint32_t total_body_length = ntohl(*reinterpret_cast<const uint32_t*>(header_bytes + 8));
  uint32_t opaque = ntohl(*reinterpret_cast<const uint32_t*>(header_bytes + 12));
  uint64_t cas = ntohll(*reinterpret_cast<const uint64_t*>(header_bytes + 16));

  // Check if the buffer has the full body of the message
  if (buffer.length() < MemcachedHeaderSize + total_body_length) {
    return MetaProtocolProxy::DecodeStatus::WaitForData;
  }

  // Populate metadata
  metadata.put("Magic", magic);
  metadata.put("Opcode", opcode);
  metadata.put("KeyLength", key_length);
  metadata.put("ExtrasLength", extras_length);
  metadata.put("DataType", data_type);
  metadata.put(magic == 0x81 ? "Status" : "Reserved", status_or_reserved);
  metadata.put("TotalBodyLength", total_body_length);
  metadata.put("Opaque", opaque);
  metadata.put("CAS", cas);
  // Inside your decode method
    metadata.putString("MagicValue", MagicByteToString(magic));
    metadata.putString("OpcodeValue", OpcodeToString(opcode));
    metadata.putString("StatusValue", ResponseStatusToString(status_or_reserved));



  // Extract the extras (flags and expiry)
  uint32_t flags, expiry;
  if (extras_length >= 8) { // Ensure there are at least 8 bytes for flags and expiry
    buffer.drain(MemcachedHeaderSize); // Remove the header
    flags = ntohl(*reinterpret_cast<const uint32_t*>(buffer.linearize(4)));
    expiry = ntohl(*reinterpret_cast<const uint32_t*>(buffer.linearize(4) + 4));
    buffer.drain(8); // Remove the extras
    metadata.put("Flags", flags);
    metadata.put("Expiry", expiry);
  }

  // Extract the key
  std::string key;
  if (key_length > 0) {
    key.assign(static_cast<const char*>(buffer.linearize(key_length)), key_length);
    buffer.drain(key_length); // Remove the key
    metadata.putString("Key", key);
  }

  // Extract the value
  size_t value_length = total_body_length - key_length - extras_length;
  std::string value;
  if (value_length > 0) {
    value.assign(static_cast<const char*>(buffer.linearize(value_length)), value_length);
    buffer.drain(value_length); // Remove the value
    metadata.putString("Value", value);
  }

  return MetaProtocolProxy::DecodeStatus::Done;
}

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

void MemcachedCodec::encode(const MetaProtocolProxy::Metadata& metadata,
                            const MetaProtocolProxy::Mutation& mutation,
                            Buffer::Instance& buffer) {
  // TODO we don't need to implement encode for now.
  // This method only need to be implemented if we want to modify the respose message
  (void)metadata;
  (void)mutation;
  (void)buffer;
}


void MemcachedCodec::onError(const MetaProtocolProxy::Metadata& metadata,
                             const MetaProtocolProxy::Error& error,
                             Buffer::Instance& buffer) {
    // Error handling logic will be implemented in the next part
}

// Additional private methods for handling specific commands

} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
