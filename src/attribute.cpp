#include "packet.h"
#include "attribute.h"
#include "utils.h"
#include <openssl/md5.h>
#include <iostream>

Attribute::Attribute(uint8_t type)
    : m_type(type)
{
}

String::String(uint8_t type, const uint8_t* data, size_t size)
        : Attribute(type),
          m_value(reinterpret_cast<const char*>(data), size)
{
}

String::String(uint8_t type, const std::string& string)
    : Attribute(type),
      m_value(string)
{
}

std::vector<uint8_t> String::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(m_value.length() + 2);
    std::copy(m_value.begin(), m_value.end(), std::next(attribute.begin(), 2));
    attribute[0] = type();
    attribute[1] = m_value.length() + 2;
    return attribute;
}

Integer::Integer(uint8_t type, const uint8_t* data, size_t size)
        : Attribute(type),
          m_value(0)
{
    if (size != 4)
        throw std::runtime_error("Invalid integer attribute size. Should be 4, actual size is " + std::to_string(size));

    m_value = data[0] * (1 << 24)
            + data[1] * (1 << 16)
            + data[2] * (1 << 8)
            + data[3];
}

Integer::Integer(uint8_t type, uint32_t value)
    : Attribute(type),
      m_value(value)
{
}

std::vector<uint8_t> Integer::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(6);
    attribute[0] = type();
    attribute[1] = 6;
    attribute[2] = m_value / (1 << 24);
    attribute[3] = (m_value / (1 << 16)) % 256;
    attribute[4] = (m_value / (1 << 8)) % 256;
    attribute[5] = m_value % 256;
    return attribute;
}

IpAddress::IpAddress(uint8_t type, const uint8_t* data, size_t size)
        : Attribute(type)
{
    if (size != 4)
        throw std::runtime_error("Invalid integer attribute size. Should be 4, actual size is " + std::to_string(size));

    for (size_t i = 0; i < size; ++i)
        m_value[i] = data[i];
}

IpAddress::IpAddress(uint8_t type, const std::array<uint8_t, 4>& address)
    : Attribute(type),
      m_value(address)
{
}

std::vector<uint8_t> IpAddress::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(6);

    attribute[0] = type();
    attribute[1] = attribute.size();
    for (size_t i = 0; i < m_value.size(); ++i)
        attribute[i + 2] = m_value[i];

    return attribute;
}

Encrypted::Encrypted(uint8_t type, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth)
        : Attribute(type)
{
    if (size > 128)
        throw std::runtime_error("Invalid encrypted attribute size. Should be max 128 bytes, actual size is " + std::to_string(size));

    std::vector<uint8_t> mdBuffer(auth.size() + secret.length());

    for (size_t i = 0; i < secret.length(); ++i)
        mdBuffer[i] = secret[i];

    for (size_t i = 0; i < auth.size(); ++i)
        mdBuffer[i + secret.length()] = auth[i];

    std::vector<uint8_t> plaintext(size);

    for (size_t i = 0; i < size / 16; ++i)
    {
        std::array<uint8_t, 16> md;

        MD5(mdBuffer.data(), mdBuffer.size(), md.data());
        for (size_t j = 0; j < 16; ++j)
            plaintext[i * 16 + j] = data[i * 16 + j] ^ md[j];

        for (size_t j = 0; j < 16; ++j)
            mdBuffer[j + secret.length()] = data[i * 16 + j];
    }

    m_value.assign(plaintext.begin(), plaintext.end());
}

Encrypted::Encrypted(uint8_t type, const std::string& password)
    : Attribute(type),
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
    res[0] = type();
    res[1] = res.size();
    auto it = std::next(res.begin(), 2);

    for (size_t i = 0; i < plaintext.length() / 16; ++i)
    {
        std::array<uint8_t, 16> md;
        MD5(mdBuffer.data(), mdBuffer.size(), md.data());

        for (size_t j = 0; j < md.size(); ++j)
            *it++ = plaintext[i * 16 + j] ^ md[j];

        for (size_t j = 0; j < 16; ++j)
            mdBuffer[j + secret.length()] = res[i * 16 + j];
    }
    return res;
}

Bytes::Bytes(uint8_t type, const uint8_t* data, size_t size)
    : Attribute(type),
      m_value(size)
{
    for (size_t i = 0; i < size; ++i)
        m_value[i] = data[i];
}

Bytes::Bytes(uint8_t type, const std::vector<uint8_t>& bytes)
    : Attribute(type),
      m_value(bytes)
{
}

std::vector<uint8_t> Bytes::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(m_value);
    attribute.insert(attribute.begin(), attribute.size() + 2);
    attribute.insert(attribute.begin(), type());
    return attribute;
}

ChapPassword::ChapPassword(uint8_t type, const uint8_t* data, size_t size)
    : Attribute(type),
      m_value(size - 1)
{
    if (size != 17)
        throw std::runtime_error("Invalid CHAP_PASSWORD attribute size. Should be 17, actual size is " + std::to_string(size));

    m_chapId = data[0];

    for (size_t i = 0; i < size - 1; ++i)
        m_value[i] = data[i + 1];
}

ChapPassword::ChapPassword(uint8_t type, uint8_t chapId, const std::vector<uint8_t>& chapValue)
    : Attribute(type),
      m_chapId(chapId),
      m_value(chapValue)
{
}

std::vector<uint8_t> ChapPassword::toVector(const std::string& /*secret*/, const std::array<uint8_t, 16>& /*auth*/) const
{
    std::vector<uint8_t> attribute(m_value);
    attribute.insert(attribute.begin(), m_chapId);
    attribute.insert(attribute.begin(), m_value.size() + 3);
    attribute.insert(attribute.begin(), type());
    return attribute;
}

std::string String::value() const
{
    return m_value;
}

std::string Integer::value() const
{
    return std::to_string(m_value);
}

std::string IpAddress::value() const
{
    return std::to_string(m_value[0]) + "." + std::to_string(m_value[1]) + "." + std::to_string(m_value[2]) + "." + std::to_string(m_value[3]);
}

std::string Encrypted::value() const
{
    return m_value;
}

std::string Bytes::value() const
{
    std::string value;

    for (const auto& b : m_value)
        value += byteToHex(b);

    return value;
}

std::string ChapPassword::value() const
{
    std::string value;

    for (const auto& b : m_value)
        value += byteToHex(b);

    return value;
}

uint8_t ChapPassword::chapId() const
{
    return m_chapId;
}

uint8_t Attribute::type() const
{
    return m_type;
}

std::string Attribute::name() const
{
    return typeToString(m_type);
}
