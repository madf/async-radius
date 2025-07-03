#include "packet.h"
#include "error.h"
#include "attribute_types.h"
#include <openssl/evp.h>
#include <stdexcept>

using Packet = RadProto::Packet;

namespace
{
    RadProto::Attribute* makeAttribute(uint8_t type, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth)
    {
        if (type == 1 || type == 11 || type == 18 || type == 22 || type == 34 || type == 35 || type == 60 || type == 63)
            return new RadProto::String(type, data, size);
        else if (type == 2)
            return new RadProto::Encrypted(type, data, size, secret, auth);
        else if (type == 3)
            return new RadProto::ChapPassword(type, data, size);
        else if (type == 4 || type == 8 || type == 9 || type == 14)
            return new RadProto::IpAddress(type, data, size);
        else if (type == 5 || type == 6 || type == 7 || type == 10 || type == 12 || type == 13 || type == 15 || type == 16 || type == 27 || type == 28 || type == 29 || type == 37 || type == 38 || type == 61 || type == 62)
            return new RadProto::Integer(type, data, size);
        else if (type == 19 || type == 20 || type == 24 || type == 25 || type == 30 || type == 31 || type == 32 || type == 33 || type == 36 || type == 39 || type == 79 || type == 80)
            return new RadProto::Bytes(type, data, size);

        throw RadProto::Exception(RadProto::Error::invalidAttributeType);
    }
}

Packet::Packet(const uint8_t* buffer, size_t size, const std::string& secret)
    : m_recalcAuth(false)
{
    if (size < 20)
        throw Exception(Error::numberOfBytesIsLessThan20);

    size_t length = buffer[2] * 256 + buffer[3];

    if (size < length)
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
            m_vendorSpecific.push_back(VendorSpecific(&buffer[attributeIndex + 2]));
        else
            m_attributes.push_back(makeAttribute(attributeType, &buffer[attributeIndex + 2], attributeLength - 2, secret, m_auth));

        attributeIndex += attributeLength;
    }

    bool eapMessage = false;
    bool messageAuthenticator = false;
    for (const auto& a : m_attributes)
    {
        if (a->type() == EAP_MESSAGE)
            eapMessage = true;

        if (a->type() == MESSAGE_AUTHENTICATOR)
            messageAuthenticator = true;
    }
    if (eapMessage && !messageAuthenticator)
        throw Exception(Error::eapMessageAttributeError);
}

Packet::Packet(uint8_t type, uint8_t id, const std::array<uint8_t, 16>& auth, const std::vector<Attribute*>& attributes, const std::vector<VendorSpecific>& vendorSpecific)
    : m_type(type),
      m_id(id),
      m_recalcAuth(m_type == 2 || m_type == 3 || m_type == 11),
      m_auth(auth),
      m_attributes(attributes),
      m_vendorSpecific(vendorSpecific)
{
}

Packet::Packet(const Packet& other)
    : m_type(other.m_type),
      m_id(other.m_id),
      m_recalcAuth(other.m_recalcAuth),
      m_auth(other.m_auth),
      m_vendorSpecific(other.m_vendorSpecific)

{
    for (const auto& a :  other.m_attributes)
        if (a)
            m_attributes.push_back(a->clone());
}

Packet::~Packet()
{
    for (const auto& ap : m_attributes)
        delete ap;
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
        const auto aData = vendorAttribute.toVector();
        sendBuffer.insert(sendBuffer.end(), aData.begin(), aData.end());
    }
    sendBuffer[2] = sendBuffer.size() / 256 % 256;
    sendBuffer[3] = sendBuffer.size() % 256;

    if (m_recalcAuth == true)
    {
        sendBuffer.resize(sendBuffer.size() + secret.length());

        for (size_t i = 0; i < secret.length(); ++i)
            sendBuffer[i + sendBuffer.size() - secret.length()] = secret[i];

        std::array<uint8_t, 16> md;
        unsigned mdSize = md.size();

        EVP_Digest(sendBuffer.data(), sendBuffer.size(), md.data(), &mdSize, EVP_md5(), nullptr);

        sendBuffer.resize(sendBuffer.size() - secret.length());

        for (size_t i = 0; i < md.size(); ++i)
            sendBuffer[i + 4] = md[i];
    }
    return sendBuffer;
}
