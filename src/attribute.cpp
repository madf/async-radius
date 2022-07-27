#include "packet.h"
#include "attribute.h"
#include "attribute_types.h"
#include <vector>
#include <openssl/md5.h>
#include <iostream>

Attribute::Attribute(uint8_t type)
    : m_type(type)
{
}

String::String(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize)
        : Attribute(type),
          m_value(reinterpret_cast<const char*>(attributeValue), attributeValueSize)
{
}

Integer::Integer(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize)
        : Attribute(type),
          m_value(0)
{
    if (attributeValueSize != 4)
        throw std::runtime_error("Invalid integer attribute size. Should be 4, actual size is " + std::to_string(attributeValueSize));

    m_value = attributeValue[0] * (1 << 24)
            + attributeValue[1] * (1 << 16)
            + attributeValue[2] * (1 << 8)
            + attributeValue[3];
}

NasIpAddress::NasIpAddress(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize)
        : Attribute(type)
{
    if (attributeValueSize != 4)
        throw std::runtime_error("Invalid nas_ip_address attribute size. Should be 4, actual size is " + std::to_string(attributeValueSize));

    m_value = std::to_string(attributeValue[0]) + "." + std::to_string(attributeValue[1]) + "." + std::to_string(attributeValue[2]) + "." + std::to_string(attributeValue[3]);
}

Encrypted::Encrypted(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize, std::string secret, std::array<uint8_t, 16> auth)
        : Attribute(type)
{
    if (attributeValueSize > 128)
        throw std::runtime_error("Invalid encrypted attribute size. Should be max 128 bytes, actual size is " + std::to_string(attributeValueSize));

    std::vector<uint8_t> value(attributeValueSize);

    size_t j = 16;
    while (j <= attributeValueSize)
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
                buffer[i + secret.length()] = attributeValue[i + j - 32];
        }
        std::array<uint8_t, 16> md;

        MD5(buffer.data(), buffer.size(), md.data());

        for (size_t i = 0; i < 16; ++i)
            value.push_back(attributeValue[i + j - 16] ^ md[i]);

        j += 16;
    }
    m_value.assign(value.begin(), value.end());
}

std::string intToHex(uint8_t number)
{
    const std::string digits = "0123456789ABCDEF";
    std::string hex;
    hex.insert(0, 1, digits[number % 16]);
    hex.insert(0, 1, digits[number / 16]);
    return hex;
}

Bytes::Bytes(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize)
        : Attribute(type)
{
    for (size_t i = 0; i < attributeValueSize; ++i)
    {
        std::string numberHex = intToHex(attributeValue[i]);
        m_value += numberHex;
    }
}

std::string String::value() const
{
    return m_value;
}

std::string Integer::value() const
{
    return std::to_string(m_value);
}

std::string NasIpAddress::value() const
{
    return m_value;
}

std::string Encrypted::value() const
{
    return m_value;
}

std::string Bytes::value() const
{
    return m_value;
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
