#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "songloader/shared/API.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"

#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/MultipleSortedListsEnumerator_1.hpp"
#include "GlobalNamespace/BPMChangeBeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapDataLoader_SpecialEventsFilter.hpp"
#include "GlobalNamespace/BeatmapDataLoader_BpmTimeProcessor.hpp"
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
#include "GlobalNamespace/IIndexFilter.hpp"
#include "GlobalNamespace/RangeIndexFilter.hpp"
#include "GlobalNamespace/EnvironmentLightGroups_LightGroupData.hpp"
#include "GlobalNamespace/LightColorBaseData.hpp"
#include "GlobalNamespace/BeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightColorBeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightRotationBeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightRotationBaseData.hpp"
#include "GlobalNamespace/BasicBeatmapObjectManager.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"

#include "System/Comparison_1.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "System/Linq/Enumerable.hpp"
#include "System/Version.hpp"

#include "GlobalNamespace/GameSongController.hpp"
#include "UnityEngine/Resources.hpp"

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

#include "GlobalNamespace/BeatmapEventDataBoxGroupLists.hpp"
#include "GlobalNamespace/BeatmapDataLoader_EventBoxGroupConvertor.hpp"
#include "GlobalNamespace/SpawnRotationBeatmapEventData.hpp"
#include "GlobalNamespace/DefaultEnvironmentEventsFactory.hpp"

#include <chrono>
#include <codecvt>
#include <locale>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;

v3::CustomBeatmapSaveData* cachedSaveData;


// This is to prevent issues with string limits
std::string to_utf8(std::u16string_view view) {
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(view.data());
}


static std::string GetVersionFromPath(std::string_view path)
{
    // SongCore has a fallback so i guess i do too
    static const std::string_view fallback = "2.0.0";

    auto truncatedText = path.substr(0, 50);
    static const std::regex versionRegex (R"("_?version"\s*:\s*"[0-9]+\.[0-9]+\.?[0-9]?")");
    std::match_results<std::string_view::const_iterator> matches;
    if(std::regex_search(truncatedText.begin(), truncatedText.end(), matches, versionRegex)) {
        if (!matches.empty()) {
            auto version = matches[0].str();
            version = version.substr(0, version.length() - 1);
            version = version.substr(version.find_last_of('\"') + 1, version.length());

            return version;
        }
    }

    return std::string(fallback);
}


BeatmapData * CustomBeatmapData_GetFilteredCopy(CustomBeatmapData* self, System::Func_2<::GlobalNamespace::BeatmapDataItem*, ::GlobalNamespace::BeatmapDataItem*>* processDataItem) {
    self->isCreatingFilteredCopy = true;
    auto copy = self->BaseCopy();

    auto linkedList = self->allBeatmapData->get_items();

    for (auto node = linkedList->get_First(); node != nullptr; node = CustomBeatmapData::LinkedListNode_1_get_Next(node)) {
        auto beatmapDataItem = node->item;

        if (!beatmapDataItem) continue;

        BeatmapDataItem* beatmapDataItem2 = processDataItem->Invoke(beatmapDataItem->GetCopy());

        if (auto event = il2cpp_utils::try_cast<BeatmapEventData>(beatmapDataItem)) {
            copy->InsertBeatmapEventData(*event);
        }

        if (auto object = il2cpp_utils::try_cast<BeatmapObjectData>(beatmapDataItem)) {
            copy->AddBeatmapObjectData(*object);
        }

        if (auto customEvent = il2cpp_utils::try_cast<CustomEventData>(beatmapDataItem)) {
            copy->InsertCustomEventData(*customEvent);
        }
    }

    self->isCreatingFilteredCopy = false;
    return copy;
}

