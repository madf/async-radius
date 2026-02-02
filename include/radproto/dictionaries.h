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
            std::string type(uint32_t code) const;
            std::string type(const std::string& name) const;

            void add(uint32_t code, const std::string& name, const std::string& type);
            void append(const BasicDictionary& basicDict);
            bool findByName(const std::string& name) const;
            bool findByCode(uint32_t code) const;
        private:
            std::map<uint32_t, std::pair<std::string, std::string>> m_rightDict;
            std::map<std::string, std::pair<uint32_t, std::string>> m_reverseDict;
    };

    class VendorDictionary
    {
        public:
            VendorDictionary() = default;
            std::string name(uint32_t code) const;
            uint32_t code(const std::string& name) const;

            void add(uint32_t code, const std::string& name);
            void append(const VendorDictionary& vendorDict);
            bool findByName(const std::string& name) const;
            bool findByCode(uint32_t code) const;
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
            bool findByName(const std::string& dependencyName, const std::string& name) const;
            bool findByCode(const std::string& dependencyName, uint32_t code) const;
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
            const VendorDictionary& vendorNames() const { return m_vendorNames; }
            const DependentDictionary& attributeValues() const { return m_attributeValues; }
            const DependentDictionary& vendorAttributes() const { return m_vendorAttributes; }
            const DependentDictionary& vendorAttributeValues() const { return m_vendorAttributeValues; }

            std::string attributeName(uint32_t code) const;
            uint32_t attributeCode(const std::string& name) const;
            std::string attributeType(uint32_t code) const;
            std::string attributeType(const std::string& name) const;
            bool attributeFindByName(const std::string& name) const;
            bool attributeFindByCode(uint32_t) const;

            std::string vendorName(uint32_t code) const;
            uint32_t vendorCode(const std::string& name) const;
            bool vendorFindByName(const std::string& name) const;
            bool vendorFindByCode(uint32_t) const;

            std::string vendorAttributeName(const std::string& vendorName, uint32_t code) const;
            uint32_t vendorAttributeCode(const std::string& vendorName, const std::string& name) const;

            std::string attributeValueName(const std::string& attributeName, uint32_t code) const;
            uint32_t attributeValueCode(const std::string& attributeName, const std::string& name) const;

            bool attributeValuefindByName(const std::string& attributeName, const std::string& name) const;
            bool attributeValuefindByCode(const std::string& dependencyName, uint32_t code) const;

            std::string vendorAttributeValueName(const std::string& valueName, uint32_t code) const;
            uint32_t vendorAttributeValueCode(const std::string& valueName, const std::string& name) const;

        private:
            BasicDictionary m_attributes;
            VendorDictionary m_vendorNames;
            DependentDictionary m_attributeValues;
            DependentDictionary m_vendorAttributes;
            DependentDictionary m_vendorAttributeValues;
    };
}
