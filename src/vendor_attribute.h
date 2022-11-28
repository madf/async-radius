#ifndef VENDOR_ATTRIBUTE_H
#define VENDOR_ATTRIBUTE_H

#include <string>
#include <vector>
#include <cstdint> //uint8_t, uint32_t

class VendorAttribute
{
    public:
        VendorAttribute(uint8_t type);
        virtual ~VendorAttribute() = default;
        std::string name() const;
        uint8_t type() const;
        virtual std::string value() const = 0;
        virtual uint8_t vendorType() const = 0;
        virtual std::string vendorId() const = 0 ;
        virtual std::vector<uint8_t> toVector() const = 0;

    private:
        uint8_t m_type;
};

class VendorSpecific: public VendorAttribute
{
    public:
        VendorSpecific(uint8_t type, const uint8_t* data, size_t size);
        VendorSpecific(uint8_t type, uint32_t vendorId, uint8_t vendorType, const std::vector<uint8_t>& vendorValue);
        std::string value() const override;
        uint8_t vendorType() const override;
        std::string vendorId() const override;
        std::vector<uint8_t> toVector() const override;
    private:
        uint32_t m_vendorId;
        uint8_t m_vendorType;
        std::vector<uint8_t> m_value;
};
#endif
