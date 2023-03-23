#include "dictionaries.h"
#include <utility>
#include <iostream>
#include <fstream>

std::string BasicDictionary::name(uint8_t code) const
{
    return m_rightDict.at(code);
}

uint8_t BasicDictionary::code(const std::string& name) const
{
    return m_reverseDict.at(name);
}

void BasicDictionary::add(uint8_t code, const std::string& name)
{
    m_rightDict.emplace(code, name);
    m_reverseDict.emplace(name, code);
}

std::string DependentDictionary::name(const std::string& attributeName, uint8_t code) const
{
    return m_rightDict.at(std::make_pair(attributeName, code));
}

uint8_t DependentDictionary::code(const std::string& attributeName, const std::string& name) const
{
    return m_reverseDict.at(std::make_pair(attributeName, name));
}

void DependentDictionary::add(uint8_t code, const std::string& name, const std::string& attributeName)
{
    m_rightDict.emplace(std::make_pair(attributeName, code), name);
    m_reverseDict.emplace(std::make_pair(attributeName, name), code);
}

Dictionaries::Dictionaries(const std::string& filePath)
{
    std::ifstream stream(filePath);
    if (!stream)
        throw std::runtime_error("Cannot open dictionary file " + filePath);

    std::string line;
    const std::string keyword("ATTRIBUTE");
    size_t lineNumber = 0;
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
                throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + ": Attribute name is missing in the attribute definition.");

            std::string name = line.substr(firstPosName, line.find_first_of("  \t", firstPosName) - firstPosName);

            size_t firstPosCode = line.find_first_not_of(" \t", firstPosName + name.size());
            if (firstPosCode == std::string::npos)
                throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + ": Attribute code is missing in the attribute " + name + " definition.");

            std::string code = line.substr(firstPosCode, line.find_first_of("  \t", firstPosCode) - firstPosCode);

            std::cout << name << ": " << code << "\n";
        }
        ++lineNumber;
    }
}
