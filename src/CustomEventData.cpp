#include "CustomEventData.h"

using namespace CustomJSONData;

std::vector<CustomEventCallbackData> CustomEventCallbacks::customEventCallbacks;

void CustomEventCallbacks::AddCustomEventCallback(void (*callback)(CustomJSONData::CustomEventData *), float aheadTime, bool callIfBeforeStartTime) {
    customEventCallbacks.push_back({
        callback, aheadTime, callIfBeforeStartTime, 0
    });
}