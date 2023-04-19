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

std::string DependentDictionary::name(const std::string& dependencyName, uint32_t code) const
{
    return m_rightDict.at(std::make_pair(dependencyName, code));
}

uint32_t DependentDictionary::code(const std::string& dependencyName, const std::string& name) const
{
    return m_reverseDict.at(std::make_pair(dependencyName, name));
}

void DependentDictionary::add(uint32_t code, const std::string& name, const std::string& dependencyName)
{
    m_rightDict.emplace(std::make_pair(dependencyName, code), name);
    m_reverseDict.emplace(std::make_pair(dependencyName, name), code);
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
    std::string vendorName;

    while (std::getline(stream, line))
    {
        tokenizer tok(line, sep);

        std::vector<std::string> tokens;
        for (const auto &t : tok)
            tokens.push_back(t);

        if (!tokens.empty())
        {
            if (tokens[0] == "ATTRIBUTE")
            {
                const auto& attrName = tokens[1];
                const auto code = std::stoul(tokens[2]);
                if (!vendorName.empty())
                    m_vendorAttributes.add(code, attrName, vendorName);
                else
                    m_attributes.add(code, attrName);
            }
            else if (tokens[0] == "VALUE")
            {
                const auto& attrNameVal = tokens[1];
                const auto& valueName = tokens[2];
                const auto valueCode = std::stoul(tokens[3]);
                if (!vendorName.empty())
                    m_vendorAttributeValues.add(valueCode, valueName, attrNameVal);
                else
                    m_attributeValues.add(valueCode, valueName, attrNameVal);
            }
            else if (tokens[0] == "VENDOR")
                m_vendorNames.add(std::stoul(tokens[2]), tokens[1]);
            else if (tokens[0] == "BEGIN-VENDOR")
                vendorName = tokens[1];
            else if (tokens[0] == "END-VENDOR")
                vendorName.clear();
        }
        ++lineNumber;
    }
    for (const auto &entry: m_attributes.rightDict())
        std::cout << entry.second << ": " << std::to_string(entry.first) << "\n";

    for (const auto &entry: m_attributeValues.rightDict())
        std::cout << "  " << entry.first.first << " - " << entry.second << ": " << std::to_string(entry.first.second) << "\n";

    for (const auto &entry: m_vendorNames.rightDict())
        std::cout << entry.second << ": " << std::to_string(entry.first) << "\n";

    for (const auto &entry: m_vendorAttributes.rightDict())
        std::cout << "  " << entry.second << ": " << std::to_string(entry.first.second) << "\n";

    for (const auto &entry: m_vendorAttributeValues.rightDict())
        std::cout << "    " << entry.first.first << " - " << entry.second << ": " << std::to_string(entry.first.second) << "\n";
}