BeatmapData * CustomBeatmapData_GetCopy(CustomBeatmapData* self) {
    auto copy = self->BaseCopy();

    auto linkedList = self->allBeatmapData->get_items();

    for (auto node = linkedList->get_First(); node != nullptr; node = CustomBeatmapData::LinkedListNode_1_get_Next(node)) {
        auto beatmapDataItem = node->item;

        if (!beatmapDataItem) continue;

        if (auto event = il2cpp_utils::try_cast<BeatmapEventData>(beatmapDataItem)) {
            copy->InsertBeatmapEventData(*event);
        }

        if (auto object = il2cpp_utils::try_cast<BeatmapObjectData>(beatmapDataItem)) {
            copy->AddBeatmapObjectData(*object);
        }

        if (auto customEvent = il2cpp_utils::try_cast<CustomEventData>(beatmapDataItem)) {
            copy->InsertCustomEventData(*customEvent);
        }
    }

    return copy;
}

MAKE_PAPER_HOOK_MATCH(BeatmapData_GetCopy, &CustomBeatmapData::GetCopy, BeatmapData *, BeatmapData* self) {
    static auto CustomBeatmapDataKlass = classof(CustomBeatmapData*);

    if (self->klass == CustomBeatmapDataKlass) {
        return CustomBeatmapData_GetCopy(reinterpret_cast<CustomBeatmapData*>(self));
    }

    return BeatmapData_GetCopy(self);
}

MAKE_PAPER_HOOK_MATCH(BeatmapData_GetFilteredCopy, &CustomBeatmapData::GetFilteredCopy, BeatmapData *, BeatmapData* self, System::Func_2<::GlobalNamespace::BeatmapDataItem*, ::GlobalNamespace::BeatmapDataItem*>* processDataItem) {
    static auto CustomBeatmapDataKlass = classof(CustomBeatmapData*);

    if (self->klass == CustomBeatmapDataKlass) {
        return CustomBeatmapData_GetFilteredCopy(reinterpret_cast<CustomBeatmapData*>(self), processDataItem);
    }

    return BeatmapData_GetFilteredCopy(self, processDataItem);
}

// This hook loads the json data (with custom data) into a BeatmapSaveData 
MAKE_PAPER_HOOK_MATCH(BeatmapSaveData_DeserializeFromJSONString, &BeatmapSaveDataVersion3::BeatmapSaveData::DeserializeFromJSONString, BeatmapSaveData*, StringW stringData) {
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json");
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!stringData) {
        CJDLogger::Logger.fmtLog<LogLevel::ERR>("No string data");
        return nullptr;
    }

    try {
        auto sharedDoc = std::make_shared<rapidjson::Document>();
        rapidjson::Document &doc = *sharedDoc;
        std::string contents(stringData);
        rapidjson::ParseResult result = doc.Parse(contents);

        if (!result || doc.IsNull() || doc.HasParseError()) {
            std::string errorCodeStr(rapidjson::GetParseError_En(result.Code()));
            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Unable to parse json due to {}", errorCodeStr);
            return nullptr;
        }
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json success");

        auto version = GetVersionFromPath(contents);

        v3::CustomBeatmapSaveData* saveData;

        if (semver::lte(std::string(version), "2.6.0")) {
            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 2.0.0 beatmap");

            saveData = v3::CustomBeatmapSaveData::Convert2_6_0(v2::CustomBeatmapSaveData::Deserialize(sharedDoc));
            saveData->isV2 = true;
          } else {
            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 3.0.0 beatmap");
            saveData = v3::CustomBeatmapSaveData::Deserialize(sharedDoc);
        }

        cachedSaveData = saveData;

        CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished reading beatmap data {}", fmt::ptr(saveData));
        auto stopTime = std::chrono::high_resolution_clock::now();
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("This took {}ms", (int) std::chrono::duration_cast<std::chrono::milliseconds>(
                stopTime - startTime).count());

        return saveData;
    } catch (std::exception const& e) {
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("There was an error loading the beatmap through CJD. Cause of error: {}", e.what());
        return nullptr;
    }
}

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_InsertItem, classof(BeatmapDataSortedListForTypes_1<BeatmapDataItem*>*), "InsertItem", void, BeatmapDataSortedListForTypes_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
    auto list = self->GetList(CustomBeatmapData::GetCustomType(item));

    list->Insert(item);
    // fix method resolution
