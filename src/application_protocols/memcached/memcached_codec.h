#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/buffer/buffer_impl.h"
#include "source/common/common/logger.h"
#include "src/meta_protocol_proxy/codec/codec.h"
#include "protocol.h"  // Include the Memcached protocol header

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

enum class MemcachedDecodeStatus {
  DecodeHeader,
  DecodeBody,
  DecodeDone,
  WaitForData,
  DecodeTextProtocol,
};

class MemcachedCodec : public MetaProtocolProxy::Codec,
                       public Logger::Loggable<Logger::Id::misc> {
public:
    MemcachedCodec() {
        // ENVOY_LOG(warn, "Memcached codec created");
        // std::cout << "[MemcachedCodec::MemcachedCodec()] Memcached codec created" << std::endl;
    };
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
    // Additional private methods and members as necessary

    MemcachedDecodeStatus handleState(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata);
    MemcachedDecodeStatus decodeHeader(Buffer::Instance& buffer);
    MemcachedDecodeStatus decodeBody(Buffer::Instance& buffer);
    MemcachedDecodeStatus decodeRequest(Buffer::Instance& buffer);
    MemcachedDecodeStatus decodeTextProtocol(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata);
    MemcachedDecodeStatus decodeTextRequest(char* chunk);
    MemcachedDecodeStatus decodeTextResponse(char* chunk);
    void toMetadata(MetaProtocolProxy::Metadata& metadata);
    std::string char_to_ascii(char* chunk, size_t len);
    std::string buffer_to_string(Buffer::Instance& buffer, size_t length);

    MemcachedDecodeStatus decode_status_{MemcachedDecodeStatus::DecodeHeader};
    MetaProtocolProxy::Memcached::MemcachedHeader memcached_header_;
    MetaProtocolProxy::MessageType message_type_;
    std::unique_ptr<Buffer::OwnedImpl> origin_msg_;

    // Additional private variables for TEXT protocol
    bool is_binary_protocol_{true};
    bool is_request_cmd_done_{false};
    size_t parsed_pos_{0}; // The position of the last byte parsed
};


} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
