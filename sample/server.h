#ifndef SERVER_H
#define SERVER_H

#include "radius.h"
#include "packet.h"
#include "dictionaries.h"
#include <boost/asio.hpp>
#include <optional>

class Server
{
    public:
        Server(boost::asio::io_service& io_service, const std::string& secret, const std::string& filePath);

    private:
        RadProto::Packet makeResponse(RadProto::Packet& request);
        void handleReceive(const boost::system::error_code& error, const std::optional<RadProto::Packet>& packet);
        void handleSend(const boost::system::error_code& ec);
        void startReceive();

        RadProto::Radius m_radius;
        RadProto::Dictionaries m_dictionaries;
};

#endif