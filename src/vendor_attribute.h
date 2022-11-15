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
        virtual std::string vendorType() const = 0;
        virtual std::string vendorId() const = 0 ;

    private:
        uint8_t m_type;
};

class VendorSpecific: public VendorAttribute
{
    public:
        VendorSpecific(uint8_t type, const uint8_t* data, size_t size);
        VendorSpecific(uint8_t type, const std::vector<uint8_t>& bytes);
        std::string value() const override;
        std::string vendorType() const override;
        std::string vendorId() const override;
    private:
        std::vector<uint8_t> m_value;
        uint8_t m_vendorType;
        uint32_t m_vendorId;
};
#endif
