#include "packet.h"
#include "attribute.h"
#include "utils.h"
#include "error.h"
#include <boost/tokenizer.hpp>
#include <openssl/evp.h>
#include <algorithm>

using Attribute = RadProto::Attribute;
namespace
{
    enum class ValueType : uint8_t
    {
        String,
        Integer,
        IpAddress,
        Encrypted,
        Bytes,
        VendorSpecific,
        ChapPassword
    };

    ValueType parseValueType(const std::string& type)
    {
        if (type == "string")
            return ValueType::String;
        else if (type == "integer" || type == "date")
            return ValueType::Integer;
        else if (type == "ipaddr")
            return ValueType::IpAddress;
        else if (type == "encrypted")
            return ValueType::Encrypted;
        else if (type == "octets")
            return ValueType::Bytes;
        else if (type == "vsa")
           throw RadProto::Exception(RadProto::Error::typeIsNotSupported);
        else
           throw RadProto::Exception(RadProto::Error::invalidValueType);
    }
}

Attribute::Attribute(uint8_t code)
    : m_code(code)
{
}

Attribute* Attribute::make(uint8_t code, const std::string& type, const std::string& data)
{
    ValueType valueType = parseValueType(type);

    switch (valueType)
    {
        case ValueType::String:
            return new String(code, data);
        case ValueType::Integer:
            return new Integer(code, std::stoul(data));
        case ValueType::Encrypted:
            return new Encrypted(code, data);
        case ValueType::IpAddress:
        {
            using tokenizer =  boost::tokenizer<boost::char_separator<char>>;
            boost::char_separator<char> sep(".");
            tokenizer tok(data, sep);

            std::array<uint8_t, 4> ipAddr;
            size_t i = 0;
            for (const auto& t : tok)
            {
                ipAddr[i] = static_cast<uint8_t>(std::stoul(t));
                ++i;
            }
            return new IpAddress(code, ipAddr);
        }
        case ValueType::Bytes:
        {
            if (data.length() % 2 != 0)
                throw RadProto::Exception(RadProto::Error::invalidHexStringLength);

            std::vector<uint8_t> bytes;

            for (size_t i = 0; i < data.length(); i += 2)
            {
                auto byte = static_cast<uint8_t>(std::stoi(data.substr(i, 2), nullptr, 16));
                bytes.push_back(byte);
            }
            return new Bytes(code, bytes);
        }
        default:
            throw RadProto::Exception(RadProto::Error::invalidValueType);
    }
}

using String = RadProto::String;
String::String(uint8_t code, const uint8_t* data, size_t size)
        : Attribute(code),
          m_value(reinterpret_cast<const char*>(data), size)
{
}

String::String(uint8_t code, const std::string& string)
    : Attribute(code),
      m_value(string)
{
}

std::vector<uint8_t> String::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(m_value.length() + 2);
    std::copy(m_value.begin(), m_value.end(), std::next(attribute.begin(), 2));
    attribute[0] = code();
    attribute[1] = m_value.length() + 2;
    return attribute;
}

String* String::clone() const
{
    return new String(*this);
}

using Integer = RadProto::Integer;
Integer::Integer(uint8_t code, const uint8_t* data, size_t size)
        : Attribute(code),
          m_value(0)
{
    if (size != 4)
        throw RadProto::Exception(RadProto::Error::invalidAttributeSize);

    m_value = data[0] * (1 << 24)
            + data[1] * (1 << 16)
            + data[2] * (1 << 8)
            + data[3];
}

Integer::Integer(uint8_t code, uint32_t value)
    : Attribute(code),
      m_value(value)
{
}

std::string Integer::toString() const
{
    return std::to_string(m_value);
}

std::vector<uint8_t> Integer::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(6);
    attribute[0] = code();
    attribute[1] = 6;
    attribute[2] = m_value / (1 << 24);
    attribute[3] = (m_value / (1 << 16)) % 256;
    attribute[4] = (m_value / (1 << 8)) % 256;
    attribute[5] = m_value % 256;
    return attribute;
}

Integer* Integer::clone() const
{
    return new Integer(*this);
}

using IpAddress = RadProto::IpAddress;
IpAddress::IpAddress(uint8_t code, const uint8_t* data, size_t size)
        : Attribute(code)
{
    if (size != 4)
        throw RadProto::Exception(RadProto::Error::invalidAttributeSize);

    for (size_t i = 0; i < size; ++i)
        m_value[i] = data[i];
}

IpAddress::IpAddress(uint8_t code, const std::array<uint8_t, 4>& address)
    : Attribute(code),
      m_value(address)
{
}

std::string IpAddress::toString() const
{
    return std::to_string(m_value[0]) + "." + std::to_string(m_value[1]) + "." + std::to_string(m_value[2]) + "." + std::to_string(m_value[3]);
}

