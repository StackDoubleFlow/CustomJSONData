#include "CustomJSONDataHooks.h"
#include "HookUtils.hpp"

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
#include "GlobalNamespace/LightColorBeatmapEventData.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/IEnvironmentLightGroups.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"

#include "UnityEngine/JsonUtility.hpp"

#include "System/String.hpp"

#include "beatsaber-hook/shared/utils/typedefs-list.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"

#include "paper/shared/Profiler.hpp"

#include "custom-types/shared/register.hpp"

#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "misc/BeatmapFieldUtils.hpp"
#include "misc/BeatmapDataLoaderUtils.hpp"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;
using namespace BeatmapSaveDataVersion4;

// clang-format on
MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v3,
                      &BeatmapDataLoaderVersion3::BeatmapDataLoader::GetBeatmapDataFromSaveDataJson,
                      GlobalNamespace::BeatmapData*, ::StringW beatmapJson, ::StringW defaultLightshowJson,
                      ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, float_t startBpm,
                      bool loadingForDesignatedEnvironment, ::GlobalNamespace::IEnvironmentInfo* environmentInfo,
                      ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Loading Beatmap Data");

  auto sharedDoc = parseDocument(std::string(beatmapJson));

  if (!sharedDoc) {
    return BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v3(beatmapJson, defaultLightshowJson, beatmapDifficulty,
                                                               startBpm, loadingForDesignatedEnvironment,
                                                               environmentInfo, playerSpecificSettings);
  }

  auto saveData = v3::CustomBeatmapSaveData::Deserialize(*sharedDoc);
  if (!saveData) {
    return BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v3(beatmapJson, defaultLightshowJson, beatmapDifficulty,
                                                               startBpm, loadingForDesignatedEnvironment,
                                                               environmentInfo, playerSpecificSettings);
  }

  LightshowSaveData* defaultLightshowSaveData =
      ((!String::IsNullOrEmpty(defaultLightshowJson))
           ? UnityEngine::JsonUtility::FromJson<LightshowSaveData*>(defaultLightshowJson)
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

  std::string contents(beatmapJson);
  auto version = GetVersionFromPath(contents);

  auto beatmapData = BeatmapDataLoaderVersion3::BeatmapDataLoader::GetBeatmapDataFromSaveData(
      saveData, defaultLightshowSaveData, beatmapDifficulty, startBpm, loadingForDesignatedEnvironment,
      environmentKeywords, environmentLightGroups, playerSpecificSettings, nullptr);
  return beatmapData;
}

