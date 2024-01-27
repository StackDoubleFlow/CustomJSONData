#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "songloader/shared/API.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"

#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/MultipleSortedListsEnumerator_1.hpp"
#include "GlobalNamespace/BPMChangeBeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapEventTransitionType.hpp"
#include "GlobalNamespace/BeatmapDataLoader.hpp"
#include "GlobalNamespace/BeatmapDataMirrorTransform.hpp"
#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "GlobalNamespace/DataConvertor_1.hpp"
#include "GlobalNamespace/DefaultEnvironmentEvents.hpp"
#include "GlobalNamespace/EnvironmentColorType.hpp"
#include "GlobalNamespace/BeatmapEventDataBoxGroup.hpp"
#include "GlobalNamespace/EnvironmentLightGroups.hpp"
#include "GlobalNamespace/LightColorBaseData.hpp"
#include "GlobalNamespace/BeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightColorBeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightRotationBeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightRotationBaseData.hpp"
#include "GlobalNamespace/BasicBeatmapObjectManager.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapLightshowSaveData.hpp"

#include "System/Comparison_1.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "System/Collections/Generic/InsertionBehavior.hpp"
#include "System/Linq/Enumerable.hpp"
#include "System/Version.hpp"
#include "System/Diagnostics/Stopwatch.hpp"

#include "GlobalNamespace/GameSongController.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"

#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "CustomEventData.h"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"
#include "BeatmapDataLoaderUtils.hpp"

#include <regex>

// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"
#include "paper/shared/Profiler.hpp"
#include "sombrero/shared/linq_functional.hpp"

#include "GlobalNamespace/BeatmapEventDataBoxGroupLists.hpp"
#include "GlobalNamespace/SpawnRotationBeatmapEventData.hpp"
#include "GlobalNamespace/DefaultEnvironmentEventsFactory.hpp"
#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/BeatmapDataStrobeFilterTransform.hpp"
#include "GlobalNamespace/LightColorBeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapEventDataLightsExtensions.hpp"
#include "GlobalNamespace/EnvironmentIntensityReductionOptions.hpp"

#include <chrono>
#include <codecvt>
#include <locale>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;

v3::CustomBeatmapSaveData* cachedSaveData;


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


static std::string GetVersionFromPath(std::string_view path) {
  // SongCore has a fallback so i guess i do too
  static const std::string_view fallback = "2.0.0";

  auto truncatedText = path.substr(0, 50);
  static const std::regex versionRegex(R"("_?version"\s*:\s*"[0-9]+\.[0-9]+\.?[0-9]?")");
  std::match_results<std::string_view::const_iterator> matches;
  if (std::regex_search(truncatedText.begin(), truncatedText.end(), matches, versionRegex)) {
    if (!matches.empty()) {
      auto version = matches[0].str();
      version = version.substr(0, version.length() - 1);
      version = version.substr(version.find_last_of('\"') + 1, version.length());

      return version;
    }
  }

  return std::string(fallback);
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
    return reinterpret_cast<CustomBeatmapData*>(self)->GetFilteredCopyOverride([&](auto i) constexpr {
      return processDataItem->Invoke(i);
    });
  }

  return BeatmapData_GetFilteredCopy(self, processDataItem);
}

static bool Approximately(float a, float b) {
  return std::abs(b - a) < std::max(1E-06F * std::max(std::abs(a), std::abs(b)), 1E-45F * 8.0F);
}

