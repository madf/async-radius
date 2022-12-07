#ifndef VENDOR_ATTRIBUTE_H
#define VENDOR_ATTRIBUTE_H

#include <string>
#include <vector>
#include <cstdint> //uint8_t, uint32_t

class VendorSpecific
{
    public:
        VendorSpecific(uint8_t type, const uint8_t* data);
        VendorSpecific(uint8_t type, uint32_t vendorId, uint8_t vendorType, const std::vector<uint8_t>& vendorValue);
        std::string name() const;
        uint8_t type() const;
        std::string value() const;
        uint8_t vendorType() const;
        uint32_t vendorId() const;
        std::vector<uint8_t> toVector() const;
    private:
        uint8_t m_type;
        uint32_t m_vendorId;
        uint8_t m_vendorType;
        std::vector<uint8_t> m_value;
};
#endif
