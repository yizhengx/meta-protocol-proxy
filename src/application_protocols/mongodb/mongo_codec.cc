#include "mongo_codec.h"
#include "envoy/common/exception.h"
#include "common/common/assert.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

struct MsgHeader {
    int32_t messageLength;
    int32_t requestID;
    int32_t responseTo;
    int32_t opCode;
};

    
    MetaProtocolProxy::DecodeStatus MongoCodec::decode(Buffer::Instance& buffer,
                                                   MetaProtocolProxy::Metadata& metadata) {
        const size_t MsgHeaderSize = sizeof(MsgHeader);

        if (buffer.length() < MsgHeaderSize) {
            return MetaProtocolProxy::DecodeStatus::WaitForData;
        }

        MsgHeader header;
        buffer.copyOut(0, MsgHeaderSize, &header);

        header.messageLength = ntohl(header.messageLength);
        header.requestID = ntohl(header.requestID);
        header.responseTo = ntohl(header.responseTo);
        header.opCode = ntohl(header.opCode);

        metadata.put("MessageLength", header.messageLength);
        metadata.put("RequestID", header.requestID);
        metadata.put("ResponseTo", header.responseTo);
        metadata.put("OpCode", header.opCode);

        // Check if there is enough data in the buffer to read the complete message
        if (buffer.length() < static_cast<size_t>(header.messageLength - sizeof(MsgHeader))) {
            // Insufficient data to parse the complete message, wait for more data
            return MetaProtocolProxy::DecodeStatus::WaitForData;
        }

        // Read the remaining message data from the buffer
        std::string messageData;
        messageData.resize(header.messageLength - sizeof(MsgHeader));
        buffer.copyOut(sizeof(MsgHeader), messageData.size(), &messageData[0]);
        buffer.drain(messageData.size());  // Remove the message data from the buffer

        // Parse the 'messageData' according to the MongoDB protocol
        switch (header.opCode) {
            case 2013:  // OP_MSG
                // Handle and parse OP_MSG message type
                // Populate metadata with relevant information from OP_MSG

                // Initialize variables to store parsed values.
                uint32_t flagBits;
                std::vector<std::string> sections;
                uint32_t checksum = 0; // Initialize the checksum.

                // Ensure there is enough data available for header and flagBits.
                if (buffer.length() < 8) {
                    throw std::runtime_error("Insufficient data for OP_MSG header and flagBits");
                }

                // Read the flag bits
                buffer.copyOut(sizeof(MsgHeader), sizeof(flagBits), &flagBits);
                flagBits = ntohl(flagBits); // Convert from little endian
                buffer.drain(sizeof(flagBits)); // Remove the flag bits from the buffer

                // Read sections until the end of the message
                while (buffer.length() > 4) { // Assuming the remaining 4 bytes are for checksum if present
                    std::string section;
                    // You need a function to read a section string from the buffer
                    // For example, buffer.readString(section);
                    // Add error handling for reading section
                    sections.push_back(section);
                }

                // Check if a checksum is present.
                if (flagBits & OP_MSG_CHECKSUM_PRESENT) {
                    if (buffer.length() < 4) {
                        throw std::runtime_error("Insufficient data for checksum in OP_MSG");
                    }
                    buffer.copyOut(buffer.length() - 4, sizeof(checksum), &checksum);
                    checksum = ntohl(checksum); // Convert from little endian
                    buffer.drain(sizeof(checksum)); // Remove the checksum from the buffer
                }
                metadata.put("OpCode", "OP_MSG");
                metadata.put("FlagBits", flagBits);

                // Example of adding each section to the metadata.
                // This is a simplified representation. You might want to parse each section further
                // based on your application's needs.
                for (size_t i = 0; i < sections.size(); ++i) {
                    metadata.putString("Section" + std::to_string(i), sections[i]);
                }

                if (flagBits & OP_MSG_CHECKSUM_PRESENT) {
                    metadata.put("Checksum", checksum);
                }
                break;
            case 2004:  // OP_QUERY
                // Handle and parse OP_QUERY message type
                // Populate metadata with relevant information from OP_QUERY
                break;
            default:
                // Handle other opCodes or unknown types
                break;
        }

        // Return appropriate DecodeStatus
        return MetaProtocolProxy::DecodeStatus::Done;
    }

    MetaProtocolProxy::DecodeStatus MongoCodec::decode(Buffer::Instance& buffer,
                                                   MetaProtocolProxy::Metadata& metadata) {
    // Check if there is enough data in the buffer to read the MongoDB header.
    if (buffer.length() < sizeof(MongoHeader)) {
        // Insufficient data to parse the header, wait for more data.
        return MetaProtocolProxy::DecodeStatus::WaitForData;
    }
    
    // Read the MongoDB header from the buffer.
    MongoHeader header;
    buffer.copyTo(&header, sizeof(MongoHeader));
    buffer.drain(sizeof(MongoHeader));  // Remove the header bytes from the buffer.
    
    // Extract information from the header.
    uint32_t messageLength = header.messageLength;
    int32_t opCode = header.opCode;
    
    // Check if there is enough data in the buffer to read the complete message.
    if (buffer.length() < static_cast<size_t>(messageLength - sizeof(uint32_t))) {
        // Insufficient data to parse the complete message, wait for more data.
        return MetaProtocolProxy::DecodeStatus::WaitForData;
    }
    
    // Read the remaining message data from the buffer.
    std::string buffer;
    buffer.resize(messageLength - sizeof(uint32_t));
    buffer.copyOut(buffer.data(), buffer.size());
    buffer.drain(buffer.size());  // Remove the message data from the buffer.
    
    // Now, you can parse the buffer according to the MongoDB protocol.
    // ...

    // Parse the 'buffer' according to the MongoDB protocol.
    // Example: You can check the 'opCode' to determine the type of MongoDB message.
    switch (opCode) {
        case 2013: {
            // Handle OP_MSG message type.
            // You can parse the buffer for OP_MSG here.
            // Example: Extract fields, flags, sections, and checksum.
            OP_MSG opMsg = parseOP_MSG(buffer);

            // Populate 'metadata' with relevant information from the OP_MSG.
            // Example: metadata.set("op_code", opCode);
            // ...

            // Return DecodeStatus indicating success or failure.
            return MetaProtocolProxy::DecodeStatus::Success;
        }
        case 2004: {
            // Handle OP_QUERY message type.
            // You can parse the buffer for OP_QUERY here.
            // Example: Extract fields like flags, collection name, query, etc.
            OP_QUERY opQuery = parseOP_QUERY(buffer);

            // Populate 'metadata' with relevant information from the OP_QUERY.
            // Example: metadata.set("op_code", opCode);
            // ...

            // Return DecodeStatus indicating success or failure.
            return MetaProtocolProxy::DecodeStatus::Success;
        }
        default:
            // Handle unsupported or unknown MongoDB message types.
            // You can log an error or return DecodeStatus::Failure as needed.
            return MetaProtocolProxy::DecodeStatus::Failure;
    }

// ...

    // Extract relevant information from the MongoDB message.
    
    // Populate the 'metadata' object with the parsed information.
    
    // Return DecodeStatus indicating success or failure.
    
    // Example:
    // if (decodeSuccess) {
    //     return MetaProtocolProxy::DecodeStatus::Success;
    // } else {
    //     return MetaProtocolProxy::DecodeStatus::Failure;
    // }
}

    // Function to parse MongoDB OP_MSG message
    OP_MSG parseOP_MSG(const Buffer::Instance& buffer) {
        // Initialize variables to store parsed values.
        uint32_t header;
        uint32_t flagBits;
        std::vector<std::string> sections;
        uint32_t checksum = 0; // Initialize the checksum.

        // Ensure there is enough data available for header and flagBits.
        if (buffer.length() < 8) {
            throw std::runtime_error("Insufficient data for OP_MSG header and flagBits");
        }

        if (!buffer.readLEInt(header)) {
            return false;  // Failed to read the header length
        }

        // Read the flag bits
        if (!buffer.readLEInt(flagBits)) {
            return false;  // Failed to read flag bits
        }

        // Read sections until the end of the message
        while (!buffer.empty()) {
            std::string section;
            if (!buffer.readString(section)) {
                return false;  // Failed to read section
            }
            sections_.push_back(section);
        }

        // Check if a checksum is present.
        if (flagBits & OP_MSG_CHECKSUM_PRESENT) {
            if (buffer.length() < 4) {
                throw std::runtime_error("Insufficient data for checksum in OP_MSG");
            }
            buffer.copyOut(4, &checksum);
        }

    }



    void MongoCodec::encode(const MetaProtocolProxy::Metadata& metadata,
                            const MetaProtocolProxy::Mutation& mutation, Buffer::Instance& buffer) {
        // TODO we don't need to implement encode for now.
        // This method only need to be implemented if we want to modify the respose message
        (void)metadata;
        (void)mutation;
        (void)buffer;
    }

    void MongoCodec::onError(const MetaProtocolProxy::Metadata& metadata,
                            const MetaProtocolProxy::Error& error, Buffer::Instance& buffer) {
        // Implement handling of errors and encoding error messages.
        // Write error messages to the buffer.
    }
} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
