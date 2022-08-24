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

    size_t attributeIndex = 20;
    while (attributeIndex < length)
    {
        const uint8_t attributeType = m_recvBuffer[attributeIndex];
        const uint8_t attributeLength = m_recvBuffer[attributeIndex + 1];

        try
        {
            m_attributes.push_back(makeAttribute(attributeType, &m_recvBuffer[attributeIndex + 2], attributeLength - 2, "secret", m_auth));
        }
        catch (const std::runtime_error& exception)
        {
            std::cout << "Attribute error: " << exception.what() << "\n";
        }

        attributeIndex += attributeLength;
    }

    bool eapMessage = false;
    bool messageAuthenticator = false;
    for (size_t i = 0; i < m_attributes.size(); ++i)
    {
        if (m_attributes[i]->type() == 79)
            eapMessage = true;

        if (m_attributes[i]->type() == 80)
            messageAuthenticator = true;
    }
    try
    {
        if (eapMessage && !messageAuthenticator)
            throw std::runtime_error{"The EAP-Message attribute is present, but the Message-Authenticator attribute is missing"};
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << "Attribute error: " << exception.what() << "\n";
    }
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
    std::vector<uint8_t> sendBuffer(20 + secret.length());

    sendBuffer[0] = m_type;

    sendBuffer[1] = m_id;

    for (size_t i = 0; i < m_auth.size(); ++i)
        sendBuffer[i + 4] = m_auth[i];

    std::vector<uint8_t> attributes;
    for (size_t i = 0; i < m_attributes.size(); ++i)
    {
        std::vector<uint8_t> attribute(m_attributes[i]->toVector(secret, m_auth));
        attributes.insert(attributes.end(), attribute.begin(), attribute.end());
    }
    sendBuffer.resize(20 + attributes.size());

    sendBuffer[2] = 0;
    sendBuffer[3] = 20 + attributes.size();;

    for (size_t i = 0; i < attributes.size(); ++i)
        sendBuffer[i + 20] = attributes[i];

    for (size_t i = 0; i < secret.length(); ++i)
        sendBuffer[i + 20 + attributes.size()] = secret[i];

    std::array<uint8_t, 16> md;

    MD5(sendBuffer.data(), sendBuffer.size() + secret.length(), md.data());

    sendBuffer.resize(20 + attributes.size());
    for (size_t i = 0; i < md.size(); ++i)
        sendBuffer[i + 4] = md[i];

    return sendBuffer;
}

Attribute* Packet::makeAttribute(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize, std::string secret, std::array<uint8_t, 16> auth)
{
    if (type == 1 || type == 11 || type == 18 || type == 22 || type == 34 || type == 35 ||
        type == 60 || type == 63)
        return new String(type, attributeValue, attributeValueSize);
    else if (type == 2)
        return new Encrypted(type, attributeValue, attributeValueSize, secret, auth);
    else if (type == 4 || type == 8 || type == 9 || type == 14)
        return new IpAddress(type, attributeValue, attributeValueSize);
    else if (type == 5 || type == 6 || type == 7 || type == 10 || type == 12 ||
             type == 13 || type == 15 || type == 16 || type == 27 || type == 28 ||
             type == 29 || type == 37 || type == 38 || type == 61 || type == 62)
        return new Integer(type, attributeValue, attributeValueSize);
    else if (type == 3 || type == 19 || type == 20 || type == 24 || type == 25 ||
             type == 26 || type == 30 || type == 31 || type == 32 || type == 33 ||
             type == 36 || type == 39 || type == 79 || type == 80)
        return new Bytes(type, attributeValue, attributeValueSize);
    throw std::runtime_error("Invalid attribute type");
}