MAKE_PAPER_HOOK_MATCH(BeatmapDataStrobeFilterTransform_CreateTransformedData,
                      &BeatmapDataStrobeFilterTransform::CreateTransformedData, IReadonlyBeatmapData*,
                      IReadonlyBeatmapData* beatmapData,
                      EnvironmentIntensityReductionOptions* environmentIntensityReductionOptions) {
  if (!beatmapData || reinterpret_cast<Il2CppObject*>(beatmapData)->klass != classof(CustomBeatmapData*)) {
    return BeatmapDataStrobeFilterTransform_CreateTransformedData(beatmapData, environmentIntensityReductionOptions);
  }
  // Won't work since the constructors are base game
  //
  //    auto fixedBeatmap = BeatmapDataStrobeFilterTransform_CreateTransformedData(beatmapData,
  //                                                                               environmentIntensityReductionOptions);
  //
  //    auto customBeatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmapData);
  //    CustomBeatmapData *newBeatmap;
  //
  ////    if (auto customBeatmapData = il2cpp_utils::try_cast<CustomBeatmapData>(beatmapData)) {
  ////        for (auto const &c: customBeatmapData.value()->customEventDatas) {
  ////            beatmapData2->InsertCustomEventDataInOrder(c);
  ////        }
  //        newBeatmap = customBeatmapData->GetFilteredCopyOverride(
  //                [&](BeatmapDataItem *const &i) -> BeatmapDataItem * {
  //
  //                    if (i->type == BeatmapDataItem::BeatmapDataItemType::BeatmapEvent) {
  //                        return nullptr;
  //                    }
  //
  //                    return i;
  //                });
  ////    } else {
  ////        newBeatmap = CustomBeatmapData::New_ctor(beatmapData->i_IBeatmapDataBasicInfo()->get_numberOfLines());
  ////
  ////    }
  //
  //
  //    auto const &linkedItems = fixedBeatmap->get_allBeatmapDataItems();
  //    for (auto node = linkedItems->get_First();
  //         node != nullptr; node = CustomBeatmapData::LinkedListNode_1_get_Next(node)) {
  //        if (!node->item || node->item->type != BeatmapDataItem::BeatmapDataItemType::BeatmapEvent) {
  //            continue;
  //        }
  //
  //        newBeatmap->InsertBeatmapEventDataInOrderOverride(il2cpp_utils::cast<BeatmapEventData>(node->item));
  //    }

  auto* customBeatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmapData);
  bool flag = environmentIntensityReductionOptions->compressExpand ==
              EnvironmentIntensityReductionOptions::CompressExpandReductionType::RemoveWithStrobeFilter;
  bool flag2 = environmentIntensityReductionOptions->rotateRings ==
               EnvironmentIntensityReductionOptions::RotateRingsReductionType::RemoveWithStrobeFilter;
  std::unordered_map<int, BeatmapDataStrobeFilterTransform::StrobeStreakData*>
      dictionary({ { BasicBeatmapEventType::Event0.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
                   { BasicBeatmapEventType::Event1.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
                   { BasicBeatmapEventType::Event2.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
                   { BasicBeatmapEventType::Event3.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
                   { BasicBeatmapEventType::Event4.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() } });

  auto* newBeatmap = customBeatmapData->BaseCopy();

  for (auto const& c : customBeatmapData->customEventDatas) {
    if (!c) {
      continue;
    }
    newBeatmap->InsertCustomEventDataInOrder(c);
  }

  for (auto const& o : customBeatmapData->beatmapObjectDatas) {
    if (!o) {
      continue;
    }
    newBeatmap->AddBeatmapObjectDataInOrderOverride(o);
  }

  for (auto const& beatmapDataItem : customBeatmapData->beatmapEventDatas) {
    if (!beatmapDataItem) {
      continue;
    }

    BasicBeatmapEventData* basicBeatmapEventData =
        il2cpp_utils::try_cast<BasicBeatmapEventData>(beatmapDataItem).value_or(nullptr);
    LightColorBeatmapEventData* lightColorBeatmapEventData =
        il2cpp_utils::try_cast<LightColorBeatmapEventData>(beatmapDataItem).value_or(nullptr);

    if (lightColorBeatmapEventData) {
      lightColorBeatmapEventData->DisableStrobe();
      newBeatmap->InsertBeatmapEventDataInOrderOverride(lightColorBeatmapEventData);
      continue;
    }
    if (!basicBeatmapEventData) {
      newBeatmap->InsertBeatmapEventDataInOrder(beatmapDataItem);
      continue;
    }
    if ((!flag || basicBeatmapEventData->basicBeatmapEventType != BasicBeatmapEventType::Event9) &&
        (!flag2 || basicBeatmapEventData->basicBeatmapEventType != BasicBeatmapEventType::Event8)) {
      if (!BeatmapEventTypeExtensions::IsCoreLightIntensityChangeEvent(basicBeatmapEventData->basicBeatmapEventType)) {
        newBeatmap->InsertBeatmapEventDataInOrderOverride(basicBeatmapEventData);
        continue;
      }

      if (BeatmapEventDataLightsExtensions::HasLightFadeEventDataValue(basicBeatmapEventData)) {
        newBeatmap->InsertBeatmapEventDataInOrderOverride(basicBeatmapEventData);
        continue;
      }

      BeatmapDataStrobeFilterTransform::StrobeStreakData* strobeStreakData =
          dictionary[basicBeatmapEventData->basicBeatmapEventType.value__];
      CRASH_UNLESS(strobeStreakData);
      if (strobeStreakData->isActive) {
        if (basicBeatmapEventData->time - strobeStreakData->lastSwitchTime < 0.1F) {
          strobeStreakData->AddStrobeData(basicBeatmapEventData);
        } else {
          if (!Approximately(strobeStreakData->strobeStartTime, strobeStreakData->lastSwitchTime)) {
            int onEventDataValue =
                BeatmapDataStrobeFilterTransform::GetOnEventDataValue(strobeStreakData->startColorType);
            auto* beatmapEventData2 = static_cast<BasicBeatmapEventData*>(basicBeatmapEventData->GetCopy());
            beatmapEventData2->_time_k__BackingField = strobeStreakData->strobeStartTime;
            beatmapEventData2->value = onEventDataValue;
            newBeatmap->InsertBeatmapEventDataInOrderOverride(beatmapEventData2);
            int value = 0;
            if (strobeStreakData->lastIsOn) {
              value = BeatmapDataStrobeFilterTransform::GetOnEventDataValue(strobeStreakData->lastColorType);
            } else {
              value = BeatmapDataStrobeFilterTransform::GetFlashAndFadeToBlackEventDataValue(
                  strobeStreakData->lastColorType);
            }
            auto* beatmapEventData3 = static_cast<BasicBeatmapEventData*>(basicBeatmapEventData->GetCopy());
            beatmapEventData3->_time_k__BackingField = strobeStreakData->lastSwitchTime;
            beatmapEventData3->value = value;
            newBeatmap->InsertBeatmapEventDataInOrderOverride(beatmapEventData3);
          } else {
            newBeatmap->InsertBeatmapEventDataInOrderOverride(strobeStreakData->originalBasicBeatmapEventData);
          }
          strobeStreakData->StartPotentialStrobe(basicBeatmapEventData);
        }
      } else {
        strobeStreakData->StartPotentialStrobe(basicBeatmapEventData);
      }
    }
  }

  for (auto const& keyValuePair : dictionary) {
    if (!keyValuePair.second) {
      continue;
    }
    if (keyValuePair.second->isActive) {
      newBeatmap->InsertBeatmapEventDataInOrderOverride(keyValuePair.second->originalBasicBeatmapEventData);
    }
  }
  return newBeatmap->i___GlobalNamespace__IReadonlyBeatmapData();
}

// MappingExtensions depends on this method getting called
MAKE_PAPER_HOOK_MATCH(BeatmapSaveData_ConvertBeatmapSaveData,
                      &BeatmapSaveDataVersion3::BeatmapSaveData::ConvertBeatmapSaveData, BeatmapSaveData*,
                      BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData* beatmapSaveData) {
  return v3::CustomBeatmapSaveData::Convert2_6_0(
      il2cpp_utils::cast<CustomJSONData::v2::CustomBeatmapSaveData>(beatmapSaveData));
}

// This hook loads the json data (with custom data) into a BeatmapSaveData
MAKE_PAPER_HOOK_MATCH(BeatmapSaveData_DeserializeFromJSONString,
                      &BeatmapSaveDataVersion3::BeatmapSaveData::DeserializeFromJSONString, BeatmapSaveData*,
                      StringW stringData) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json");
  auto startTime = std::chrono::high_resolution_clock::now();

  if (!stringData) {
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("No string data");
    return nullptr;
  }

  try {
    auto sharedDoc = std::make_shared<rapidjson::Document>();
    rapidjson::Document& doc = *sharedDoc;
    std::string contents(stringData);
    rapidjson::ParseResult result = doc.Parse(contents);

    if (!result || doc.IsNull() || doc.HasParseError()) {
      std::string errorCodeStr(rapidjson::GetParseError_En(result.Code()));
      CJDLogger::Logger.fmtLog<LogLevel::DBG>("Unable to parse json due to {}", errorCodeStr);
      return nullptr;
    }
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json success");

    auto version = GetVersionFromPath(contents);

    v3::CustomBeatmapSaveData* saveData = nullptr;

    if (semver::lte(std::string(version), "2.6.0")) {
      CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 2.0.0 beatmap");

      saveData = il2cpp_utils::cast<v3::CustomBeatmapSaveData>(
          BeatmapSaveDataVersion3::BeatmapSaveData::ConvertBeatmapSaveData(
              v2::CustomBeatmapSaveData::Deserialize(sharedDoc)));
      saveData->isV2 = true;
    } else {
      CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 3.0.0 beatmap");
      saveData = v3::CustomBeatmapSaveData::Deserialize(sharedDoc);
    }

    cachedSaveData = saveData;

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished reading beatmap data {}", fmt::ptr(saveData));
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::Logger.fmtLog<LogLevel::DBG>(
        "This took {}ms",
        static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count()));

    return saveData;
  } catch (std::exception const& e) {
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("There was an error loading the beatmap through CJD. Cause of error: {}",
                                            e.what());
    return nullptr;
  }
}

MAKE_HOOK_OVERRIDE_1PARAM(CustomAddBeatmapObjectData, &BeatmapData::AddBeatmapObjectData, BeatmapData,
                              CustomBeatmapData, GlobalNamespace::BeatmapObjectData*, item) {
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

BeatmapCallbacksController* beatmapCallbacksController;

MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_ManualUpdate, &BeatmapCallbacksController::ManualUpdate, void,
                      BeatmapCallbacksController* self, float songTime) {
  if (songTime == self->_prevSongTime) {
    return BeatmapCallbacksController_ManualUpdate(self, songTime);
  }

  // TRANSPILE HERE
  if (self != beatmapCallbacksController) {
    CustomEventCallbacks::RegisterCallbacks(self);
    beatmapCallbacksController = self;
  }

  return BeatmapCallbacksController_ManualUpdate(self, songTime);
}

inline float GetAheadTime(Il2CppObject* obj) {

  static auto* CustomNoteKlass = classof(CustomJSONData::CustomNoteData*);
  static auto* CustomObstacleKlass = classof(CustomJSONData::CustomObstacleData*);

  if (obj->klass == CustomNoteKlass) {
    return static_cast<CustomJSONData::CustomNoteData*>(obj)->aheadTimeNoodle;
  }

  if (obj->klass == CustomObstacleKlass) {
    return static_cast<CustomJSONData::CustomObstacleData*>(obj)->aheadTimeNoodle;
  }

  return 0;
}

MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_Dispose, &BeatmapCallbacksController::Dispose, void,
                      BeatmapCallbacksController* self) {
  CustomEventCallbacks::firstNode.emplace(nullptr);
  return BeatmapCallbacksController_Dispose(self);
}

MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_ManualUpdateTranspile, &BeatmapCallbacksController::ManualUpdate, void,
                      BeatmapCallbacksController* self, float songTime) {
  // TRANSPILE HERE
  if (self != beatmapCallbacksController) {
    CustomEventCallbacks::RegisterCallbacks(self);
    beatmapCallbacksController = self;
  }
  //

  if (songTime == self->_prevSongTime) {
    return;
  }

  self->_songTime = songTime;
  self->_processingCallbacks = true;
  if (songTime > self->_prevSongTime) {
    auto enumerator = self->_callbacksInTimes->GetEnumerator();

    while (enumerator.MoveNext()) {
      auto keyValuePair = enumerator.get_Current();
      auto* value = keyValuePair.get_Value();

      using NodePtr = System::Collections::Generic::LinkedListNode_1<GlobalNamespace::BeatmapDataItem*>*;

      auto* firstNode = CustomEventCallbacks::firstNode ? (NodePtr)CustomEventCallbacks::firstNode : nullptr;

      for (auto* linkedListNode =
               (value->lastProcessedNode != nullptr)
                   ? CustomJSONData::LinkedListNode_1_get_Next(value->lastProcessedNode)
                   : (firstNode ? firstNode : self->_beatmapData->get_allBeatmapDataItems()->get_First());
           linkedListNode != nullptr; linkedListNode = CustomJSONData::LinkedListNode_1_get_Next(linkedListNode)) {
        auto* value2 = linkedListNode->get_Value();
        if (value2->time - value->aheadTime - GetAheadTime(value2) > songTime) {
          break;
        }
        if (value2->type == BeatmapDataItem::BeatmapDataItemType::BeatmapEvent ||
            /// TRANSPILE HERE
            value2->type.value__ == 2 ||
            /// TRANSPILE HERE
            (value2->type == BeatmapDataItem::BeatmapDataItemType::BeatmapObject &&
             value2->time >= self->_startFilterTime)) {
          value->CallCallbacks(value2);
        }
        value->lastProcessedNode = linkedListNode;
      }
    }
    enumerator.Dispose();
  } else {
    auto callbacksInTimesEnumerator = self->_callbacksInTimes->GetEnumerator();

    while (callbacksInTimesEnumerator.MoveNext()) {
      auto keyValuePair2 = callbacksInTimesEnumerator.get_Current();
      auto* value3 = keyValuePair2.get_Value();
      auto* linkedListNode2 = value3->lastProcessedNode;
      while (linkedListNode2 != nullptr) {
        auto* value4 = linkedListNode2->get_Value();
        if (value4->time - value3->aheadTime <= songTime) {
          break;
        }

        /// TRANSPILE HERE
        /// STOPS INFINITE LOOP BY RUNNING THIS REGARDLESS IF THE CONDITION ABOVE IS MET
        /// WHILE THIS SHOULD BE FIXED IN PINKCORE, WE KEEP IT AS A SAFEGUARD
        if (value4->type != BeatmapDataItem::BeatmapDataItemType::BeatmapEvent) {
          break;
        }
        ///

        auto* beatmapEventData = static_cast<BeatmapEventData*>(value4);
        if (beatmapEventData->previousSameTypeEventData != nullptr) {
          value3->CallCallbacks(beatmapEventData->previousSameTypeEventData);
        } else {
          auto* def = beatmapEventData->GetDefault(beatmapEventData);
          if (def != nullptr) {
            value3->CallCallbacks(def);
          }
        }

        value3->lastProcessedNode = linkedListNode2 = linkedListNode2->get_Previous();
      }
    }
    callbacksInTimesEnumerator.Dispose();
  }

  self->_prevSongTime = songTime;
  self->_processingCallbacks = false;
}

