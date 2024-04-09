#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

const size_t MEMCACHED_HEADER_SIZE = 24;

enum class MemcachedCommand : uint8_t {
    Get = 0x00,
    Set = 0x01,
    // ... other commands
};

struct MemcachedRequestHeader {
    uint8_t magic;
    MemcachedCommand command;
    uint16_t key_length;
    uint8_t extras_length;
    uint8_t data_type;
    uint16_t vbucket_id;
    uint32_t total_body_length;
    uint32_t opaque;
    uint64_t cas;

    bool decode(Buffer::Instance& buffer);
    // Encoding not required for request handling
};

struct MemcachedResponseHeader {
    uint8_t magic;
    MemcachedCommand command;
    uint16_t status;
    uint16_t key_length;
    uint8_t extras_length;
    uint8_t data_type;
    uint32_t total_body_length;
    uint32_t opaque;
    uint64_t cas;

    bool encode(Buffer::Instance& buffer);
    // Decoding not required for response generation
};

} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
