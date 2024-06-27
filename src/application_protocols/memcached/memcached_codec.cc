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
  case MemcachedDecodeStatus::DecodeTextProtocol:
    return parseTextProtocol(buffer);
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

  uint8_t magic_code = 0x80;

  if buffer.length() >= 1: // we probabaly don't need this line
    bool is_magic = buffer.peekBEInt<uint8_t>(0) >= magic_code;

    if !is_magic:
      is_binary_protocol_ = false;
      std::cout << "[MemcachedCodec::decodeHeader()] Not a memcached binary protocol" << std::endl;
      return MemcachedDecodeStatus::DecodeTextProtocol;

  
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

MemcachedDecodeStatus MemcachedCodec::decodeTextProtocol(Buffer::Instance& buffer) {
  while (true) {
    // parse command

    size_t pre_parsed_pos_ = parsed_pos_;
    size_t pos;

    bool end_of_chunk = false;
    for (size_t i = parsed_pos_+1; i < buffer.length(); i++) {
      if buffer.peekBEInt<uint8_t>(i-1) == 13 and buffer.peekBEInt<uint8_t>(i) == 10:
        // end of the command
        pos = i;
        end_of_chunk = true;
        break;
    } 

    if (!end_of_chunk) {
      std::cout << "[MemcachedCodec::decodeHeader()] Waiting for more data " << std::endl;
      return MemcachedDecodeStatus::WaitForData;
    }
    parsed_pos = pos;

    char cur_bytes_chunk[parsed_pos-pre_parsed_pos_+1];
    buffer.copyOut(pre_parsed_pos_+1, parsed_pos-pre_parsed_pos_+1, cur_bytes_chunk);

    MemcachedDecodeStatus status;
    if (message_type_ == MetaProtocolProxy::MessageType::Request) {
      status = decodeTextRequest(cur_bytes_chunk);
    } else {
      status = decodeTextResponse(cur_bytes_chunk);
    }

    if (status == MemcachedDecodeStatus::DecodeDone) {
      // handle message saving 
      origin_msg_ = std::make_unique<Buffer::OwnedImpl>();
      origin_msg_->move(buffer, parsed_pos_+1);
      std::cout << "[MemcachedCodec::decodeTextProtocol()] Memcached text protocol decoded, message type: " << message_type_ << std::endl;
      return MemcachedDecodeStatus::DecodeDone;
    } 
  }
}

MemcachedDecodeStatus MemcachedCodec::decodeTextRequest(char* chunk) {

  if (is_request_cmd_done_){
    std::cout << "[MemcachedCodec::decodeHeader()] Request command is already done, decoding finished" << std::endl;
    return MemcachedDecodeStatus::DecodeDone;
  }

  is_request_cmd_done_ = true;

  size_t chunk_length = std::strlen(chunk);
  if (chunk_length < 3) {
    std::cout << "[MemcachedCodec::decodeHeader()] chunk length<3, probably other commands" << std::endl;
    return MemcachedDecodeStatus::DecodeDone;
  }

  bool checkCommand = [&](const char* command, size_t length) {
    if (chunk_length >= length) {
        char buffer[8] = {0}; // Buffer size sufficient for the longest command
        buffer_.copyOut(0, length, buffer);
        if (memcmp(buffer, command, length) == 0) {
            std::cout << "[MemcachedCodec::decodeHeader()] " << command << " command" << std::endl;
            return true;
        }
    }
    return false
  };

  if (checkCommand("set", 3) || checkCommand("add", 3) || checkCommand("cas", 3) ||
    checkCommand("append", 6) || checkCommand("prepend", 7) || checkCommand("replace", 7)) {
    return MemcachedDecodeStatus::WaitForData; // continue decoding
  }
  return MemcachedDecodeStatus::DecodeDone;
}

MemcachedDecodeStatus MemcachedCodec::decodeTextResponse(char* chunk) {
    size_t chunk_length = std::strlen(chunk);

    if (chunk_length < 5) {
        std::cout << "[MemcachedCodec::decodeTextResponse()] Chunk length < 3, probably other content" << std::endl;
        return MemcachedDecodeStatus::DecodeDone;
    }

    bool checkContent = [&](const char* content, size_t length) {
      if (chunk_length == length && std::memcmp(chunk, content, length) == 0){
        std::cout << "[MemcachedCodec::decodeTextResponse()] content: ";
        for (size_t i = 0; i < length - 2; ++i) {
            std::cout << content[i];
        }
        std::cout << std::endl;
        return true;
      }
      return false;
    };

    if (checkContent("STORED\r\n", 7) || checkContent("NOT_STORED\r\n", 11) || checkContent("EXISTS\r\n", 8) ||
        checkContent("NOT_FOUND\r\n", 10) || checkContent("END\r\n", 5)) {
        return MemcachedDecodeStatus::DecodeDone; // continue decoding
    }

    return MemcachedDecodeStatus::WaitForData;
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
