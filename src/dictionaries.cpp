#include "dictionaries.h"
#include <utility>
#include <iostream>
#include <fstream>

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

Dictionaries::Dictionaries(const std::string& filePath)
{
    std::ifstream inf(filePath);
    if (!inf)
        throw std::runtime_error("File Dictonary " + filePath + " failed to open.");

    while (inf)
    {
        std::string strInput;
        getline(inf, strInput);
        std::cout << strInput << "\n";
    }
}
