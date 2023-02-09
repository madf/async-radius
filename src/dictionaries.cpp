#include "dictionaries.h"
#include <utility>
#include <iostream>

BasicDictionary::BasicDictionary(uint8_t type, std::string name)
    : m_type(type),
      m_name(name)
{
}

uint8_t BasicDictionary::getType() const
{
    return m_type;
}

std::string BasicDictionary::getName() const
{
    return m_name;
}

void BasicDictionary::add()
{
    m_rightDict.insert(std::pair<uint8_t, std::string>(m_type, m_name));
    m_reverseDict.insert(std::pair<std::string, uint8_t>(m_name, m_type));
}

DependentDictionary::DependentDictionary(uint8_t type, std::string name, std::string attributeName)
    : m_type(type),
      m_name(name),
      m_attributeName(attributeName)
{
}

uint8_t DependentDictionary::getType() const
{
    return m_type;
}

std::string DependentDictionary::getName() const
{
    return m_name;
}

std::string DependentDictionary::getAttributeName() const
{
    return m_attributeName;
}

void DependentDictionary::add()
{
    m_rightDict.insert(std::make_pair(std::pair<std::string, uint8_t>(m_attributeName, m_type), m_name));
    m_reverseDict.insert(std::make_pair(std::pair<std::string, std::string>(m_attributeName, m_name), m_type));
}
