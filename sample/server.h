#pragma once

#include "socket.h"
#include "packet.h"
#include "dictionaries.h"
#include <boost/asio.hpp>
#include <optional>
#include <cstdint> //uint8_t, uint32_t

class Server
{
    public:
        Server(boost::asio::io_context& io_context, const std::string& secret, uint16_t port, const std::string& filePath);

    private:
        RadProto::Packet makeResponse(const RadProto::Packet& request);
        void handleReceive(const boost::system::error_code& error, const std::optional<RadProto::Packet>& packet, const boost::asio::ip::udp::endpoint& source);
        void handleSend(const boost::system::error_code& ec);
        void startReceive();

        RadProto::Socket m_radius;
        RadProto::Dictionaries m_dictionaries;
};
