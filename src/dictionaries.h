#ifndef DICTIONARIES_H
#define DICTIONARIES_H

#include <string>
#include <map>
#include <cstdint> //uint8_t, uint32_t

class BasicDictionary
{
    public:
        BasicDictionary() = default;
        std::string name(uint8_t type) const;
        uint8_t type(const std::string& name) const;
        void add(uint8_t type, const std::string& name);
    private:
        std::map<uint8_t, std::string> m_rightDict;
        std::map<std::string, uint8_t> m_reverseDict;
};

class DependentDictionary
{
    public:
        DependentDictionary() = default;
        std::string name(const std::string& attributeName, uint8_t type) const;
        uint8_t type(const std::string& attributeName, const std::string& name) const;
        void add(uint8_t type, const std::string& name, const std::string& attributeName);
    private:
        std::map<std::pair<std::string, uint8_t>, std::string> m_rightDict;
        std::map<std::pair<std::string, std::string>, uint8_t> m_reverseDict;
};

class Dictionaries
{
    public:
        Dictionaries() = default;
        const BasicDictionary& attributes() const { return m_attributes; }
        const BasicDictionary& vendorNames() const { return m_vendorNames; }
        const DependentDictionary& attributeValues() const { return m_attributeValues; }
        const DependentDictionary& vendorAttributes() const { return m_vendorAttributes; }
        const DependentDictionary& vendorAttributeValues() const { return m_vendorAttributeValues; }
    private:
        BasicDictionary m_attributes;
        BasicDictionary m_vendorNames;
        DependentDictionary m_attributeValues;
        DependentDictionary m_vendorAttributes;
        DependentDictionary m_vendorAttributeValues;
};
#endif
