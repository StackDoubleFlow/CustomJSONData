#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "custom-types/shared/macros.hpp"

#include "CJDLogger.h"

namespace GlobalNamespace {
class BeatmapObjectCallbackController;
}

namespace CustomJSONData {

class CustomEventData {
public:
    std::string_view type;
    float time;
    rapidjson::Value *data;
};

struct CustomEventCallbackData {
    void (*callback)(GlobalNamespace::BeatmapObjectCallbackController *callbackController,
                     CustomJSONData::CustomEventData *);
    float aheadTime;
    bool callIfBeforeStartTime;
    int nextEventIndex;
};

class CustomEventCallbacks {
public:
    static std::vector<CustomEventCallbackData> customEventCallbacks;

    static void AddCustomEventCallback(
        void (*callback)(GlobalNamespace::BeatmapObjectCallbackController *callbackController,
                         CustomJSONData::CustomEventData *),
        float aheadTime = 0, bool callIfBeforeStartTime = false);
};

} // end namespace CustomJSONData