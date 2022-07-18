#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <array>
#include <cstdint> //uint8_t, uint32_t

class Attribute
{
    public:
        Attribute(uint8_t type);
        virtual ~Attribute() = default;
        std::string name() const;
        virtual std::string value() const = 0;

    private:
        uint8_t m_type;
};

class String: public Attribute
{
    public:
        String(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize);
        std::string value() const override;
    private:
        std::string m_value;
};

class Integer: public Attribute
{
    public:
        Integer(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize);
        std::string value() const override;
    private:
        uint32_t m_value;
};

class NasIpAddress : public Attribute
{
    public:
        NasIpAddress(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize);
        std::string value() const override;
    private:
        std::string m_value;
};

class Encrypted : public Attribute
{
    public:
        Encrypted (uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize, std::string secret, std::array<uint8_t, 16> auth);
        std::string value() const override;
    private:
        std::string m_value;
};

class Bytes: public Attribute
{
    public:
        Bytes(uint8_t type, const uint8_t* attributeValue, size_t attributeValueSize);
        std::string value() const override;
    private:
        std::string m_value;
};
#endif