//
//    auto context = CJDLogger::GetLogger().WithContext("insert log");
//
//    il2cpp_utils::LogMethods(context, reinterpret_cast<Il2CppObject*>(list)->klass, true);
//
//    auto* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod(list, "Insert", std::vector<Il2CppClass*>{}, ::std::vector<const Il2CppType*>{&classof(BeatmapDataItem*)->byval_arg})));
//    ::il2cpp_utils::RunMethodRethrow<::System::Collections::Generic::LinkedListNode_1<BeatmapDataItem>*, false>(list, ___internal__method, item);
}

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_RemoveItem, classof(BeatmapDataSortedListForTypes_1<BeatmapDataItem*>*), "RemoveItem", void, BeatmapDataSortedListForTypes_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
    auto list = self->GetList(CustomBeatmapData::GetCustomType(item));
    auto node = list->NodeForItem(item);
    list->Remove(node);
}

BeatmapCallbacksController* beatmapCallbacksController;

MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_ManualUpdate, &BeatmapCallbacksController::ManualUpdate, void, BeatmapCallbacksController* self, float songTime) {
    if (songTime == self->prevSongTime)
    {
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

    static auto CustomNoteKlass = classof(CustomJSONData::CustomNoteData*);
    static auto CustomObstacleKlass = classof(CustomJSONData::CustomObstacleData*);

    if (obj->klass == CustomNoteKlass) {
        return static_cast<CustomJSONData::CustomNoteData*>(obj)->aheadTimeNoodle;
    }

    if (obj->klass == CustomObstacleKlass) {
        return static_cast<CustomJSONData::CustomObstacleData*>(obj)->aheadTimeNoodle;
    }

    return 0;
}

MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_ManualUpdateTranspile, &BeatmapCallbacksController::ManualUpdate, void, BeatmapCallbacksController* self, float songTime) {
    // TRANSPILE HERE
    if (self != beatmapCallbacksController) {
        CustomEventCallbacks::RegisterCallbacks(self);
        beatmapCallbacksController = self;
    }
    //

    if (songTime == self->prevSongTime)
    {
        return;
    }


    self->songTime = songTime;
    self->processingCallbacks = true;
    if (songTime > self->prevSongTime) {
        auto enumerator = self->callbacksInTimes->GetEnumerator();

        while (enumerator.MoveNext()) {
            auto keyValuePair = enumerator.get_Current();
            auto value = keyValuePair.get_Value();

            using NodePtr = System::Collections::Generic::LinkedListNode_1<GlobalNamespace::BeatmapDataItem*>*;

            auto firstNode = CustomEventCallbacks::firstNode ? (NodePtr) CustomEventCallbacks::firstNode : nullptr;

            for (auto linkedListNode = (value->lastProcessedNode != nullptr)
                                                                   ? CustomBeatmapData::LinkedListNode_1_get_Next(value->lastProcessedNode)
                                                                   : (firstNode ? firstNode : self->beatmapData->get_allBeatmapDataItems()->get_First());
                 linkedListNode != nullptr; linkedListNode = CustomBeatmapData::LinkedListNode_1_get_Next(linkedListNode)) {
                auto value2 = linkedListNode->get_Value();
                if (value2->time - value->aheadTime - GetAheadTime(value2) > songTime) {
                    break;
                }
                if (value2->type == BeatmapDataItem::BeatmapDataItemType::BeatmapEvent ||
                /// TRANSPILE HERE
                value2->type.value == 2 ||
                /// TRANSPILE HERE
                    (value2->type == BeatmapDataItem::BeatmapDataItemType::BeatmapObject &&
                     value2->time >= self->startFilterTime)) {
                    value->CallCallbacks(value2);
                }
                value->lastProcessedNode = linkedListNode;
            }
        }
        enumerator.Dispose();
    } else {
        auto callbacksInTimesEnumerator = self->callbacksInTimes->GetEnumerator();

        while (callbacksInTimesEnumerator.MoveNext()) {
            auto keyValuePair2 = callbacksInTimesEnumerator.get_Current();
            auto value3 = keyValuePair2.get_Value();
            auto linkedListNode2 = value3->lastProcessedNode;
            while (linkedListNode2 != nullptr) {
                auto value4 = linkedListNode2->get_Value();
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

                auto *beatmapEventData = static_cast<BeatmapEventData *>(value4);
                if (beatmapEventData->previousSameTypeEventData != nullptr) {
                    value3->CallCallbacks(beatmapEventData->previousSameTypeEventData);
                } else {
                    auto def = beatmapEventData->GetDefault(beatmapEventData);
                    if (def != nullptr) {
                        value3->CallCallbacks(def);
                    }
                }


                value3->lastProcessedNode = linkedListNode2 = linkedListNode2->get_Previous();
            }
        }
        callbacksInTimesEnumerator.Dispose();
    }

    self->prevSongTime = songTime;
    self->processingCallbacks = false;
}


