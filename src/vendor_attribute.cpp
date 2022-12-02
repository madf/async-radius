#include "packet.h"
#include "utils.h"
#include "vendor_attribute.h"
#include "attribute_types.h"

VendorSpecific::VendorSpecific(uint8_t type, const uint8_t* data, size_t size)
    : m_type(type),
      m_value(size)
{
    m_vendorId = data[0] * (1 << 24)
               + data[1] * (1 << 16)
               + data[2] * (1 << 8)
               + data[3];

    m_vendorType = data[4];

    size_t vendorLength = data[5];
    m_value.resize(vendorLength - 2);

    for (size_t i = 0; i < vendorLength - 2; ++i)
        m_value[i] = data[i + 6];
}

VendorSpecific::VendorSpecific(uint8_t type, uint32_t vendorId, uint8_t vendorType, const std::vector<uint8_t>& vendorValue)
    : m_type(type),
      m_vendorId(vendorId),
      m_vendorType(vendorType),
      m_value(vendorValue)
{
}

std::vector<uint8_t> VendorSpecific::toVector() const
{
    std::vector<uint8_t> attribute(m_value.size() + 8);
    attribute[0] = type();
    attribute[1] = m_value.size() + 8;
    attribute[2] = m_vendorId / (1 << 24);
    attribute[3] = (m_vendorId / (1 << 16)) % 256;
    attribute[4] = (m_vendorId / (1 << 8)) % 256;
    attribute[5] = m_vendorId % 256;
    attribute[6] = vendorType();
    attribute[7] = m_value.size() + 2;
    for (size_t i = 0; i < m_value.size() - 2; ++i)
        attribute[i + 8] = m_value[i];
    return attribute;
}

std::string VendorSpecific::value() const
{
    std::string value;

    for (const auto& b : m_value)
        value += byteToHex(b);

    return value;
}

uint8_t VendorSpecific::vendorType() const
{
    return m_vendorType;
}

std::string VendorSpecific::vendorId() const
{
    return std::to_string(m_vendorId);
}

uint8_t VendorSpecific::type() const
{
    return m_type;
}

std::string VendorSpecific::name() const
{
    return typeToString(m_type);
}

