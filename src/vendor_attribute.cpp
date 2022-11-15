#include "packet.h"
#include "utils.h"
#include "vendor_attribute.h"
#include "attribute_types.h"
#include <iostream>

VendorAttribute::VendorAttribute(uint8_t type)
    : m_type(type)
{
}

VendorSpecific::VendorSpecific(uint8_t type, const uint8_t* data, size_t size)
    : VendorAttribute(type),
      m_value(size)
{
    m_vendorId = data[0] * (1 << 24)
           + data[1] * (1 << 16)
           + data[2] * (1 << 8)
           + data[3];

    m_vendorType = data[4];

    size_t vendorLength = data[5];
    for (size_t i = 0; i < vendorLength; ++i)
        m_value[i] = data[i + 6];
}

std::string VendorSpecific::value() const
{
    std::string value;

    for (const auto& b : m_value)
        value += byteToHex(b);

    return value;
}

std::string VendorSpecific::vendorType() const
{
    return std::to_string(m_vendorType);
}

std::string VendorSpecific::vendorId() const
{
    return std::to_string(m_vendorId);
}

uint8_t VendorAttribute::type() const
{
    return m_type;
}

std::string VendorAttribute::name() const
{
    return "VENDOR_SPECIFIC";
}

