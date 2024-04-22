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

} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
