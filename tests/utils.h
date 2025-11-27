#pragma once

#include "attribute_codes.h"
#include "attribute.h"
#include "vendor_attribute.h"

RadProto::Attribute* findAttribute(const std::vector<RadProto::Attribute*>& attributes, RadProto::Attribute_Codes code);

