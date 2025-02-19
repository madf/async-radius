#pragma once

#include "packet.h"
#include <boost/asio.hpp>
#include <cstdint> //uint8_t, uint32_t
#include <array>
#include <functional>
#include <optional>

namespace RadProto
{
    class Socket
    {
        public:
            Socket(boost::asio::io_service& io_service, const std::string& secret, uint16_t port);
            void asyncReceive(const std::function<void(const boost::system::error_code&, const std::optional<Packet>&, const boost::asio::ip::udp::endpoint&)>& callback);
            void asyncSend(const Packet& response, const boost::asio::ip::udp::endpoint& destination, const std::function<void(const boost::system::error_code&)>& callback);
            boost::system::error_code close(boost::system::error_code& ec);

        private:
            void handleReceive(const boost::system::error_code& error, std::size_t bytes, const std::function<void(const boost::system::error_code&, const std::optional<Packet>&, const boost::asio::ip::udp::endpoint&)>& callback);
            void handleSend(const boost::system::error_code& ec, const std::function<void(const boost::system::error_code&)>& callback);

            boost::asio::ip::udp::socket m_socket;
            boost::asio::ip::udp::endpoint m_remoteEndpoint;
            std::array<uint8_t, 4096> m_buffer;
            std::string m_secret;
    };
}
