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
        case 1: return "USER_NAME";
        case 2: return "USER_PASSWORD";
        case 3: return "CHAP_PASSWORD";
        case 4: return "NAS_IP_ADDRESS";
        case 5: return "NAS_PORT";
        case 6: return "SERVICE_TYPE";
        case 7: return "FRAMED_PROTOCOL";
        case 8: return "FRAMED_IP_ADDRESS";
        case 9: return "FRAMED_IP_NETMASK";
        case 10: return "FRAMED_ROUTING";
        case 11: return "FILTER_ID";
        case 12: return "FRAMED_MTU";
        case 13: return "FRAMED_COMPRESSION";
        case 14: return "LOGIN_IP_HOST";
        case 15: return "LOGIN_SERVICE";
        case 16: return "LOGIN_TCP_PORT";
        case 17: return "REPLY_MESSAGE";
        case 19: return "CALLBACK_NUMBER";
        case 20: return "CALLBACK_ID";
        case 22: return "FRAMED_ROUTE";
        case 23: return "FRAMED_IPX_NETWORK";
        case 24: return "STATE";
        case 25: return "CLASS";
        case 26: return "VENDOR_SPECIFIC";
        case 27: return "SESSION_TIMEOUT";
        case 28: return "IDLE_TIMEOUT";
        case 29: return "TERMINATION_ACTION";
        case 30: return "CALLED_STATION_ID";
        case 31: return "CALLING_STATION_ID";
        case 32: return "NAS_IDENTIFIER";
        case 33: return "PROXY_STATE";
        case 34: return "LOGIN_LAT_SERVICE";
        case 35: return "LOGIN_LAT_NODE";
        case 36: return "LOGIN_LAT_GROUP";
        case 37: return "FRAMED_APPLETALK_LINK";
        case 38: return "FRAMED_APPLETALK_NETWORK";
        case 39: return "FRAMED_APPLETALK_ZONE";
        case 60: return "CHAP_CHALLENGE";
        case 61: return "NAS_PORT_TYPE";
        case 62: return "PORT_LIMIT";
        case 63: return "LOGIN_LAT_PORT";
    }
    return "unknown";
}

std::string Attribute::name() const
{
    return typeToString(m_type);
}
