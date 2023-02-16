#include "dictionaries.h"
#include <utility>

BasicDictionary::BasicDictionary()
{
}

std::string BasicDictionary::getName(uint8_t type) const
{
    return m_rightDict.at(type);
}

uint8_t BasicDictionary::getType(const std::string& name) const
{
    return m_reverseDict.at(name);
}

void BasicDictionary::add(uint8_t type, const std::string& name)
{
    m_rightDict.insert(std::pair<uint8_t, std::string>(type, name));
    m_reverseDict.insert(std::pair<std::string, uint8_t>(name, type));
}

DependentDictionary::DependentDictionary()
{
}

std::string DependentDictionary::getName(const std::string& attributeName, uint8_t type) const
{
    return m_rightDict.at(make_pair(attributeName, type));
}

uint8_t DependentDictionary::getType(const std::string& attributeName, const std::string& name) const
{
    return m_reverseDict.at(make_pair(attributeName, name));
}

void DependentDictionary::add(uint8_t type, const std::string& name, const std::string& attributeName)
{
    m_rightDict.insert(std::make_pair(std::pair<std::string, uint8_t>(attributeName, type), name));
    m_reverseDict.insert(std::make_pair(std::pair<std::string, std::string>(attributeName, name), type));
}
