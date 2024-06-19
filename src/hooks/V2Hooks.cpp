#include "CustomBeatmapSaveDatav2.h"

#include "CustomJSONDataHooks.h"

#include "HookUtils.hpp"

#include "BeatmapSaveDataVersion2_6_0AndEarlier/zzzz__EventData_def.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveDataItem.hpp"
#include "BeatmapDataLoaderVersion2_6_0AndEarlier/BeatmapDataLoader.hpp"

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
#include "GlobalNamespace/LightColorBeatmapEventData.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"

#include "UnityEngine/JsonUtility.hpp"

#include "beatsaber-hook/shared/utils/typedefs-list.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"

#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapData.h"
#include "misc/BeatmapFieldUtils.hpp"
#include "misc/BeatmapDataLoaderUtils.hpp"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"
#include "paper/shared/Profiler.hpp"

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;
using namespace BeatmapSaveDataVersion4;

std::optional<v2::CustomBeatmapSaveData*> ParseBeatmapSaveDataJson_v2(StringW stringData) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json");
  auto startTime = std::chrono::high_resolution_clock::now();

  if (!stringData) {
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("No string data");
    return std::nullopt;
  }

  try {
    auto contents = std::string(stringData);
    auto sharedDoc = parseDocument(contents);
    if (!sharedDoc) return nullptr;

    auto version = GetVersionFromPath(contents);

    v2::CustomBeatmapSaveData* saveData = v2::CustomBeatmapSaveData::Deserialize(*sharedDoc);

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 2.0.0 beatmap");

    // cachedSaveData = saveData;

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

// clang-format off
/*
			if (string.IsNullOrEmpty(beatmapJson))
			{
				return null;
			}
			BeatmapSaveDataVersion2_6_0AndEarlier.BeatmapSaveData beatmapSaveData = JsonUtility.FromJson<BeatmapSaveDataVersion2_6_0AndEarlier.BeatmapSaveData>(beatmapJson);
			if (beatmapSaveData == null)
			{
				return null;
			}
			LightshowSaveData defaultLightshowSaveData = (!string.IsNullOrEmpty(defaultLightshowSaveDataJson)) ? JsonUtility.FromJson<LightshowSaveData>(defaultLightshowSaveDataJson) : null;
			EnvironmentKeywords environmentKeywords;
			IEnvironmentLightGroups environmentLightGroups;
			if (environmentInfo != null)
			{
				environmentKeywords = new EnvironmentKeywords(environmentInfo.environmentKeywords);
				environmentLightGroups = environmentInfo.environmentLightGroups;
			}
			else
			{
				environmentKeywords = new EnvironmentKeywords(new string[0]);
				environmentLightGroups = new EnvironmentLightGroups(new List<LightGroupSO>());
			}
			return BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.GetBeatmapDataFromSaveData(beatmapSaveData, defaultLightshowSaveData, beatmapDifficulty, startBpm, loadingForDesignatedEnvironment, environmentKeywords, environmentLightGroups, playerSpecificSettings);
*/
// clang-format on
MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v2,
                      &BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::GetBeatmapDataFromSaveDataJson,
                      GlobalNamespace::BeatmapData*, StringW beatmapJson, StringW defaultLightshowSaveDataJson,
                      GlobalNamespace::BeatmapDifficulty beatmapDifficulty, float_t startBpm,
                      bool loadingForDesignatedEnvironment, GlobalNamespace::IEnvironmentInfo* environmentInfo,
                      GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Loading Beatmap Data");

  auto saveData = ParseBeatmapSaveDataJson_v2(beatmapJson);
  if (!saveData) {
    return BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v2(
        beatmapJson, defaultLightshowSaveDataJson, beatmapDifficulty, startBpm, loadingForDesignatedEnvironment,
        environmentInfo, playerSpecificSettings);
  }
  auto saveDataPtr = saveData.value();

  LightshowSaveData* defaultLightshowSaveData =
      ((!String::IsNullOrEmpty(defaultLightshowSaveDataJson))
           ? UnityEngine::JsonUtility::FromJson<LightshowSaveData*>(defaultLightshowSaveDataJson)
           : nullptr);

  EnvironmentKeywords* environmentKeywords;
  IEnvironmentLightGroups* environmentLightGroups;

  if (environmentInfo != nullptr) {
    environmentKeywords = EnvironmentKeywords::New_ctor(environmentInfo->environmentKeywords);
    environmentLightGroups = environmentInfo->environmentLightGroups;
  } else {
    environmentKeywords =
        EnvironmentKeywords::New_ctor(ListW<StringW>({})->i___System__Collections__Generic__IReadOnlyList_1_T_());
    environmentLightGroups = EnvironmentLightGroups::New_ctor(ListW<UnityW<LightGroupSO>>::New().getPtr())
                                 ->i___GlobalNamespace__IEnvironmentLightGroups();
  }

  // std::string contents(beatmapJson);
  // auto version = GetVersionFromPath(contents);

  auto beatmapData = BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::GetBeatmapDataFromSaveData(
      saveDataPtr, defaultLightshowSaveData, beatmapDifficulty, startBpm, loadingForDesignatedEnvironment,
      environmentKeywords, environmentLightGroups, playerSpecificSettings);
  return beatmapData;
}

