#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <vector>
#include <array>
#include <cstdint> //uint8_t, uint32_t

class Attribute
{
    public:
        Attribute(uint8_t type);
        virtual ~Attribute() = default;
        std::string name() const;
        uint8_t type() const;
        virtual std::string value() const = 0;
        virtual std::vector<uint8_t> toVector(const std::string& secret, std::array<uint8_t, 16> auth) const = 0;

    private:
        uint8_t m_type;
};

class String: public Attribute
{
    public:
        String(uint8_t type, const uint8_t* data, size_t size);
        String(uint8_t type, const std::string string);
        std::string value() const override;
        std::vector<uint8_t> toVector(const std::string& secret, std::array<uint8_t, 16> auth) const override;
    private:
        std::string m_value;
};

class Integer: public Attribute
{
    public:
        Integer(uint8_t type, const uint8_t* data, size_t size);
        Integer(uint8_t type, uint32_t value);
        std::string value() const override;
        std::vector<uint8_t> toVector(const std::string& secret, std::array<uint8_t, 16> auth) const override;
    private:
        uint32_t m_value;
};

class IpAddress : public Attribute
{
    public:
        IpAddress(uint8_t type, const uint8_t* data, size_t size);
        IpAddress(uint8_t type, std::array<uint8_t, 4> address);
        std::string value() const override;
        std::vector<uint8_t> toVector(const std::string& secret, std::array<uint8_t, 16> auth) const override;
    private:
        std::array<uint8_t, 4> m_value;
};

class Encrypted : public Attribute
{
    public:
        Encrypted (uint8_t type, const uint8_t* data, size_t size, std::string secret, std::array<uint8_t, 16> auth);
        Encrypted(uint8_t type, std::string password);
        std::string value() const override;
        std::vector<uint8_t> toVector(const std::string& secret, std::array<uint8_t, 16> auth) const override;
    private:
        std::string m_value;
};

class Bytes: public Attribute
{
    public:
        Bytes(uint8_t type, const uint8_t* data, size_t size);
        Bytes(uint8_t type, const std::vector<uint8_t>& bytes);
        std::string value() const override;
        std::vector<uint8_t> toVector(const std::string& secret, std::array<uint8_t, 16> auth) const override;
    private:
        std::vector<uint8_t> m_value;
};
#endif
