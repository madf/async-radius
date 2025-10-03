#include "utils.h"
#include "attribute_codes.h"
#include <cstdint> //uint8_t, uint32_t

std::string RadProto::byteToHex(uint8_t byte)
{
    static const std::string digits = "0123456789ABCDEF";
    return {digits[byte / 16], digits[byte % 16]};
}
