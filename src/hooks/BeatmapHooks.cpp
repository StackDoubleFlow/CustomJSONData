
#include "CustomJSONDataHooks.h"

#include "HookUtils.hpp"
#include "CustomBeatmapData.h"
#include "BeatmapDataLoaderUtils.hpp"

#include "BeatmapSaveDataVersion2_6_0AndEarlier/zzzz__EventData_def.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveDataItem.hpp"
#include "BeatmapDataLoaderVersion2_6_0AndEarlier/BeatmapDataLoader.hpp"

#include "BeatmapDataLoaderVersion3/BeatmapDataLoader.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveDataItem.hpp"

#include "BeatmapDataLoaderVersion4/BeatmapDataLoader.hpp"
#include "BeatmapSaveDataVersion4/LightshowSaveData.hpp"

#include "GlobalNamespace/BpmTimeProcessor.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/IEnvironmentInfo.hpp"
#include "GlobalNamespace/IEnvironmentLightGroups.hpp"
#include "GlobalNamespace/EnvironmentLightGroups.hpp"
#include "GlobalNamespace/DefaultEnvironmentEvents.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/BasicBeatmapEventDataProcessor.hpp"
#include "GlobalNamespace/BeatmapDataStrobeFilterTransform.hpp"
#include "GlobalNamespace/LightColorBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentIntensityReductionOptions.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/BeatmapEventDataLightsExtensions.hpp"
#include "GlobalNamespace/BurstSliderSpawner.hpp"
#include "GlobalNamespace/NoteCutDirectionExtensions.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnMovementData.hpp"

#include "System/Action.hpp"

#include "UnityEngine/JsonUtility.hpp"

#include "System/Reflection/MemberInfo.hpp"
#include "System/Collections/Generic/InsertionBehavior.hpp"

#include "beatsaber-hook/shared/utils/typedefs-list.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"

#include "paper/shared/Profiler.hpp"

#include "sombrero/shared/linq_functional.hpp"
#include "sombrero/shared/Vector2Utils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

#include "custom-types/shared/register.hpp"

#include "songcore/shared/CustomJSONData.hpp"

#include "JSONWrapper.h"
#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "BeatmapFieldUtils.hpp"
#include "BeatmapDataLoaderUtils.hpp"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

#include <regex>
#include <chrono>
#include <codecvt>
#include <locale>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;
using namespace BeatmapSaveDataVersion4;

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_InsertItem,
                        classof(BeatmapDataSortedListForTypeAndIds_1<BeatmapDataItem*>*), "InsertItem",
                        System::Collections::Generic::LinkedListNode_1<BeatmapDataItem*>*,
                        BeatmapDataSortedListForTypeAndIds_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
  auto* list = self->GetList(CustomBeatmapData::GetCustomType(item), item->get_subtypeGroupIdentifier());

  auto* node = list->Insert(item);
  // Remove to avoid exception
  self->_itemToNodeMap->TryInsert(item, node, InsertionBehavior::OverwriteExisting);

  return node;
}

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_RemoveItem,
                        classof(BeatmapDataSortedListForTypeAndIds_1<BeatmapDataItem*>*), "RemoveItem", void,
                        BeatmapDataSortedListForTypeAndIds_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
  auto* list = self->GetList(CustomBeatmapData::GetCustomType(item), item->get_subtypeGroupIdentifier());
  System::Collections::Generic::LinkedListNode_1<BeatmapDataItem*>* node = nullptr;
  if (self->_itemToNodeMap->TryGetValue(item, byref(node))) {
    list->Remove(node);
  }
}

MAKE_PAPER_HOOK_MATCH(BeatmapData_GetCopy, &CustomBeatmapData::GetCopy, BeatmapData*, BeatmapData* self) {
  static auto* CustomBeatmapDataKlass = classof(CustomBeatmapData*);

  if (self->klass == CustomBeatmapDataKlass) {
    return reinterpret_cast<CustomBeatmapData*>(self)->GetCopyOverride();
  }

  return BeatmapData_GetCopy(self);
}

