#include "dictionaries.h"
#include <boost/tokenizer.hpp>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>

std::string BasicDictionary::name(uint32_t code) const
{
    return m_rightDict.at(code);
}

uint32_t BasicDictionary::code(const std::string& name) const
{
    return m_reverseDict.at(name);
}

void BasicDictionary::add(uint32_t code, const std::string& name)
{
    m_rightDict.emplace(code, name);
    m_reverseDict.emplace(name, code);
}

std::string DependentDictionary::name(const std::string& attributeName, uint32_t code) const
{
    return m_rightDict.at(std::make_pair(attributeName, code));
}

uint32_t DependentDictionary::code(const std::string& attributeName, const std::string& name) const
{
    return m_reverseDict.at(std::make_pair(attributeName, name));
}

void DependentDictionary::add(uint32_t code, const std::string& name, const std::string& attributeName)
{
    m_rightDict.emplace(std::make_pair(attributeName, code), name);
    m_reverseDict.emplace(std::make_pair(attributeName, name), code);
}

Dictionaries::Dictionaries(const std::string& filePath)
{
    std::ifstream stream(filePath);
    if (!stream)
        throw std::runtime_error("Cannot open dictionary file " + filePath);

    using tokenizer =  boost::tokenizer<boost::char_separator<char>>;
    boost::char_separator<char> sep(" \t");

    std::string line;
    size_t lineNumber = 0;
    bool vendorFlag = false;

    BasicDictionary attr;
    DependentDictionary attrValue;
    BasicDictionary vendName;
    DependentDictionary vendAttr;
    DependentDictionary vendAttrValue;

    std::string vendorName;
    while (std::getline(stream, line))
    {
        tokenizer tok(line, sep);

        std::vector<std::string> tokens;
        for (const auto &t : tok)
            tokens.push_back(t);

        if (tokens.size() != 0)
        {
            if (tokens[0] == "ATTRIBUTE")
            {
                std::string attrName = tokens[1];
                std::string code = tokens[2];
                if (vendorFlag)
                    vendAttr.add(std::stoi(code), attrName, vendorName);
                else
                    attr.add(std::stoi(code), attrName);
            }
            else if (tokens[0] == "VALUE")
            {
                std::string attrNameVal = tokens[1];
                std::string valueName = tokens[2];
                std::string valueCode = tokens[3];
                if (vendorFlag)
                    vendAttrValue.add(std::stoi(valueCode), valueName, attrNameVal);
                else
                    attrValue.add(std::stoi(valueCode), valueName, attrNameVal);

            }
            else if (tokens[0] == "VENDOR")
            {
                vendorName = tokens[1];
                std::string vendorCode = tokens[2];
                vendName.add(std::stoi(vendorCode), vendorName);
            }
            else if (tokens[0] == "BEGIN-VENDOR")
                vendorFlag = true;
            else if (tokens[0] == "END-VENDOR")
                vendorFlag = false;
        }
        ++lineNumber;
    }
//RFC

//    for (const auto &entry: attr.rightDict())
//        std::cout << entry.second << ": " << std::to_string(entry.first) << "\n";


//    for (const auto &entry: attrValue.rightDict())
//        std::cout << "  " << entry.first.first << " - " << entry.second << ": " << std::to_string(entry.first.second) << "\n";

//Vendor

    for (const auto &entry: vendName.rightDict())
        std::cout << entry.second << ": " << std::to_string(entry.first) << "\n";

    for (const auto &entry: vendAttr.rightDict())
        std::cout << "  " << entry.second << ": " << std::to_string(entry.first.second) << "\n";

    for (const auto &entry: vendAttrValue.rightDict())
        std::cout << "    " << entry.first.first << " - " << entry.second << ": " << std::to_string(entry.first.second) << "\n";
}
