#include "packet.h"
#include "error.h"
#include "attribute_types.h"
#include <openssl/md5.h>
#include <stdexcept>

using Packet = RadProto::Packet;
Packet::Packet(const std::array<uint8_t, 4096>& buffer, size_t bytes, const std::string& secret)
{
    if (bytes < 20)
        throw Exception(Error::numberOfBytesIsLessThan20);

    size_t length = buffer[2] * 256 + buffer[3];

    if (bytes < length)
        throw Exception(Error::requestLengthIsShort);

    m_type = buffer[0];

    m_id = buffer[1];

    for (std::size_t i = 0; i < m_auth.size(); ++i)
        m_auth[i] = buffer[i + 4];

    size_t attributeIndex = 20;
    while (attributeIndex < length)
    {
        const uint8_t attributeType = buffer[attributeIndex];
        const uint8_t attributeLength = buffer[attributeIndex + 1];

        if (attributeType == VENDOR_SPECIFIC)
            m_vendorSpecific.push_back(new VendorSpecific(&buffer[attributeIndex + 2]));
        else
            m_attributes.push_back(makeAttribute(attributeType, &buffer[attributeIndex + 2], attributeLength - 2, secret, m_auth));

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
        throw Exception(Error::eapMessageAttributeError);
}

Packet::Packet(uint8_t type, uint8_t id, const std::array<uint8_t, 16>& auth, const std::vector<Attribute*>& attributes, const std::vector<VendorSpecific*>& vendorSpecific)
    : m_type(type),
      m_id(id),
      m_auth(auth),
      m_attributes(attributes),
      m_vendorSpecific(vendorSpecific)
{
}

Packet::Packet(const Packet& other)
{
    for(size_t i = 0; i < other.m_vendorSpecific.size(); ++i)
        if (other.m_vendorSpecific[i])
            m_vendorSpecific.push_back(new VendorSpecific(*other.m_vendorSpecific[i]));

    for(size_t i = 0; i < other.m_attributes.size(); ++i)
        if (other.m_attributes[i])
            m_attributes.push_back(other.m_attributes[i]->clone());
}

Packet::~Packet()
{
    for (const auto& ap : m_attributes)
        delete ap;

    for (const auto& vp : m_vendorSpecific)
        delete vp;
}

const std::vector<uint8_t> Packet::makeSendBuffer(const std::string& secret) const
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
    for (const auto& vendorAttribute : m_vendorSpecific)
    {
        const auto aData = vendorAttribute->toVector();
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

RadProto::Attribute* Packet::makeAttribute(uint8_t type, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth)
{
    if (type == 1 || type == 11 || type == 18 || type == 22 || type == 34 || type == 35 ||
        type == 60 || type == 63)
        return new String(type, data, size);
    else if (type == 2)
        return new Encrypted(type, data, size, secret, auth);
    else if (type == 3)
        return new ChapPassword(type, data, size);
    else if (type == 4 || type == 8 || type == 9 || type == 14)
        return new IpAddress(type, data, size);
    else if (type == 5 || type == 6 || type == 7 || type == 10 || type == 12 ||
             type == 13 || type == 15 || type == 16 || type == 27 || type == 28 ||
             type == 29 || type == 37 || type == 38 || type == 61 || type == 62)
        return new Integer(type, data, size);
    else if (type == 19 || type == 20 || type == 24 || type == 25 ||
             type == 30 || type == 31 || type == 32 || type == 33 ||
             type == 36 || type == 39 || type == 79 || type == 80)
        return new Bytes(type, data, size);

    throw std::runtime_error("Invalid attribute type " + std::to_string(type));
}
