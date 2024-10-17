#pragma once

#include "attribute_types.h"
#include "attribute.h"
#include "vendor_attribute.h"

RadProto::Attribute* findAttribute(const std::vector<RadProto::Attribute*>& attributes, RadProto::Attribute_Types type);

