#include "utils.h"

RadProto::Attribute* findAttribute(const std::vector<RadProto::Attribute*>& attributes, RadProto::Attribute_Codes code)
{
    for (const auto& b : attributes)
    {
        if (b->code() == code)
            return b;
    }
    return nullptr;
}
