// This is my most favorite file in this project

#include "JSONWrapper.h"

using namespace CustomJSONData;

DEFINE_TYPE(JSONWrapper);

JSONWrapper::~JSONWrapper() {
    delete value;
}