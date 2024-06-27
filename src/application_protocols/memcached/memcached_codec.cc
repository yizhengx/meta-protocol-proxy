#include "memcached_codec.h"
#include "envoy/common/exception.h"
#include "source/common/common/assert.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

// static std::string MagicByteToString(uint8_t magic) {
//     switch (magic) {
//         case 0x80: return "Request";
//         case 0x81: return "Response";
//         default: return "Unknown";
//     }
// }

// static std::string ResponseStatusToString(uint16_t status) {
//     switch (status) {
//         case 0x0000: return "No error";
//         case 0x0001: return "Key not found";
//         case 0x0002: return "Key exists";
//         case 0x0003: return "Value too large";
//         case 0x0004: return "Invalid arguments";
//         case 0x0005: return "Item not stored";
//         case 0x0006: return "Incr/Decr on non-numeric value";
//         case 0x0081: return "Unknown command";
//         case 0x0082: return "Out of memory";
//         default: return "Other error";
//     }
// }

// static std::string OpcodeToString(uint8_t opcode) {
//     switch (opcode) {
//         case 0x00: return "Get";
//         case 0x01: return "Set";
//         case 0x02: return "Add";
//         case 0x03: return "Replace";
//         case 0x04: return "Delete";
//         case 0x05: return "Increment";
//         case 0x06: return "Decrement";
//         case 0x07: return "Quit";
//         case 0x08: return "Flush";
//         case 0x09: return "GetQ";
//         case 0x0A: return "No-op";
//         case 0x0B: return "Version";
//         case 0x0C: return "GetK";
//         case 0x0D: return "GetKQ";
//         case 0x0E: return "Append";
//         case 0x0F: return "Prepend";
//         case 0x10: return "Stat";
//         case 0x11: return "SetQ";
//         case 0x12: return "AddQ";
//         case 0x13: return "ReplaceQ";
//         case 0x14: return "DeleteQ";
//         case 0x15: return "IncrementQ";
//         case 0x16: return "DecrementQ";
//         case 0x17: return "QuitQ";
//         case 0x18: return "FlushQ";
//         case 0x19: return "AppendQ";
//         case 0x1A: return "PrependQ";
//         default: return "Unknown Opcode";
//     }
// }

// MetaProtocolProxy::DecodeStatus MemcachedCodec::decode(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {
//   // ENVOY_LOG(warn, "Memcached decoder: {} bytes available, msg type: {}", buffer.length(),
//   //           static_cast<int>(metadata.getMessageType())); 

//   MetaProtocolProxy::MessageType messageType_ = metadata.getMessageType();
//   std::string message_type_str = messageType_ == MetaProtocolProxy::MessageType::Request ? "Request" : "Response";
//   std::cout << "[MemcachedCodec::decode()] Memcached decoder: " << buffer.length() << " bytes available, msg type: " << message_type_str << std::endl;

//   const size_t MemcachedHeaderSize = 24; // Size of Memcached protocol header

//   // Check if the buffer has enough data for the Memcached header
//   if (buffer.length() < MemcachedHeaderSize) {
//     std::cout << "[MemcachedCodec::decode()] Returned: waiting for more data (1st branch) " << std::endl;
//     return MetaProtocolProxy::DecodeStatus::WaitForData;
//   }

//   // Extract the first 24 bytes from the buffer as header
//   uint8_t header_bytes[MemcachedHeaderSize];
//   buffer.copyOut(0, MemcachedHeaderSize, header_bytes);

