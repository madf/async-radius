#include "server.h"
#include "error.h"
#include "packet_codes.h"

using boost::asio::ip::udp;
using boost::system::error_code;

namespace pls = std::placeholders;

std::string packetTypeToString(int type)
{
    switch (type)
    {
        case ACCESS_REQUEST: return "ACCESS_REQUEST";
        case ACCESS_ACCEPT: return "ACCESS_ACCEPT";
        case ACCESS_REJECT: return "ACCESS_REJECT";
        case ACCOUNTING_REQUEST: return "ACCOUNTING_REQUEST";
        case ACCOUNTING_RESPONSE: return "ACCOUNTING_RESPONSE";
        case ACCESS_CHALLENGE: return "ACCESS_CHALLENGE";
        case STATUS_SERVER: return "STATUS_SERVER";
        case STATUS_CLIENT: return "STATUS_CLIENT";
    }
    return "uncnown";
}

Server::Server(boost::asio::io_service& io_service, const std::string& secret)
    : m_socket(io_service, udp::endpoint(udp::v4(), 9999)),
      m_secret(secret)
{
}

void Server::asyncReceive(const std::function<void(const error_code&, const std::optional<Packet>&)>& callback)
{
    m_socket.async_receive_from(boost::asio::buffer(m_recvBuffer), m_remoteEndpoint,
       [this, callback](const error_code& error, std::size_t bytes) {handleReceive(error, bytes, callback);});
}

void Server::asyncSend(const Packet& response, const std::function<void(const error_code&)>& callback)
{
    m_socket.async_send_to(boost::asio::buffer(response.makeSendBuffer(m_secret)), m_remoteEndpoint,
       [this, callback](const error_code& ec, std::size_t /*bytesTransferred*/) {handleSend(ec, callback);});
}

void Server::handleReceive(const error_code& error, std::size_t bytes, std::function<void(const error_code&, const std::optional<Packet>&)> callback)
{
        std::optional<Packet> oEmpty;
        std::optional<Packet> oRequest;

        if (bytes < 20)
            callback(lib::Error::numberBytesLess20, oEmpty);

    try
    {
        const Packet request = Packet(m_recvBuffer, bytes, m_secret);
        oRequest = request;
    }
    catch (const std::runtime_error& exception)
    {
        if (std::string (exception.what()) == "lib::Error::numberBytesLess20")
            callback(lib::Error::numberBytesLess20, oEmpty);

        if (std::string (exception.what()) == "lib::Error::requestLengthShort")
            callback(lib::Error::requestLengthShort, oEmpty);

        if (std::string (exception.what()) == "lib::Error::eapMessageAttributeError")
            callback(lib::Error::eapMessageAttributeError, oEmpty);
    }
    callback(error, oRequest);
}

void Server::handleSend(const error_code& ec, std::function<void(const error_code&)> callback)
{
    callback(ec);
}
