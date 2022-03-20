#include "CustomEventData.h"

using namespace CustomJSONData;

DEFINE_TYPE(CustomJSONData, CustomEventData)

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
