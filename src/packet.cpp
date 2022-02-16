#include "request.h"

Request::Request(const std::array<uint8_t, 4096>& m_recvBuffer)
{
    m_type = m_recvBuffer[0];

    m_id = m_recvBuffer[1];

    for (std::size_t i = 0; i < 16; ++i)
        m_auth[i] = m_recvBuffer[i + 4];
}

uint8_t Request::type() const
{
    return m_type;
}

uint8_t Request::id() const
{
    return m_id;
}

const std::array<uint8_t, 16>& Request::auth() const
{
    return m_auth;
}
