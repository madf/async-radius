#ifndef SERVER_H
#define SERVER_H

#include "packet.h"
#include <boost/asio.hpp>
#include <cstdint> //uint8_t, uint32_t
#include <array>
#include <functional>
#include <optional>

namespace RadProto
{
    class Server
    {
        public:
            Server(boost::asio::io_service& io_service, const std::string& secret);
            void asyncReceive(const std::function<void(const boost::system::error_code&, const std::optional<Packet>&)>& callback);
            void asyncSend(const Packet& response, const std::function<void(const boost::system::error_code&)>& callback);

        private:
            void handleReceive(const boost::system::error_code& error, std::size_t bytes, const std::function<void(const boost::system::error_code&, const std::optional<Packet>&)>& callback);
            void handleSend(const boost::system::error_code& ec, const std::function<void(const boost::system::error_code&)>& callback);

            boost::asio::ip::udp::socket m_socket;
            boost::asio::ip::udp::endpoint m_remoteEndpoint;
            std::array<uint8_t, 4096> m_recvBuffer;
            std::string m_secret;
    };
}

#endif
