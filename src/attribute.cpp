#include "packet.h"
#include "attribute.h"
#include "attribute_types.h"
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

std::vector<uint8_t> String::toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const
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

std::vector<uint8_t> Integer::toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const
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

std::vector<uint8_t> IpAddress::toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const
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

    std::vector<uint8_t> value(size);

    size_t j = 16;
    while (j <= size)
    {
        std::vector<uint8_t> buffer(16 + secret.length());

        for (size_t i = 0; i < secret.length(); ++i)
            buffer[i] = secret[i];

        if (j == 16)
        {
            for (size_t i = 0; i < 16; ++i)
                buffer[i + secret.length()] = auth[i];
        }
        else
        {
            for (size_t i = 0; i < 16; ++i)
                buffer[i + secret.length()] = data[i + j - 32];
        }
        std::array<uint8_t, 16> md;

        MD5(buffer.data(), buffer.size(), md.data());

        for (size_t i = 0; i < 16; ++i)
            value.push_back(data[i + j - 16] ^ md[i]);

        j += 16;
    }
    m_value.assign(value.begin(), value.end());
}

Encrypted::Encrypted(uint8_t type, const std::string& password)
    : Attribute(type),
      m_value(password)
{
}

std::vector<uint8_t> Encrypted::toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const
{
    std::string value = m_value;

    if (value.length() % 16 != 0)
    {
        for (size_t i = 0; i < (16 - m_value.length() % 16); ++i)
            value.push_back(0);
    }

    std::vector<uint8_t> buffer(16 + secret.length());

    for (size_t i = 0; i < secret.length(); ++i)
        buffer[i] = secret[i];
    for (size_t i = 0; i < 16; ++i)
        buffer[i + secret.length()] = auth[i];

    std::array<uint8_t, 16> md;
    MD5(buffer.data(), buffer.size(), md.data());

    std::vector<uint8_t> attribute;
    std::vector<uint8_t> result(16);
    for (size_t j = 0; j < value.length() / 16; ++j)
    {
        for (size_t i = 0; i < 16; ++i)
        {
            result[i] = value[i + j * 16] ^ md[i];
            attribute.push_back(result[i]);
        }
        if (j != value.length() - 1)
        {
            for (size_t i = 0; i < 16; ++i)
                buffer[i + secret.length()] = result[i];

            MD5(buffer.data(), buffer.size(), md.data());
        }
    }

    auto it = attribute.begin();
    it = attribute.insert(it, attribute.size() + 2);
    it = attribute.insert(it, type());
    return attribute;
}

std::string byteToHex(uint8_t number)
{
    const std::string digits = "0123456789ABCDEF";
    std::string hex;
    hex.append(1, digits[number / 16]);
    hex.append(1, digits[number % 16]);
    return hex;
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

std::vector<uint8_t> Bytes::toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const
{
    std::vector<uint8_t> attribute(m_value);
    auto it = attribute.begin();
    it = attribute.insert(it, attribute.size() + 2);
    it = attribute.insert(it, type());
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

std::string typeToString(int type)
{
    switch (type)
    {
        case USER_NAME: return "USER_NAME";
        case USER_PASSWORD: return "USER_PASSWORD";
        case CHAP_PASSWORD: return "CHAP_PASSWORD";
        case NAS_IP_ADDRESS: return "NAS_IP_ADDRESS";
        case NAS_PORT: return "NAS_PORT";
        case SERVICE_TYPE: return "SERVICE_TYPE";
        case FRAMED_PROTOCOL: return "FRAMED_PROTOCOL";
        case FRAMED_IP_ADDRESS: return "FRAMED_IP_ADDRESS";
        case FRAMED_IP_NETMASK: return "FRAMED_IP_NETMASK";
        case FRAMED_ROUTING: return "FRAMED_ROUTING";
        case FILTER_ID: return "FILTER_ID";
        case FRAMED_MTU: return "FRAMED_MTU";
        case FRAMED_COMPRESSION: return "FRAMED_COMPRESSION";
        case LOGIN_IP_HOST: return "LOGIN_IP_HOST";
        case LOGIN_SERVICE: return "LOGIN_SERVICE";
        case LOGIN_TCP_PORT: return "LOGIN_TCP_PORT";
        case REPLY_MESSAGE: return "REPLY_MESSAGE";
        case CALLBACK_NUMBER: return "CALLBACK_NUMBER";
        case CALLBACK_ID: return "CALLBACK_ID";
        case FRAMED_ROUTE: return "FRAMED_ROUTE";
        case FRAMED_IPX_NETWORK: return "FRAMED_IPX_NETWORK";
        case STATE: return "STATE";
        case CLASS: return "CLASS";
        case VENDOR_SPECIFIC: return "VENDOR_SPECIFIC";
        case SESSION_TIMEOUT: return "SESSION_TIMEOUT";
        case IDLE_TIMEOUT: return "IDLE_TIMEOUT";
        case TERMINATION_ACTION: return "TERMINATION_ACTION";
        case CALLED_STATION_ID: return "CALLED_STATION_ID";
        case CALLING_STATION_ID: return "CALLING_STATION_ID";
        case NAS_IDENTIFIER: return "NAS_IDENTIFIER";
        case PROXY_STATE: return "PROXY_STATE";
        case LOGIN_LAT_SERVICE: return "LOGIN_LAT_SERVICE";
        case LOGIN_LAT_NODE: return "LOGIN_LAT_NODE";
        case LOGIN_LAT_GROUP: return "LOGIN_LAT_GROUP";
        case FRAMED_APPLETALK_LINK: return "FRAMED_APPLETALK_LINK";
        case FRAMED_APPLETALK_NETWORK: return "FRAMED_APPLETALK_NETWORK";
        case FRAMED_APPLETALK_ZONE: return "FRAMED_APPLETALK_ZONE";
        case CHAP_CHALLENGE: return "CHAP_CHALLENGE";
        case NAS_PORT_TYPE: return "NAS_PORT_TYPE";
        case PORT_LIMIT: return "PORT_LIMIT";
        case LOGIN_LAT_PORT: return "LOGIN_LAT_PORT";
        case EAP_MESSAGE: return "EAP_MESSAGE";
        case MESSAGE_AUTHENTICATOR: return "MESSAGE_AUTHENTICATOR";
    }
    return "unknown";
}

uint8_t Attribute::type() const
{
    return m_type;
}

std::string Attribute::name() const
{
    return typeToString(m_type);
}