//   // Parse the Memcached header
//   uint8_t magic = header_bytes[0];
//   std::cout << "[MemcachedCodec::decode()] Magic: " << static_cast<int>(magic) << std::endl;
//   uint8_t opcode = header_bytes[1];
//   uint16_t key_length = ntohs(*reinterpret_cast<const uint16_t*>(header_bytes + 2));
//   std::cout << "[MemcachedCodec::decode()] Key length: " << key_length << std::endl;
//   uint8_t extras_length = header_bytes[4];
//   std::cout << "[MemcachedCodec::decode()] Extras length: " << static_cast<int>(extras_length) << std::endl;
//   uint8_t data_type = header_bytes[5];
//   uint16_t status_or_reserved = ntohs(*reinterpret_cast<const uint16_t*>(header_bytes + 6));
//   uint32_t total_body_length = ntohl(*reinterpret_cast<const uint32_t*>(header_bytes + 8));
//   std::cout << "[MemcachedCodec::decode()] Total body length: " << total_body_length << std::endl;
//   uint32_t opaque = ntohl(*reinterpret_cast<const uint32_t*>(header_bytes + 12));
//   uint64_t cas = ntohl(*reinterpret_cast<const uint64_t*>(header_bytes + 16));

//   // Check if the buffer has the full body of the message
//   while (buffer.length() < MemcachedHeaderSize + total_body_length) {
//     std::cout << "[MemcachedCodec::decode()] Returned: waiting for more data (2nd branch) " << std::endl;
//     return MetaProtocolProxy::DecodeStatus::WaitForData;
//   }

//   // Populate metadata
//   metadata.put("Magic", magic);
//   metadata.put("Opcode", opcode);
//   metadata.put("KeyLength", key_length);
//   metadata.put("ExtrasLength", extras_length);
//   metadata.put("DataType", data_type);
//   metadata.put(magic == 0x81 ? "Status" : "Reserved", status_or_reserved);
//   metadata.put("TotalBodyLength", total_body_length);
//   metadata.put("Opaque", opaque);
//   metadata.put("CAS", cas);
//   // Inside your decode method
//     metadata.putString("MagicValue", MagicByteToString(magic));
//     metadata.putString("OpcodeValue", OpcodeToString(opcode));
//     metadata.putString("StatusValue", ResponseStatusToString(status_or_reserved));

//   // Extract the extras (flags and expiry)
//   buffer.drain(MemcachedHeaderSize); // Remove the header
//   uint32_t flags, expiry;
//   if (extras_length >= 8) { // Ensure there are at least 8 bytes for flags and expiry
//     flags = ntohl(*reinterpret_cast<const uint32_t*>(buffer.linearize(4)));
//     expiry = ntohl(*reinterpret_cast<const uint32_t*>(buffer.linearize(4)) + 4);
//     buffer.drain(8); // Remove the extras
//     metadata.put("Flags", flags);
//     metadata.put("Expiry", expiry);
//   }

//   // Extract the key
//   std::string key;
//   if (key_length > 0) {
//     key.assign(static_cast<const char*>(buffer.linearize(key_length)), key_length);
//     buffer.drain(key_length); // Remove the key
//     metadata.putString("Key", key);
//   }

//   // Extract the value
//   size_t value_length = total_body_length - key_length - extras_length;
//   std::string value;
//   if (value_length > 0) {
//     value.assign(static_cast<const char*>(buffer.linearize(value_length)), value_length);
//     buffer.drain(value_length); // Remove the value
//     metadata.putString("Value", value);
//   }

//   std::cout << "[MemcachedCodec::decode()] Returned: done" << std::endl;

//   return MetaProtocolProxy::DecodeStatus::Done;
// }


