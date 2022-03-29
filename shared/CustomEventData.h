#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "custom-types/shared/macros.hpp"

#include "CJDLogger.h"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BeatmapDataCallbackWrapper.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"

namespace GlobalNamespace {
class BeatmapCallbacksController;
}

#define FindMethodGetter(methodName) \
::il2cpp_utils::il2cpp_type_check::MetadataGetter<methodName>::get()

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomEventData, GlobalNamespace::BeatmapDataItem,
public:
      DECLARE_CTOR(ctor, float time, /* std::string_view*/ void* type, size_t typeHash, /* rapidjson::Value */ void* data);

    DECLARE_OVERRIDE_METHOD(CustomEventData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

        public:
  std::string_view type;
  size_t typeHash;
  rapidjson::Value const* data;
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapDataCallbackWrapper, GlobalNamespace::BeatmapDataCallbackWrapper,
      DECLARE_CTOR(ctor);

        DECLARE_SIMPLE_DTOR();

      DECLARE_OVERRIDE_METHOD(void, CallCallback, FindMethodGetter(&GlobalNamespace::BeatmapDataCallbackWrapper::CallCallback), GlobalNamespace::BeatmapDataItem* item);

      DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapCallbacksController *, controller);

      std::function<void(GlobalNamespace::BeatmapCallbacksController * controller, GlobalNamespace::BeatmapDataItem* item)> redirectEvent;
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
    void (*callback)(GlobalNamespace::BeatmapCallbacksController *callbackController,
                     CustomJSONData::CustomEventData *);

    constexpr CustomEventCallbackData(void (*callback)(GlobalNamespace::BeatmapCallbacksController *, CustomEventData *))
            : callback(callback) {}
};

class CustomEventCallbacks {
public:
    static std::vector<CustomEventCallbackData> customEventCallbacks;

    static void AddCustomEventCallback(void (*callback)(GlobalNamespace::BeatmapCallbacksController *callbackController,CustomJSONData::CustomEventData *));

    static void RegisterCallbacks(GlobalNamespace::BeatmapCallbacksController* callbackController);
};

} // end namespace CustomJSONData