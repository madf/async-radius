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
        String(uint8_t type, const std::array<uint8_t, 4096>& m_recvBuffer, size_t attributeIndex);
        std::string value() const override;
    private:
        std::string m_attribute;
};

class Integer: public Attribute
{
    public:
        Integer(uint8_t type, const std::array<uint8_t, 4096>& m_recvBuffer, size_t attributeIndex);
        std::string value() const override;
    private:
        uint32_t m_attribute;
};
#endif