// clang-format off
/*
		bool flag = loadingForDesignatedEnvironment || (beatmapSaveData.useNormalEventsAsCompatibleEvents && defaultLightshowSaveData == null);
			bool flag2 = playerSpecificSettings == null || playerSpecificSettings.GetEnvironmentEffectsFilterPreset(beatmapDifficulty) != EnvironmentEffectsFilterPreset.NoEffects;
			bool flag3 = flag && flag2;
			BeatmapData beatmapData = new BeatmapData(4);
			List<BpmChangeEventData> bpmEvents = beatmapSaveData.bpmEvents;
			BpmTimeProcessor bpmTimeProcessor = new BpmTimeProcessor(startBpm, bpmEvents);
			BeatmapDataLoaderVersion3.BeatmapDataLoader.ColorNoteConverter colorNoteConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.ColorNoteConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion3.BeatmapDataLoader.BombNoteConverter bombNoteConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.BombNoteConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion3.BeatmapDataLoader.ObstacleConverter obstacleConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.ObstacleConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion3.BeatmapDataLoader.SliderConverter sliderConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.SliderConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion3.BeatmapDataLoader.BurstSliderConverter burstSliderConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.BurstSliderConverter(bpmTimeProcessor);
			BeatmapDataLoaderVersion3.BeatmapDataLoader.WaypointConverter waypointConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.WaypointConverter(bpmTimeProcessor);
			foreach (ValueTuple<BeatmapSaveDataItem, int> valueTuple in new MultipleSortedBeatmapObjectsListsEnumerator<BeatmapSaveDataItem>(new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>[]
			{
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.colorNotes, 0),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.bombNotes, 1),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.obstacles, 2),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.sliders, 3),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.burstSliders, 4),
				new ValueTuple<IReadOnlyList<BeatmapSaveDataItem>, int>(beatmapSaveData.waypoints, 5)
			}))
			{
				BeatmapObjectData beatmapObjectData;
				switch (valueTuple.Item2)
				{
				case 0:
					beatmapObjectData = colorNoteConverter.Convert((ColorNoteData)valueTuple.Item1);
					break;
				case 1:
					beatmapObjectData = bombNoteConverter.Convert((BombNoteData)valueTuple.Item1);
					break;
				case 2:
					beatmapObjectData = obstacleConverter.Convert((BeatmapSaveDataVersion3.ObstacleData)valueTuple.Item1);
					break;
				case 3:
					beatmapObjectData = sliderConverter.Convert((BeatmapSaveDataVersion3.SliderData)valueTuple.Item1);
					break;
				case 4:
					beatmapObjectData = burstSliderConverter.Convert((BurstSliderData)valueTuple.Item1);
					break;
				case 5:
					beatmapObjectData = waypointConverter.Convert((BeatmapSaveDataVersion3.WaypointData)valueTuple.Item1);
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
			if (stopwatch != null)
			{
				stopwatch.Start();
			}
			bpmTimeProcessor.Reset();
			if (bpmEvents.Count == 0)
			{
				beatmapData.InsertBeatmapEventData(new BPMChangeBeatmapEventData(0f, startBpm));
			}
			else
			{
				BeatmapDataLoaderVersion3.BeatmapDataLoader.BpmEventConverter bpmEventConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.BpmEventConverter(bpmTimeProcessor);
				foreach (BpmChangeEventData bpmChangeEventSaveData in bpmEvents)
				{
					BPMChangeBeatmapEventData bpmchangeBeatmapEventData = bpmEventConverter.Convert(bpmChangeEventSaveData);
					if (bpmchangeBeatmapEventData != null)
					{
						beatmapData.InsertBeatmapEventData(bpmchangeBeatmapEventData);
					}
				}
			}
			BeatmapDataLoaderVersion3.BeatmapDataLoader.RotationEventConverter rotationEventConverter = new BeatmapDataLoaderVersion3.BeatmapDataLoader.RotationEventConverter(bpmTimeProcessor);
			foreach (RotationEventData rotationEventSaveData in beatmapSaveData.rotationEvents)
			{
				SpawnRotationBeatmapEventData spawnRotationBeatmapEventData = rotationEventConverter.Convert(rotationEventSaveData);
				if (spawnRotationBeatmapEventData != null)
				{
					beatmapData.InsertBeatmapEventData(spawnRotationBeatmapEventData);
				}
			}
			if (flag3)
			{
				BeatmapDataLoaderVersion3.BeatmapDataLoader.ConvertBasicEvents(beatmapData, beatmapSaveData, bpmTimeProcessor, environmentKeywords);
				BeatmapDataLoaderVersion3.BeatmapDataLoader.ConvertEventBoxGroups(beatmapData, beatmapSaveData, bpmTimeProcessor, environmentLightGroups);
			}
			else if (defaultLightshowSaveData != null)
			{
				BeatmapDataLoaderVersion4.BeatmapDataLoader.LoadLightshow(beatmapData, defaultLightshowSaveData, bpmTimeProcessor, environmentKeywords, environmentLightGroups);
			}
			else
			{
				DefaultEnvironmentEventsFactory.InsertDefaultEvents(beatmapData);
			}
			beatmapData.ProcessRemainingData();
			beatmapData.ProcessAndSortBeatmapData();
			if (stopwatch != null)
			{
				stopwatch.Stop();
			}
			return beatmapData;
*/
// clang-format on
MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataFromSaveData_v3,
                      &BeatmapDataLoaderVersion3::BeatmapDataLoader::GetBeatmapDataFromSaveData,
                      GlobalNamespace::BeatmapData*, ::BeatmapSaveDataVersion3::BeatmapSaveData* beatmapSaveData,
                      ::BeatmapSaveDataVersion4::LightshowSaveData* defaultLightshowSaveData,
                      ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, float_t startBpm,
                      bool loadingForDesignatedEnvironment, ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                      ::GlobalNamespace::IEnvironmentLightGroups* environmentLightGroups,
                      ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                      ::System::Diagnostics::Stopwatch* stopwatch) {
  CJDLogger::Logger.Backtrace(100);

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing save data {}", fmt::ptr(beatmapSaveData));
  auto startTime = std::chrono::high_resolution_clock::now();
  Paper::Profiler profile;
  profile.startTimer();

  bool flag = loadingForDesignatedEnvironment ||
              (beatmapSaveData->useNormalEventsAsCompatibleEvents && defaultLightshowSaveData == nullptr);
  bool flag2 = playerSpecificSettings == nullptr || playerSpecificSettings->GetEnvironmentEffectsFilterPreset(
                                                        beatmapDifficulty) != EnvironmentEffectsFilterPreset::NoEffects;
  bool flag3 = flag && flag2;

  CustomBeatmapData* beatmapData = CustomBeatmapData::New_ctor(4);

  if (auto cBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData)) {
    beatmapData->customData = CustomJSONData::JSONWrapperOrNull(cBeatmapSaveData.value()->customData);
    beatmapData->levelCustomData = CustomJSONData::JSONWrapperOrNull(cBeatmapSaveData.value()->levelCustomData);
    beatmapData->beatmapCustomData = CustomJSONData::JSONWrapperOrNull(cBeatmapSaveData.value()->beatmapCustomData);
    beatmapData->v2orEarlier = cBeatmapSaveData.value()->isV2;
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Special events list {} {}",
                                          fmt::ptr(beatmapSaveData->basicEventTypesWithKeywords->d),
                                          beatmapSaveData->basicEventTypesWithKeywords->d->_size);

  profile.mark("Converted special events");

  ListW<BeatmapSaveDataVersion3::BpmChangeEventData*> bpmEvents = beatmapSaveData->bpmEvents;
  CustomJSONData::BpmTimeProcessor bpmTimeProcessor(startBpm, bpmEvents);

  auto* bpmTimeProcessorIl2cpp = CustomJSONData::NewFast<GlobalNamespace::BpmTimeProcessor*>(
      startBpm, bpmEvents->i___System__Collections__Generic__IReadOnlyList_1_T_());

  auto const BeatToTime = [&bpmTimeProcessor](float beat) constexpr {
    auto time = bpmTimeProcessor.ConvertBeatToTime(beat);
    return time;
  };

  profile.mark("Parsed bpm events");

  // objects
  bpmTimeProcessor.Reset();
  bpmTimeProcessorIl2cpp->Reset();
  {
    CppConverter<GlobalNamespace::BeatmapObjectData*, BeatmapSaveDataVersion3::BeatmapSaveDataItem*> objectConverter;
    objectConverter.AddConverter<v3::CustomBeatmapSaveData_ColorNoteData*>(
        [&BeatToTime](v3::CustomBeatmapSaveData_ColorNoteData* data) {
          auto* noteData = CreateCustomBasicNoteData(
              BeatToTime(data->b), data->get_line(), ConvertNoteLineLayer(data->layer),
              ConvertColorType(data->get_color()), ConvertNoteCutDirection(data->get_cutDirection()), data->customData);

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
          auto* obstacle =
              CustomObstacleData::New_ctor(beat, data->line, GetNoteLineLayer(data->layer),
                                           BeatToTime(data->b + data->duration) - beat, data->width, data->height);

          obstacle->customData = CustomJSONData::JSONWrapperOrNull(data->customData);

          return obstacle;
        });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_SliderData*>(
        [&BeatToTime](v3::CustomBeatmapSaveData_SliderData* data) {
          return CustomSliderData_CreateCustomSliderData(
              ConvertColorType(data->get_colorType()), BeatToTime(data->b), data->get_headLine(),
              ConvertNoteLineLayer(data->get_headLayer()), ConvertNoteLineLayer(data->get_headLayer()),
              data->get_headControlPointLengthMultiplier(), ConvertNoteCutDirection(data->get_headCutDirection()),
              BeatToTime(data->get_tailBeat()), data->get_tailLine(), ConvertNoteLineLayer(data->get_tailLayer()),
              ConvertNoteLineLayer(data->get_tailLayer()), data->get_tailControlPointLengthMultiplier(),
              ConvertNoteCutDirection(data->get_tailCutDirection()),
              ConvertSliderMidAnchorMode(data->get_sliderMidAnchorMode()), data->customData);
        });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_BurstSliderData*>(
        [&BeatToTime](v3::CustomBeatmapSaveData_BurstSliderData* data) {
          return CustomSliderData_CreateCustomBurstSliderData(
              ConvertColorType(data->get_colorType()), BeatToTime(data->b), data->get_headLine(),
              ConvertNoteLineLayer(data->get_headLayer()), ConvertNoteLineLayer(data->get_tailLayer()),
              ConvertNoteCutDirection(data->get_headCutDirection()), BeatToTime(data->get_tailBeat()),
              data->get_tailLine(), ConvertNoteLineLayer(data->get_tailLayer()),
              ConvertNoteLineLayer(data->get_tailLayer()), NoteCutDirection::Any, data->get_sliceCount(),
              data->get_squishAmount(), data->customData);
        });

    objectConverter.AddConverter<BeatmapSaveDataVersion3::WaypointData*>(
        [&BeatToTime](BeatmapSaveDataVersion3::WaypointData* data) constexpr {
          return CustomJSONData::NewFast<GlobalNamespace::WaypointData*>(
              BeatToTime(data->b), data->get_line(), ConvertNoteLineLayer(data->get_layer()),
              GlobalNamespace::OffsetDirection(data->get_offsetDirection().value__));
        });

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Creating object list");

    std::vector<BeatmapSaveDataVersion3::BeatmapSaveDataItem*> beatmapDataObjectItems;
    beatmapDataObjectItems.reserve(beatmapSaveData->colorNotes->get_Count() + beatmapSaveData->bombNotes->get_Count() +
                                   beatmapSaveData->obstacles->get_Count() + beatmapSaveData->sliders->get_Count() +
                                   beatmapSaveData->burstSliders->get_Count() +
                                   beatmapSaveData->waypoints->get_Count());

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
  }

  {
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Doing event items");
    std::vector<BeatmapSaveDataVersion3::BeatmapSaveDataItem*> beatmapDataEventItems;
    beatmapDataEventItems.reserve(bpmEvents->get_Count() + beatmapSaveData->basicBeatmapEvents->get_Count() +
                                  beatmapSaveData->colorBoostBeatmapEvents->get_Count() +
                                  beatmapSaveData->rotationEvents->get_Count());

    // bpm
    if (bpmEvents.size() == 0) {
      beatmapData->InsertBeatmapEventData(BPMChangeBeatmapEventData::New_ctor(0.0f, startBpm));
    } else {
      CJDLogger::Logger.fmtLog<LogLevel::DBG>("bpm events");
      addAllToVector(beatmapDataEventItems, bpmEvents.getPtr());
    }

    // rotation
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("rotation events");
    addAllToVector(beatmapDataEventItems, beatmapSaveData->rotationEvents);

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
                                                                         data->rotation);
        });

    if (flag3) {
      // add basic event types
      for (auto const& basicEventTypesForKeyword :
           VList<BeatmapSaveDataCommon::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>(
               beatmapSaveData->basicEventTypesWithKeywords->data)) {
        beatmapData->AddSpecialBasicBeatmapEventKeyword(basicEventTypesForKeyword->keyword);
      }

      auto specialEventsFilter = SafePtr(BeatmapDataLoaderVersion3::BeatmapDataLoader::SpecialEventsFilter::New_ctor(
          beatmapSaveData->basicEventTypesWithKeywords, environmentKeywords));

      eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>(
          [&BeatToTime, &specialEventsFilter](v3::CustomBeatmapSaveData_BasicEventData* data) constexpr {
            if (!specialEventsFilter->IsEventValid(data->eventType)) {
              return static_cast<CustomBeatmapEventData*>(nullptr);
            }

            auto* event = CustomBeatmapEventData::New_ctor(BeatToTime(data->b), data->eventType.value__, data->value,
                                                           data->floatValue);

            event->customData = CustomJSONData::JSONWrapperOrNull(data->customData);

            return event;
          });

      eventConverter.AddConverter<BeatmapSaveDataVersion3::ColorBoostEventData*>(
          [&BeatToTime](BeatmapSaveDataVersion3::ColorBoostEventData* data) constexpr {
            return CustomJSONData::NewFast<ColorBoostBeatmapEventData*>(BeatToTime(data->beat), data->boost);
          });

      CJDLogger::Logger.fmtLog<LogLevel::DBG>("basic events");
      addAllToVector(beatmapDataEventItems, beatmapSaveData->basicBeatmapEvents);
      CJDLogger::Logger.fmtLog<LogLevel::DBG>("color boost events");
      addAllToVector(beatmapDataEventItems, beatmapSaveData->colorBoostBeatmapEvents);

      BeatmapDataLoaderVersion3::BeatmapDataLoader::ConvertEventBoxGroups(
          beatmapData, beatmapSaveData, bpmTimeProcessorIl2cpp, environmentLightGroups);
    } else if (defaultLightshowSaveData != nullptr) {
      // this will generate non custom events
      // we need to be safe about this some day
      // not today though
      BeatmapDataLoaderVersion4::BeatmapDataLoader::LoadLightshow(
          beatmapData, defaultLightshowSaveData, bpmTimeProcessorIl2cpp, environmentKeywords, environmentLightGroups);
    } else {
      DefaultEnvironmentEventsFactory::InsertDefaultEvents(beatmapData);
    }

    profile.mark("Grouped beatmap events");

    cleanAndSort(beatmapDataEventItems);
    bpmTimeProcessor.Reset();
    bpmTimeProcessorIl2cpp->Reset();
    for (auto const& o : beatmapDataEventItems) {
      auto* beatmapEventData = eventConverter.ProcessItem(o);

      if (beatmapEventData != nullptr) {
        beatmapData->InsertBeatmapEventDataOverride(beatmapEventData);
      }
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::INF>("Beatmap events {}", beatmapData->beatmapEventDatas.size());

  profile.mark("Processed and added beatmap events");

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("event groups");

  // Convert event boxes (remove)
  {
    if (flag3) {
      // bpmTimeProcessor.Reset();
      // ListW<BeatmapSaveDataVersion3::LightColorEventBoxGroup*> lightColorEventBoxGroups =
      //     beatmapSaveData->lightColorEventBoxGroups;
      // ListW<BeatmapSaveDataVersion3::LightRotationEventBoxGroup*> lightRotationEventBoxGroups =
      //     beatmapSaveData->lightRotationEventBoxGroups;
      // ListW<BeatmapSaveDataVersion3::LightTranslationEventBoxGroup*> lightTranslationEventBoxGroups =
      //     beatmapSaveData->lightTranslationEventBoxGroups;
      // BeatmapSaveDataVersion3::FxEventsCollection* fxEventsCollection = beatmapSaveData->_fxEventsCollection;
      // ListW<BeatmapSaveDataVersion3::FxEventBoxGroup*> vfxEventBoxGroups = beatmapSaveData->vfxEventBoxGroups;

      // EventBoxGroupConvertor cppEventBoxConverter(environmentLightGroups);

      // std::vector<BeatmapSaveDataVersion3::EventBoxGroup*> eventBoxes;
      // eventBoxes.reserve(beatmapSaveData->lightColorEventBoxGroups->get_Count() +
      //                    beatmapSaveData->lightRotationEventBoxGroups->get_Count() +
      //                    beatmapSaveData->lightTranslationEventBoxGroups->get_Count());

      // CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightColorEventBoxGroups handling events");
      // addAllToVector(eventBoxes, beatmapSaveData->lightColorEventBoxGroups);
      // CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightRotationEventBoxGroups handling events");
      // addAllToVector(eventBoxes, beatmapSaveData->lightRotationEventBoxGroups);
      // CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightTranslationEventBoxGroups handling events");
      // addAllToVector(eventBoxes, beatmapSaveData->lightTranslationEventBoxGroups);

      // cleanAndSort(eventBoxes);

      // CJDLogger::Logger.fmtLog<LogLevel::DBG>("Sorted events");

      // profile.mark(fmt::format("Grouped beatmap event boxes {}", eventBoxes.size()));

      // for (auto const& o : eventBoxes) {
      //   auto* beatmapEventDataBoxGroup = cppEventBoxConverter.Convert(o); // eventBoxGroupConvertor->Convert(o);
      //   if (beatmapEventDataBoxGroup != nullptr) {
      //     beatmapEventDataBoxGroupLists->Insert(o->g, beatmapEventDataBoxGroup);
      //   }
      // }

      CJDLogger::Logger.fmtLog<LogLevel::DBG>("VFX box group");
      profile.mark("Processed and added beatmap events boxes");
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Custom events");
  bpmTimeProcessor.Reset();
  bpmTimeProcessorIl2cpp->Reset();
  if (auto customBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData)) {
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
      profile.mark("Processed beatmap custom events");
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished! processing");
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

void CustomJSONData::v3::InstallHooks() {
  // Install hooks
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v3)
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataLoader_GetBeatmapDataFromSaveData_v3)
}
