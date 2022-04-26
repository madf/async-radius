#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <cstdint> //uint8_t, uint32_t

class Attribute
{
    public:
        Attribute(uint8_t type);
        uint8_t type() const;
        virtual std::string value() const = 0;

    private:
        uint8_t m_type;
};

class String: public Attribute
{
    public:
        String(uint8_t type);
        virtual std::string value() const;
};

#endif
