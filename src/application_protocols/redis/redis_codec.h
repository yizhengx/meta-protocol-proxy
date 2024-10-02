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
  WaitForData,
  DecodeMsg,
  DecodeDone,
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
    void toMetadata(MetaProtocolProxy::Metadata& metadata);
    RedisDecodeStatus decodeMsg(Buffer::Instance& buffer);
    std::string buffer_to_string(Buffer::Instance& buffer, size_t length);
    MetaProtocolProxy::MessageType message_type_;
    std::unique_ptr<Buffer::OwnedImpl> origin_msg_;

    DecodeStatus decode_status_ = RedisDecodeStatus::DecodeMsg;
    bool is_start_pos_op = true; // start reading position of the buffer
    size_t start_pos = 0;   // start reading position of the buffer
    size_t item_needed = 0; // how many items needed to complete the current msg
    size_t crlf_needed = 0; // how many crlf needed to complete the current item
};


} // namespace Redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