MAKE_PAPER_HOOK_MATCH(GetBeatmapDataFromBeatmapSaveData, &BeatmapDataLoader::GetBeatmapDataFromBeatmapSaveData, BeatmapData *,
                BeatmapSaveDataVersion3::BeatmapSaveData* beatmapSaveData,
                float startBpm, bool loadingForDesignatedEnvironment,
                ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                ::GlobalNamespace::EnvironmentLightGroups* environmentLightGroups,
                ::GlobalNamespace::DefaultEnvironmentEvents* defaultEnvironmentEvents) {

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing save data {} cached {}", fmt::ptr(beatmapSaveData), fmt::ptr(cachedSaveData));
    auto startTime = std::chrono::high_resolution_clock::now();
    Paper::Profiler profile;
    profile.startTimer();

    bool flag = loadingForDesignatedEnvironment || (beatmapSaveData->useNormalEventsAsCompatibleEvents && defaultEnvironmentEvents->get_isEmpty());
    CustomBeatmapData* beatmapData = CustomBeatmapData::New_ctor(4);

    static auto CustomBeatmapSaveDataKlass = classof(v3::CustomBeatmapSaveData*);

    if (beatmapSaveData->klass == CustomBeatmapSaveDataKlass) {
        auto cBeatmapSaveData = reinterpret_cast<v3::CustomBeatmapSaveData*>(beatmapSaveData);

        beatmapData->customData = ToJsonWrapper(cBeatmapSaveData->customData);
        beatmapData->levelCustomData = ToJsonWrapper(cBeatmapSaveData->levelCustomData);
        beatmapData->beatmapCustomData = ToJsonWrapper(cBeatmapSaveData->beatmapCustomData);
        beatmapData->v2orEarlier = cBeatmapSaveData->isV2;
    }


    beatmapData->InsertBeatmapEventData(CustomJSONData::NewFast<BPMChangeBeatmapEventData*>(-100.0f, startBpm));

    CRASH_UNLESS(beatmapSaveData->basicEventTypesWithKeywords);
    if (beatmapSaveData->basicEventTypesWithKeywords->d && beatmapSaveData->basicEventTypesWithKeywords->d->items) {
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("Klass {}", il2cpp_utils::ClassStandardName(beatmapSaveData->basicEventTypesWithKeywords->d->items->klass));
        for (auto basicEventTypesForKeyword: VList(beatmapSaveData->basicEventTypesWithKeywords->d)) {
            if (!basicEventTypesForKeyword || !basicEventTypesForKeyword->k) continue;

            beatmapData->AddSpecialBasicBeatmapEventKeyword(basicEventTypesForKeyword->k);
        }
    }

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Special events list {}", fmt::ptr(beatmapSaveData->basicEventTypesWithKeywords->d));

    profile.mark("Converted special events");

    auto bpmEvents = beatmapSaveData->bpmEvents;
    BpmTimeProcessor bpmTimeProcessor(startBpm, bpmEvents);

    auto const BeatToTime = [&bpmTimeProcessor](float beat) constexpr {
        return bpmTimeProcessor.ConvertBeatToTime(beat);
    };

    profile.mark("Parsed bpm events");

    // TODO: Remove converter
    CppConverter<BeatmapObjectData*, BeatmapSaveData::BeatmapSaveDataItem*> objectConverter;
    objectConverter.AddConverter<v3::CustomBeatmapSaveData_ColorNoteData*>([&BeatToTime](v3::CustomBeatmapSaveData_ColorNoteData* data) constexpr {
        auto noteData = CreateCustomBasicNoteData(
                BeatToTime(data->b),
                data->get_line(),
                ConvertNoteLineLayer(data->get_layer()),
                ConvertColorType(data->get_color()),
                data->get_cutDirection(),
                data->customData);
        noteData->SetCutDirectionAngleOffset(data->get_angleOffset());

        return noteData;
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_BombNoteData*>([&BeatToTime](v3::CustomBeatmapSaveData_BombNoteData* data) constexpr {
        auto b = CreateCustomBombNoteData(
            BeatToTime(data->b),
            data->get_line(),
            ConvertNoteLineLayer(data->get_layer()),
            data->customData);
        return b;
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_ObstacleData*>([&BeatToTime](v3::CustomBeatmapSaveData_ObstacleData* data) constexpr {
        float beat = BeatToTime(data->b);
        auto obstacle = CustomObstacleData::New_ctor(
                beat,
                data->get_line(),
                GetNoteLineLayer(data->get_layer()),
                BeatToTime(data->b + data->get_duration()) - beat,
                data->get_width(),
                data->get_height());

        obstacle->customData = ToJsonWrapper(data->customData);

        return obstacle;
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_SliderData*>([&BeatToTime](v3::CustomBeatmapSaveData_SliderData* data) constexpr {
        return CustomSliderData_CreateCustomSliderData(
                ConvertColorType(data->get_colorType()),
                BeatToTime(data->b),
                data->get_headLine(),
                ConvertNoteLineLayer(data->get_headLayer()),
                ConvertNoteLineLayer(data->get_headLayer()),
                data->get_headControlPointLengthMultiplier(),
                data->get_headCutDirection(),
                BeatToTime(data->get_tailBeat()),
                data->get_tailLine(),
                ConvertNoteLineLayer(data->get_tailLayer()),
                ConvertNoteLineLayer(data->get_tailLayer()),
                data->get_tailControlPointLengthMultiplier(),
                data->get_tailCutDirection(),
                data->get_sliderMidAnchorMode(),
                data->customData
                );
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_BurstSliderData*>([&BeatToTime](v3::CustomBeatmapSaveData_BurstSliderData* data) constexpr {
        return CustomSliderData_CreateCustomBurstSliderData(
                ConvertColorType(data->get_colorType()),
                BeatToTime(data->b),
                data->get_headLine(),
                ConvertNoteLineLayer(data->get_headLayer()),
                ConvertNoteLineLayer(data->get_tailLayer()),
                data->get_headCutDirection(),
                BeatToTime(data->get_tailBeat()),
                data->get_tailLine(),
                ConvertNoteLineLayer(data->get_tailLayer()),
                ConvertNoteLineLayer(data->get_tailLayer()),
                NoteCutDirection::Any,
                data->get_sliceCount(),
                data->get_squishAmount(),
                data->customData
        );
    });

    objectConverter.AddConverter<BeatmapSaveData::WaypointData*>([&BeatToTime](BeatmapSaveData::WaypointData* data) constexpr {
        return CustomJSONData::NewFast<WaypointData*>(BeatToTime(data->b),
                                      data->get_line(),
                                      ConvertNoteLineLayer(data->get_layer()),
                                      data->get_offsetDirection());
    });


    auto addAllToVector = [](auto& vec, auto const& listPtr) constexpr {
        VList<BeatmapSaveData::BeatmapSaveDataItem*> vList(reinterpret_cast<List<BeatmapSaveData::BeatmapSaveDataItem*>*>(listPtr));

        std::copy(vList.begin(), vList.end(), std::back_inserter(vec));
    };

    auto addAllToVector2 = [](auto& vec, auto const& listPtr) constexpr {
        VList<BeatmapSaveData::EventBoxGroup*> vList(reinterpret_cast<List<BeatmapSaveData::EventBoxGroup*>*>(listPtr));

        std::copy(vList.begin(), vList.end(), std::back_inserter(vec));
    };

    auto cleanAndSort = [](auto& vec) constexpr {
        for (auto it = vec.begin(); it != vec.end();) {
            if (*it) {
                it++;
                continue;
            }

            it = vec.erase(it);
        }

        std::stable_sort(vec.begin(), vec.end(), TimeCompare<typename std::remove_reference_t<decltype(vec)>::const_reference>);
    };

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Creating object list");

    std::vector<BeatmapSaveData::BeatmapSaveDataItem*> beatmapDataObjectItems(
            beatmapSaveData->colorNotes->get_Count() +
            beatmapSaveData->bombNotes->get_Count() +
            beatmapSaveData->obstacles->get_Count() +
            beatmapSaveData->sliders->get_Count() +
            beatmapSaveData->burstSliders->get_Count() +
            beatmapSaveData->waypoints->get_Count()
    );

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

    profile.mark("Grouped all beatmap objects");

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Cleaning and sorting beatmap objects");
    cleanAndSort(beatmapDataObjectItems);
    for (auto const& o : beatmapDataObjectItems) {
        auto* beatmapObjectData = objectConverter.ProcessItem(o);
        if (beatmapObjectData != nullptr) {
            beatmapData->AddBeatmapObjectData(beatmapObjectData);
        }
    }

    profile.mark("Processed and added beatmap objects");
    bpmTimeProcessor.Reset();

    CppConverter<BeatmapEventData*, BeatmapSaveData::BeatmapSaveDataItem*> eventConverter;
    eventConverter.AddConverter<BeatmapSaveData::BpmChangeEventData*>([&BeatToTime](BeatmapSaveData::BeatmapSaveData::BpmChangeEventData* data) constexpr {
        return CustomJSONData::NewFast<BPMChangeBeatmapEventData*>(BeatToTime(data->b),
                                                   data->m);
    });

    eventConverter.AddConverter<BeatmapSaveData::RotationEventData*>([&BeatToTime](BeatmapSaveData::BeatmapSaveData::RotationEventData* data) constexpr {
        SpawnRotationBeatmapEventData::SpawnRotationEventType executionTime =
                                                                     data->get_executionTime() == BeatmapSaveData::ExecutionTime::Early
                                                                     ? SpawnRotationBeatmapEventData::SpawnRotationEventType::Early
                                                                     : SpawnRotationBeatmapEventData::SpawnRotationEventType::Late;

        return CustomJSONData::NewFast<SpawnRotationBeatmapEventData*>(BeatToTime(data->b),
                                                        executionTime,
                                                        data->get_rotation());
    });

    // only for v3 maps
    if (flag) {
        auto specialEventsFilter = BeatmapDataLoader::SpecialEventsFilter::New_ctor(beatmapSaveData->basicEventTypesWithKeywords, environmentKeywords);

        eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>([&BeatToTime, &specialEventsFilter](v3::CustomBeatmapSaveData_BasicEventData* data) constexpr {
            if (!specialEventsFilter->IsEventValid(data->et))
            {
                return (CustomBeatmapEventData*) nullptr;
            }


            auto event = CustomBeatmapEventData::New_ctor(
                    BeatToTime(data->get_beat()),
                    (GlobalNamespace::BasicBeatmapEventType) data->get_eventType(),
                    data->get_value(),
                    data->get_floatValue());

            event->customData = ToJsonWrapper(data->customData);

            return event;
        });

        eventConverter.AddConverter<BeatmapSaveData::ColorBoostEventData*>([&BeatToTime](BeatmapSaveData::ColorBoostEventData* data) constexpr {
            return CustomJSONData::NewFast<ColorBoostBeatmapEventData*>(
                    BeatToTime(data->get_beat()),
                    data->get_boost());
        });
    } else {
        eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>([](v3::CustomBeatmapSaveData_BasicEventData* data) constexpr {
            return nullptr;
        });

        eventConverter.AddConverter<BeatmapSaveData::ColorBoostEventData*>([](BeatmapSaveData::ColorBoostEventData* data) constexpr {
            return nullptr;
        });
    }

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Doing event items");
    std::vector<BeatmapSaveData::BeatmapSaveDataItem*> beatmapDataEventItems(
            bpmEvents->get_Count() +
            beatmapSaveData->basicBeatmapEvents->get_Count() +
            beatmapSaveData->colorBoostBeatmapEvents->get_Count() +
            beatmapSaveData->rotationEvents->get_Count()
    );

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("bpm events");
    addAllToVector(beatmapDataEventItems, bpmEvents);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("basic events");
    addAllToVector(beatmapDataEventItems, beatmapSaveData->basicBeatmapEvents);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("color boost events");
    addAllToVector(beatmapDataEventItems, beatmapSaveData->colorBoostBeatmapEvents);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("rotation events");
    addAllToVector(beatmapDataEventItems, beatmapSaveData->rotationEvents);

    profile.mark("Grouped beatmap events");

    cleanAndSort(beatmapDataEventItems);
    for (auto const& o : beatmapDataEventItems) {
        auto* beatmapEventData = eventConverter.ProcessItem(o);

        if (beatmapEventData != nullptr) {
            beatmapData->InsertBeatmapEventData(beatmapEventData);
        }
    }

    CJDLogger::Logger.fmtLog<LogLevel::INF>("Beatmap events {}", beatmapData->beatmapDataItemsPerType->items->get_Item(csTypeOf(BasicBeatmapEventData*))->get_items()->get_Count());

    profile.mark("Processed and added beatmap events");

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("event groups");
    auto bpmTimeProcessorIl2cpp = CustomJSONData::NewFast<BeatmapDataLoader::BpmTimeProcessor*>(startBpm, bpmEvents);
    auto beatmapEventDataBoxGroupLists = CustomJSONData::NewFast<BeatmapEventDataBoxGroupLists*>(beatmapData, reinterpret_cast<IBeatToTimeConvertor *>(bpmTimeProcessorIl2cpp), false);
    auto eventBoxGroupConvertor = CustomJSONData::NewFast<BeatmapDataLoader::EventBoxGroupConvertor*>(environmentLightGroups);

    EventBoxGroupConvertor cppEventBoxConverter(environmentLightGroups);

    std::vector<BeatmapSaveData::EventBoxGroup*> eventBoxes(
            beatmapSaveData->lightColorEventBoxGroups->get_Count() +
            beatmapSaveData->lightRotationEventBoxGroups->get_Count()
    );

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightColorEventBoxGroups handling events");
    addAllToVector2(eventBoxes, beatmapSaveData->lightColorEventBoxGroups);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightRotationEventBoxGroups handling events");
    addAllToVector2(eventBoxes, beatmapSaveData->lightRotationEventBoxGroups);

    cleanAndSort(eventBoxes);

    profile.mark(fmt::format("Grouped beatmap event boxes {}", eventBoxes.size()));

    for (auto const& o : eventBoxes) {
        auto beatmapEventDataBoxGroup = cppEventBoxConverter.Convert(o); // eventBoxGroupConvertor->Convert(o);
        if (beatmapEventDataBoxGroup != nullptr)
        {
            beatmapEventDataBoxGroupLists->Insert(o->g, beatmapEventDataBoxGroup);
        }
    }

    profile.mark("Processed and added beatmap events boxes");

    if (!flag)
    {
        DefaultEnvironmentEventsFactory::InsertDefaultEnvironmentEvents(beatmapData, beatmapEventDataBoxGroupLists, defaultEnvironmentEvents, environmentLightGroups);
    }

    bpmTimeProcessor.Reset();
    if (auto customBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData))
    {
        if (customBeatmapSaveData.value()->customEventsData) {
            std::stable_sort(customBeatmapSaveData.value()->customEventsData->begin(), customBeatmapSaveData.value()->customEventsData->end(), [](auto const& a, auto const& b) constexpr {return a.time < b.time;});

            for (auto &customEventSaveData: *customBeatmapSaveData.value()->customEventsData) {
                beatmapData->InsertCustomEventData(
                        CustomEventData::New_ctor(bpmTimeProcessor.ConvertBeatToTime(customEventSaveData.time),
                                                  (void *) &customEventSaveData.type,
                                                  customEventSaveData.typeHash,
                                                  (void *) const_cast<rapidjson::Value *>(customEventSaveData.data)));
            }

            CJDLogger::Logger.fmtLog<LogLevel::INF>("Added {} custom events", customBeatmapSaveData.value()->customEventsData->size());
            profile.mark("Processed beatmap custom events");
        }
    }

    // Figure out a way to rewrite this to not be stupid
    beatmapEventDataBoxGroupLists->SyncWithBeatmapData();
    profile.mark("Syncing event box groups");

    beatmapData->ProcessRemainingData();

    profile.mark("Processed processed remaining data");

    profile.endTimer();
    profile.printMarks(CJDLogger::Logger.tag);

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished processing beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("This took {}ms", (int) std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return beatmapData;
}


// CustomJSONData::CustomLevelInfoSaveData*, std::string const&, BeatmapSaveDataVersion3::BeatmapSaveData*, GlobalNamespace::BeatmapDataBasicInfo*
void BeatmapDataLoadedEvent(CustomJSONData::CustomLevelInfoSaveData* customSaveData, const std::string &filename, BeatmapSaveDataVersion3::BeatmapSaveData *beatmapData, GlobalNamespace::BeatmapDataBasicInfo*) {
    if (!beatmapData) {
        CJDLogger::Logger.fmtLog<LogLevel::WRN>("Beatmap is null, no custom level data");
        return;
    }

    auto *customBeatmapData = reinterpret_cast<v3::CustomBeatmapSaveData *>(beatmapData);


    if (customSaveData->customData) {
        customBeatmapData->beatmapCustomData = customSaveData->customData->get();
    }

    v3::CustomDataOptUTF16 levelCustomData;
    for (auto* beatmapSet : customSaveData->difficultyBeatmapSets) {

        for (auto* beatmap : beatmapSet->difficultyBeatmaps) {
            if (!beatmap || !beatmap->beatmapFilename)
                continue;

            auto *customBeatmap = reinterpret_cast<CustomJSONData::CustomDifficultyBeatmap *>(beatmap);

            std::string beatmapFilename = to_utf8(csstrtostr(customBeatmap->beatmapFilename));
            if (beatmapFilename == filename && customBeatmap->customData) {
                levelCustomData = customBeatmap->customData->get();
            }
        }
    }
    customBeatmapData->levelCustomData = levelCustomData;
}

void CustomJSONData::InstallHooks() {
    auto logger = CJDLogger::GetLoggerOld().WithContext("InstallHooks");

    // Install hooks
//    INSTALL_HOOK(logger, BeatmapData_AddBeatmapObjectData)
    INSTALL_HOOK_ORIG(logger, BeatmapSaveData_DeserializeFromJSONString)
    INSTALL_HOOK_ORIG(logger, GetBeatmapDataFromBeatmapSaveData)
    INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_InsertItem);
    INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_RemoveItem);
    INSTALL_HOOK_ORIG(logger, BeatmapData_GetFilteredCopy);
    INSTALL_HOOK_ORIG(logger, BeatmapData_GetCopy);

    // Stupid workaround because stupid NE
    Modloader::requireMod("NoodleExtensions");
    if (true || !Modloader::getMods().contains("NoodleExtensions")) {
        INSTALL_HOOK_ORIG(logger, BeatmapCallbacksController_ManualUpdateTranspile)
    } else {
        INSTALL_HOOK(logger, BeatmapCallbacksController_ManualUpdate)
    }

    RuntimeSongLoader::API::AddBeatmapDataBasicInfoLoadedEvent(BeatmapDataLoadedEvent);

    il2cpp_functions::Class_Init(classof(BeatmapData*));
    custom_types::Register::AutoRegister();
}
