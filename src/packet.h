#ifndef REQUEST_H
#define REQUEST_H

#include <array>
#include <cstdint> //uint8_t, uint32_t

class Request
{
    public:
        Request(const std::array<uint8_t, 4096>& m_recvBuffer);
        uint8_t type() const;
        uint8_t id() const;
        const std::array<uint8_t, 16>& auth() const;

    private:
        uint8_t m_type;
        uint8_t m_id;
        std::array<uint8_t, 16> m_auth;
};

#endif
