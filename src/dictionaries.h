#ifndef DICTIONARIES_H
#define DICTIONARIES_H

#include <string>
#include <map>
#include <cstdint> //uint8_t, uint32_t

class BasicDictionary
{
    public:
        BasicDictionary();
        std::string getName(uint8_t type) const;
        uint8_t getType(const std::string& name) const;
        void add(uint8_t type, const std::string& name);
    private:
        std::map<uint8_t, std::string> m_rightDict;
        std::map<std::string, uint8_t> m_reverseDict;
};

class DependentDictionary
{
    public:
        DependentDictionary();
        std::string getName(const std::string& attributeName, uint8_t type) const;
        uint8_t getType(const std::string& attributeName, const std::string& name) const;
        void add(uint8_t type, const std::string& name, const std::string& attributeName);
    private:
        std::map<std::pair<std::string, uint8_t>, std::string> m_rightDict;
        std::map<std::pair<std::string, std::string>, uint8_t> m_reverseDict;
};

class Dictionaries
{
    public:
        Dictionaries();
        const BasicDictionary& getAttributes() const;
        const BasicDictionary& getVendorNames() const;
        const DependentDictionary& getAttributeValues() const;
        const DependentDictionary& getVendorAttributes() const;
        const DependentDictionary& getVendorAttributeValues() const;
    private:
        BasicDictionary m_attributes;
        BasicDictionary m_vendorNames;
        DependentDictionary m_attributeValues;
        DependentDictionary m_vendorAttributes;
        DependentDictionary m_vendorAttributeValues;
};
#endif
