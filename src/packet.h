#ifndef PACKET_H
#define PACKET_H

#include <array>
#include <vector>
#include <string>
#include <cstdint> //uint8_t, uint32_t

class Packet
{
    public:
        Packet(const std::array<uint8_t, 4096>& m_recvBuffer, size_t bytes);
        Packet(uint8_t type, uint8_t id, const std::array<uint8_t, 16>& auth);
        uint8_t type() const;
        uint8_t id() const;
        const std::array<uint8_t, 16>& auth() const;
        const std::vector<uint8_t> makeSendBuffer(const std::string& secret);

    private:
        uint8_t m_type;
        uint8_t m_id;
        std::array<uint8_t, 16> m_auth;
};

#endif
