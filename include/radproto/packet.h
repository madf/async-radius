#ifndef PACKET_H
#define PACKET_H

#include "attribute.h"
#include "vendor_attribute.h"
#include <array>
#include <vector>
#include <string>
#include <cstdint> //uint8_t, uint32_t

namespace RadProto
{
    class Packet
    {
        public:
            Packet(const std::array<uint8_t, 4096>& m_recvBuffer, size_t bytes, const std::string& secret);
            Packet(uint8_t type, uint8_t id, const std::array<uint8_t, 16>& auth, const std::vector<Attribute*>& attributes, const std::vector<VendorSpecific*>& vendorSpecific);
            Packet(Packet const & dc);
            ~Packet();
            uint8_t type() const { return m_type; }
            uint8_t id() const { return m_id; };
            const std::array<uint8_t, 16>& auth() const { return m_auth; }
            const std::vector<Attribute*>& attributes() const { return m_attributes; }
            const std::vector<VendorSpecific*>& vendorSpecific() const { return m_vendorSpecific; }
            const std::vector<uint8_t> makeSendBuffer(const std::string& secret) const;
            Attribute* makeAttribute(uint8_t type, const uint8_t* data, size_t size, const std::string& secret, const std::array<uint8_t, 16>& auth);

        private:
            uint8_t m_type;
            uint8_t m_id;
            std::array<uint8_t, 16> m_auth;
            std::vector<Attribute*> m_attributes;
            std::vector<VendorSpecific*> m_vendorSpecific;
            Attribute* attributePtr;
    };
}

#endif
