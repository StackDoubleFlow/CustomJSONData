// This is my most favorite file in this project

#include "JSONWrapper.h"

using namespace CustomJSONData;

DEFINE_TYPE(CustomJSONData, JSONWrapper);
DEFINE_TYPE(CustomJSONData, JSONWrapperUTF16);
DEFINE_TYPE(CustomJSONData, DocumentWrapper);

JSONWrapper *JSONWrapper::GetCopy() {
    auto copy = JSONWrapper::New_ctor();

    copy->value = value;
    copy->associatedData = associatedData;

    return copy;
}

JSONWrapperUTF16 *JSONWrapperUTF16::GetCopy() {
    auto copy = JSONWrapperUTF16::New_ctor();

    copy->value = value;
    copy->associatedData = associatedData;

    return copy;
}
