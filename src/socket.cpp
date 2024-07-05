#include "socket.h"
#include "error.h"
#include "packet_codes.h"

using boost::asio::ip::udp;
using boost::system::error_code;

namespace pls = std::placeholders;

std::string packetTypeToString(int type)
{
    switch (type)
    {
        case RadProto::ACCESS_REQUEST: return "ACCESS_REQUEST";
        case RadProto::ACCESS_ACCEPT: return "ACCESS_ACCEPT";
        case RadProto::ACCESS_REJECT: return "ACCESS_REJECT";
        case RadProto::ACCOUNTING_REQUEST: return "ACCOUNTING_REQUEST";
        case RadProto::ACCOUNTING_RESPONSE: return "ACCOUNTING_RESPONSE";
        case RadProto::ACCESS_CHALLENGE: return "ACCESS_CHALLENGE";
        case RadProto::STATUS_SERVER: return "STATUS_SERVER";
        case RadProto::STATUS_CLIENT: return "STATUS_CLIENT";
    }
    return "uncnown";
}

using Socket = RadProto::Socket;
Socket::Socket(boost::asio::io_service& io_service, const std::string& secret, uint16_t port)
    : m_socket(io_service, udp::endpoint(udp::v4(), port)),
      m_secret(secret)
{
}

void Socket::asyncReceive(const std::function<void(const error_code&, const std::optional<Packet>&)>& callback)
{
    m_socket.async_receive_from(boost::asio::buffer(m_recvBuffer), m_remoteEndpoint,
       [this, callback](const error_code& error, std::size_t bytes) {handleReceive(error, bytes, callback);});
}

void Socket::asyncSend(const Packet& response, const std::function<void(const error_code&)>& callback)
{
    const std::vector<uint8_t> vResponse = response.makeSendBuffer(m_secret);
    std::copy(vResponse.begin(), vResponse.end(), m_recvBuffer.begin());

    m_socket.async_send_to(boost::asio::buffer(m_recvBuffer, vResponse.size()), m_remoteEndpoint,
       [this, callback](const error_code& ec, std::size_t /*bytesTransferred*/) {handleSend(ec, callback);});
}

void Socket::handleReceive(const error_code& error, std::size_t bytes, const std::function<void(const error_code&, const std::optional<Packet>&)>& callback)
{
    if (error)
        callback(error, std::nullopt);
    if (bytes < 20)
        callback(Error::numberOfBytesIsLessThan20, std::nullopt);
    try
    {
        callback(error, std::make_optional<Packet>(m_recvBuffer, bytes, m_secret));
    }
    catch (const Exception& exception)
    {
        callback(exception.getErrorCode(), std::nullopt);
    }
}

void Socket::handleSend(const error_code& ec, const std::function<void(const error_code&)>& callback)
{
    callback(ec);
}
