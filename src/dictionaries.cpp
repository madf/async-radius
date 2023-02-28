#include "dictionaries.h"
#include <utility>

std::string BasicDictionary::name(uint8_t type) const
{
    return m_rightDict.at(type);
}

uint8_t BasicDictionary::type(const std::string& name) const
{
    return m_reverseDict.at(name);
}

void BasicDictionary::add(uint8_t type, const std::string& name)
{
    m_rightDict.emplace(type, name);
    m_reverseDict.emplace(name, type);
}

std::string DependentDictionary::name(const std::string& attributeName, uint8_t type) const
{
    return m_rightDict.at(std::make_pair(attributeName, type));
}

uint8_t DependentDictionary::type(const std::string& attributeName, const std::string& name) const
{
    return m_reverseDict.at(std::make_pair(attributeName, name));
}

void DependentDictionary::add(uint8_t type, const std::string& name, const std::string& attributeName)
{
    m_rightDict.emplace(std::make_pair(attributeName, type), name);
    m_reverseDict.emplace(std::make_pair(attributeName, name), type);
}

const BasicDictionary& Dictionaries::attributes() const
{
    return m_attributes;
}

const BasicDictionary& Dictionaries::vendorNames() const
{
    return m_vendorNames;
}

const DependentDictionary& Dictionaries::attributeValues() const
{
    return m_attributeValues;
}

const DependentDictionary& Dictionaries::vendorAttributes() const
{
    return m_vendorAttributes;
}

const DependentDictionary& Dictionaries::vendorAttributeValues() const
{
    return m_vendorAttributeValues;
}
