// This is my most favorite file in this project

#include "JSONWrapper.h"

DEFINE_TYPE(CustomJSONData::JSONWrapper);

void CustomJSONData::JSONWrapper::Finalize() {
    delete value;
    associatedData.clear();
}