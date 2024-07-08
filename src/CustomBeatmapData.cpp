#include "CustomBeatmapData.h"

#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "GlobalNamespace/SortedList_2.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/ISortedListItemProcessor_1.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "CustomJSONDataHooks.h"

#include "System/Collections/Generic/HashSet_1.hpp"

using namespace GlobalNamespace;

DEFINE_TYPE(CustomJSONData, CustomBeatmapData);
DEFINE_TYPE(CustomJSONData, CustomBeatmapEventData);
DEFINE_TYPE(CustomJSONData, CustomObstacleData);
DEFINE_TYPE(CustomJSONData, CustomNoteData);
DEFINE_TYPE(CustomJSONData, CustomWaypointData);
DEFINE_TYPE(CustomJSONData, CustomSliderData);

void CustomJSONData::CustomBeatmapData::ctor(int numberOfLines) {
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe("", "BeatmapData", ".ctor", 1);
  PAPER_IL2CPP_CATCH_HANDLER(il2cpp_utils::RunMethodRethrow(this, ctor, numberOfLines);)

  INVOKE_CTOR();

  ____beatmapDataItemsPerTypeAndId->_sortedListsDataProcessors
      ->Add(csTypeOf(CustomEventData*), nullptr);
  // _beatmapDataItemsPerTypeAndId->_items->Add(csTypeOf(CustomEventData*),
  //                                            reinterpret_cast<ISortedList_1<BeatmapDataItem*>*>(
  //                                                SortedList_2<CustomEventData*, BeatmapDataItem*>::New_ctor(nullptr)));
}

void CustomJSONData::CustomBeatmapData::AddBeatmapObjectDataOverride(
    GlobalNamespace::BeatmapObjectData* beatmapObjectData) {
  static auto const* base = il2cpp_utils::FindMethodUnsafe("", "BeatmapData", "AddBeatmapObjectData", 1);

  beatmapObjectDatas.emplace_back(beatmapObjectData);
  il2cpp_utils::RunMethodRethrow(this, base, beatmapObjectData);
}


void CustomJSONData::CustomBeatmapData::InsertBeatmapEventDataOverride(
    GlobalNamespace::BeatmapEventData* beatmapObjectData) {
  static auto const* base = il2cpp_utils::FindMethodUnsafe("", "BeatmapData", "InsertBeatmapEventData", 1);

  beatmapEventDatas.emplace_back(beatmapObjectData);
  PAPER_IL2CPP_CATCH_HANDLER(il2cpp_utils::RunMethodRethrow(this, base, beatmapObjectData);)
}


void CustomJSONData::CustomBeatmapData::InsertCustomEventData(CustomJSONData::CustomEventData* customEventData) {
  customEventDatas.emplace_back(customEventData);
  auto *node = _beatmapDataItemsPerTypeAndId->InsertItem(customEventData);
  if (updateAllBeatmapDataOnInsert) {
    InsertToAllBeatmapData(customEventData, node);
  }
}

void CustomJSONData::CustomBeatmapData::InsertCustomEventDataInOrder(CustomEventData* customEventData) {
  InsertCustomEventData(customEventData);
  InsertToAllBeatmapData(customEventData, nullptr); // default is null
}

System::Type* CustomJSONData::CustomBeatmapData::GetCustomType(Il2CppObject* obj) {
  return GetCustomType(obj->klass);
}

System::Type* CustomJSONData::CustomBeatmapData::GetCustomType(Il2CppClass* obj) {
  static auto *CustomKlass = classof(CustomEventData*);

  if (obj == nullptr) {
    return nullptr;
  }

  static std::unordered_map<Il2CppClass*, Il2CppReflectionType*> typeMap;

  auto& typePtr = typeMap[obj];

  if (typePtr == nullptr) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>(
        "Checking if {} is equal to ref {} for obj {} vs ref {}", obj->namespaze, CustomKlass->namespaze,
        il2cpp_utils::ClassStandardName(obj), il2cpp_utils::ClassStandardName(CustomKlass));
    if (std::string_view(obj->namespaze) == std::string_view(CustomKlass->namespaze) && obj != CustomKlass) {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("They are custom, using parent type {}",
                                                     il2cpp_utils::ClassStandardName(obj->parent));

      typePtr = il2cpp_utils::GetSystemType(obj->parent);
    } else {
      CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("They are not custom (basegame), using self type {}",
                                                     il2cpp_utils::ClassStandardName(obj));

      typePtr = il2cpp_utils::GetSystemType(obj);
    }
  }

  return reinterpret_cast<System::Type*>(typePtr);
}