// clang-format off
/*
			List<EventData> list = new List<EventData>(this.events.Count);
			foreach (EventData eventData in this.events)
			{
				if (eventData.type == BeatmapEventType.Event10)
				{
					eventData = new EventData(eventData.time, BeatmapEventType.BpmChange, eventData.value, eventData.floatValue);
				}
				if (eventData.type == BeatmapEventType.BpmChange)
				{
					if (eventData.value != 0)
					{
						eventData = new EventData(eventData.time, eventData.type, eventData.value, (float)eventData.value);
					}
				}
				else
				{
					eventData = new EventData(eventData.time, eventData.type, eventData.value, 1f);
				}
				list.Add(eventData);
			}
			this._events = list;
*/
// clang-format on

MAKE_PAPER_HOOK_MATCH(BeatmapSaveData_ConvertBeatmapSaveDataPreV2_5_0Inline,
                      &BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ConvertBeatmapSaveDataPreV2_5_0Inline,
                      void, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData* self) {
  CJDLogger::Logger.info("Using CJD 2.5.0 fixup");

  std::vector<BeatmapSaveDataVersion2_6_0AndEarlier::EventData*> events;
  events.reserve(self->events->_size);

  for (auto const& originalEventData : VList<BeatmapSaveDataVersion2_6_0AndEarlier::EventData*>(self->events)) {
    auto newEventData = v2::CustomBeatmapSaveData_EventData::New_ctor(
        originalEventData->time, originalEventData->type, originalEventData->value, originalEventData->floatValue);

    auto const customEventData = il2cpp_utils::try_cast<v2::CustomBeatmapSaveData_EventData>(originalEventData);
    auto customData = customEventData.has_value() ? customEventData.value()->customData : std::nullopt;

    newEventData->customData = customData;

    if (newEventData->_type == BeatmapSaveDataCommon::BeatmapEventType::Event10) {
      newEventData->_type = BeatmapSaveDataCommon::BeatmapEventType::BpmChange;
    }

    if (newEventData->_type == BeatmapSaveDataCommon::BeatmapEventType::BpmChange) {
      if (newEventData->_value != 0) {
        newEventData->_floatValue = (float)newEventData->_value;
      }
    } else {
      newEventData->_floatValue = 1.0f;
    }

    events.emplace_back(newEventData);
  }
  auto list = ListW<BeatmapSaveDataVersion2_6_0AndEarlier::EventData*>::New(
      std::span<BeatmapSaveDataVersion2_6_0AndEarlier::EventData* const>(events));

  self->_events = list;
}

