#ifndef DICTIONARIES_H
#define DICTIONARIES_H

#include <string>
#include <map>
#include <cstdint> //uint8_t, uint32_t

class BasicDictionary
{
    public:
        BasicDictionary(uint8_t type, const std::string name);
        uint8_t getType() const;
        std::string getName() const;
        void add();
    private:
        uint8_t m_type;
        std::string m_name;
        std::map<uint8_t, std::string> m_rightDict;
        std::map<std::string, uint8_t> m_reverseDict;
};
#endif
