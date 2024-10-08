#include "dictionaries.h"
#include "error.h"
#include <boost/tokenizer.hpp>
#include <vector>
#include <utility>
#include <fstream>

using BasicDictionary = RadProto::BasicDictionary;
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
    for (const auto& entry: m_rightDict)
        if (entry.second == name && entry.first != code)
            throw Exception(Error::suchAttributeNameAlreadyExists, "[BasicDictionary::add]. Attribute name " + name + " already exists with code " + std::to_string(entry.first));

    m_rightDict.insert_or_assign(code, name);
    m_reverseDict.emplace(name, code);
}

void BasicDictionary::append(const BasicDictionary& basicDict)
{
    for (const auto& entry: basicDict.m_rightDict)
    {
        for (const auto& item: m_rightDict)
            if (entry.second == item.second && entry.first != item.first)
                throw Exception(Error::suchAttributeNameAlreadyExists, "[BasicDictionary::append]. Attribute name " + entry.second + " already exists with code " + std::to_string(item.first));

        m_rightDict.insert_or_assign(entry.first, entry.second);
    }

    for (const auto& entry: basicDict.m_reverseDict)
        m_reverseDict.emplace(entry.first, entry.second);
}

using DependentDictionary = RadProto::DependentDictionary;
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
    for (const auto& entry: m_rightDict)
        if (entry.second == name && entry.first.first == dependencyName && entry.first.second != code)
            throw Exception(Error::suchAttributeNameAlreadyExists, "[DependentDictionary::add]. Value name " + name + " of attribute " + dependencyName + " already exists with code " + std::to_string(entry.first.second));

    m_rightDict.insert_or_assign(std::make_pair(dependencyName, code), name);
    m_reverseDict.emplace(std::make_pair(dependencyName, name), code);
}

void DependentDictionary::append(const DependentDictionary& dependentDict)
{
    for (const auto& entry: dependentDict.m_rightDict)
    {
        for (const auto& item: m_rightDict)
            if (item.second == entry.second && item.first.first == entry.first.first && item.first.second != entry.first.second)
                throw Exception(Error::suchAttributeNameAlreadyExists, "[DependentDictionary::append]. Value name " + entry.second + " of attribute " + entry.first.first + " already exists with code " + std::to_string(item.first.second));

        m_rightDict.insert_or_assign(std::make_pair(entry.first.first, entry.first.second), entry.second);
    }
    for (const auto& entry: dependentDict.m_reverseDict)
        m_reverseDict.insert_or_assign(entry.first, entry.second);
}

using Dictionaries = RadProto::Dictionaries;
Dictionaries::Dictionaries(const std::string& filePath)
{
    std::ifstream stream(filePath);
    if (!stream)
        throw std::runtime_error("Cannot open dictionary file " + filePath);

    using tokenizer =  boost::tokenizer<boost::char_separator<char>>;
    boost::char_separator<char> sep(" \t");

    std::string line;
    std::string vendorName;

    while (std::getline(stream, line))
    {
        tokenizer tok(line, sep);

        std::vector<std::string> tokens;
        for (const auto& t : tok)
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
            else if (tokens[0] == "$INCLUDE")
            {
                if (tokens[1].substr(0, 1) == "/")
                    append(Dictionaries(tokens[1]));
                else
                    append(Dictionaries(filePath.substr(0, filePath.rfind('/') + 1) + tokens[1]));
            }
        }
    }
}

void Dictionaries::append(const Dictionaries& fillingDictionaries)
{
    m_attributes.append(fillingDictionaries.m_attributes);
    m_vendorNames.append(fillingDictionaries.m_vendorNames);
    m_attributeValues.append(fillingDictionaries.m_attributeValues);
    m_vendorAttributes.append(fillingDictionaries.m_vendorAttributes);
    m_vendorAttributeValues.append(fillingDictionaries.m_vendorAttributeValues);
}

std::string Dictionaries::attributeName(uint32_t code) const
{
    return attributes().name(code);
}

uint32_t Dictionaries::attributeCode(const std::string& name) const
{
    return attributes().code(name);
}

std::string Dictionaries::vendorName(uint32_t code) const
{
    return vendorNames().name(code);
}

uint32_t Dictionaries::vendorCode(const std::string& name) const
{
    return vendorNames().code(name);
}

std::string Dictionaries::vendorAttributeName(const std::string& vendorName, uint32_t code) const
{
    return vendorAttributes().name(vendorName, code);
}

uint32_t Dictionaries::vendorAttributeCode(const std::string& vendorName,const std::string& name) const
{
    return vendorAttributes().code(vendorName, name);
}

std::string Dictionaries::attributeValueName(const std::string& attributeName, uint32_t code) const
{
    return attributeValues().name(attributeName, code);
}

uint32_t Dictionaries::attributeValueCode(const std::string& attributeName, const std::string& name) const
{
    return attributeValues().code(attributeName, name);
}

std::string Dictionaries::vendorAttributeValueName(const std::string& valueName, uint32_t code) const
{
    return vendorAttributeValues().name(valueName, code);
}

uint32_t Dictionaries::vendorAttributeValueCode(const std::string& valueName, const std::string& name) const
{
    return vendorAttributeValues().code(valueName, name);
}
