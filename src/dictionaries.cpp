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

Dictionaries::Dictionaries(const std::string& filePath, const std::string& keyword)
{
    std::ifstream stream(filePath);
    if (!stream)
        throw std::runtime_error("Cannot open dictionary file " + filePath);

    std::string line;
    while (std::getline(stream, line))
    {
        size_t firstPosKeyword = line.find(keyword);
        if (firstPosKeyword != line.npos)
        {
            size_t lastPosKeyword = line.find_first_of(" \t", firstPosKeyword) - 1;

            size_t firstPosName = line.find_first_not_of(" \t", lastPosKeyword + 1);
            size_t lastPosName = line.find_first_of("  \t", firstPosName) - 1;
            std::string name = line.substr(firstPosName, lastPosName - firstPosName + 1);

            size_t firstPosType = line.find_first_not_of(" \t", lastPosName + 1);
            size_t lastPosType= line.find_first_of("  \t", firstPosType) - 1;
            std::string type = line.substr(firstPosType, lastPosType - firstPosType + 1);

            if (name == "User-Name" || name == "User-Password")
                std::cout << name << ": " << type << "\n";
        }
    }
}
