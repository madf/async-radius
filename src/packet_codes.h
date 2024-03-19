#pragma once

namespace RadProto
{
    enum Packet_Codes
    {
        ACCESS_REQUEST = 1,
        ACCESS_ACCEPT = 2,
        ACCESS_REJECT = 3,
        ACCOUNTING_REQUEST = 4,
        ACCOUNTING_RESPONSE = 5,
        ACCESS_CHALLENGE = 11,
        STATUS_SERVER = 12,
        STATUS_CLIENT = 13
    };
}