MetaProtocolProxy::DecodeStatus MemcachedCodec::decode(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {

  message_type_ = metadata.getMessageType();
  std::string message_type_str = message_type_ == MetaProtocolProxy::MessageType::Request ? "Request" : "Response";
  std::cout << "[MemcachedCodec::decode()] Memcached decoder: " << buffer.length() << " bytes available, msg type: " << message_type_str << std::endl;
  // ENVOY_LOG(warn, "Memcached decoder: {} bytes available, msg type: {}", buffer.length(), static_cast<int>(metadata.getMessageType()));

  while (decode_status_ != MemcachedDecodeStatus::DecodeDone) {
    decode_status_ = handleState(buffer);
    if (decode_status_ == MemcachedDecodeStatus::WaitForData) {
      return MetaProtocolProxy::DecodeStatus::WaitForData;
    }
  }

  // fill the metadata with the headers exacted from the message
  toMetadata(metadata);
  // reset decode status
  decode_status_ = MemcachedDecodeStatus::DecodeHeader;
  return MetaProtocolProxy::DecodeStatus::Done;
}


MemcachedDecodeStatus MemcachedCodec::handleState(Buffer::Instance& buffer) {
  switch (decode_status_) {
  case MemcachedDecodeStatus::DecodeHeader: // decode header
    return decodeHeader(buffer);
  case MemcachedDecodeStatus::DecodeBody: // decode body
    return decodeBody(buffer);
  case MemcachedDecodeStatus::WaitForData:
    return MemcachedDecodeStatus::WaitForData;
  default:
    PANIC("not reached");
  }
  return MemcachedDecodeStatus::DecodeDone;
}

MemcachedDecodeStatus MemcachedCodec::decodeHeader(Buffer::Instance& buffer) {
  // Check if the buffer has enough data for the Memcached header
  std::cout << "[MemcachedCodec::decodeHeader()] Buffer length: " << buffer.length() << std::endl;
  // ENVOY_LOG(warn, "Memcached decodeHeader: {} bytes available", buffer.length());

  uint8_t byte;
  uint8_t magic_code = 0x80;

  for (int i = 0; i < 9; i++) {
    byte = buffer.peekBEInt<uint8_t>(i);
    if (i==0){
      std::cout << "[MemcachedCodec::decodeHeader()] Is Magic: " << byte >= magic_code << std::endl;
    }
    std::cout << "[MemcachedCodec::decodeHeader()] Byte " << i << ": " << static_cast<int>(byte) << std::endl;
  }

  
  if (buffer.length() < MEMCACHED_HEADER_SIZE) {
    std::cout << "[MemcachedCodec::decodeHeader()] Waiting for more data " << std::endl;
    // ENVOY_LOG(warn, "Memcached decodeHeader: waiting for more data");
    return MemcachedDecodeStatus::WaitForData;
  }

  if (!memcached_header_.decode(buffer)) {
    throw EnvoyException("Invalid Memcached header");
  }

  std::cout << "[MemcachedCodec::decodeHeader()] Memcached header decoded: key length: total body length: " << memcached_header_.get_total_body_length() << std::endl;
  // ENVOY_LOG(warn, "Memcached decodeHeader: Memcached header decoded: key length: {}, total body length: {}", memcached_header_.get_key_length(), memcached_header_.get_total_body_length());
  return MemcachedDecodeStatus::DecodeBody;

}

MemcachedDecodeStatus MemcachedCodec::decodeBody(Buffer::Instance& buffer) {
  // Check if the buffer has the full body of the message
  if (buffer.length() < MEMCACHED_HEADER_SIZE + memcached_header_.get_total_body_length()) {
    std::cout << "[MemcachedCodec::decodeBody()] Waiting for more data " << std::endl;
    // ENVOY_LOG(warn, "Memcached decodeBody: waiting for more data");
    return MemcachedDecodeStatus::WaitForData;
  }

  // decode body: skip it for now since we dont care about the body

  origin_msg_ = std::make_unique<Buffer::OwnedImpl>();
  origin_msg_->move(buffer, MEMCACHED_HEADER_SIZE + memcached_header_.get_total_body_length());
  std::cout << "[MemcachedCodec::decodeBody()] Memcached body decoded " << std::endl;
  return MemcachedDecodeStatus::DecodeDone;
}

void MemcachedCodec::toMetadata(MetaProtocolProxy::Metadata& metadata) {
  metadata.originMessage().move(*origin_msg_);
}


void MemcachedCodec::encode(const MetaProtocolProxy::Metadata& metadata,
                            const MetaProtocolProxy::Mutation& mutation,
                            Buffer::Instance& buffer) {
  // TODO we don't need to implement encode for now.
  // This method only need to be implemented if we want to modify the respose message
  // ENVOY_LOG(warn, "Memcached encoder: {} bytes available, msg type: {}", buffer.length(), static_cast<int>(metadata.getMessageType()));
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
