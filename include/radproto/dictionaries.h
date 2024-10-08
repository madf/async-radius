#pragma once

#include <string>
#include <map>
#include <cstdint> //uint8_t, uint32_t

namespace RadProto
{
    class BasicDictionary
    {
        public:
            BasicDictionary() = default;
            std::string name(uint32_t code) const;
            uint32_t code(const std::string& name) const;
            void add(uint32_t code, const std::string& name);
            void append(const BasicDictionary& basicDict);
        private:
            std::map<uint32_t, std::string> m_rightDict;
            std::map<std::string, uint32_t> m_reverseDict;
    };

    class DependentDictionary
    {
        public:
            DependentDictionary() = default;
            std::string name(const std::string& dependencyName, uint32_t code) const;
            uint32_t code(const std::string& dependencyName, const std::string& name) const;
            void add(uint32_t code, const std::string& name, const std::string& dependencyName);
            void append(const DependentDictionary& dependentDict);
        private:
            std::map<std::pair<std::string, uint32_t>, std::string> m_rightDict;
            std::map<std::pair<std::string, std::string>, uint32_t> m_reverseDict;
    };

    class Dictionaries
    {
        public:
            Dictionaries(const std::string& filePath);
            void append(const Dictionaries& fillingDictionaries);
            const BasicDictionary& attributes() const { return m_attributes; }
            const BasicDictionary& vendorNames() const { return m_vendorNames; }
            const DependentDictionary& attributeValues() const { return m_attributeValues; }
            const DependentDictionary& vendorAttributes() const { return m_vendorAttributes; }
            const DependentDictionary& vendorAttributeValues() const { return m_vendorAttributeValues; }

            std::string attributeName(uint32_t code) const;
            uint32_t attributeCode(const std::string& name) const;

            std::string vendorName(uint32_t code) const;
            uint32_t vendorCode(const std::string& name) const;

            std::string vendorAttributeName(const std::string& vendorName, uint32_t code) const;
            uint32_t vendorAttributeCode(const std::string& vendorName, const std::string& name) const;

            std::string attributeValueName(const std::string& attributeName, uint32_t code) const;
            uint32_t attributeValueCode(const std::string& attributeName, const std::string& name) const;

            std::string vendorAttributeValueName(const std::string& valueName, uint32_t code) const;
            uint32_t vendorAttributeValueCode(const std::string& valueName, const std::string& name) const;

        private:
            BasicDictionary m_attributes;
            BasicDictionary m_vendorNames;
            DependentDictionary m_attributeValues;
            DependentDictionary m_vendorAttributes;
            DependentDictionary m_vendorAttributeValues;
    };
}