std::vector<uint8_t> IpAddress::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(6);

    attribute[0] = code();
    attribute[1] = attribute.size();
    for (size_t i = 0; i < m_value.size(); ++i)
        attribute[i + 2] = m_value[i];

    return attribute;
}

IpAddress* IpAddress::clone() const
{
    return new IpAddress(*this);
}

using Encrypted = RadProto::Encrypted;
Encrypted::Encrypted(uint8_t code, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth)
        : Attribute(code)
{
    if (size > 128)
        throw RadProto::Exception(RadProto::Error::invalidAttributeSize);

    std::vector<uint8_t> mdBuffer(auth.size() + secret.length());

    for (size_t i = 0; i < secret.length(); ++i)
        mdBuffer[i] = secret[i];

    for (size_t i = 0; i < auth.size(); ++i)
        mdBuffer[i + secret.length()] = auth[i];

    std::vector<uint8_t> plaintext(size);

    for (size_t i = 0; i < size / 16; ++i)
    {
        std::array<uint8_t, 16> md;
        unsigned mdSize = md.size();

        EVP_Digest(mdBuffer.data(), mdBuffer.size(), md.data(), &mdSize, EVP_md5(), nullptr);

        for (size_t j = 0; j < 16; ++j)
            plaintext[i * 16 + j] = data[i * 16 + j] ^ md[j];

        for (size_t j = 0; j < 16; ++j)
            mdBuffer[j + secret.length()] = data[i * 16 + j];
    }

    m_value.assign(plaintext.begin(), std::find(plaintext.begin(), plaintext.end(), 0));
}

Encrypted::Encrypted(uint8_t code, const std::string& password)
    : Attribute(code),
      m_value(password)
{
}

std::vector<uint8_t> Encrypted::toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const
{
    std::string plaintext = m_value;

    if (plaintext.length() % 16 != 0)
        plaintext.append(16 - m_value.length() % 16, '\0');

    std::vector<uint8_t> mdBuffer(auth.size() + secret.length());

    for (size_t i = 0; i < secret.length(); ++i)
        mdBuffer[i] = secret[i];

    for (size_t i = 0; i < auth.size(); ++i)
        mdBuffer[i + secret.length()] = auth[i];

    std::vector<uint8_t> res(plaintext.length() + 2);
    res[0] = code();
    res[1] = res.size();
    auto it = std::next(res.begin(), 2);

    for (size_t i = 0; i < plaintext.length() / 16; ++i)
    {
        std::array<uint8_t, 16> md;
        unsigned mdSize = md.size();

        EVP_Digest(mdBuffer.data(), mdBuffer.size(), md.data(), &mdSize, EVP_md5(), nullptr);

        for (size_t j = 0; j < md.size(); ++j)
            *it++ = plaintext[i * 16 + j] ^ md[j];

        for (size_t j = 0; j < 16; ++j)
            mdBuffer[j + secret.length()] = res[i * 16 + 2 + j];
    }
    return res;
}

Encrypted* Encrypted::clone() const
{
    return new Encrypted(*this);
}

using Bytes = RadProto::Bytes;
Bytes::Bytes(uint8_t code, const uint8_t* data, size_t size)
    : Attribute(code),
      m_value(size)
{
    for (size_t i = 0; i < size; ++i)
        m_value[i] = data[i];
}

Bytes::Bytes(uint8_t code, const std::vector<uint8_t>& bytes)
    : Attribute(code),
      m_value(bytes)
{
}

std::string Bytes::toString() const
{
    std::string value;

    for (const auto& b : m_value)
        value += byteToHex(b);

    return value;
}

std::vector<uint8_t> Bytes::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(m_value);
    attribute.insert(attribute.begin(), attribute.size() + 2);
    attribute.insert(attribute.begin(), code());
    return attribute;
}
Bytes* Bytes::clone() const
{
    return new Bytes(*this);
}

using ChapPassword = RadProto::ChapPassword;
ChapPassword::ChapPassword(uint8_t code, const uint8_t* data, size_t size)
    : Attribute(code),
      m_value(size - 1)
{
    if (size != 17)
        throw RadProto::Exception(RadProto::Error::invalidAttributeSize);

    m_chapId = data[0];

    for (size_t i = 0; i < size - 1; ++i)
        m_value[i] = data[i + 1];
}

ChapPassword::ChapPassword(uint8_t code, uint8_t chapId, const std::vector<uint8_t>& chapValue)
    : Attribute(code),
      m_chapId(chapId),
      m_value(chapValue)
{
}

std::string ChapPassword::toString() const
{
    std::string value;

    for (const auto& b : m_value)
        value += byteToHex(b);

    return std::to_string(m_chapId) + " " + value;
}

std::vector<uint8_t> ChapPassword::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(m_value);
    attribute.insert(attribute.begin(), m_chapId);
    attribute.insert(attribute.begin(), m_value.size() + 3);
    attribute.insert(attribute.begin(), code());
    return attribute;
}

ChapPassword* ChapPassword::clone() const
{
    return new ChapPassword(*this);
}
