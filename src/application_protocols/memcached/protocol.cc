#include "protocol.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

bool MemcachedRequestHeader::decode(Buffer::Instance& buffer) {
    if (buffer.length() < MEMCACHED_HEADER_SIZE) {
        return false; // Need more data
    }

    magic = buffer.peekBEInt<uint8_t>(0);
    command = static_cast<MemcachedCommand>(buffer.peekBEInt<uint8_t>(1));
    key_length = buffer.peekBEInt<uint16_t>(2);
    extras_length = buffer.peekBEInt<uint8_t>(4);
    data_type = buffer.peekBEInt<uint8_t>(5);
    vbucket_id = buffer.peekBEInt<uint16_t>(6);
    total_body_length = buffer.peekBEInt<uint32_t>(8);
    opaque = buffer.peekBEInt<uint32_t>(12);
    cas = buffer.peekBEInt<uint64_t>(16);

    buffer.drain(MEMCACHED_HEADER_SIZE);
    return true;
}

bool MemcachedResponseHeader::encode(Buffer::Instance& buffer) {
    buffer.writeBEInt<uint8_t>(magic);
    buffer.writeBEInt<uint8_t>(static_cast<uint8_t>(command));
    buffer.writeBEInt<uint16_t>(status);
    buffer.writeBEInt<uint16_t>(key_length);
    buffer.writeBEInt<uint8_t>(extras_length);
    buffer.writeBEInt<uint8_t>(data_type);
    buffer.writeBEInt<uint32_t>(total_body_length);
    buffer.writeBEInt<uint32_t>(opaque);
    buffer.writeBEInt<uint64_t>(cas);

    return true;
}

} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
