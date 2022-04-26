#include "attribute.h"
#include <iostream>

Attribute::Attribute(uint8_t type)
    : m_type(type)
{
}

uint8_t Attribute::type() const
{
    return m_type;
}

String::String(uint8_t type)
        : Attribute(type)
{
}

std::string String::value() const
{
    return "1"; //temporarily
}