MAKE_PAPER_HOOK_MATCH(
    BeatmapData_GetFilteredCopy, &CustomBeatmapData::GetFilteredCopy, BeatmapData*, BeatmapData* self,
    System::Func_2<::GlobalNamespace::BeatmapDataItem*, ::GlobalNamespace::BeatmapDataItem*>* processDataItem) {
  static auto* CustomBeatmapDataKlass = classof(CustomBeatmapData*);

  if (self->klass == CustomBeatmapDataKlass) {
    return reinterpret_cast<CustomBeatmapData*>(self)->GetFilteredCopyOverride(
        [&](auto i) constexpr { return processDataItem->Invoke(i); });
  }

  return BeatmapData_GetFilteredCopy(self, processDataItem);
}

// essentially hook a method to then redirect the call to the "override func"
// if that "override func" calls this func again, it calls the original method
#define MAKE_HOOK_OVERRIDE_1PARAM(name, method, Base, Child, ParamT1, param1)                                          \
  void name##__override(Child* self, ParamT1 param1);                                                                  \
  thread_local bool name##_short_circuit =                                                                             \
      false; /* This is done to ensure that the overriden method does not recursively call itself */                   \
  MAKE_HOOK_MATCH(name, method, void, Base* self, ParamT1 param1) {                                                    \
    auto cast = il2cpp_utils::try_cast<Child>(self);                                                                   \
    if (cast && !name##_short_circuit) {                                                                               \
      name##_short_circuit = true;                                                                                     \
      name##__override(*cast, param1);                                                                                 \
      name##_short_circuit = false;                                                                                    \
      return;                                                                                                          \
    } else {                                                                                                           \
      return name(self, param1);                                                                                       \
    }                                                                                                                  \
  }                                                                                                                    \
  void name##__override(Child* self, ParamT1 param1)

MAKE_HOOK_OVERRIDE_1PARAM(CustomAddBeatmapObjectData, &BeatmapData::AddBeatmapObjectData, BeatmapData,
                          CustomBeatmapData, BeatmapObjectData*, item) {
  self->AddBeatmapObjectDataOverride(item);
}

MAKE_HOOK_OVERRIDE_1PARAM(CustomAddBeatmapObjectDataInOrder, &BeatmapData::AddBeatmapObjectDataInOrder, BeatmapData,
                          CustomBeatmapData, BeatmapObjectData*, item) {
  self->AddBeatmapObjectDataInOrderOverride(item);
}

MAKE_HOOK_OVERRIDE_1PARAM(CustomInsertBeatmapEventData, &BeatmapData::InsertBeatmapEventData, BeatmapData,
                          CustomBeatmapData, BeatmapEventData*, item) {
  self->InsertBeatmapEventDataOverride(item);
}

MAKE_HOOK_OVERRIDE_1PARAM(CustomInsertBeatmapEventDataInOrder, &BeatmapData::InsertBeatmapEventDataInOrder, BeatmapData,
                          CustomBeatmapData, BeatmapEventData*, item) {
  self->InsertBeatmapEventDataInOrderOverride(item);
}

