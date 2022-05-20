#include "packet.h"
#include <openssl/md5.h>
#include <iostream>
#include <stdexcept>

Packet::Packet(const std::array<uint8_t, 4096>& m_recvBuffer, size_t bytes)
{
    if (bytes < 20)
        throw std::runtime_error{"The number of received bytes in the request - " + std::to_string(bytes) + " is less than 20 bytes"};

    size_t length = m_recvBuffer[2] * 256 + m_recvBuffer[3];

        std::cout << "Length: " << length << "\n";



    if (bytes < length)
        throw std::runtime_error{"Request length " + std::to_string(bytes) + " is less than specified in the request - " + std::to_string(length)};

    m_type = m_recvBuffer[0];

    m_id = m_recvBuffer[1];

    for (std::size_t i = 0; i < m_auth.size(); ++i)
        m_auth[i] = m_recvBuffer[i + 4];

    size_t i = 20;
    while (i < length)
    {
        std::cout << "Index: " << i << "\n";
        const uint8_t attributeType = m_recvBuffer[i];
        std::cout << "Attribute type: " << std::to_string(attributeType) << "\n";
        const uint8_t attributeLength = m_recvBuffer[i + 1];
        std::cout << "Attribute length: " << std::to_string(attributeLength) << "\n";
        i += attributeLength;
    }
}

Packet::Packet(uint8_t type, uint8_t id, const std::array<uint8_t, 16>& auth)
    : m_type(type),
      m_id(id),
      m_auth(auth)
{
}

Packet::~Packet()
{
    for (const auto& ap : m_attributes)
        delete ap;
}

uint8_t Packet::type() const
{
    return m_type;
}

uint8_t Packet::id() const
{
    return m_id;
}

const std::array<uint8_t, 16>& Packet::auth() const
{
    return m_auth;
}

const std::vector<Attribute*>& Packet::attributes() const
{
    return m_attributes;
}

const std::vector<uint8_t> Packet::makeSendBuffer(const std::string& secret)
{
    std::vector<uint8_t> sendBuffer(20 + secret.length());

    sendBuffer[0] = m_type;

    sendBuffer[1] = m_id;

    sendBuffer[2] = 0;
    sendBuffer[3] = 20;

    for (size_t i = 0; i < m_auth.size(); ++i)
        sendBuffer[i + 4] = m_auth[i];

    for (size_t i = 0; i < secret.length(); ++i)
        sendBuffer[i + 20] = secret[i];

    std::array<uint8_t, 16> md;

    MD5(sendBuffer.data(), 20 + secret.length(), md.data());

    sendBuffer.resize(20);
    for (size_t i = 0; i < md.size(); ++i)
        sendBuffer[i + 4] = md[i];

    return sendBuffer;
}
