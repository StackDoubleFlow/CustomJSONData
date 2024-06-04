#include <vector>
#include <utility>
#include "CustomEventData.h"

#include "CJDLogger.h"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"

#include "CustomJSONDataHooks.h"

#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"

#include "GlobalNamespace/CallbacksInTime.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;

DEFINE_TYPE(CustomJSONData, CustomEventData)
DEFINE_TYPE(CustomJSONData, CustomBeatmapDataCallbackWrapper)

CJD_MOD_EXPORT std::vector<CustomEventCallbackData> CustomEventCallbacks::customEventCallbacks;
CJD_MOD_EXPORT SafePtr<System::Collections::Generic::LinkedListNode_1<GlobalNamespace::BeatmapDataItem*>>
    CustomEventCallbacks::firstNode;

void CustomEventData::ctor(float time, void* type, size_t typeHash, void* data) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapDataItem*), ".ctor", 4);
  il2cpp_utils::RunMethodRethrow(this, ctor, time, 0, 0, BeatmapDataItemType(2));
  BeatmapDataItem::_time_k__BackingField = time;
  BeatmapDataItem::type = 2;
  this->typeHash = typeHash;
  this->type = *static_cast<decltype(this->type)*>(type);
  this->data = static_cast<decltype(this->data)>(data);
  CRASH_UNLESS(data);
}

CustomEventData* CustomEventData::GetCopy() {
  auto* copy = CustomJSONData::CustomEventData::New_ctor(this->time, (void*)&this->type, typeHash, (void*)this->data);
  return copy;
}

void CustomBeatmapDataCallbackWrapper::ctor() {
  INVOKE_CTOR();
  reinterpret_cast<BeatmapDataCallbackWrapper*>(this)->_ctor(0.0F, csTypeOf(CustomEventData*),
                                                             ArrayW<int>((il2cpp_array_size_t)0));

  redirectEvent = nullptr;
}

void CustomBeatmapDataCallbackWrapper::CallCallback(BeatmapDataItem* item) {
  CJDLogger::Logger.fmtLog<LogLevel::INF>("CALLING CUSTOM CALLBACK! {}", il2cpp_utils::ClassStandardName(item->klass, true));
  static auto CustomEventDataKlass = classof(CustomEventData*);
  // CRASH_UNLESS(item->klass == CustomEventDataKlass);



  PAPER_IL2CPP_CATCH_HANDLER(
      //
      if (redirectEvent) {
        redirectEvent(controller, item);
        return;
      }
      //
  )

  if (item->klass != CustomEventDataKlass) {
    CJDLogger::Logger.debug("Invokiong item {} time {}", il2cpp_utils::ClassStandardName(item->klass), item->time);
    return;
  }

  PAPER_IL2CPP_CATCH_HANDLER(
      //
      auto castedItem = il2cpp_utils::cast<CustomEventData>(item);
      for (auto const& customEvents : CustomEventCallbacks::customEventCallbacks) {
        try {
          customEvents.callback(controller, castedItem);
        } catch (std::exception const& e) {
          CJDLogger::Logger.fmtLog<LogLevel::ERR>("Caught exception in callback {}", fmt::ptr(customEvents.callback));
          throw e;
        } catch (...) {
          CJDLogger::Logger.fmtLog<LogLevel::ERR>("Caught exception in callback {}", fmt::ptr(customEvents.callback));
          throw;
        }
      })
}

void CJD_MOD_EXPORT CustomEventCallbacks::AddCustomEventCallback(
    void (*callback)(GlobalNamespace::BeatmapCallbacksController*, CustomJSONData::CustomEventData*)) {
  customEventCallbacks.emplace_back(callback);
}

void CJD_MOD_EXPORT
CustomEventCallbacks::RegisterCallbacks(GlobalNamespace::BeatmapCallbacksController* callbackController) {
  CJDLogger::Logger.fmtLog<LogLevel::INF>("REGISTER CUSTOM CALLBACK!");
  auto* wrapper = CustomBeatmapDataCallbackWrapper::New_ctor();
  wrapper->controller = callbackController;

  // register it
  // AddBeatmapCallback<T>
  // do this to avoid using delegates
  auto callbacksInTime = callbackController->_callbacksInTimes->get_Item(0);
  callbacksInTime->AddCallback(wrapper);
  CJDLogger::Logger.fmtLog<LogLevel::INF>("REGISTERED CUSTOM CALLBACK!");
}
