#ifndef PACKET_H
#define PACKET_H

#include <array>
#include <vector>
#include <cstdint> //uint8_t, uint32_t

class Packet
{
    public:
        Packet(const std::array<uint8_t, 4096>& m_recvBuffer);
        uint8_t type() const;
        uint8_t id() const;
        const std::array<uint8_t, 16>& auth() const;
        std::vector<uint8_t>& make_sendBuffer();

    private:
        uint8_t m_type;
        uint8_t m_id;
        std::array<uint8_t, 16> m_auth;
        std::vector<uint8_t> m_sendBuffer;
};

#endif
