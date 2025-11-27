#include "packet.h"
#include "error.h"
#include "attribute_codes.h"
#include <openssl/evp.h>
#include <stdexcept>

using Packet = RadProto::Packet;

namespace
{
    RadProto::Attribute* makeAttribute(uint8_t code, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth)
    {
        if (code == 1 || code == 11 || code == 18 || code == 22 || code == 34 || code == 35 || code == 60 || code == 63)
            return new RadProto::String(code, data, size);
        else if (code == 2)
            return new RadProto::Encrypted(code, data, size, secret, auth);
        else if (code == 3)
            return new RadProto::ChapPassword(code, data, size);
        else if (code == 4 || code == 8 || code == 9 || code == 14)
            return new RadProto::IpAddress(code, data, size);
        else if (code == 5 || code == 6 || code == 7 || code == 10 || code == 12 || code == 13 || code == 15 || code == 16 || code == 27 || code == 28 || code == 29 || code == 37 || code == 38 || code == 61 || code == 62)
            return new RadProto::Integer(code, data, size);
        else if (code == 19 || code == 20 || code == 24 || code == 25 || code == 30 || code == 31 || code == 32 || code == 33 || code == 36 || code == 39 || code == 79 || code == 80)
            return new RadProto::Bytes(code, data, size);

        throw RadProto::Exception(RadProto::Error::invalidAttributeCode);
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

    m_code = buffer[0];

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
        if (a->code() == EAP_MESSAGE)
            eapMessage = true;

        if (a->code() == MESSAGE_AUTHENTICATOR)
            messageAuthenticator = true;
    }
    if (eapMessage && !messageAuthenticator)
        throw Exception(Error::eapMessageAttributeError);
}

Packet::Packet(uint8_t code, uint8_t id, const std::array<uint8_t, 16>& auth, const std::vector<Attribute*>& attributes, const std::vector<VendorSpecific>& vendorSpecific)
    : m_code(code),
      m_id(id),
      m_recalcAuth(m_code == 2 || m_code == 3 || m_code == 11),
      m_auth(auth),
      m_attributes(attributes),
      m_vendorSpecific(vendorSpecific)
{
}

Packet::Packet(const Packet& other)
    : m_code(other.m_code),
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

    sendBuffer[0] = m_code;
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
