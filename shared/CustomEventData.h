#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "custom-types/shared/macros.hpp"

#include "CJDLogger.h"
#include "GlobalNamespace/BeatmapDataItem.hpp"

namespace GlobalNamespace {
class BeatmapObjectCallbackController;
}

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomEventData, GlobalNamespace::BeatmapDataItem,
public:
      DECLARE_CTOR(ctor, float time, /* std::string_view*/ void* type, size_t typeHash, /* rapidjson::Value */ void* data);

    DECLARE_OVERRIDE_METHOD(CustomEventData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

        public:
  std::string_view type;
  size_t typeHash;
  rapidjson::Value const* data;
)

namespace CustomJSONData {

class CustomEventSaveData {
public:
    std::string_view type;
    size_t typeHash;
    float time;
    rapidjson::Value const* data;

    constexpr CustomEventSaveData(std::string_view type, size_t typeHash, float time, rapidjson::Value const* data)
            : type(type),
              typeHash(typeHash),
              time(time),
              data(data) {}

    constexpr CustomEventSaveData(std::string_view type, float time, rapidjson::Value const* data)
            : type(type),
              time(time),
              data(data) {
        typeHash = std::hash<std::string_view>()(type);
    }
};

struct CustomEventCallbackData {
    void (*callback)(GlobalNamespace::BeatmapObjectCallbackController *callbackController,
                     CustomJSONData::CustomEventSaveData *);
    float aheadTime;
    bool callIfBeforeStartTime;
    int nextEventIndex;
};

class CustomEventCallbacks {
public:
    static std::vector<CustomEventCallbackData> customEventCallbacks;

    static void AddCustomEventCallback(
        void (*callback)(GlobalNamespace::BeatmapObjectCallbackController *callbackController,
                         CustomJSONData::CustomEventSaveData *),
        float aheadTime = 0, bool callIfBeforeStartTime = true);
};

} // end namespace CustomJSONData