MAKE_PAPER_HOOK_MATCH(InsertDefaultEnvironmentEvents, &DefaultEnvironmentEventsFactory::InsertDefaultEnvironmentEvents,
                      void, BeatmapData* beatmapData, BeatmapEventDataBoxGroupLists* beatmapEventDataBoxGroupLists,
                      DefaultEnvironmentEvents* defaultEnvironmentEvents,
                      IEnvironmentLightGroups* environmentLightGroups) {
  if (defaultEnvironmentEvents == nullptr || defaultEnvironmentEvents->get_isEmpty()) {
    // TRANSPILE HERE
    beatmapData->InsertBeatmapEventData(CustomBeatmapEventData::New_ctor(0.0F, BasicBeatmapEventType::Event0, 1, 1.0F));
    beatmapData->InsertBeatmapEventData(CustomBeatmapEventData::New_ctor(0.0F, BasicBeatmapEventType::Event4, 1, 1.0F));
    // END TRANSPILE HERE
    return;
  }
  if (defaultEnvironmentEvents->basicBeatmapEvents) {
    for (auto* basicBeatmapEvent : defaultEnvironmentEvents->basicBeatmapEvents) {
      // TRANSPILE HERE
      beatmapData->InsertBeatmapEventData(CustomBeatmapEventData::New_ctor(
          0.0F, basicBeatmapEvent->eventType, basicBeatmapEvent->value, basicBeatmapEvent->floatValue));
      // END TRANSPILE HERE
    }
  }
  if (defaultEnvironmentEvents->lightGroupEvents) {
    for (auto* lightGroupEvent : defaultEnvironmentEvents->lightGroupEvents) {
      auto* dataForGroup = environmentLightGroups->GetDataForGroup(lightGroupEvent->lightGroup->groupId);
      if (dataForGroup && !reinterpret_cast<System::Object*>(dataForGroup)->Equals(nullptr)) {
        auto* beatmapEventDataBoxGroup = BeatmapEventDataBoxGroupFactory::CreateSingleLightBeatmapEventDataBoxGroup(
            0.0F, dataForGroup->numberOfElements, lightGroupEvent);
        beatmapEventDataBoxGroupLists->Insert(dataForGroup->groupId, beatmapEventDataBoxGroup);
      }
    }
  }
}

