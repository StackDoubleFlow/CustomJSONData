#include "CustomEventData.h"

using namespace CustomJSONData;

std::vector<CustomEventCallbackData> CustomEventCallbacks::customEventCallbacks;

void CustomEventCallbacks::AddCustomEventCallback(void (*callback)(CustomJSONData::CustomEventData *), float aheadTime, bool callIfBeforeStartTime) {
    CJDLogger::GetLogger().debug("Added custom event callback at %p", callback);
    customEventCallbacks.push_back({
        callback, aheadTime, callIfBeforeStartTime, 0
    });
}