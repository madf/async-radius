#include "utils.h"

RadProto::Attribute* findAttribute(const std::vector<RadProto::Attribute*>& attributes, RadProto::Attribute_Types type)
{
    for (const auto& b : attributes)
    {
        if (b->type() == type)
            return b;
    }
    return nullptr;
}
