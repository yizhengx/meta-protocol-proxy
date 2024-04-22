#include "memcached_codec.h"
#include "envoy/common/exception.h"
#include "source/common/common/assert.h"

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
  uint64_t cas = ntohl(*reinterpret_cast<const uint64_t*>(header_bytes + 16));

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
    expiry = ntohl(*reinterpret_cast<const uint32_t*>(buffer.linearize(4)) + 4);
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

void MemcachedCodec::encode(const MetaProtocolProxy::Metadata& metadata,
                            const MetaProtocolProxy::Mutation& mutation,
                            Buffer::Instance& buffer) {
  // TODO we don't need to implement encode for now.
  // This method only need to be implemented if we want to modify the respose message
  (void)metadata;
  (void)mutation;
  (void)buffer;
}


void MemcachedCodec::onError(const MetaProtocolProxy::Metadata&,
                             const MetaProtocolProxy::Error&,
                             Buffer::Instance&) {
    // Error handling logic will be implemented in the next part
}

// Additional private methods for handling specific commands

} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
