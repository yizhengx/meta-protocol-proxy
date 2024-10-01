#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/buffer/buffer_impl.h"
#include "source/common/common/logger.h"
#include "src/meta_protocol_proxy/codec/codec.h"
#include "protocol.h"  // Include the Redis protocol header

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Redis {

enum class RedisDecodeStatus {
  DecodeHeader,
  DecodeBody,
  DecodeDone,
  WaitForData,
  DecodeTextProtocol,
};

class RedisCodec : public MetaProtocolProxy::Codec,
                       public Logger::Loggable<Logger::Id::misc> {
public:
    RedisCodec() {
        // ENVOY_LOG(warn, "Redis codec created");
        // std::cout << "[RedisCodec::RedisCodec()] Redis codec created" << std::endl;
    };
    ~RedisCodec() override = default;

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

    RedisDecodeStatus handleState(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata);
    RedisDecodeStatus decodeHeader(Buffer::Instance& buffer);
    RedisDecodeStatus decodeBody(Buffer::Instance& buffer);
    RedisDecodeStatus decodeRequest(Buffer::Instance& buffer);
    RedisDecodeStatus decodeTextProtocol(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata);
    RedisDecodeStatus decodeTextRequest(char* chunk);
    RedisDecodeStatus decodeTextResponse(char* chunk);
    void toMetadata(MetaProtocolProxy::Metadata& metadata);
    std::string char_to_ascii(char* chunk, size_t len);
    std::string buffer_to_string(Buffer::Instance& buffer, size_t length);

    RedisDecodeStatus decode_status_{RedisDecodeStatus::DecodeHeader};
    MetaProtocolProxy::Redis::RedisHeader redis_header_;
    MetaProtocolProxy::MessageType message_type_;
    std::unique_ptr<Buffer::OwnedImpl> origin_msg_;

    // Additional private variables for TEXT protocol
    bool is_request_cmd_done_{false};
    size_t parsed_pos_{0}; // The position of the last byte parsed
};


} // namespace Redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
