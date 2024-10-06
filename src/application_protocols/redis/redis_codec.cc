#include "redis_codec.h"
#include "envoy/common/exception.h"
#include "source/common/common/assert.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Redis {

MetaProtocolProxy::DecodeStatus RedisCodec::decode(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {

  message_type_ = metadata.getMessageType();

  while (decode_status_ != RedisDecodeStatus::DecodeDone) {
    decode_status_ = handleState(buffer, metadata);
    if (decode_status_ == RedisDecodeStatus::WaitForData) {
      return MetaProtocolProxy::DecodeStatus::WaitForData;
    }
  }

  // fill the metadata with the headers exacted from the message
  std::cout << "TO METADATA: BUFFER -> " << buffer_to_string(buffer, buffer.length()) << std::endl;
  toMetadata(metadata);
  decode_status_ = RedisDecodeStatus::DecodeMsg;
  start_pos = 0;   // start reading position of the buffer
  item_needed = 0; // how many items needed to complete the current msg
  crlf_needed = 0; // how many crlf needed to complete the current item
  return MetaProtocolProxy::DecodeStatus::Done;
}


RedisDecodeStatus RedisCodec::handleState(Buffer::Instance& buffer, MetaProtocolProxy::Metadata&) {
  switch (decode_status_) {
  case RedisDecodeStatus::DecodeMsg: // decode header 
    return decodeMsg(buffer);
  default:
    PANIC("not reached");
  }
  return RedisDecodeStatus::DecodeDone;
}

RedisDecodeStatus RedisCodec::decodeMsg(Buffer::Instance& buffer) {
  std::cout << "Redis decodeMsg: " << buffer_to_string(buffer, buffer.length()) << std::endl;

  while (start_pos < buffer.length()) {
    if ( crlf_needed == 0) {
      // start reading a new item
      char op = static_cast<char>(buffer.peekInt<uint8_t>(start_pos));
      // find the next CRLF to make sure we have the whole control message
      size_t crlf_pos = start_pos;
      for (size_t i = start_pos+1; i < buffer.length(); i++) {
        if (buffer.peekInt<uint8_t>(i-1) == 13 and buffer.peekInt<uint8_t>(i) == 10){
          // end of the item
          crlf_pos = i;
          break;
        }
      }
      if (crlf_pos == start_pos) {
        // not enough data
        return RedisDecodeStatus::WaitForData;
      }
      if (op == '+'){
        std::cout << "Simple string: " << buffer_to_string(buffer, crlf_pos-start_pos) << std::endl;
        // simple string, since we have the CRLF, we can extract the item
        if (item_needed == 1) {
          return RedisDecodeStatus::DecodeDone;
        } else {
          item_needed -= 1;
        }
      } else if (op == '-'){
        // simple error
        if (item_needed == 1) {
          return RedisDecodeStatus::DecodeDone;
        } else {
          item_needed -= 1;
        }
      } else if (op == ':'){
        // integer
        if (item_needed == 1) {
          return RedisDecodeStatus::DecodeDone;
        } else {
          item_needed -= 1;
        }
      } else if (op == '$'){
        // bulk string
        crlf_needed = 1;
        std::cout << "Bulk string: " << buffer_to_string(buffer, crlf_pos-start_pos) << std::endl;
      } else if (op == '*'){
        // array, parse the op message to see how many items in the array
        int base = 1;
        int num = 0;
        for (size_t i = crlf_pos-2; i > start_pos; i--) {
          std::cout << "Array pick int " <<  buffer.peekBEInt<int8_t>(i) << std::endl;
          std::cout << "Array pick int " <<  static_cast<int>(buffer.peekBEInt<int8_t>(i)) - '0' << std::endl;
          std::cout << "Array pick int " <<  static_cast<int>(buffer.peekBEInt<int8_t>(i)) << std::endl;
          num += base * (static_cast<int>(buffer.peekBEInt<int8_t>(i)) - '0');
          std::cout << "Array num: " << num << std::endl;
          base *= 10;
        }
        item_needed += num;
        std::cout << "Array: " << num << std::endl;
      }  else {
        // invalid
        PANIC("Invalid Redis op code");
      }
      start_pos = crlf_pos+1;
    } else {
      // if we reach here, it means the start_pos > 1
      if (buffer.peekInt<uint8_t>(start_pos-1) == '\r' && buffer.peekInt<uint8_t>(start_pos) == '\n') {
        // found crlf
        std::cout << "--> ELSE BRANCH" << std::endl;
        std::cout << "POS: " << start_pos << std::endl;
        std::cout << "FOUND CRLF" << std::endl;
        std::cout << "CRLF needed: " << crlf_needed << std::endl;
        std::cout << "Item needed: " << item_needed << std::endl;
        crlf_needed -= 1;
        if (crlf_needed == 0) {
          // we have the whole item
          if (item_needed == 1) {
            std::cout << "Decode done" << std::endl;
            return RedisDecodeStatus::DecodeDone;
          } else {
            item_needed -= 1;
          }
        }
      }
      start_pos += 1;
    }
  }
  return RedisDecodeStatus::WaitForData;
}

std::string RedisCodec::buffer_to_string(Buffer::Instance& buffer, size_t length) {
  std::string result;
  for (size_t i = 0; i < length; i++) {
    char byte = static_cast<char>(buffer.peekInt<uint8_t>(i));
    if (byte == '\r') {
      result += "*";
    } else if (byte == '\n') {
      result += "#";
    } else {
      result += byte;
    }
  }
  return result;
}


void RedisCodec::toMetadata(MetaProtocolProxy::Metadata& metadata) {
  metadata.originMessage().move(*origin_msg_);
}


void RedisCodec::encode(const MetaProtocolProxy::Metadata& metadata,
                            const MetaProtocolProxy::Mutation& mutation,
                            Buffer::Instance& buffer) {
  // TODO we don't need to implement encode for now.
  // This method only need to be implemented if we want to modify the respose message
  // ENVOY_LOG(warn, "Redis encoder: {} bytes available, msg type: {}", buffer.length(), static_cast<int>(metadata.getMessageType()));
  (void)metadata;
  (void)mutation;
  (void)buffer;
}


void RedisCodec::onError(const MetaProtocolProxy::Metadata&,
                             const MetaProtocolProxy::Error&,
                             Buffer::Instance&) {
    // Error handling logic will be implemented in the next part
}

// Additional private methods for handling specific commands

} // namespace Redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
