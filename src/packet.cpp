#include "packet.h"
#include "attribute_types.h"
#include <openssl/md5.h>
#include <iostream>
#include <stdexcept>

Packet::Packet(const std::array<uint8_t, 4096>& m_recvBuffer, size_t bytes, const std::string& secret)
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

    size_t attributeIndex = 20;
    while (attributeIndex < length)
    {
        const uint8_t attributeType = m_recvBuffer[attributeIndex];
        const uint8_t attributeLength = m_recvBuffer[attributeIndex + 1];

        m_attributes.push_back(makeAttribute(attributeType, &m_recvBuffer[attributeIndex + 2], attributeLength - 2, secret, m_auth));

        attributeIndex += attributeLength;
    }

    bool eapMessage = false;
    bool messageAuthenticator = false;
    for (size_t i = 0; i < m_attributes.size(); ++i)
    {
        if (m_attributes[i]->type() == EAP_MESSAGE)
            eapMessage = true;

        if (m_attributes[i]->type() == MESSAGE_AUTHENTICATOR)
            messageAuthenticator = true;
    }
    if (eapMessage && !messageAuthenticator)
        throw std::runtime_error{"The EAP-Message attribute is present, but the Message-Authenticator attribute is missing"};
}

Packet::Packet(uint8_t type, uint8_t id, const std::array<uint8_t, 16>& auth, const std::vector<Attribute*>& attributes)
    : m_type(type),
      m_id(id),
      m_auth(auth),
      m_attributes(attributes)
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
    std::vector<uint8_t> sendBuffer(20);

    sendBuffer[0] = m_type;
    sendBuffer[1] = m_id;

    for (size_t i = 0; i < m_auth.size(); ++i)
        sendBuffer[i + 4] = m_auth[i];
    for (const auto& attribute : m_attributes)
    {
        const auto aData = attribute->toVector(secret, m_auth);
        sendBuffer.insert(sendBuffer.end(), aData.begin(), aData.end());
    }

    sendBuffer[2] = sendBuffer.size() / 256 % 256;
    sendBuffer[3] = sendBuffer.size() % 256;

    sendBuffer.resize(sendBuffer.size() + secret.length());

    for (size_t i = 0; i < secret.length(); ++i)
        sendBuffer[i + sendBuffer.size() - secret.length()] = secret[i];

    std::array<uint8_t, 16> md;
    MD5(sendBuffer.data(), sendBuffer.size(), md.data());

    sendBuffer.resize(sendBuffer.size() - secret.length());

    for (size_t i = 0; i < md.size(); ++i)
        sendBuffer[i + 4] = md[i];

    return sendBuffer;
}

Attribute* Packet::makeAttribute(uint8_t type, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth)
{
    if (type == 1 || type == 11 || type == 18 || type == 22 || type == 34 || type == 35 ||
        type == 60 || type == 63)
        return new String(type, data, size);
    else if (type == 2)
        return new Encrypted(type, data, size, secret, auth);
    else if (type == 4 || type == 8 || type == 9 || type == 14)
        return new IpAddress(type, data, size);
    else if (type == 5 || type == 6 || type == 7 || type == 10 || type == 12 ||
             type == 13 || type == 15 || type == 16 || type == 27 || type == 28 ||
             type == 29 || type == 37 || type == 38 || type == 61 || type == 62)
        return new Integer(type, data, size);
    else if (type == 3 || type == 19 || type == 20 || type == 24 || type == 25 ||
             type == 26 || type == 30 || type == 31 || type == 32 || type == 33 ||
             type == 36 || type == 39 || type == 79 || type == 80)
        return new Bytes(type, data, size);
    throw std::runtime_error("Invalid attribute type");
}
