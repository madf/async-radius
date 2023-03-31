#include "dictionaries.h"
#include <boost/tokenizer.hpp>
#include <vector>
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

    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep("\t");

    std::string line;
    size_t lineNumber = 0;
    bool vendorFlag = false;
    while (std::getline(stream, line))
    {
        tokenizer tok(line, sep);

        std::vector<std::string> tokens;
        for (const auto &t : tok)
            tokens.push_back(t);

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (tokens[i] == "ATTRIBUTE")
            {
                std::string name = tokens[i + 1];
                std::string code = tokens[i + 2];
                if (vendorFlag)
                    std::cout << "  " << name << ": " << code << "\n";
                else
                    std::cout << name << ": " << code << "\n";
            }
            else if (tokens[i] == "VENDOR")
            {
                std::string vendorName = tokens[i + 1];
                std::string vendorCode = tokens[i + 2];
                std::cout << "Vendor " << vendorName << ": " << vendorCode << "\n";
            }
            else if (tokens[i] == "BEGIN-VENDOR")
                vendorFlag = true;
            else if (tokens[i] == "END-VENDOR")
                vendorFlag = false;
        }
        ++lineNumber;
    }
}