MAKE_PAPER_HOOK_MATCH(BurstSliderSpawner_ProcessSliderData, &BurstSliderSpawner::ProcessSliderData, void,
                      ::GlobalNamespace::SliderData* sliderData,
                      ByRef<::GlobalNamespace::BeatmapObjectSpawnMovementData::SliderSpawnData> sliderSpawnData,
                      float_t rotation, bool forceIsFirstNote,
                      ::GlobalNamespace::BurstSliderSpawner::ProcessNoteDataDelegate* processNoteData) {

  static auto CustomKlass = classof(CustomJSONData::CustomSliderData*);

  auto* customSliderData = il2cpp_utils::try_cast<CustomJSONData::CustomSliderData>(sliderData).value_or(nullptr);
  if (customSliderData == nullptr) {
    return BurstSliderSpawner_ProcessSliderData(sliderData, sliderSpawnData, rotation, forceIsFirstNote,
                                                processNoteData);
  }
  float num = sliderSpawnData->jumpDuration * 0.5f;
  float time = sliderData->time;
  auto headMoveStartPos = sliderSpawnData->headMoveStartPos;
  auto headJumpStartPos = sliderSpawnData->headJumpStartPos;
  auto headJumpEndPos = sliderSpawnData->headJumpEndPos;
  float headJumpGravity = sliderSpawnData->headJumpGravity;
  auto vector = headJumpStartPos;
  vector.y += headJumpGravity * num * num * 0.5f;
  float tailTime = sliderData->tailTime;
  auto tailJumpStartPos = sliderSpawnData->tailJumpStartPos;
  float tailJumpGravity = sliderSpawnData->tailJumpGravity;
  auto vector2 = tailJumpStartPos;
  vector2.y += tailJumpGravity * num * num * 0.5f;
  auto vector3 = Sombrero::FastVector2(vector2.x - vector.x, vector2.y - vector.y);
  float magnitude = vector3.Magnitude();
  float angle = (NoteCutDirectionExtensions::RotationAngle(sliderData->headCutDirection) - 90.0f +
                 sliderData->headCutDirectionAngleOffset) *
                0.017453292f;
  auto vector4 = Sombrero::FastVector2(std::cos(angle), std::sin(angle)) * 0.5f * magnitude;
  int sliceCount = sliderData->sliceCount;
  float squishAmount = sliderData->squishAmount;
  float num3 = tailTime - time;
  float num4 = 0.5f * num3;

  auto bezierCurve = [](Sombrero::FastVector2 p0, Sombrero::FastVector2 p1, Sombrero::FastVector2 p2, float t,
                        Sombrero::FastVector2& pos, Sombrero::FastVector2& tangent) constexpr {
    float num = 1.0f - t;
    pos = p0 * num * num + p1 * 2.0f * num * t + p2 * t * t;
    tangent = (p1 - p0) * 2.0f * (1.0f - t) + (p2 - p1) * 2.0f * t;
  };

  for (int i = 1; i < sliceCount; i++) {
    float sliceT = (float)i / (float)(sliceCount - 1);
    int index = ((i < sliceCount - 1) ? sliderData->headLineIndex : sliderData->tailLineIndex);
    auto noteLineLayer = ((i < sliceCount - 1) ? sliderData->headLineLayer : sliderData->tailLineLayer);
    /// TRANSPILE HERE
    auto noteData = CreateCustomBurstNoteData(std::lerp(time, tailTime, sliceT), index, noteLineLayer,
                                              sliderData->headBeforeJumpLineLayer, sliderData->colorType,
                                              NoteCutDirection::Any, 1.0f, customSliderData->customData->value);
    // copy the AD from the head note
    noteData->customData->associatedData = customSliderData->customData->associatedData;
    /// END TRANSPILE HERE
    noteData->timeToPrevColorNote = sliceT * num4;
    Sombrero::FastVector2 position;
    Sombrero::FastVector2 tangent;
    bezierCurve(Sombrero::FastVector2::zero(), vector4, vector3, sliceT * squishAmount, position, tangent);
    noteData->SetCutDirectionAngleOffset(Sombrero::FastVector2::SignedAngle({ 0.0f, -1.0f }, tangent));
    noteData->timeToNextColorNote = ((i == sliceCount - 1) ? 1.0f : 0.4f);
    auto noteSpawnData = BeatmapObjectSpawnMovementData::NoteSpawnData(
        Sombrero::FastVector3(headMoveStartPos.x + position.x, headMoveStartPos.y, headMoveStartPos.z),
        Sombrero::FastVector3(headJumpStartPos.x + position.x, headJumpStartPos.y, headJumpStartPos.z),
        Sombrero::FastVector3(headJumpEndPos.x + position.x, headJumpEndPos.y, headJumpEndPos.z),
        2.0f * (vector.y + position.y - headJumpStartPos.y) / (num * num), sliderSpawnData->moveDuration,
        sliderSpawnData->jumpDuration);
    processNoteData->Invoke(noteData, noteSpawnData, rotation, forceIsFirstNote);
  }
}

void CustomJSONData::InstallBeatmapHooks() {
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomAddBeatmapObjectData);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomAddBeatmapObjectDataInOrder);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomInsertBeatmapEventData);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomInsertBeatmapEventDataInOrder);

  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomBeatmapDataSortedListForTypes_InsertItem);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomBeatmapDataSortedListForTypes_RemoveItem);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapData_GetFilteredCopy);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapData_GetCopy);

  INSTALL_HOOK_ORIG(CJDLogger::Logger, BurstSliderSpawner_ProcessSliderData);
}