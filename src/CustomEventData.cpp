#include <vector>
#include <utility>
#include "CustomEventData.h"

#include "CustomJSONDataHooks.h"

#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"

#include "GlobalNamespace/CallbacksInTime.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;

DEFINE_TYPE(CustomJSONData, CustomEventData)
DEFINE_TYPE(CustomJSONData, CustomBeatmapDataCallbackWrapper)

std::vector<CustomEventCallbackData> CustomEventCallbacks::customEventCallbacks;

void CustomEventData::ctor(float time, void *type, size_t typeHash, void *data) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapDataItem*), ".ctor", 4);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, time, 0, 0, (BeatmapDataItemType)2));
    INVOKE_CTOR();
    this->typeHash = typeHash;
    this->type = *static_cast<decltype(this->type)*>(type);
    this->data = static_cast<decltype(this->data)>(data);
}

CustomEventData *CustomEventData::GetCopy() {
    auto copy = CustomJSONData::CustomEventData::New_ctor(this->time, (void*) &this->type, typeHash, (void*) this->data);
    return copy;
}

void CustomBeatmapDataCallbackWrapper::ctor() {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapDataCallbackWrapper*), ".ctor", 3);
    PAPER_IL2CPP_CATCH_HANDLER(
        il2cpp_utils::RunMethodRethrow(this, ctor, 0.0f, csTypeOf(CustomEventData*), ArrayW<int>((il2cpp_array_size_t) 0));
    )
    INVOKE_CTOR();
}

void CustomBeatmapDataCallbackWrapper::CallCallback(BeatmapDataItem * item) {
    if (redirectEvent) {
        redirectEvent(controller, item);
    } else {
        static auto CustomEventDataKlass = classof(CustomEventData*);
        CRASH_UNLESS(item->klass == CustomEventDataKlass);

        for (auto const &customEvents: CustomEventCallbacks::customEventCallbacks) {
            customEvents.callback(controller, static_cast<CustomEventData *>(item));
        }
    }
}

void CustomEventCallbacks::AddCustomEventCallback(
        void (*callback)(GlobalNamespace::BeatmapCallbacksController *, CustomJSONData::CustomEventData *)) {
    customEventCallbacks.emplace_back(callback);
}

void CustomEventCallbacks::RegisterCallbacks(GlobalNamespace::BeatmapCallbacksController *callbackController) {
    auto wrapper = CustomBeatmapDataCallbackWrapper::New_ctor();
    wrapper->controller = callbackController;
    callbackController->callbacksInTimes->get_Item(0)->AddCallback(wrapper);
}