CustomJSONData::CustomBeatmapData* CustomJSONData::CustomBeatmapData::BaseCopy() {
  auto *copy = CustomJSONData::CustomBeatmapData::New_ctor(numberOfLines);

  // copy the rest
  copy->doc = this->doc;
  if (this->customData) {
    copy->customData = this->customData->GetCopy();
  }
  if (copy->beatmapCustomData) {
    copy->beatmapCustomData = this->beatmapCustomData->GetCopy();
  }
  if (copy->levelCustomData) {
    copy->levelCustomData = this->levelCustomData->GetCopy();
  }

  copy->v2orEarlier = v2orEarlier;

  auto enumerator = this->_specialBasicBeatmapEventKeywords->GetEnumerator();
  while (enumerator.MoveNext()) {
    copy->AddSpecialBasicBeatmapEventKeyword(enumerator.Current);
  }

  return copy;
}

void CustomJSONData::CustomBeatmapEventData::ctor(float time,
                                                  ::GlobalNamespace::BasicBeatmapEventType basicBeatmapEventType,
                                                  int value, float floatValue) {
  static auto const* BeatmapEventData_Ctor =
      CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(BasicBeatmapEventData*), ".ctor", 4));
  il2cpp_utils::RunMethodRethrow<void, false>(this, BeatmapEventData_Ctor, time, basicBeatmapEventType, value,
                                              floatValue);
  INVOKE_CTOR();
  this->_time_k__BackingField = time;
  this->basicBeatmapEventType = basicBeatmapEventType;
  this->value = value;
  this->floatValue = floatValue;
}

CustomJSONData::CustomBeatmapEventData* CustomJSONData::CustomBeatmapEventData::GetCopy() {
  auto *copy = CustomJSONData::CustomBeatmapEventData::New_ctor(this->time, type.value__, this->value, this->floatValue);
  copy->set_previousSameTypeEventData(previousSameTypeEventData);
  copy->set_nextSameTypeEventData(nextSameTypeEventData);
  copy->type = type;
  copy->basicBeatmapEventType = basicBeatmapEventType;
  copy->subtypeIdentifier = subtypeIdentifier;
  copy->sameTypeIndex = sameTypeIndex;
  // For some reason this is needed
  if (this->customData) {
    copy->customData = this->customData->GetCopy();
  }
  return copy;
}

void CustomJSONData::CustomObstacleData::ctor(float time, int lineIndex, ::GlobalNamespace::NoteLineLayer lineLayer,
                                              float duration, int width, int height) {
  static auto const* NoteData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(ObstacleData*), ".ctor", 6));
  ObstacleData* instance = this;
  il2cpp_utils::RunMethodRethrow<void, false>(instance, NoteData_Ctor, time, lineIndex, lineLayer, duration, width,
                                            height);
  INVOKE_CTOR();
  this->aheadTimeNoodle = 0;
}

CustomJSONData::CustomObstacleData* CustomJSONData::CustomObstacleData::GetCopy() {
  auto *copy = CustomJSONData::CustomObstacleData::New_ctor(this->time, this->lineIndex, this->lineLayer, this->duration,
                                                           this->width, height);
  if (this->customData) {
    copy->customData = this->customData->GetCopy();
  }
  copy->bpm = this->bpm;
  copy->aheadTimeNoodle = this->aheadTimeNoodle;
  return copy;
}

void CustomJSONData::CustomSliderData::ctor(
    GlobalNamespace::SliderData::Type sliderType, ::GlobalNamespace::ColorType colorType, bool hasHeadNote,
    float headTime, int headLineIndex, ::GlobalNamespace::NoteLineLayer headLineLayer,
    ::GlobalNamespace::NoteLineLayer headBeforeJumpLineLayer, float headControlPointLengthMultiplier,
    ::GlobalNamespace::NoteCutDirection headCutDirection, float headCutDirectionAngleOffset, bool hasTailNote,
    float tailTime, int tailLineIndex, ::GlobalNamespace::NoteLineLayer tailLineLayer,
    ::GlobalNamespace::NoteLineLayer tailBeforeJumpLineLayer, float tailControlPointLengthMultiplier,
    ::GlobalNamespace::NoteCutDirection tailCutDirection, float tailCutDirectionAngleOffset,
    ::GlobalNamespace::SliderMidAnchorMode midAnchorMode, int sliceCount, float squishAmount) {
  static auto const* NoteData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(SliderData*), ".ctor", 21));
  CustomSliderData* instance = this;
  il2cpp_utils::RunMethodRethrow<void, false>(
      instance, NoteData_Ctor, sliderType, colorType, hasHeadNote, headTime, headLineIndex, headLineLayer,
      headBeforeJumpLineLayer, headControlPointLengthMultiplier, headCutDirection, headCutDirectionAngleOffset,
      hasTailNote, tailTime, tailLineIndex, tailLineLayer, tailBeforeJumpLineLayer, tailControlPointLengthMultiplier,
      tailCutDirection, tailCutDirectionAngleOffset, midAnchorMode, sliceCount, squishAmount);
  INVOKE_CTOR();
}

