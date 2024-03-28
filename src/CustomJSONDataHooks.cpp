#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "songcore/shared/CustomJSONData.hpp"

#include "BeatmapDataLoaderVersion2_6_0AndEarlier/BeatmapDataLoader.hpp"
#include "BeatmapDataLoaderVersion3/BeatmapDataLoader.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveDataItem.hpp"
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

#include "UnityEngine/JsonUtility.hpp"

#include "System/Reflection/MemberInfo.hpp"
#include "System/Collections/Generic/InsertionBehavior.hpp"

#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

#include <regex>

// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"
#include "paper/shared/Profiler.hpp"
#include "sombrero/shared/linq_functional.hpp"
#include "sombrero/shared/Vector2Utils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "BeatmapFieldUtils.hpp"
#include "BeatmapDataLoaderUtils.hpp"

#include <chrono>
#include <codecvt>
#include <locale>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;
using namespace BeatmapSaveDataVersion4;

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
    return name(self, param1);                                                                                         \
    }                                                                                                                  \
  }                                                                                                                    \
  void name##__override(Child* self, ParamT1 param1)

static std::string GetVersionFromPath(std::string_view path) {
  // SongCore has a fallback so i guess i do too
  static std::string_view const fallback = "2.0.0";

  auto truncatedText = path.substr(0, 50);
  static std::regex const versionRegex(R"("_?version"\s*:\s*"[0-9]+\.[0-9]+\.?[0-9]?")");
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
    return reinterpret_cast<CustomBeatmapData*>(self)->GetFilteredCopyOverride(
        [&](auto i) constexpr { return processDataItem->Invoke(i); });
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
  std::unordered_map<int, BeatmapDataStrobeFilterTransform::StrobeStreakData*> dictionary(
      { { BasicBeatmapEventType::Event0.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
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

//TODO: split up
std::optional<v2::CustomBeatmapSaveData*> GetBeatmapSaveDataFromJsonString_v2(StringW stringData) {
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

    v2::CustomBeatmapSaveData* saveData = nullptr;

    if (semver::lte(std::string(version), "2.6.0")) {
      CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 2.0.0 beatmap");

      saveData = il2cpp_utils::cast<v2::CustomBeatmapSaveData>(v2::CustomBeatmapSaveData::Deserialize(sharedDoc));
    } else {
      return std::nullopt;
    }

    //cachedSaveData = saveData;

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished reading beatmap data {}", fmt::ptr(saveData));
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::Logger.fmtLog<LogLevel::DBG>(
        "This took {}ms",
        static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count()));

    return saveData;
  } catch (std::exception const& e) {
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("There was an error loading the beatmap through CJD. Cause of error: {}",
                                            e.what());
    return std::nullopt;
  }
}

CustomBeatmapData* GetBeatmapDataFromSaveData_v3(v3::CustomBeatmapSaveData* beatmapSaveData, GlobalNamespace::BeatmapDifficulty beatmapDifficulty, 
                                                float_t startBpm, bool loadingForDesignatedEnvironment, 
                                                GlobalNamespace::EnvironmentKeywords* environmentKeywords, GlobalNamespace::IEnvironmentLightGroups* environmentLightGroups, 
                                                GlobalNamespace::DefaultEnvironmentEvents* defaultEnvironmentEvents, BeatmapSaveDataVersion4::LightshowSaveData* defaultLightshowEventsSaveData, 
                                                GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, std::string version) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing save data {}", fmt::ptr(beatmapSaveData));
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
         VList<BeatmapSaveDataCommon::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>(
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

  ListW<BeatmapSaveDataVersion3::BpmChangeEventData*> bpmEvents = beatmapSaveData->bpmEvents;
  CustomJSONData::BpmTimeProcessor bpmTimeProcessor(startBpm, bpmEvents); 

  auto const BeatToTime = [&bpmTimeProcessor](float beat) constexpr {
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Start BeatToTime");
    auto time = bpmTimeProcessor.ConvertBeatToTime(beat);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Stop BeatToTime");
    return time;
  };

  profile.mark("Parsed bpm events");

  // TODO: Remove converter
  CppConverter<GlobalNamespace::BeatmapObjectData*, BeatmapSaveDataVersion3::BeatmapSaveDataItem*> objectConverter;
  objectConverter.AddConverter<v3::CustomBeatmapSaveData_ColorNoteData*>(
      [&BeatToTime](v3::CustomBeatmapSaveData_ColorNoteData* data) {
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("CJDCONVERTER customData copy 3 {}", data->customData->value.has_value());
        auto* noteData =
            CreateCustomBasicNoteData(BeatToTime(data->b), data->get_line(), ConvertNoteLineLayer(data->layer),
                                      ConvertColorType(data->get_color()), ConvertCutDirection(data->get_cutDirection()), data->customData);

        return noteData;
      });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_BombNoteData*>(
      [&BeatToTime](v3::CustomBeatmapSaveData_BombNoteData* data) constexpr {
        auto* b = CreateCustomBombNoteData(BeatToTime(data->b), data->get_line(),
                                           ConvertNoteLineLayer(data->get_layer()), data->customData);
        return b;
      });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_ObstacleData*>(
      [&BeatToTime](v3::CustomBeatmapSaveData_ObstacleData* data) constexpr {
        float beat = BeatToTime(data->b);
        auto* obstacle = CustomObstacleData::New_ctor(
            beat, data->get_line(), BeatmapDataLoaderVersion3::BeatmapDataLoader::ObstacleConverter::GetNoteLineLayer(data->get_layer()),
            BeatToTime(data->b + data->get_duration()) - beat, data->get_width(), data->get_height());

        obstacle->customData = data->customData->GetCopy();

        return obstacle;
      });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_SliderData*>(
      [&BeatToTime](v3::CustomBeatmapSaveData_SliderData* data) {
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("CJDCONVERTER customData copy {}", data->customData->value.has_value());
        return CustomSliderData_CreateCustomSliderData(
            ConvertColorType(data->get_colorType()), BeatToTime(data->b), data->get_headLine(),
            ConvertNoteLineLayer(data->get_headLayer()), ConvertNoteLineLayer(data->get_headLayer()),
            data->get_headControlPointLengthMultiplier(), ConvertCutDirection(data->get_headCutDirection()),
            BeatToTime(data->get_tailBeat()), data->get_tailLine(), ConvertNoteLineLayer(data->get_tailLayer()),
            ConvertNoteLineLayer(data->get_tailLayer()), data->get_tailControlPointLengthMultiplier(),
            ConvertCutDirection(data->get_tailCutDirection()), ConvertSliderMidAnchorMode(data->get_sliderMidAnchorMode()), data->customData);
      });

  objectConverter.AddConverter<v3::CustomBeatmapSaveData_BurstSliderData*>(
      [&BeatToTime](v3::CustomBeatmapSaveData_BurstSliderData* data) {
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("CJDCONVERTER customData copy 2 {}", data->customData->value.has_value());
        return CustomSliderData_CreateCustomBurstSliderData(
            ConvertColorType(data->get_colorType()), BeatToTime(data->b), data->get_headLine(),
            ConvertNoteLineLayer(data->get_headLayer()), ConvertNoteLineLayer(data->get_tailLayer()),
            ConvertCutDirection(data->get_headCutDirection()), BeatToTime(data->get_tailBeat()), data->get_tailLine(),
            ConvertNoteLineLayer(data->get_tailLayer()), ConvertNoteLineLayer(data->get_tailLayer()),
            NoteCutDirection::Any, data->get_sliceCount(), data->get_squishAmount(), data->customData);
      });

  objectConverter.AddConverter<BeatmapSaveDataVersion3::WaypointData*>([&BeatToTime](
                                                                   BeatmapSaveDataVersion3::WaypointData* data) constexpr {
    return CustomJSONData::NewFast<GlobalNamespace::WaypointData*>(BeatToTime(data->b), data->get_line(),
                                                  ConvertNoteLineLayer(data->get_layer()), GlobalNamespace::OffsetDirection(data->get_offsetDirection().value__));
  });

  auto addAllToVector = [&](auto& vec, auto const& listPtr) constexpr {
    VList<BeatmapSaveDataVersion3::BeatmapSaveDataItem*> vList(listPtr);

    std::copy(vList.begin(), vList.end(), std::back_inserter(vec));
  };

  auto addAllToVector2 = [&](auto& vec, auto const& listPtr) constexpr {
    VList<BeatmapSaveDataVersion3::EventBoxGroup*> vList(listPtr);

    std::copy(vList.begin(), vList.end(), std::back_inserter(vec));
  };

  auto cleanAndSort = [&](auto& vec) constexpr {
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

  std::vector<BeatmapSaveDataVersion3::BeatmapSaveDataItem*> beatmapDataObjectItems;
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

  profile.mark("Processed and added beatmap objects");
  bpmTimeProcessor.Reset();

  CppConverter<BeatmapEventData*, BeatmapSaveDataVersion3::BeatmapSaveDataItem*> eventConverter;
  eventConverter.AddConverter<BeatmapSaveDataVersion3::BpmChangeEventData*>(
      [&BeatToTime](BeatmapSaveDataVersion3::BpmChangeEventData* data) constexpr {
        return CustomJSONData::NewFast<BPMChangeBeatmapEventData*>(BeatToTime(data->b), data->m);
      });

  eventConverter.AddConverter<BeatmapSaveDataVersion3::RotationEventData*>(
      [&BeatToTime](BeatmapSaveDataVersion3::RotationEventData* data) constexpr {
        SpawnRotationBeatmapEventData::SpawnRotationEventType executionTime =
            data->get_executionTime() == BeatmapSaveDataCommon::ExecutionTime::Early
                ? SpawnRotationBeatmapEventData::SpawnRotationEventType::Early
                : SpawnRotationBeatmapEventData::SpawnRotationEventType::Late;

        return CustomJSONData::NewFast<SpawnRotationBeatmapEventData*>(BeatToTime(data->b), executionTime,
                                                                       data->get_rotation());
      });

  // only for v3 maps
  auto specialEventsFilter = SafePtr(BeatmapDataLoaderVersion3::BeatmapDataLoader::SpecialEventsFilter::New_ctor(
      beatmapSaveData->basicEventTypesWithKeywords, environmentKeywords));
  if (flag3 || !defaultLightshowEventsSaveData) {
    eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>(
        [&BeatToTime, &specialEventsFilter](v3::CustomBeatmapSaveData_BasicEventData* data) constexpr {
          if (!specialEventsFilter->IsEventValid(data->eventType)) {
            return static_cast<CustomBeatmapEventData*>(nullptr);
          }

          auto* event = CustomBeatmapEventData::New_ctor(BeatToTime(data->b), data->eventType.value__, data->value,
                                                         data->floatValue);

          event->customData = ToJsonWrapper(data->customData);

          return event;
        });

    eventConverter.AddConverter<BeatmapSaveDataVersion3::ColorBoostEventData*>(
        [&BeatToTime](BeatmapSaveDataVersion3::ColorBoostEventData* data) constexpr {
          return CustomJSONData::NewFast<ColorBoostBeatmapEventData*>(BeatToTime(data->beat), data->boost);
        });
  } else {
    eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>(
        [](v3::CustomBeatmapSaveData_BasicEventData* /*data*/) constexpr { return nullptr; });

    eventConverter.AddConverter<BeatmapSaveDataVersion3::ColorBoostEventData*>(
        [](BeatmapSaveDataVersion3::ColorBoostEventData* /*data*/) constexpr { return nullptr; });
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Doing event items");
  std::vector<BeatmapSaveDataVersion3::BeatmapSaveDataItem*> beatmapDataEventItems;
  beatmapDataEventItems.reserve(bpmEvents->get_Count() + beatmapSaveData->basicBeatmapEvents->get_Count() +
                                beatmapSaveData->colorBoostBeatmapEvents->get_Count() +
                                beatmapSaveData->rotationEvents->get_Count());

  ListW<BeatmapSaveDataVersion3::BasicEventData*> list = beatmapSaveData->basicBeatmapEvents;
  ListW<BeatmapSaveDataVersion3::ColorBoostEventData*> list2 =
      beatmapSaveData->colorBoostBeatmapEvents;

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
  auto* bpmTimeProcessorIl2cpp = CustomJSONData::NewFast<GlobalNamespace::BpmTimeProcessor*>(startBpm, bpmEvents->i___System__Collections__Generic__IReadOnlyList_1_T_());
  bpmTimeProcessorIl2cpp->currentBpmChangesDataIdx = bpmTimeProcessor.currentBpmChangesDataIdx;

  auto* beatmapEventDataBoxGroupLists = CustomJSONData::NewFast<BeatmapEventDataBoxGroupLists*>(
      beatmapData, bpmTimeProcessorIl2cpp->i___GlobalNamespace__IBeatToTimeConverter(), false);
  if (flag3 || !defaultLightshowEventsSaveData) {

    ListW<BeatmapSaveDataVersion3::LightColorEventBoxGroup*> lightColorEventBoxGroups =
        beatmapSaveData->lightColorEventBoxGroups;
    ListW<BeatmapSaveDataVersion3::LightRotationEventBoxGroup*> lightRotationEventBoxGroups =
        beatmapSaveData->lightRotationEventBoxGroups;
    ListW<BeatmapSaveDataVersion3::LightTranslationEventBoxGroup*> lightTranslationEventBoxGroups =
        beatmapSaveData->lightTranslationEventBoxGroups;
    BeatmapSaveDataVersion3::FxEventsCollection* fxEventsCollection =
        beatmapSaveData->_fxEventsCollection;
    ListW<BeatmapSaveDataVersion3::FxEventBoxGroup*> vfxEventBoxGroups =
        beatmapSaveData->vfxEventBoxGroups;
    /*if (!flag3) {
      lightColorEventBoxGroups = defaultLightshowEventsSaveData->lightColorEventBoxGroups;
      lightRotationEventBoxGroups = defaultLightshowEventsSaveData->lightRotationEventBoxGroups;
      lightTranslationEventBoxGroups = defaultLightshowEventsSaveData->lightTranslationEventBoxGroups;
      fxEventsCollection = defaultLightshowEventsSaveData->_fxEventsCollection;
      vfxEventBoxGroups = defaultLightshowEventsSaveData->vfxEventBoxGroups;
    }*/

    EventBoxGroupConvertor cppEventBoxConverter(environmentLightGroups);

    std::vector<BeatmapSaveDataVersion3::EventBoxGroup*> eventBoxes;
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

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Sorted events");

    profile.mark(fmt::format("Grouped beatmap event boxes {}", eventBoxes.size()));

    for (auto const& o : eventBoxes) {
      auto* beatmapEventDataBoxGroup = cppEventBoxConverter.Convert(o); // eventBoxGroupConvertor->Convert(o);
      if (beatmapEventDataBoxGroup != nullptr) {
        beatmapEventDataBoxGroupLists->Insert(o->g, beatmapEventDataBoxGroup);
      }
    }

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("VFX box group");
    profile.mark("Processed and added beatmap events boxes");
  } else {
    DefaultEnvironmentEventsFactory::InsertDefaultEvents(beatmapData);
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Custom events");
  bpmTimeProcessor.Reset();
  if (auto customBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData)) {
    if (customBeatmapSaveData.value()->customEventsData) {
      std::stable_sort(customBeatmapSaveData.value()->customEventsData->begin(),
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

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished! processing");
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

  return beatmapData;
}

MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v2, &BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::GetBeatmapDataFromSaveDataJson, GlobalNamespace::BeatmapData*,
                      StringW beatmapJson, StringW defaultLightshowSaveDataJson, GlobalNamespace::BeatmapDifficulty beatmapDifficulty, float_t startBpm, bool loadingForDesignatedEnvironment, GlobalNamespace::IEnvironmentInfo* environmentInfo, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Loading Beatmap Data");

  auto saveData = GetBeatmapSaveDataFromJsonString_v2(beatmapJson);
  if(!saveData) {
    return BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v2(beatmapJson, defaultLightshowSaveDataJson, beatmapDifficulty, startBpm, loadingForDesignatedEnvironment, environmentInfo, playerSpecificSettings);
  }
  auto saveDataPtr = saveData.value();

  LightshowSaveData* lightshowSaveData = ((!String::IsNullOrEmpty(defaultLightshowSaveDataJson)) ? UnityEngine::JsonUtility::FromJson<LightshowSaveData*>(defaultLightshowSaveDataJson) : nullptr);

  EnvironmentKeywords* environmentKeywords;
  IEnvironmentLightGroups* environmentLightGroups;

  if (environmentInfo != nullptr)
	{
		environmentKeywords = EnvironmentKeywords::New_ctor(environmentInfo->environmentKeywords);
		environmentLightGroups = environmentInfo->environmentLightGroups;
	}
	else
	{
		environmentKeywords = EnvironmentKeywords::New_ctor(ListW<StringW>({})->i___System__Collections__Generic__IReadOnlyList_1_T_());
		environmentLightGroups = EnvironmentLightGroups::New_ctor(ListW<UnityW<LightGroupSO>>::New().getPtr())->i___GlobalNamespace__IEnvironmentLightGroups();
	}

  std::string contents(beatmapJson);
  auto version = GetVersionFromPath(contents);

  auto beatmapData = GetBeatmapDataFromSaveData_v3(v3::CustomBeatmapSaveData::Convert2_6_0(saveDataPtr), beatmapDifficulty, startBpm, loadingForDesignatedEnvironment, environmentKeywords, environmentLightGroups, nullptr, lightshowSaveData, playerSpecificSettings, version);
  return beatmapData;
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

MAKE_PAPER_HOOK_MATCH(InsertDefaultEvents, &DefaultEnvironmentEventsFactory::InsertDefaultEvents,
                      void, BeatmapData* beatmapData) {
  // TRANSPILE HERE
  beatmapData->InsertBeatmapEventData(CustomBeatmapEventData::New_ctor(0.0F, BasicBeatmapEventType::Event0, 1, 1.0F));
  beatmapData->InsertBeatmapEventData(CustomBeatmapEventData::New_ctor(0.0F, BasicBeatmapEventType::Event4, 1, 1.0F));
  // END TRANSPILE HERE
}

void CustomJSONData::InstallHooks() {
  auto logger = Paper::ConstLoggerContext("CJDHooks");

  // Install hooks
  // Stupid workaround because stupid NE
  INSTALL_HOOK_ORIG(logger, BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v2)
  INSTALL_HOOK_ORIG(logger, BeatmapCallbacksController_ManualUpdateTranspile);
  INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_InsertItem);
  INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_RemoveItem);
  INSTALL_HOOK_ORIG(logger, BeatmapData_GetFilteredCopy);
  INSTALL_HOOK_ORIG(logger, BeatmapData_GetCopy);
  INSTALL_HOOK_ORIG(logger, BeatmapDataStrobeFilterTransform_CreateTransformedData);
  INSTALL_HOOK_ORIG(logger, InsertDefaultEvents);
  INSTALL_HOOK(logger, BeatmapCallbacksController_Dispose);

  il2cpp_functions::Class_Init(classof(BeatmapData*));
  custom_types::Register::AutoRegister();
}
