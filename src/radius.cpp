#include "radius.h"
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

using Radius = RadProto::Radius;
Radius::Radius(boost::asio::io_service& io_service, const std::string& secret)
    : m_socket(io_service, udp::endpoint(udp::v4(), 9999)),
      m_secret(secret)
{
}

void Radius::asyncReceive(const std::function<void(const error_code&, const std::optional<Packet>&)>& callback)
{
    m_socket.async_receive_from(boost::asio::buffer(m_recvBuffer), m_remoteEndpoint,
       [this, callback](const error_code& error, std::size_t bytes) {handleReceive(error, bytes, callback);});
}

void Radius::asyncSend(const Packet& response, const std::function<void(const error_code&)>& callback)
{
    m_socket.async_send_to(boost::asio::buffer(response.makeSendBuffer(m_secret)), m_remoteEndpoint,
       [this, callback](const error_code& ec, std::size_t /*bytesTransferred*/) {handleSend(ec, callback);});
}

void Radius::handleReceive(const error_code& error, std::size_t bytes, const std::function<void(const error_code&, const std::optional<Packet>&)>& callback)
{
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

void Radius::handleSend(const error_code& ec, const std::function<void(const error_code&)>& callback)
{
    callback(ec);
}
