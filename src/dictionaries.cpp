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
    std::ifstream stream(filePath);
    if (!stream)
        throw std::runtime_error("Cannot open dictionary file " + filePath);

    std::string line;
    const std::string keyword("ATTRIBUTE");
    while (std::getline(stream, line))
    {
        size_t firstPosKeyword = line.find(keyword, 0);
        if (firstPosKeyword == std::string::npos && stream.eof())
        {
            throw std::runtime_error("Keyword " + keyword + " not found.");
        }
        else if (firstPosKeyword != std::string::npos)
        {
            size_t firstPosName = line.find_first_not_of(" \t", firstPosKeyword + keyword.size());
            if (firstPosName == std::string::npos)
                throw std::runtime_error("Attribute name not found.");

            std::string name = line.substr(firstPosName, line.find_first_of("  \t", firstPosName) - firstPosName);

            size_t firstPosType = line.find_first_not_of(" \t", firstPosName + name.size());
            if (firstPosType == std::string::npos)
                throw std::runtime_error("Attribute type not found.");

            std::string type = line.substr(firstPosType, line.find_first_of("  \t", firstPosType) - firstPosType);

            std::cout << name << ": " << type << "\n";
        }
    }
}
