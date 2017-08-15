//
// packet.hpp
//

#pragma once

// TODO
#define MAX_PACKET_SIZE 65000

#include <memory>

namespace core::udp {
class Packet {
private:

#pragma pack(push, 1)
    struct data {
        short size;
        short packet_type;
        char body[MAX_PACKET_SIZE];
    };
#pragma pack(pop)

public:
    explicit Packet(Packet &packet);
    explicit Packet(Packet &&packet);
    explicit Packet(short size, short packet_type, const char* data);
    explicit Packet(short size, short packet_type);
    explicit Packet(const char* data);

    void SetSize(short size) { data_->size = size; }
    void SetType(short type) { data_->packet_type = type; }
    
    short Size() { return data_->size; }
    short PacketType() { return data_->packet_type; }
    const char* Data() { return data_->body; }

private:
    friend class Socket;
    std::shared_ptr<data> data_;
};
}
