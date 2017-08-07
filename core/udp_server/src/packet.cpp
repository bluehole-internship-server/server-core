//
// packet.cpp
//

#include <string.h>

#include "packet.hpp"

namespace core::udp {
Packet::Packet(Packet &&packet)
{
    std::swap(data_, packet.data_);
}

Packet::Packet(short size, short packet_type, const char* data)
    : data_(std::make_shared<Packet::data>())
{
    if (size > MAX_PACKET_SIZE) return;
    data_->size = size;
    data_->packet_type = packet_type;
    memcpy(data_->body, data, size);
}

Packet::Packet(const char* data)
    : data_(std::make_shared<Packet::data>())
{
    data_->size = *reinterpret_cast<const short*>(data);
    data_->packet_type = *reinterpret_cast<const short*>(data + sizeof(short));
    if (data_->size > MAX_PACKET_SIZE) return;
    memcpy(data_->body, data + 4, data_->size);
}

}