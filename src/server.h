#ifndef SERVER_H
#define SERVER_H

#include "packet.h"
#include "dictionaries.h"
#include <boost/asio.hpp>
#include <array>
#include <cstdint> //uint8_t, uint32_t

class Server
{
    public:
        Server(boost::asio::io_service& io_service, const std::string& secret);

    private:
        void startReceive();
        void handleReceive(const boost::system::error_code& error, std::size_t bytes);
        void handleSend(const boost::system::error_code& error, std::size_t bytes_transferred);
        Packet makeResponse(const Packet& request);
        const Dictionaries& dictionaries() const { return m_dictionaries; }
        void printPacket(const Packet& p);

        boost::asio::ip::udp::socket m_socket;
        boost::asio::ip::udp::endpoint m_remoteEndpoint;
        std::array<uint8_t, 4096> m_recvBuffer;
        std::string m_secret;
        Dictionaries m_dictionaries;
};

#endif