// TODO: Update
MAKE_PAPER_HOOK_MATCH(GetBeatmapDataFromBeatmapSaveData, &BeatmapDataLoader::GetBeatmapDataFromBeatmapSaveData,
                      BeatmapData*, ::BeatmapSaveDataVersion3::BeatmapSaveData* beatmapSaveData,
                      ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, float_t startBpm,
                      bool loadingForDesignatedEnvironment, ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                      ::GlobalNamespace::IEnvironmentLightGroups* environmentLightGroups,
                      ::GlobalNamespace::DefaultEnvironmentEvents* defaultEnvironmentEvents,
                      ::BeatmapSaveDataVersion3::BeatmapLightshowSaveData* defaultLightshowEventsSaveData,
                      ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                      ::System::Diagnostics::Stopwatch* stopwatch) {

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing save data {} cached {}", fmt::ptr(beatmapSaveData),
                                          fmt::ptr(cachedSaveData));
  auto startTime = std::chrono::high_resolution_clock::now();
  Paper::Profiler profile;
  profile.startTimer();

  bool flag = loadingForDesignatedEnvironment ||
              (beatmapSaveData->useNormalEventsAsCompatibleEvents && defaultEnvironmentEvents->isEmpty);
  bool flag2 = playerSpecificSettings == nullptr || playerSpecificSettings->GetEnvironmentEffectsFilterPreset(
                                                     beatmapDifficulty) != EnvironmentEffectsFilterPreset::NoEffects;
  bool flag3 = flag && flag2;
  CustomBeatmapData* beatmapData = CustomBeatmapData::New_ctor(4);

  static auto* CustomBeatmapSaveDataKlass = classof(v3::CustomBeatmapSaveData*);

  if (auto cBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData)) {

    beatmapData->customData = ToJsonWrapper(cBeatmapSaveData.value()->customData);
    beatmapData->levelCustomData = ToJsonWrapper(cBeatmapSaveData.value()->levelCustomData);
    beatmapData->beatmapCustomData = ToJsonWrapper(cBeatmapSaveData.value()->beatmapCustomData);
    beatmapData->v2orEarlier = cBeatmapSaveData.value()->isV2;
  }

  beatmapData->InsertBeatmapEventDataOverride(CustomJSONData::NewFast<BPMChangeBeatmapEventData*>(-100.0F, startBpm));

  CRASH_UNLESS(beatmapSaveData->basicEventTypesWithKeywords);
  if (beatmapSaveData->basicEventTypesWithKeywords->data) {
    for (auto* basicEventTypesForKeyword :
         VList<BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>(
             beatmapSaveData->basicEventTypesWithKeywords->data)) {
      if (!basicEventTypesForKeyword || !basicEventTypesForKeyword->k) {
        continue;
      }

      beatmapData->AddSpecialBasicBeatmapEventKeyword(basicEventTypesForKeyword->k);
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Special events list {} {}",
                                          fmt::ptr(beatmapSaveData->basicEventTypesWithKeywords->d),
                                          beatmapSaveData->basicEventTypesWithKeywords->d->_size);

  profile.mark("Converted special events");

  ListW<BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData*> bpmEvents = beatmapSaveData->bpmEvents;
  BpmTimeProcessor bpmTimeProcessor(startBpm, bpmEvents);

  auto const BeatToTime = [&bpmTimeProcessor](float beat) constexpr {
    return bpmTimeProcessor.ConvertBeatToTime(beat);
  };

  profile.mark("Parsed bpm events");

  // TODO: Remove converter
  CppConverter<BeatmapObjectData*, BeatmapSaveData::BeatmapSaveDataItem*> objectConverter;
  objectConverter.AddConverter<v3::CustomBeatmapSaveData_ColorNoteData*>([&BeatToTime](
      v3::CustomBeatmapSaveData_ColorNoteData * data) constexpr {
    auto* noteData =
        CreateCustomBasicNoteData(BeatToTime(data->b), data->get_line(), ConvertNoteLineLayer(data->get_layer()),
                                  ConvertColorType(data->get_color()), data->get_cutDirection(), data->customData);
    noteData->SetCutDirectionAngleOffset(data->get_angleOffset());

    return noteData;
  });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_BombNoteData*>([&BeatToTime](
      v3::CustomBeatmapSaveData_BombNoteData * data) constexpr {
    auto* b = CreateCustomBombNoteData(BeatToTime(data->b), data->get_line(), ConvertNoteLineLayer(data->get_layer()),
                                       data->customData);
    return b;
  });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_ObstacleData*>([&BeatToTime](
      v3::CustomBeatmapSaveData_ObstacleData * data) constexpr {
    float beat = BeatToTime(data->b);
    auto* obstacle = CustomObstacleData::New_ctor(
        beat, data->get_line(), BeatmapDataLoader::ObstacleConvertor::GetNoteLineLayer(data->get_layer()),
        BeatToTime(data->b + data->get_duration()) - beat, data->get_width(), data->get_height());

    obstacle->customData = data->customData->GetCopy();

    return obstacle;
  });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_SliderData*>([&BeatToTime](
      v3::CustomBeatmapSaveData_SliderData * data) constexpr {
    return CustomSliderData_CreateCustomSliderData(
        ConvertColorType(data->get_colorType()), BeatToTime(data->b), data->get_headLine(),
        ConvertNoteLineLayer(data->get_headLayer()), ConvertNoteLineLayer(data->get_headLayer()),
        data->get_headControlPointLengthMultiplier(), data->get_headCutDirection(), BeatToTime(data->get_tailBeat()),
        data->get_tailLine(), ConvertNoteLineLayer(data->get_tailLayer()), ConvertNoteLineLayer(data->get_tailLayer()),
        data->get_tailControlPointLengthMultiplier(), data->get_tailCutDirection(), data->get_sliderMidAnchorMode(),
        data->customData);
  });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_BurstSliderData*>([&BeatToTime](
      v3::CustomBeatmapSaveData_BurstSliderData * data) constexpr {
    return CustomSliderData_CreateCustomBurstSliderData(
        ConvertColorType(data->get_colorType()), BeatToTime(data->b), data->get_headLine(),
        ConvertNoteLineLayer(data->get_headLayer()), ConvertNoteLineLayer(data->get_tailLayer()),
        data->get_headCutDirection(), BeatToTime(data->get_tailBeat()), data->get_tailLine(),
        ConvertNoteLineLayer(data->get_tailLayer()), ConvertNoteLineLayer(data->get_tailLayer()), NoteCutDirection::Any,
        data->get_sliceCount(), data->get_squishAmount(), data->customData);
  });

  objectConverter.AddConverter<BeatmapSaveData::WaypointData*>([&BeatToTime](BeatmapSaveData::WaypointData *
                                                                             data) constexpr {
    return CustomJSONData::NewFast<WaypointData*>(BeatToTime(data->b), data->get_line(),
                                                  ConvertNoteLineLayer(data->get_layer()), data->get_offsetDirection());
  });

  auto addAllToVector = [](auto& vec, auto const& listPtr) constexpr {
    VList<BeatmapSaveData::BeatmapSaveDataItem*> vList(listPtr);

    std::copy(vList.begin(), vList.end(), std::back_inserter(vec));
  };

  auto addAllToVector2 = [](auto& vec, auto const& listPtr) constexpr {
    VList<BeatmapSaveData::EventBoxGroup*> vList(listPtr);

    
    std::copy(vList.begin(), vList.end(), std::back_inserter(vec));
  };

  auto cleanAndSort = [](auto& vec) constexpr {
    for (auto it = vec.begin(); it != vec.end();) {
      auto const& v = *it;
      if (!v) {
        it = vec.erase(it);
        continue;
      }

      it++;
    }

    std::stable_sort(vec.begin(), vec.end(),
                     TimeCompare<typename std::remove_reference_t<decltype(vec)>::const_reference>);
  };

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Creating object list");

  std::vector<BeatmapSaveData::BeatmapSaveDataItem*> beatmapDataObjectItems;
  beatmapDataObjectItems.reserve(beatmapSaveData->colorNotes->get_Count() + beatmapSaveData->bombNotes->get_Count() +
                                 beatmapSaveData->obstacles->get_Count() + beatmapSaveData->sliders->get_Count() +
                                 beatmapSaveData->burstSliders->get_Count() + beatmapSaveData->waypoints->get_Count());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Color notes {}", fmt::ptr(beatmapSaveData->colorNotes));
  addAllToVector(beatmapDataObjectItems, beatmapSaveData->colorNotes);
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Bomb notes {}", fmt::ptr(beatmapSaveData->bombNotes));
  addAllToVector(beatmapDataObjectItems, beatmapSaveData->bombNotes);
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Obstacles {}", fmt::ptr(beatmapSaveData->obstacles));
  addAllToVector(beatmapDataObjectItems, beatmapSaveData->obstacles);
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Sliders {}", fmt::ptr(beatmapSaveData->sliders));
  addAllToVector(beatmapDataObjectItems, beatmapSaveData->sliders);
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Burst sliders {}", fmt::ptr(beatmapSaveData->burstSliders));
  addAllToVector(beatmapDataObjectItems, beatmapSaveData->burstSliders);
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Waypoints {}", fmt::ptr(beatmapSaveData->waypoints));
  addAllToVector(beatmapDataObjectItems, beatmapSaveData->waypoints);

  profile.mark("Grouped all beatmap objects size {}", !beatmapDataObjectItems.empty());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Cleaning and sorting beatmap objects");
  cleanAndSort(beatmapDataObjectItems);
  for (auto const& o : beatmapDataObjectItems) {
    auto* beatmapObjectData = objectConverter.ProcessItem(o);
    if (beatmapObjectData != nullptr) {
      beatmapData->AddBeatmapObjectDataOverride(beatmapObjectData);
    }
  }
  if (stopwatch != nullptr) {
    stopwatch->Start();
  }

  profile.mark("Processed and added beatmap objects");
  bpmTimeProcessor.Reset();

  CppConverter<BeatmapEventData*, BeatmapSaveData::BeatmapSaveDataItem*> eventConverter;
  eventConverter.AddConverter<BeatmapSaveData::BpmChangeEventData*>([&BeatToTime](
      BeatmapSaveData::BeatmapSaveData::BpmChangeEventData * data) constexpr {
    return CustomJSONData::NewFast<BPMChangeBeatmapEventData*>(BeatToTime(data->b), data->m);
  });

  eventConverter.AddConverter<BeatmapSaveData::RotationEventData*>([&BeatToTime](
      BeatmapSaveData::BeatmapSaveData::RotationEventData * data) constexpr {
    SpawnRotationBeatmapEventData::SpawnRotationEventType executionTime =
        data->get_executionTime() == BeatmapSaveData::ExecutionTime::Early
            ? SpawnRotationBeatmapEventData::SpawnRotationEventType::Early
            : SpawnRotationBeatmapEventData::SpawnRotationEventType::Late;

    return CustomJSONData::NewFast<SpawnRotationBeatmapEventData*>(BeatToTime(data->b), executionTime,
                                                                   data->get_rotation());
  });

  // only for v3 maps
  auto specialEventsFilter = SafePtr(BeatmapDataLoader::SpecialEventsFilter::New_ctor(
      beatmapSaveData->basicEventTypesWithKeywords, environmentKeywords));
  if (flag3 || !defaultLightshowEventsSaveData->isEmpty) {
    eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>(
        [&BeatToTime, &specialEventsFilter ](v3::CustomBeatmapSaveData_BasicEventData * data) constexpr {
          if (!specialEventsFilter->IsEventValid(data->eventType)) {
            return static_cast<CustomBeatmapEventData*>(nullptr);
          }

          auto* event = CustomBeatmapEventData::New_ctor(BeatToTime(data->b), data->eventType.value__, data->value,
                                                         data->floatValue);

          event->customData = ToJsonWrapper(data->customData);

          return event;
        });

    eventConverter.AddConverter<BeatmapSaveData::ColorBoostEventData*>([&BeatToTime](
        BeatmapSaveData::ColorBoostEventData * data) constexpr {
      return CustomJSONData::NewFast<ColorBoostBeatmapEventData*>(BeatToTime(data->beat), data->boost);
    });
  } else {
    eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>(
        [](v3::CustomBeatmapSaveData_BasicEventData * /*data*/) constexpr { return nullptr; });

    eventConverter.AddConverter<BeatmapSaveData::ColorBoostEventData*>(
        [](BeatmapSaveData::ColorBoostEventData * /*data*/) constexpr { return nullptr; });
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Doing event items");
  std::vector<BeatmapSaveData::BeatmapSaveDataItem*> beatmapDataEventItems;
  beatmapDataEventItems.reserve(bpmEvents->get_Count() + beatmapSaveData->basicBeatmapEvents->get_Count() +
                                beatmapSaveData->colorBoostBeatmapEvents->get_Count() +
                                beatmapSaveData->rotationEvents->get_Count());

  ListW<BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData*> list = beatmapSaveData->basicBeatmapEvents;
  ListW<BeatmapSaveDataVersion3::BeatmapSaveData::ColorBoostEventData*> list2 = beatmapSaveData->colorBoostBeatmapEvents;
  if (!flag3) {
    list = ((defaultLightshowEventsSaveData != nullptr) ? defaultLightshowEventsSaveData->basicBeatmapEvents : nullptr);
    if (!list) {
      list = ListW<BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData*>();
    }

    list2 = ((defaultLightshowEventsSaveData != nullptr) ? defaultLightshowEventsSaveData->colorBoostBeatmapEvents
                                                         : nullptr);
    if (!list2) {
      list2 = ListW<BeatmapSaveDataVersion3::BeatmapSaveData::ColorBoostEventData*>();
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("bpm events");
  addAllToVector(beatmapDataEventItems, bpmEvents.getPtr());
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("basic events");
  addAllToVector(beatmapDataEventItems, list.getPtr());
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("color boost events");
  addAllToVector(beatmapDataEventItems, list2.getPtr());
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("rotation events");
  addAllToVector(beatmapDataEventItems, beatmapSaveData->rotationEvents);

  profile.mark("Grouped beatmap events");

  cleanAndSort(beatmapDataEventItems);
  for (auto const& o : beatmapDataEventItems) {
    auto* beatmapEventData = eventConverter.ProcessItem(o);

    if (beatmapEventData != nullptr) {
      beatmapData->InsertBeatmapEventDataOverride(beatmapEventData);
    }
  }
  bpmTimeProcessor.Reset();

  CJDLogger::Logger.fmtLog<LogLevel::INF>("Beatmap events {}", beatmapData->beatmapEventDatas.size());

  profile.mark("Processed and added beatmap events");

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("event groups");
  auto* bpmTimeProcessorIl2cpp = CustomJSONData::NewFast<BeatmapDataLoader::BpmTimeProcessor*>(startBpm, bpmEvents);
  bpmTimeProcessorIl2cpp->currentBpmChangesDataIdx = bpmTimeProcessor.currentBpmChangesDataIdx;

  auto* beatmapEventDataBoxGroupLists = CustomJSONData::NewFast<BeatmapEventDataBoxGroupLists*>(
      beatmapData, bpmTimeProcessorIl2cpp->i___GlobalNamespace__IBeatToTimeConvertor(), false);
  if (flag3 || !defaultLightshowEventsSaveData->isEmpty) {

    ListW<BeatmapSaveDataVersion3::BeatmapSaveData::LightColorEventBoxGroup*> lightColorEventBoxGroups =
        beatmapSaveData->lightColorEventBoxGroups;
    ListW<BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationEventBoxGroup*> lightRotationEventBoxGroups =
        beatmapSaveData->lightRotationEventBoxGroups;
    ListW<BeatmapSaveDataVersion3::BeatmapSaveData::LightTranslationEventBoxGroup*> lightTranslationEventBoxGroups =
        beatmapSaveData->lightTranslationEventBoxGroups;
    BeatmapSaveDataVersion3::BeatmapSaveData::FxEventsCollection* fxEventsCollection = beatmapSaveData->_fxEventsCollection;
    ListW<BeatmapSaveDataVersion3::BeatmapSaveData::FxEventBoxGroup*> vfxEventBoxGroups = beatmapSaveData->vfxEventBoxGroups;
    if (!flag3) {
      lightColorEventBoxGroups = defaultLightshowEventsSaveData->lightColorEventBoxGroups;
      lightRotationEventBoxGroups = defaultLightshowEventsSaveData->lightRotationEventBoxGroups;
      lightTranslationEventBoxGroups = defaultLightshowEventsSaveData->lightTranslationEventBoxGroups;
      fxEventsCollection = defaultLightshowEventsSaveData->_fxEventsCollection;
      vfxEventBoxGroups = defaultLightshowEventsSaveData->vfxEventBoxGroups;
    }

    EventBoxGroupConvertor cppEventBoxConverter(environmentLightGroups);


    
    std::vector<BeatmapSaveData::EventBoxGroup*> eventBoxes;
    eventBoxes.reserve(beatmapSaveData->lightColorEventBoxGroups->get_Count() +
                       beatmapSaveData->lightRotationEventBoxGroups->get_Count() +
                       beatmapSaveData->lightTranslationEventBoxGroups->get_Count());

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightColorEventBoxGroups handling events");
    addAllToVector2(eventBoxes, beatmapSaveData->lightColorEventBoxGroups);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightRotationEventBoxGroups handling events");
    addAllToVector2(eventBoxes, beatmapSaveData->lightRotationEventBoxGroups);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightTranslationEventBoxGroups handling events");
    addAllToVector2(eventBoxes, beatmapSaveData->lightTranslationEventBoxGroups);

    cleanAndSort(eventBoxes);

    profile.mark(fmt::format("Grouped beatmap event boxes {}", eventBoxes.size()));

    for (auto const& o : eventBoxes) {
      auto* beatmapEventDataBoxGroup = cppEventBoxConverter.Convert(o); // eventBoxGroupConvertor->Convert(o);
      if (beatmapEventDataBoxGroup != nullptr) {
        beatmapEventDataBoxGroupLists->Insert(o->g, beatmapEventDataBoxGroup);
      }
    }

    BeatmapDataLoader::VfxEventBoxGroupConvertor* vfxEventBoxGroupConvertor =
        BeatmapDataLoader::VfxEventBoxGroupConvertor::New_ctor(environmentLightGroups, fxEventsCollection);

    for (auto const& fxEventBoxGroup :
         ListW<BeatmapSaveDataVersion3::BeatmapSaveData::FxEventBoxGroup*>(vfxEventBoxGroups)) {
      BeatmapEventDataBoxGroup* beatmapEventDataBoxGroup2 = vfxEventBoxGroupConvertor->Convert(fxEventBoxGroup);
      if (beatmapEventDataBoxGroup2 != nullptr) {
        beatmapEventDataBoxGroupLists->Insert(fxEventBoxGroup->groupId, beatmapEventDataBoxGroup2);
      }
    }

    profile.mark("Processed and added beatmap events boxes");
  } else {
    DefaultEnvironmentEventsFactory::InsertDefaultEnvironmentEvents(beatmapData, beatmapEventDataBoxGroupLists,
                                                                   defaultEnvironmentEvents, environmentLightGroups);
  }

  bpmTimeProcessor.Reset();
  if (auto customBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData)) {
    if (customBeatmapSaveData.value()->customEventsData) {
      std::stable_sort(
          customBeatmapSaveData.value()->customEventsData->begin(),
          customBeatmapSaveData.value()->customEventsData->end(),
          [](auto const& a, auto const& b) constexpr { return a.time < b.time; });

      for (auto& customEventSaveData : *customBeatmapSaveData.value()->customEventsData) {
        beatmapData->InsertCustomEventData(CustomEventData::New_ctor(
            bpmTimeProcessor.ConvertBeatToTime(customEventSaveData.time), static_cast<void*>(&customEventSaveData.type),
            customEventSaveData.typeHash, static_cast<void*>(const_cast<rapidjson::Value*>(customEventSaveData.data))));
      }

      CJDLogger::Logger.fmtLog<LogLevel::INF>("Added {} custom events",
                                              customBeatmapSaveData.value()->customEventsData->size());
      profile.mark("Processed beatmap custom events");
    }
  }

  // Figure out a way to rewrite this to not be stupid
  beatmapEventDataBoxGroupLists->SyncWithBeatmapData();
  profile.mark("Syncing event box groups");

  beatmapData->ProcessRemainingData();

  profile.mark("Processed processed remaining data");

  beatmapData->ProcessAndSortBeatmapData();

  profile.mark("Processed and sorted beatmap data");

  profile.endTimer();
  profile.printMarks(CJDLogger::Logger.tag);

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished processing beatmap data");
  auto stopTime = std::chrono::high_resolution_clock::now();
  CJDLogger::Logger.fmtLog<LogLevel::DBG>(
      "This took {}ms",
      static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count()));

  if (stopwatch != nullptr) {
    stopwatch->Stop();
  }

  return beatmapData;
}

// CustomJSONData::CustomLevelInfoSaveData*, std::string const&, BeatmapSaveDataVersion3::BeatmapSaveData*,
// GlobalNamespace::BeatmapDataBasicInfo*
void BeatmapDataLoadedEvent(CustomJSONData::CustomLevelInfoSaveData* customInfoData, std::string const& filename,
                            BeatmapSaveDataVersion3::BeatmapSaveData* beatmapData,
                            GlobalNamespace::BeatmapDataBasicInfo* /*unused*/) {
  PAPER_IL2CPP_CATCH_HANDLER(
      using namespace Sombrero::Linq::Functional; if (!beatmapData) {
        CJDLogger::Logger.fmtLog<LogLevel::WRN>("Beatmap is null, no custom level data");
        return;
      } CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Setting info.dat customData to beatmap");

      auto* customBeatmapData = il2cpp_utils::cast<v3::CustomBeatmapSaveData>(beatmapData);

      if (customInfoData->customData && !customInfoData->customData->get().IsNull()) {
        customBeatmapData->beatmapCustomData = customInfoData->customData->get();
      }

      // TODO: SelectMany
      StandardLevelInfoSaveData::DifficultyBeatmap* diff =
          customInfoData->difficultyBeatmapSets |
          Select([&](StandardLevelInfoSaveData::DifficultyBeatmapSet* beatmapSet) {
            return beatmapSet->difficultyBeatmaps |
                   FirstOrDefault([&](StandardLevelInfoSaveData::DifficultyBeatmap* diff) {
                     if (!diff || !diff->beatmapFilename) return false;

                     auto* customDiff = il2cpp_utils::cast<CustomJSONData::CustomDifficultyBeatmap>(diff);

                     return customDiff->beatmapFilename == filename;
                   });
          }) |
          FirstOrDefault([](auto x) { return x; });

      if (!diff) { CJDLogger::Logger.fmtLog<LogLevel::INF>("No custom diff found"); }

      v3::CustomDataOptUTF16 levelCustomData =
          il2cpp_utils::cast<CustomJSONData::CustomDifficultyBeatmap>(diff)->customData;
      if (levelCustomData && !levelCustomData->get().IsNull()) {
        customBeatmapData->levelCustomData = levelCustomData;
      } CJDLogger::Logger.fmtLog<LogLevel::INF>("Done with beatmap info.dat");)
}

void CustomJSONData::InstallHooks() {
  auto logger = CJDLogger::GetLoggerOld().WithContext("InstallHooks");

  // Install hooks
  // Stupid workaround because stupid NE
  INSTALL_HOOK_ORIG(logger, BeatmapCallbacksController_ManualUpdateTranspile)
  INSTALL_HOOK_ORIG(logger, BeatmapSaveData_ConvertBeatmapSaveData)
  INSTALL_HOOK_ORIG(logger, BeatmapSaveData_DeserializeFromJSONString)
  INSTALL_HOOK_ORIG(logger, GetBeatmapDataFromBeatmapSaveData)
  INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_InsertItem);
  INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_RemoveItem);
  INSTALL_HOOK_ORIG(logger, BeatmapData_GetFilteredCopy);
  INSTALL_HOOK_ORIG(logger, BeatmapData_GetCopy);
  INSTALL_HOOK_ORIG(logger, BeatmapDataStrobeFilterTransform_CreateTransformedData);
  INSTALL_HOOK_ORIG(logger, InsertDefaultEnvironmentEvents);
  INSTALL_HOOK(logger, BeatmapCallbacksController_Dispose);

  RuntimeSongLoader::API::AddBeatmapDataBasicInfoLoadedEvent(BeatmapDataLoadedEvent);

  il2cpp_functions::Class_Init(classof(BeatmapData*));
  custom_types::Register::AutoRegister();
}
