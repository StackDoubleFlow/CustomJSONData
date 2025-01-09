
#include "CustomJSONDataHooks.h"

#include "HookUtils.hpp"
#include "CustomBeatmapData.h"

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
#include "GlobalNamespace/VariableMovementDataProvider.hpp"
#include "GlobalNamespace/NoteSpawnData.hpp"
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

#include "paper2_scotland2/shared/Profiler.hpp"

#include "sombrero/shared/linq_functional.hpp"
#include "sombrero/shared/Vector2Utils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

#include "custom-types/shared/register.hpp"

#include "songcore/shared/CustomJSONData.hpp"

#include "JSONWrapper.h"
#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "misc/BeatmapFieldUtils.hpp"
#include "misc/BeatmapDataLoaderUtils.hpp"
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

MAKE_PAPER_HOOK_MATCH(CustomAddBeatmapObjectData, &BeatmapData::AddBeatmapObjectData, void, BeatmapData* self,
                      BeatmapObjectData* item) {
  if (auto customSelf = il2cpp_utils::try_cast<CustomBeatmapData>(self).value_or(nullptr)) {
    customSelf->beatmapObjectDatas.emplace_back(item);
  }
  CustomAddBeatmapObjectData(self, item);
}

MAKE_PAPER_HOOK_MATCH(CustomInsertBeatmapEventData, &BeatmapData::InsertBeatmapEventData, void, BeatmapData* self,
                      BeatmapEventData* item) {
  if (auto customSelf = il2cpp_utils::try_cast<CustomBeatmapData>(self).value_or(nullptr)) {
    customSelf->beatmapEventDatas.emplace_back(item);
  }
  CustomInsertBeatmapEventData(self, item);
}
MAKE_PAPER_HOOK_MATCH(BurstSliderSpawner_ProcessSliderData, &BurstSliderSpawner::ProcessSliderData, void,
            ::GlobalNamespace::SliderData* sliderData,
            ::ByRef<::GlobalNamespace::SliderSpawnData> sliderSpawnData, bool forceIsFirstNote,
            ::GlobalNamespace::VariableMovementDataProvider* variableMovementDataProvider,
            ::GlobalNamespace::BurstSliderSpawner_ProcessNoteDataDelegate* processNoteData) {

  static auto CustomKlass = classof(CustomJSONData::CustomSliderData*);

  auto* customSliderData = il2cpp_utils::try_cast<CustomJSONData::CustomSliderData>(sliderData).value_or(nullptr);
  if (customSliderData == nullptr) {
  return BurstSliderSpawner_ProcessSliderData(sliderData, sliderSpawnData, forceIsFirstNote,
                        variableMovementDataProvider, processNoteData);
  }

  float halfJumpDuration = variableMovementDataProvider->halfJumpDuration;
  float headGravity = variableMovementDataProvider->CalculateCurrentNoteJumpGravity(sliderSpawnData->headGravityBase);
  float tailGravity = variableMovementDataProvider->CalculateCurrentNoteJumpGravity(sliderSpawnData->tailGravityBase);

  auto vector = Sombrero::FastVector2(sliderSpawnData->headNoteOffset.x, 
    sliderSpawnData->headNoteOffset.y + headGravity * halfJumpDuration * halfJumpDuration * 0.5f);
  
  auto vector2 = Sombrero::FastVector2(sliderSpawnData->tailNoteOffset.x,
    sliderSpawnData->tailNoteOffset.y + tailGravity * halfJumpDuration * halfJumpDuration * 0.5f);

  auto vector3 = vector2 - vector;
  float magnitude = vector3.Magnitude();
  
  float angle = (NoteCutDirectionExtensions::RotationAngle(sliderData->headCutDirection) - 90.0f +
         sliderData->headCutDirectionAngleOffset) * 0.017453292f;
  auto vector4 = Sombrero::FastVector2(std::cos(angle), std::sin(angle)) * 0.5f * magnitude;
  
  int sliceCount = sliderData->sliceCount;
  float squishAmount = sliderData->squishAmount;
  float time = sliderData->time;
  float tailTime = sliderData->tailTime;
  float num3 = (tailTime - time) * 0.5f;

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

  // TRANSPILE HERE
  auto noteData =
      CreateCustomBurstNoteData(std::lerp(time, tailTime, sliceT), sliderData->beat, sliderData->rotation, index,
                                noteLineLayer, sliderData->headBeforeJumpLineLayer, sliderData->colorType,
                                NoteCutDirection::Any, 1.0f, customSliderData->customData->value);
  // copy the AD from the original note
  noteData->customData->associatedData = customSliderData->customData->associatedData;
  // TRANSPILE END
  
  noteData->timeToPrevColorNote = sliceT * num3;
  Sombrero::FastVector2 position;
  Sombrero::FastVector2 tangent;
  bezierCurve(Sombrero::FastVector2::zero(), vector4, vector3, sliceT * squishAmount, position, tangent);
  noteData->SetCutDirectionAngleOffset(Sombrero::FastVector2::SignedAngle({ 0.0f, -1.0f }, tangent));
  noteData->timeToNextColorNote = ((i == sliceCount - 1) ? 1.0f : 0.4f);

  auto headNoteOffset = Sombrero::FastVector3(sliderSpawnData->headNoteOffset);
  auto noteSpawnData = NoteSpawnData(headNoteOffset + Sombrero::FastVector3(vector.x, 0.0f, 0.0f),
                                     headNoteOffset + Sombrero::FastVector3(vector.x, 0.0f, 0.0f),
                                     headNoteOffset + Sombrero::FastVector3(vector.x, 0.0f, 0.0f),
                                     headGravity * halfJumpDuration * halfJumpDuration * 0.5f + vector.y);

  processNoteData->Invoke(noteData, noteSpawnData, forceIsFirstNote);
  }
}

void CustomJSONData::InstallBeatmapHooks() {
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomAddBeatmapObjectData);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomInsertBeatmapEventData);

  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomBeatmapDataSortedListForTypes_InsertItem);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, CustomBeatmapDataSortedListForTypes_RemoveItem);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapData_GetFilteredCopy);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapData_GetCopy);

  INSTALL_HOOK_ORIG(CJDLogger::Logger, BurstSliderSpawner_ProcessSliderData);
}