CustomJSONData::CustomSliderData* CustomJSONData::CustomSliderData::GetCopy() {
  auto *copy = CustomJSONData::CustomSliderData::New_ctor(
      sliderType, colorType, hasHeadNote, time, headLineIndex, headLineLayer, headBeforeJumpLineLayer,
      headControlPointLengthMultiplier, headCutDirection, headCutDirectionAngleOffset, hasTailNote, tailTime,
      tailLineIndex, tailLineLayer, tailBeforeJumpLineLayer, tailControlPointLengthMultiplier, tailCutDirection,
      tailCutDirectionAngleOffset, midAnchorMode, sliceCount, squishAmount);
  if (this->customData) {
    copy->customData = this->customData->GetCopy();
  }
  copy->bpm = this->bpm;
  return copy;
}

void CustomJSONData::CustomNoteData::ctor(float time, int lineIndex, ::GlobalNamespace::NoteLineLayer noteLineLayer,
                                          ::GlobalNamespace::NoteLineLayer beforeJumpNoteLineLayer,
                                          ::GlobalNamespace::NoteData::GameplayType gameplayType,
                                          ::GlobalNamespace::NoteData::ScoringType scoringType,
                                          ::GlobalNamespace::ColorType colorType,
                                          ::GlobalNamespace::NoteCutDirection cutDirection, float timeToNextColorNote,
                                          float timeToPrevColorNote, int flipLineIndex, float flipYSide,
                                          float cutDirectionAngleOffset, float cutSfxVolumeMultiplier) {
  static auto const* NoteData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(NoteData*), ".ctor", 14));
  NoteData* instance = this;
  il2cpp_utils::RunMethodRethrow<void, false>(instance, NoteData_Ctor, time, lineIndex, noteLineLayer,
                                              beforeJumpNoteLineLayer, gameplayType, scoringType, colorType,
                                              cutDirection, timeToNextColorNote, timeToPrevColorNote, flipLineIndex,
                                              flipYSide, cutDirectionAngleOffset, cutSfxVolumeMultiplier);
  INVOKE_CTOR();
  this->aheadTimeNoodle = 0;
}

CustomJSONData::CustomNoteData* CustomJSONData::CustomNoteData::GetCopy() {
  auto * copy = CustomJSONData::CustomNoteData::New_ctor(time, lineIndex, noteLineLayer, beforeJumpNoteLineLayer,
                                                       gameplayType, scoringType, colorType, cutDirection,
                                                       timeToNextColorNote, timeToPrevColorNote, flipLineIndex,
                                                       flipYSide, cutDirectionAngleOffset, cutSfxVolumeMultiplier);
  if (this->customData) {
    copy->customData = this->customData->GetCopy();
  }
  copy->bpm = this->bpm;
  copy->aheadTimeNoodle = this->aheadTimeNoodle;
  return copy;
}

void CustomJSONData::CustomWaypointData::ctor(float time, int lineIndex, GlobalNamespace::NoteLineLayer noteLineLayer,
                                              GlobalNamespace::OffsetDirection offsetDirection) {
  static auto const* NoteData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(WaypointData*), ".ctor", 4));
  WaypointData* instance = this;
  il2cpp_utils::RunMethodRethrow<void, false>(instance, NoteData_Ctor, time, lineIndex, noteLineLayer, offsetDirection);
  INVOKE_CTOR();
}

CustomJSONData::CustomWaypointData* CustomJSONData::CustomWaypointData::GetCopy() {
  auto *copy = CRASH_UNLESS(CustomJSONData::CustomWaypointData::New_ctor(time, lineIndex, lineLayer, offsetDirection));
  copy->bpm = this->bpm;
  
  return copy;
}