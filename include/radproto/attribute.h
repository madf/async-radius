#pragma once

#include <string>
#include <vector>
#include <array>
#include <cstdint> //uint8_t, uint32_t

namespace RadProto
{
    class Attribute
    {
        public:
            Attribute(uint8_t type);
            virtual ~Attribute() = default;
            uint8_t type() const { return m_type; }
            virtual std::string toString() const = 0;
            virtual std::vector<uint8_t> toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const = 0;
            virtual Attribute* clone() const = 0;
        private:
            uint8_t m_type;
    };

    class String: public Attribute
    {
        public:
            String(uint8_t type, const uint8_t* data, size_t size);
            String(uint8_t type, const std::string& string);
            std::string toString() const override { return m_value; }
            std::vector<uint8_t> toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const override;
            String* clone() const override;
        private:
            std::string m_value;
    };

    class Integer: public Attribute
    {
        public:
            Integer(uint8_t type, const uint8_t* data, size_t size);
            Integer(uint8_t type, uint32_t value);
            std::string toString() const override;
            std::vector<uint8_t> toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const override;
            Integer* clone() const override;
        private:
            uint32_t m_value;
    };

    class IpAddress : public Attribute
    {
        public:
            IpAddress(uint8_t type, const uint8_t* data, size_t size);
            IpAddress(uint8_t type, const std::array<uint8_t, 4>& address);
            std::string toString() const override;
            std::vector<uint8_t> toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const override;
            IpAddress* clone() const override;
        private:
            std::array<uint8_t, 4> m_value;
    };

    class Encrypted : public Attribute
    {
        public:
            Encrypted (uint8_t type, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth);
            Encrypted(uint8_t type, const std::string& password);
            std::string toString() const override { return m_value; }
            std::vector<uint8_t> toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const override;
            Encrypted* clone() const override;
        private:
            std::string m_value;
    };

    class Bytes: public Attribute
    {
        public:
            Bytes(uint8_t type, const uint8_t* data, size_t size);
            Bytes(uint8_t type, const std::vector<uint8_t>& bytes);
            std::string toString() const override;
            std::vector<uint8_t> toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const override;
            Bytes* clone() const override;
        private:
            std::vector<uint8_t> m_value;
    };

    class ChapPassword: public Attribute
    {
        public:
            ChapPassword(uint8_t type, const uint8_t* data, size_t size);
            ChapPassword(uint8_t type, uint8_t chapId, const std::vector<uint8_t>& chapValue);
            std::string toString() const override;
            uint8_t chapId() const { return m_chapId; }
            std::vector<uint8_t> chapValue() const { return m_value; }
            std::vector<uint8_t> toVector(const std::string& secret, const std::array<uint8_t, 16>& auth) const override;
            ChapPassword* clone() const override;
        private:
            uint8_t m_chapId;
            std::vector<uint8_t> m_value;
    };
}