// clang-format off
/*
		beatmapSaveData.version != "2.6.0";
			if (!string.IsNullOrEmpty(beatmapSaveData.version))
			{
				Version version = new Version(beatmapSaveData.version);
				Version value = new Version("2.5.0");
				if (version.CompareTo(value) < 0)
				{
					BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.ConvertBeatmapSaveDataPreV2_5_0Inline(beatmapSaveData);
				}
			}
			else
			{
				BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.ConvertBeatmapSaveDataPreV2_5_0Inline(beatmapSaveData);
			}
			if (!BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.BeatmapSaveDataAreSorted(beatmapSaveData.notes))
			{
				beatmapSaveData.notes.Sort();
			}
			if (!BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.BeatmapSaveDataAreSorted(beatmapSaveData.obstacles))
			{
				beatmapSaveData.obstacles.Sort();
			}
			if (!BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.BeatmapSaveDataAreSorted(beatmapSaveData.sliders))
			{
				beatmapSaveData.sliders.Sort();
			}
			if (!BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.BeatmapSaveDataAreSorted(beatmapSaveData.waypoints))
			{
				beatmapSaveData.waypoints.Sort();
			}
			if (!BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.BeatmapSaveDataAreSorted(beatmapSaveData.events))
			{
				beatmapSaveData.events.Sort();
			}
			bool flag = loadingForDesignatedEnvironment || defaultLightshowSaveData == null;
			bool flag2 = playerSpecificSettings == null || playerSpecificSettings.GetEnvironmentEffectsFilterPreset(beatmapDifficulty) != EnvironmentEffectsFilterPreset.NoEffects;
			bool flag3 = flag && flag2;
			BeatmapData beatmapData = new BeatmapData(4);
			BpmTimeProcessor bpmTimeProcessor = new BpmTimeProcessor(startBpm, beatmapSaveData.events);
			BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.ColorNoteConverter colorNoteConverter = new BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.ColorNoteConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.ObstacleConverter obstacleConverter = new BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.ObstacleConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.WaypointConverter waypointConverter = new BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.WaypointConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.SliderConverter sliderConverter = new BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.SliderConverter(bpmTimeProcessor);
			foreach (ValueTuple<BeatmapSaveDataItem, int> valueTuple in new MultipleSortedBeatmapObjectsListsEnumerator<BeatmapSaveDataItem>(new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>[]
			{
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.notes, 0),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.obstacles, 1),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.waypoints, 2),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.sliders, 3)
			}))
			{
				BeatmapObjectData beatmapObjectData;
				switch (valueTuple.Item2)
				{
				case 0:
					beatmapObjectData = colorNoteConverter.Convert((BeatmapSaveDataVersion2_6_0AndEarlier.NoteData)valueTuple.Item1);
					break;
				case 1:
					beatmapObjectData = obstacleConverter.Convert((BeatmapSaveDataVersion2_6_0AndEarlier.ObstacleData)valueTuple.Item1);
					break;
				case 2:
					beatmapObjectData = waypointConverter.Convert((BeatmapSaveDataVersion2_6_0AndEarlier.WaypointData)valueTuple.Item1);
					break;
				case 3:
					beatmapObjectData = sliderConverter.Convert((BeatmapSaveDataVersion2_6_0AndEarlier.SliderData)valueTuple.Item1);
					break;
				default:
					beatmapObjectData = null;
					break;
				}
				BeatmapObjectData beatmapObjectData2 = beatmapObjectData;
				if (beatmapObjectData2 != null)
				{
					beatmapData.AddBeatmapObjectData(beatmapObjectData2);
				}
			}
			bpmTimeProcessor.Reset();
			BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.SpecialEventsFilter specialEventsFilter = new BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.SpecialEventsFilter(beatmapSaveData.specialEventsKeywordFilters, environmentKeywords);
			BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.BasicEventConverter basicEventConverter = new BeatmapDataLoaderVersion2_6_0AndEarlier.BeatmapDataLoader.BasicEventConverter(bpmTimeProcessor, specialEventsFilter, flag3);
			foreach (EventData e in beatmapSaveData.events)
			{
				BeatmapEventData beatmapEventData = basicEventConverter.Convert(e);
				if (beatmapEventData != null)
				{
					beatmapData.InsertBeatmapEventData(beatmapEventData);
				}
			}
			if (!flag3 && defaultLightshowSaveData != null)
			{
				BeatmapDataLoaderVersion4.BeatmapDataLoader.LoadLightshow(beatmapData, defaultLightshowSaveData, bpmTimeProcessor, environmentKeywords, environmentLightGroups);
			}
			else
			{
				DefaultEnvironmentEventsFactory.InsertDefaultEvents(beatmapData);
			}
			beatmapData.ProcessRemainingData();
			beatmapData.ProcessAndSortBeatmapData();
			return beatmapData;
*/
// clang-format on
MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataFromSaveData_v2,
                      &BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::GetBeatmapDataFromSaveData,
                      GlobalNamespace::BeatmapData*,
                      ::BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData* beatmapSaveData,
                      ::BeatmapSaveDataVersion4::LightshowSaveData* defaultLightshowSaveData,
                      ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, float_t startBpm,
                      bool loadingForDesignatedEnvironment, ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                      ::GlobalNamespace::IEnvironmentLightGroups* environmentLightGroups,
                      ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings) {

  CJDLogger::Logger.info("Converting v2 save data to beatmap data");
  CJDLogger::Logger.Backtrace(100);

  if (!beatmapSaveData) {
    return BeatmapDataLoader_GetBeatmapDataFromSaveData_v2(
        beatmapSaveData, defaultLightshowSaveData, beatmapDifficulty, startBpm, loadingForDesignatedEnvironment,
        environmentKeywords, environmentLightGroups, playerSpecificSettings);
  }

  Paper::Profiler profile;
  profile.startTimer();

  // i hate this
  auto fancyCast = [](auto&& list) {
    return reinterpret_cast<::System::Collections::Generic::IReadOnlyList_1<
        ::BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveDataItem*>*>(list);
  };
  auto fancyCast2 = [](auto&& list) {
    return reinterpret_cast<
        ::System::Collections::Generic::IReadOnlyList_1<::BeatmapSaveDataVersion2_6_0AndEarlier::EventData*>*>(
        list->i___System__Collections__Generic__IReadOnlyList_1_T_());
  };

  if (String::IsNullOrEmpty(beatmapSaveData->version) || semver::lt(beatmapSaveData->version, "2.5.0")) {
    CJDLogger::Logger.info("Fixing pre 2.5.0 map: {}", beatmapSaveData->version);
    BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::ConvertBeatmapSaveDataPreV2_5_0Inline(beatmapSaveData);
  }

  bool flag = loadingForDesignatedEnvironment || defaultLightshowSaveData == nullptr;
  bool flag2 = playerSpecificSettings == nullptr || playerSpecificSettings->GetEnvironmentEffectsFilterPreset(
                                                        beatmapDifficulty) != EnvironmentEffectsFilterPreset::NoEffects;
  CJDLogger::Logger.info("flag1 {} || {} == nullptr", loadingForDesignatedEnvironment,
                         fmt::ptr(defaultLightshowSaveData));

  CJDLogger::Logger.info(
      "flag2 {} == nullptr || {} != EnvironmentEffectsFilterPreset::NoEffects", fmt::ptr(playerSpecificSettings),
      playerSpecificSettings && playerSpecificSettings->GetEnvironmentEffectsFilterPreset(beatmapDifficulty).value__);

  bool flag3 = flag && flag2;
  auto beatmapData = CustomBeatmapData::New_ctor(4);

  if (auto cBeatmapSaveData = il2cpp_utils::try_cast<v2::CustomBeatmapSaveData>(beatmapSaveData)) {
    beatmapData->customData = CustomJSONData::JSONWrapperOrNull(cBeatmapSaveData.value()->customData);
    beatmapData->levelCustomData = CustomJSONData::JSONWrapperOrNull(cBeatmapSaveData.value()->levelCustomData);
    beatmapData->beatmapCustomData = CustomJSONData::JSONWrapperOrNull(cBeatmapSaveData.value()->beatmapCustomData);
    beatmapData->v2orEarlier = true;
  }

  // auto bpmTimeProcessor = GlobalNamespace::BpmTimeProcessor::New_ctor(startBpm,
  // fancyCast2(beatmapSaveData->events));

  ListW<::BeatmapSaveDataVersion2_6_0AndEarlier::EventData*> bpmEvents = beatmapSaveData->events;
  sortInPlace(std::span(bpmEvents));

  CustomJSONData::BpmTimeProcessor bpmTimeProcessor(startBpm, bpmEvents);
  CJDLogger::Logger.info("BPM Events {}", bpmTimeProcessor.bpmChangeDataList.size());
  CJDLogger::Logger.info("Events total {}", beatmapSaveData->events->_size);

  auto bpmTimeProcessor2 = GlobalNamespace::BpmTimeProcessor::New_ctor(startBpm, fancyCast2(bpmEvents));

  auto const BeatToTime = [&bpmTimeProcessor, &bpmTimeProcessor2](float beat) constexpr {
    auto time = bpmTimeProcessor.ConvertBeatToTime(beat);
    return time;
  };

  auto const ResetBPM = [&bpmTimeProcessor, bpmTimeProcessor2]() constexpr {
    bpmTimeProcessor.Reset();
    bpmTimeProcessor2->Reset();
  };

  ResetBPM();
  {
    CppConverter<GlobalNamespace::BeatmapObjectData*, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveDataItem*>
        objectConverter;
    objectConverter.AddConverter<v2::CustomBeatmapSaveData_NoteData*>(
        [&BeatToTime](v2::CustomBeatmapSaveData_NoteData* n) -> GlobalNamespace::BeatmapObjectData* {
          switch (n->type) {
          case BeatmapSaveDataVersion2_6_0AndEarlier::NoteType::NoteA:
          case BeatmapSaveDataVersion2_6_0AndEarlier::NoteType::NoteB:
            return CreateCustomBasicNoteData(BeatToTime(n->time), n->lineIndex, ConvertNoteLineLayer(n->lineLayer),
                                             (n->type == BeatmapSaveDataVersion2_6_0AndEarlier::NoteType::NoteA)
                                                 ? ColorType::ColorA
                                                 : ColorType::ColorB,
                                             ConvertNoteCutDirection(n->cutDirection), n->customData);
          case BeatmapSaveDataVersion2_6_0AndEarlier::NoteType::Bomb:
            return CreateCustomBombNoteData(BeatToTime(n->time), n->lineIndex, ConvertNoteLineLayer(n->lineLayer),
                                            n->customData);
          default:
            return nullptr;
          }

          return nullptr;
        });

    objectConverter.AddConverter<v2::CustomBeatmapSaveData_ObstacleData*>(
        [&BeatToTime](v2::CustomBeatmapSaveData_ObstacleData* o) constexpr {
          float num = BeatToTime(o->time);
          float num2 = BeatToTime(o->time + o->duration);
          auto* obstacle =
              CustomObstacleData::New_ctor(num, o->lineIndex, ConvertNoteLineLayer(GetLayerForObstacleType(o->type)),
                                           num2 - num, o->width, GetHeightForObstacleType(o->type));

          obstacle->customData = CustomJSONData::JSONWrapperOrNull(o->customData);

          return obstacle;
        });

    objectConverter.AddConverter<v2::CustomBeatmapSaveData_SliderData*>(
        [&BeatToTime](v2::CustomBeatmapSaveData_SliderData* data) {
          return CustomSliderData_CreateCustomSliderData(
              ConvertColorType(data->colorType), BeatToTime(data->time), data->headLineIndex,
              ConvertNoteLineLayer(data->headLineLayer), ConvertNoteLineLayer(data->headLineLayer),
              data->headControlPointLengthMultiplier, ConvertNoteCutDirection(data->headCutDirection),
              BeatToTime(data->tailTime), data->tailLineIndex, ConvertNoteLineLayer(data->tailLineLayer),
              ConvertNoteLineLayer(data->tailLineLayer), data->tailControlPointLengthMultiplier,
              ConvertNoteCutDirection(data->tailCutDirection), ConvertSliderMidAnchorMode(data->sliderMidAnchorMode),
              data->customData);
        });

    objectConverter.AddConverter<BeatmapSaveDataVersion2_6_0AndEarlier::WaypointData*>(
        [&BeatToTime](BeatmapSaveDataVersion2_6_0AndEarlier::WaypointData* data) constexpr {
          return CustomJSONData::NewFast<GlobalNamespace::WaypointData*>(BeatToTime(data->time), data->lineIndex,
                                                                         ConvertNoteLineLayer(data->lineLayer),
                                                                         ConvertOffsetDirection(data->offsetDirection));
        });

    std::vector<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveDataItem*> beatmapDataObjectItems;
    beatmapDataObjectItems.reserve(beatmapSaveData->notes->_size + beatmapSaveData->obstacles->_size +
                                   +beatmapSaveData->waypoints->_size + beatmapSaveData->sliders->_size);

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Color notes {} {}", fmt::ptr(beatmapSaveData->notes),
                                            beatmapSaveData->notes->_size);
    addAllToVector(beatmapDataObjectItems, beatmapSaveData->notes);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Obstacles {}", fmt::ptr(beatmapSaveData->obstacles));
    addAllToVector(beatmapDataObjectItems, beatmapSaveData->obstacles);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Sliders {}", fmt::ptr(beatmapSaveData->sliders));
    addAllToVector(beatmapDataObjectItems, beatmapSaveData->sliders);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Waypoints {}", fmt::ptr(beatmapSaveData->waypoints));
    addAllToVector(beatmapDataObjectItems, beatmapSaveData->waypoints);

    cleanAndSort(beatmapDataObjectItems);
    for (auto const& o : beatmapDataObjectItems) {
      auto* beatmapObjectData = objectConverter.ProcessItem(o);

      if (beatmapObjectData != nullptr) {
        beatmapData->AddBeatmapObjectDataOverride(beatmapObjectData);
      }
    }
  }

  ResetBPM();
  {
    auto specialEventsFilter =
        BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::SpecialEventsFilter::New_ctor(
            beatmapSaveData->specialEventsKeywordFilters, environmentKeywords);

    bool canUseEnvironmentEventsAndShouldLoadDynamicEvents = flag3;

    auto basicEventConverter =
        [&BeatToTime, &specialEventsFilter, &canUseEnvironmentEventsAndShouldLoadDynamicEvents](
            v2::CustomBeatmapSaveData_EventData* e) constexpr -> GlobalNamespace::BeatmapEventData* {
      if (!specialEventsFilter->IsEventValid(e->type)) {
        return nullptr;
      }
      if (e->type == BeatmapSaveDataCommon::BeatmapEventType::BpmChange) {
        return nullptr;
      }
      if (e->type == BeatmapSaveDataCommon::BeatmapEventType::Event14) {
        return SpawnRotationBeatmapEventData::New_ctor(BeatToTime(e->time),
                                                       SpawnRotationBeatmapEventData::SpawnRotationEventType::Early,
                                                       SpawnRotationForEventValue(e->value));
      }
      if (e->type == BeatmapSaveDataCommon::BeatmapEventType::Event15) {
        return SpawnRotationBeatmapEventData::New_ctor(BeatToTime(e->time),
                                                       SpawnRotationBeatmapEventData::SpawnRotationEventType::Late,
                                                       SpawnRotationForEventValue(e->value));
      }
      if (e->type == BeatmapSaveDataCommon::BeatmapEventType::Event5 &&
          canUseEnvironmentEventsAndShouldLoadDynamicEvents) {
        return ColorBoostBeatmapEventData::New_ctor(BeatToTime(e->time), e->value == 1);
      }
      if (!canUseEnvironmentEventsAndShouldLoadDynamicEvents) {
        return nullptr;
      }
      auto event = CustomBeatmapEventData::New_ctor(BeatToTime(e->time), ConvertBasicBeatmapEventType(e->type),
                                                    e->value, e->floatValue);

      event->customData = CustomJSONData::JSONWrapperOrNull(e->customData);

      return event;
    };

    VList<GlobalNamespace::BeatmapEventData*> events = beatmapSaveData->events;
    std::stable_sort(events.begin(), events.end(), TimeCompare<GlobalNamespace::BeatmapEventData const* const>);

    for (auto const& e : events) {
      auto beatmapEventData = basicEventConverter(il2cpp_utils::cast<v2::CustomBeatmapSaveData_EventData>(e));
      if (beatmapEventData != nullptr) {
        beatmapData->InsertBeatmapEventDataOverride(beatmapEventData);
      }
    }
  }

  ResetBPM();
  if (auto customBeatmapSaveData = il2cpp_utils::try_cast<v2::CustomBeatmapSaveData>(beatmapSaveData)) {
    if (customBeatmapSaveData.value()->customEventsData) {
      std::stable_sort(customBeatmapSaveData.value()->customEventsData->begin(),
                       customBeatmapSaveData.value()->customEventsData->end(),
                       [](auto const& a, auto const& b) constexpr { return a.time < b.time; });

      for (auto const& customEventSaveData : *customBeatmapSaveData.value()->customEventsData) {
        beatmapData->InsertCustomEventData(
            CustomEventData::New(bpmTimeProcessor.ConvertBeatToTime(customEventSaveData.time), customEventSaveData.type,
                                 customEventSaveData.typeHash, customEventSaveData.data));
      }

      CJDLogger::Logger.fmtLog<LogLevel::INF>("Added {} custom events",
                                              customBeatmapSaveData.value()->customEventsData->size());
    }
  }

  ResetBPM();
  {
    CJDLogger::Logger.info("Lightshow time {} || {} != nullptr", flag3, fmt::ptr(defaultLightshowSaveData));
    if (!flag3 && defaultLightshowSaveData != nullptr) {
      BeatmapDataLoaderVersion4::BeatmapDataLoader::LoadLightshow(
          beatmapData, defaultLightshowSaveData, bpmTimeProcessor2, environmentKeywords, environmentLightGroups);
    } else {
      CJDLogger::Logger.info("Inserting default environment events flag1 {} flag2 {} flag3 {}", flag, flag2, flag3);

      DefaultEnvironmentEventsFactory::InsertDefaultEvents(beatmapData);
    }
  }
  beatmapData->ProcessRemainingData();
  beatmapData->ProcessAndSortBeatmapData();

  profile.endTimer();

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished processing beatmap data");
  auto stopTime = std::chrono::high_resolution_clock::now();
  CJDLogger::Logger.fmtLog<LogLevel::DBG>(
      "This took {}ms",
      static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(profile.elapsedTime()).count()));

  return beatmapData;
}

void CustomJSONData::v2::InstallHooks() {
  // Install hooks
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v2)
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataLoader_GetBeatmapDataFromSaveData_v2)
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapSaveData_ConvertBeatmapSaveDataPreV2_5_0Inline)
}
