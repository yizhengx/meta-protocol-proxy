#include "mongodb_codec.h"
#include "envoy/common/exception.h"
#include "source/common/common/assert.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/types.hpp>
#include <iostream>

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {



// MetaProtocolProxy::DecodeStatus MongoDBCodec::decode(Buffer::Instance& buffer,
//                                                 MetaProtocolProxy::Metadata& metadata) {
//     const size_t MsgHeaderSize = sizeof(MsgHeader);

//     if (buffer.length() < MsgHeaderSize) {
//         return MetaProtocolProxy::DecodeStatus::WaitForData;
//     }

//     MsgHeader header;
//     buffer.copyOut(0, MsgHeaderSize, &header);

//     header.messageLength = ntohl(header.messageLength);
//     header.requestID = ntohl(header.requestID);
//     header.responseTo = ntohl(header.responseTo);
//     header.opCode = ntohl(header.opCode);

//     metadata.put("MessageLength", header.messageLength);
//     metadata.put("RequestID", header.requestID);
//     metadata.put("ResponseTo", header.responseTo);
//     metadata.put("OpCode", header.opCode);

//     // Check if there is enough data in the buffer to read the complete message
//     if (buffer.length() < static_cast<size_t>(header.messageLength - sizeof(MsgHeader))) {
//         // Insufficient data to parse the complete message, wait for more data
//         return MetaProtocolProxy::DecodeStatus::WaitForData;
//     }

//     // Read the remaining message data from the buffer
//     std::string messageData;
//     messageData.resize(header.messageLength - sizeof(MsgHeader));
//     buffer.copyOut(sizeof(MsgHeader), messageData.size(), &messageData[0]);
//     buffer.drain(messageData.size());  // Remove the message data from the buffer

//     // Parse the 'messageData' according to the MongoDB protocol
//     switch (header.opCode) {
//         case 2013:  // OP_MSG
//             // Handle and parse OP_MSG message type
//             // Populate metadata with relevant information from OP_MSG

//             // Initialize variables to store parsed values.
//             uint32_t flagBits;
//             std::vector<std::string> sections;
//             uint32_t checksum = 0; // Initialize the checksum.

//             // Ensure there is enough data available for header and flagBits.
//             if (buffer.length() < 8) {
//                 throw std::runtime_error("Insufficient data for OP_MSG header and flagBits");
//             }

//             // Read the flag bits
//             buffer.copyOut(sizeof(MsgHeader), sizeof(flagBits), &flagBits);
//             flagBits = ntohl(flagBits); // Convert from little endian
//             buffer.drain(sizeof(flagBits)); // Remove the flag bits from the buffer

//             // Read sections until the end of the message
//             while (buffer.length() > 4) { // Assuming the remaining 4 bytes are for checksum if present
//                 std::string section;
//                 // You need a function to read a section string from the buffer
//                 // For example, buffer.readString(section);
//                 // Add error handling for reading section
//                 sections.push_back(section);
//             }

//             // Check if a checksum is present.
//             if (flagBits & OP_MSG_CHECKSUM_PRESENT) {
//                 if (buffer.length() < 4) {
//                     throw std::runtime_error("Insufficient data for checksum in OP_MSG");
//                 }
//                 buffer.copyOut(buffer.length() - 4, sizeof(checksum), &checksum);
//                 checksum = ntohl(checksum); // Convert from little endian
//                 buffer.drain(sizeof(checksum)); // Remove the checksum from the buffer
//             }
//             metadata.put("OpCode", "OP_MSG");
//             metadata.put("FlagBits", flagBits);

//             // Example of adding each section to the metadata.
//             // This is a simplified representation. You might want to parse each section further
//             // based on your application's needs.
//             for (size_t i = 0; i < sections.size(); ++i) {
//                 metadata.putString("Section" + std::to_string(i), sections[i]);
//             }

//             if (flagBits & OP_MSG_CHECKSUM_PRESENT) {
//                 metadata.put("Checksum", checksum);
//             }
//             break;
//         case 2004:  // OP_QUERY
//             // Handle and parse OP_QUERY message type
//             // Populate metadata with relevant information from OP_QUERY
//             break;
//         default:
//             // Handle other opCodes or unknown types
//             break;
//     }

//     // Return appropriate DecodeStatus
//     return MetaProtocolProxy::DecodeStatus::Done;
// }

// MetaProtocolProxy::DecodeStatus MongoDBCodec::decode(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {
//     // Check if there is enough data in the buffer to read the MongoDB header.
//     if (buffer.length() < sizeof(MongoHeader)) {
//         // Insufficient data to parse the header, wait for more data.
//         return MetaProtocolProxy::DecodeStatus::WaitForData;
//     }
    
//     // Read the MongoDB header from the buffer.
//     MongoHeader header;
//     buffer.copyTo(&header, sizeof(MongoHeader));
//     buffer.drain(sizeof(MongoHeader));  // Remove the header bytes from the buffer.
    
//     // Extract information from the header.
//     uint32_t messageLength = header.messageLength;
//     int32_t opCode = header.opCode;
    
//     // Check if there is enough data in the buffer to read the complete message.
//     if (buffer.length() < static_cast<size_t>(messageLength - sizeof(uint32_t))) {
//         // Insufficient data to parse the complete message, wait for more data.
//         return MetaProtocolProxy::DecodeStatus::WaitForData;
//     }
    
//     // Read the remaining message data from the buffer.
//     std::string buffer;
//     buffer.resize(messageLength - sizeof(uint32_t));
//     buffer.copyOut(buffer.data(), buffer.size());
//     buffer.drain(buffer.size());  // Remove the message data from the buffer.
    
//     // Now, you can parse the buffer according to the MongoDB protocol.
//     // ...

//     // Parse the 'buffer' according to the MongoDB protocol.
//     // Example: You can check the 'opCode' to determine the type of MongoDB message.
//     switch (opCode) {
//         case 2013: {
//             // Handle OP_MSG message type.
//             // You can parse the buffer for OP_MSG here.
//             // Example: Extract fields, flags, sections, and checksum.
//             OP_MSG opMsg = parseOP_MSG(buffer);

//             // Populate 'metadata' with relevant information from the OP_MSG.
//             // Example: metadata.set("op_code", opCode);
//             // ...

//             // Return DecodeStatus indicating success or failure.
//             return MetaProtocolProxy::DecodeStatus::Success;
//         }
//         case 2004: {
//             // Handle OP_QUERY message type.
//             // You can parse the buffer for OP_QUERY here.
//             // Example: Extract fields like flags, collection name, query, etc.
//             OP_QUERY opQuery = parseOP_QUERY(buffer);

//             // Populate 'metadata' with relevant information from the OP_QUERY.
//             // Example: metadata.set("op_code", opCode);
//             // ...

//             // Return DecodeStatus indicating success or failure.
//             return MetaProtocolProxy::DecodeStatus::Success;
//         }
//         default:
//             // Handle unsupported or unknown MongoDB message types.
//             // You can log an error or return DecodeStatus::Failure as needed.
//             return MetaProtocolProxy::DecodeStatus::Failure;
//     }

//     // ...

//     // Extract relevant information from the MongoDB message.
    
//     // Populate the 'metadata' object with the parsed information.
    
//     // Return DecodeStatus indicating success or failure.
    
//     // Example:
//     // if (decodeSuccess) {
//     //     return MetaProtocolProxy::DecodeStatus::Success;
//     // } else {
//     //     return MetaProtocolProxy::DecodeStatus::Failure;
//     // }
// }

// Function to parse MongoDB OP_MSG message
// OP_MSG parseOP_MSG(const Buffer::Instance& buffer) {
//     // Initialize variables to store parsed values.
//     uint32_t header;
//     uint32_t flagBits;
//     std::vector<std::string> sections;
//     uint32_t checksum = 0; // Initialize the checksum.

//     // Ensure there is enough data available for header and flagBits.
//     if (buffer.length() < 8) {
//         throw std::runtime_error("Insufficient data for OP_MSG header and flagBits");
//     }

//     if (!buffer.readLEInt(header)) {
//         return false;  // Failed to read the header length
//     }

//     // Read the flag bits
//     if (!buffer.readLEInt(flagBits)) {
//         return false;  // Failed to read flag bits
//     }

//     // Read sections until the end of the message
//     while (!buffer.empty()) {
//         std::string section;
//         if (!buffer.readString(section)) {
//             return false;  // Failed to read section
//         }
//         sections_.push_back(section);
//     }

//     // Check if a checksum is present.
//     if (flagBits & OP_MSG_CHECKSUM_PRESENT) {
//         if (buffer.length() < 4) {
//             throw std::runtime_error("Insufficient data for checksum in OP_MSG");
//         }
//         buffer.copyOut(4, &checksum);
//     }

// }

MetaProtocolProxy::DecodeStatus MongoDBCodec::decode(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {

    message_type_ = metadata.getMessageType();
    std::string message_type_str = message_type_ == MetaProtocolProxy::MessageType::Request ? "Request" : "Response";

    // std::cout << "[MongoDBCodec::decode()] MongoDB start decoder: " << buffer.length() << " bytes available, msg type: " << message_type_str << std::endl;

    while (decode_status_ != MongoDBDecodeStatus::DecodeDone) {
        decode_status_ = handleState(buffer, metadata);
        if (decode_status_ == MongoDBDecodeStatus::WaitForData) {
            return MetaProtocolProxy::DecodeStatus::WaitForData;
        }
    }

    toMetadata(metadata);
    decode_status_ = MongoDBDecodeStatus::DecodeHeader;
    return MetaProtocolProxy::DecodeStatus::Done;
}

MongoDBDecodeStatus MongoDBCodec::handleState(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {
    switch (decode_status_) {
    case MongoDBDecodeStatus::DecodeHeader:
        return decodeHeader(buffer, metadata);
    case MongoDBDecodeStatus::DecodeBody:
        return decodeBody(buffer, metadata);
    case MongoDBDecodeStatus::WaitForData:
        return MongoDBDecodeStatus::WaitForData;
    default:
        PANIC("not reached");
    }
    return MongoDBDecodeStatus::DecodeDone;
}

MongoDBDecodeStatus MongoDBCodec::decodeHeader(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {
    // std::cout << "MongoDB decodeHeader: " << buffer.length() << " bytes available" << std::endl;
    // Wait for more data if the header is not complete
    // std::cout << "MongoDB decodeHeader: " << buffer_to_string(buffer, buffer.length()) << std::endl;
    if (buffer.length() < sizeof(MsgHeader)) {
        // ENVOY_LOG(debug, "continue {}", buffer.length());
        // std::cout << "MongoDB decodeHeader: waiting for more data" << std::endl;
        return MongoDBDecodeStatus::WaitForData;
    }

    if (!mongo_header_.decode(buffer)) {
        throw EnvoyException(fmt::format("MongoDB header invalid"));
    }

    if (mongo_header_.header_.responseTo == 0) {
        // metadata.setMessageType(MetaProtocolProxy::MessageType::Request);
        metadata.setRequestId(mongo_header_.header_.requestID);
    } else {
        // metadata.setMessageType(MetaProtocolProxy::MessageType::Response);
        metadata.setRequestId(mongo_header_.header_.responseTo);
    }

    message_type_ = metadata.getMessageType();
        std::string message_type_str = message_type_ == MetaProtocolProxy::MessageType::Request ? "Request" : "Response";
    std::string message_prefix = message_type_ == MetaProtocolProxy::MessageType::Request ? ">>>>>> " : "<<<<<< ";
    std::cout << message_prefix + " MongoDB decodeHeader " + message_type_str +" done: " << message_type_str << " RequestID: " << metadata.getRequestId() << " | messageLength: " << mongo_header_.header_.messageLength << " | requestID: " << mongo_header_.header_.requestID << " | responseTo: " << mongo_header_.header_.responseTo << " | opCode: " << mongo_header_.header_.opCode << std::endl;

    // std::cout << "MongoDB decodeHeader: message length: " << mongo_header_.getMessageLength() << std::endl;

    return MongoDBDecodeStatus::DecodeBody;
}

MongoDBDecodeStatus MongoDBCodec::decodeBody(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata) {
    // Wait for more data if the buffer is not a complete message
    // std::cout << "MongoDB decodeBody: " << buffer_to_string(buffer, buffer.length()) << std::endl;
    if (buffer.length() < static_cast<uint64_t>(mongo_header_.getMessageLength())) {
        // std::cout << "MongoDB decodeBody: waiting for more data" << std::endl;
        return MongoDBDecodeStatus::WaitForData;
    }
    message_type_ = metadata.getMessageType();
        std::string message_type_str = message_type_ == MetaProtocolProxy::MessageType::Request ? "Request" : "Response";
    // std::cout << "MongoDB decodeMsg " + message_type_str +" done: " << buffer_to_string(buffer, mongo_header_.getMessageLength()) << std::endl;
    // print the message type, metadata requestID, and actual message
    std::string message_prefix = message_type_ == MetaProtocolProxy::MessageType::Request ? ">>>>>> " : "<<<<<< ";
    std::cout << message_prefix + " MongoDB decodeMsg " + message_type_str +" done: " << message_type_str << " RequestID: " << metadata.getRequestId() << " Message: " << buffer_to_string(buffer, mongo_header_.getMessageLength()) << std::endl;

    // inpect the buffer to see if "ismaster
    if (seen_is_master == false && buffer_to_string(buffer, mongo_header_.getMessageLength()).find("ismaster") != std::string::npos) {
        seen_is_master = true;
    }

    if (buffer_to_string(buffer, mongo_header_.getMessageLength()).find("The client metadata document may only be sent in the first isMaster") != std::string::npos) {
        std::cout << "[MongoDBCodec::decodeBody()] - The client metadata document may only be sent in the first isMaster && Seen: " << seen_is_master << std::endl;
    }
    // move the decoded message out of the buffer
    origin_msg_ = std::make_unique<Buffer::OwnedImpl>();
    origin_msg_->move(buffer, mongo_header_.getMessageLength());

    return MongoDBDecodeStatus::DecodeDone;
}

void parse_bytes_to_bson(const uint8_t* bytes, size_t length) {
    try {
        // Convert the byte array to a BSON document
        bsoncxx::document::value doc_value = bsoncxx::document::value::read_from(bytes, length);
        
        // View the BSON document
        bsoncxx::document::view doc_view = doc_value.view();

        // Convert to JSON for output
        std::cout << "Parsed BSON as JSON: " << bsoncxx::to_json(doc_view) << std::endl;
    } catch (const bsoncxx::exception& e) {
        std::cerr << "Error parsing BSON: " << e.what() << std::endl;
    }
}
}

std::string MongoDBCodec::buffer_to_string(Buffer::Instance& buffer, size_t length) {
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

void MongoDBCodec::toMetadata(MetaProtocolProxy::Metadata& metadata) {
    metadata.originMessage().move(*origin_msg_);
}

void MongoDBCodec::encode(const MetaProtocolProxy::Metadata& metadata,
                        const MetaProtocolProxy::Mutation& mutation, Buffer::Instance& buffer) {
    // TODO we don't need to implement encode for now.
    // This method only need to be implemented if we want to modify the respose message
    (void)metadata;
    (void)mutation;
    (void)buffer;
}

void MongoDBCodec::onError(const MetaProtocolProxy::Metadata&,
                        const MetaProtocolProxy::Error&, Buffer::Instance&) {
    // Implement handling of errors and encoding error messages.
    // Write error messages to the buffer.
}

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
