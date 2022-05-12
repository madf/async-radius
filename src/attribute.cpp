#include "attribute.h"
#include "attribute_types.h"
#include <iostream>

Attribute::Attribute(uint8_t type)
    : m_type(type)
{
}

String::String(uint8_t type)
        : Attribute(type)
{
}

std::string String::value() const
{
    return "1"; //temporarily
}

std::string typeToString(int type)
{
    switch (type)
    {
        case USER_NAME: return "USER_NAME";
        case USER_PASSWORD: return "USER_PASSWORD";
        case CHAP_PASSWORD: return "CHAP_PASSWORD";
        case NAS_IP_ADDRESS: return "NAS_IP_ADDRESS";
        case NAS_PORT: return "NAS_PORT";
        case SERVICE_TYPE: return "SERVICE_TYPE";
        case FRAMED_PROTOCOL: return "FRAMED_PROTOCOL";
        case FRAMED_IP_ADDRESS: return "FRAMED_IP_ADDRESS";
        case FRAMED_IP_NETMASK: return "FRAMED_IP_NETMASK";
        case FRAMED_ROUTING: return "FRAMED_ROUTING";
        case FILTER_ID: return "FILTER_ID";
        case FRAMED_MTU: return "FRAMED_MTU";
        case FRAMED_COMPRESSION: return "FRAMED_COMPRESSION";
        case LOGIN_IP_HOST: return "LOGIN_IP_HOST";
        case LOGIN_SERVICE: return "LOGIN_SERVICE";
        case LOGIN_TCP_PORT: return "LOGIN_TCP_PORT";
        case REPLY_MESSAGE: return "REPLY_MESSAGE";
        case CALLBACK_NUMBER: return "CALLBACK_NUMBER";
        case CALLBACK_ID: return "CALLBACK_ID";
        case FRAMED_ROUTE: return "FRAMED_ROUTE";
        case FRAMED_IPX_NETWORK: return "FRAMED_IPX_NETWORK";
        case STATE: return "STATE";
        case CLASS: return "CLASS";
        case VENDOR_SPECIFIC: return "VENDOR_SPECIFIC";
        case SESSION_TIMEOUT: return "SESSION_TIMEOUT";
        case IDLE_TIMEOUT: return "IDLE_TIMEOUT";
        case TERMINATION_ACTION: return "TERMINATION_ACTION";
        case CALLED_STATION_ID: return "CALLED_STATION_ID";
        case CALLING_STATION_ID: return "CALLING_STATION_ID";
        case NAS_IDENTIFIER: return "NAS_IDENTIFIER";
        case PROXY_STATE: return "PROXY_STATE";
        case LOGIN_LAT_SERVICE: return "LOGIN_LAT_SERVICE";
        case LOGIN_LAT_NODE: return "LOGIN_LAT_NODE";
        case LOGIN_LAT_GROUP: return "LOGIN_LAT_GROUP";
        case FRAMED_APPLETALK_LINK: return "FRAMED_APPLETALK_LINK";
        case FRAMED_APPLETALK_NETWORK: return "FRAMED_APPLETALK_NETWORK";
        case FRAMED_APPLETALK_ZONE: return "FRAMED_APPLETALK_ZONE";
        case CHAP_CHALLENGE: return "CHAP_CHALLENGE";
        case NAS_PORT_TYPE: return "NAS_PORT_TYPE";
        case PORT_LIMIT: return "PORT_LIMIT";
        case LOGIN_LAT_PORT: return "LOGIN_LAT_PORT";
    }
    return "unknown";
}

std::string Attribute::name() const
{
    return typeToString(m_type);
}
