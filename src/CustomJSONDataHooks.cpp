#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "songloader/shared/API.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "pinkcore/shared/CustomTypes/CustomLevelInfoSaveData.hpp"

#include "GlobalNamespace/BeatmapSaveData.hpp"
#include "GlobalNamespace/BeatmapSaveData_NoteData.hpp"
#include "GlobalNamespace/BeatmapSaveData_ObstacleData.hpp"
#include "GlobalNamespace/BeatmapSaveData_WaypointData.hpp"
#include "GlobalNamespace/BeatmapSaveData_EventData.hpp"
#include "GlobalNamespace/BeatmapSaveData_SpecialEventKeywordFiltersData.hpp"
#include "GlobalNamespace/BeatmapSaveData_SpecialEventsForKeyword.hpp"
#include "GlobalNamespace/BeatmapDataLoader_BpmChangeData.hpp"
#include "GlobalNamespace/BeatmapObjectType.hpp"
#include "GlobalNamespace/BeatmapDataLoader.hpp"
#include "GlobalNamespace/BeatmapDataMirrorTransform.hpp"
#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "System/Comparison_1.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Linq/Enumerable.hpp"

#include "CustomBeatmapSaveData.h"
#include "CustomBeatmapData.h"
#include "CustomEventData.h"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include <chrono>
#include <codecvt>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;

// This is to prevent issues with string limits
std::string to_utf8(std::u16string_view view) {
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(view.data());
}

CustomBeatmapSaveData *cachedSaveData;

#define FindMethodGetter(qualifiedTypeName, methodName) \
::il2cpp_utils::il2cpp_type_check::MetadataGetter<#methodName, qualifiedTypeName, decltype(&qualifiedTypeName::methodName)>::get();

// This hook loads the json data (with custom data) into a BeatmapSaveData 
MAKE_HOOK_MATCH(BeatmapSaveData_DeserializeFromJSONString, &GlobalNamespace::BeatmapSaveData::DeserializeFromJSONString, BeatmapSaveData*, Il2CppString *stringData) {
    CJDLogger::GetLogger().debug("Parsing json");
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!stringData) {
        CJDLogger::GetLogger().error("No string data");
        return nullptr;
    }

    try {
        std::string str = to_utf8(csstrtostr(stringData));

        auto sharedDoc = std::make_shared<rapidjson::Document>();
        rapidjson::Document &doc = *sharedDoc;
        rapidjson::ParseResult result = doc.Parse(str);

        if (!result || doc.IsNull() || doc.HasParseError()) {
            std::string errorCodeStr(rapidjson::GetParseError_En(result.Code()));
            CJDLogger::GetLogger().debug("Unable to parse json due to %s", errorCodeStr.c_str());
            return nullptr;
        }
        CJDLogger::GetLogger().debug("Parsing json success");

        CJDLogger::GetLogger().debug("Parse notes");

        VList<BeatmapSaveData::NoteData *> notes;
        auto notesArrIt = doc.FindMember("_notes");

        if (notesArrIt != doc.MemberEnd() && notesArrIt->value.IsArray()) {
            auto &notesArr = notesArrIt->value;
            notes = VList<BeatmapSaveData::NoteData *>(notesArr.Size());


            for (rapidjson::SizeType i = 0; i < notesArr.Size(); i++) {
                rapidjson::Value &note_json = notesArr[i];

                float time = note_json["_time"].GetFloat();
                int lineIndex = note_json["_lineIndex"].GetInt();
                NoteLineLayer lineLayer = NoteLineLayer(note_json["_lineLayer"].GetInt());
                BeatmapSaveData::NoteType type = BeatmapSaveData::NoteType(note_json["_type"].GetInt());
                NoteCutDirection cutDirection = NoteCutDirection(note_json["_cutDirection"].GetInt());
                auto note = CRASH_UNLESS(
                        il2cpp_utils::New<CustomBeatmapSaveData_NoteData *>(time, lineIndex, lineLayer, type,
                                                                            cutDirection));

                auto customDataIt = note_json.FindMember("_customData");
                if (customDataIt != note_json.MemberEnd() && customDataIt->value.IsObject()) {
                    note->customData = customDataIt->value;
                }
                notes[i] = note;
            }
        }

        CJDLogger::GetLogger().debug("Parsed %i notes", notes.size());

        CJDLogger::GetLogger().debug("Parse obstacles");
        auto obstaclesArrIt = doc.FindMember("_obstacles");

        VList<BeatmapSaveData::ObstacleData *> obstacles;

        if (obstaclesArrIt->value.IsArray()) {
            auto &obstaclesArr = obstaclesArrIt->value;

            obstacles = VList<BeatmapSaveData::ObstacleData *>(obstaclesArr.Size());

            for (rapidjson::SizeType i = 0; i < obstaclesArr.Size(); i++) {
                rapidjson::Value &obstacle_json = obstaclesArr[i];

                float time = obstacle_json["_time"].GetFloat();
                int lineIndex = obstacle_json["_lineIndex"].GetInt();
                ObstacleType type = ObstacleType(obstacle_json["_type"].GetInt());
                float duration = obstacle_json["_duration"].GetFloat();
                int width = obstacle_json["_width"].GetInt();
                auto obstacle = CRASH_UNLESS(
                        il2cpp_utils::New<CustomBeatmapSaveData_ObstacleData *>(time, lineIndex, type, duration,
                                                                                width));

                auto customDataIt = obstacle_json.FindMember("_customData");
                if (customDataIt != obstacle_json.MemberEnd() && customDataIt->value.IsObject()) {
                    obstacle->customData = customDataIt->value;
                }
                obstacles[i] = obstacle;
            }
        }

        CJDLogger::GetLogger().debug("Parsed %i obstacles", obstacles.size());

        CJDLogger::GetLogger().debug("Parse events");

        auto eventsArrIt = doc.FindMember("_events");
        VList<BeatmapSaveData::EventData *> events;

        if (eventsArrIt != doc.MemberEnd() && eventsArrIt->value.IsArray()) {
            // Parse events
            rapidjson::Value &eventsArr = doc["_events"];
            events = VList<BeatmapSaveData::EventData *>(eventsArr.Size());

            CJDLogger::GetLogger().info("eventsSaveData old size: %i", events.size());
            for (rapidjson::SizeType i = 0; i < eventsArr.Size(); i++) {
                rapidjson::Value &event_json = eventsArr[i];

                float time = event_json["_time"].GetFloat();
                BeatmapSaveData::BeatmapEventType type = BeatmapSaveData::BeatmapEventType(
                        event_json["_type"].GetInt());
                int value = event_json["_value"].GetInt();
                auto event = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapSaveData_EventData *>(time, type, value));

                auto customDataIt = event_json.FindMember("_customData");
                if (customDataIt != event_json.MemberEnd() && customDataIt->value.IsObject()) {
                    event->customData = customDataIt->value;
                }
                events[i] = event;
            }
        }
        CJDLogger::GetLogger().debug("Parsed %i events", events.size());

        CJDLogger::GetLogger().debug("Parse waypoints");
        auto waypoints_arrIt = doc.FindMember("_waypoints");

        VList<BeatmapSaveData::WaypointData *> waypoints;

        if (waypoints_arrIt != doc.MemberEnd() && waypoints_arrIt->value.IsArray()) {
            rapidjson::Value &waypoints_arr = doc["_waypoints"];

            waypoints = VList<BeatmapSaveData::WaypointData *>(waypoints_arr.Size());

            for (rapidjson::SizeType i = 0; i < waypoints_arr.Size(); i++) {
                rapidjson::Value &waypoint_json = waypoints_arr[i];

                float time = waypoint_json["_time"].GetFloat();
                int lineIndex = waypoint_json["_lineIndex"].GetInt();
                NoteLineLayer lineLayer = NoteLineLayer(waypoint_json["_lineLayer"].GetInt());
                OffsetDirection offsetDirection = OffsetDirection(waypoint_json["_offsetDirection"].GetInt());
                auto waypoint = BeatmapSaveData::WaypointData::New_ctor(time, lineIndex, lineLayer, offsetDirection);
                waypoints[i] = waypoint;
            }
        }
        CJDLogger::GetLogger().debug("Parsed %i waypoints", waypoints.size());

        CJDLogger::GetLogger().debug("Parse specialEventsKeywordFilters");
        auto specialEventsKeywordFiltersJsonObjIt = doc.FindMember("_specialEventsKeywordFilters");
        VList<BeatmapSaveData::SpecialEventsForKeyword *> specialEventsKeywordFiltersList;

        if (specialEventsKeywordFiltersJsonObjIt != doc.MemberEnd()) {
            rapidjson::Value const &specialEventsKeywordFiltersJsonObj = specialEventsKeywordFiltersJsonObjIt->value;


            auto _keywords = specialEventsKeywordFiltersJsonObj.FindMember("_keywords");

            if (_keywords != specialEventsKeywordFiltersJsonObj.MemberEnd()) {
                specialEventsKeywordFiltersList = VList<BeatmapSaveData::SpecialEventsForKeyword *>(_keywords->value.Size());

                for (auto const &keyword_json: _keywords->value.GetArray()) {
                    std::string keyword = keyword_json["_keyword"].GetString();
                    Il2CppString *keyword_il2cpp = il2cpp_utils::newcsstr(keyword);

                    auto specialEventsArray = keyword_json["_specialEvents"].GetArray();
                    VList<BeatmapSaveData::BeatmapEventType> specialEvents(specialEventsArray.Size());

                    for (auto &specialEvent: specialEventsArray) {
                        // safety, why not?
                        if (!specialEvent.IsNumber())
                            continue;

                        specialEvents.push_back(specialEvent.GetInt());
                    }

                    specialEventsKeywordFiltersList.push_back(
                            BeatmapSaveData::SpecialEventsForKeyword::New_ctor(keyword_il2cpp, *specialEvents));
                }
            }
        }
        auto specialEventsKeywordFilters = BeatmapSaveData::SpecialEventKeywordFiltersData::New_ctor(
                specialEventsKeywordFiltersList);

        CJDLogger::GetLogger().debug("Parse root");
        auto saveData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapSaveData *>(*events, *notes, *waypoints, *obstacles,
                                                                                specialEventsKeywordFilters));
        CJDLogger::GetLogger().info("eventsSaveDataList pointer right after constructor: %p", saveData->events);
        saveData->doc = sharedDoc;
        saveData->customEventsData = std::make_shared<std::vector<CustomJSONData::CustomEventData>>();
        auto customDataIt = doc.FindMember("_customData");
        if (customDataIt->value.IsObject()) {
            saveData->customData = customDataIt->value;
            rapidjson::Value &customData = *saveData->customData;

            auto customEventsIt = customData.FindMember("_customEvents");
            if (customEventsIt != customData.MemberEnd() && customEventsIt->value.IsArray()) {
                CJDLogger::GetLogger().debug("Parse custom events");

                rapidjson::Value &customEventsArr = customEventsIt->value;
                for (rapidjson::SizeType i = 0; i < customEventsArr.Size(); i++) {
                    rapidjson::Value &eventValue = customEventsArr[i];

                    // Any consequences? Nah never
                    if (!eventValue.HasMember("_type"))
                        continue;

                    float time = 0;
                    // Dammit Reaxt
                    auto timeIt = eventValue.FindMember("_time");
                    if (timeIt != eventValue.MemberEnd()) {
                        rapidjson::Value &timeValue = timeIt->value;
                        if (timeValue.GetType() == rapidjson::Type::kStringType) {
                            // Reaxt why
                            time = std::stof(timeValue.GetString());
                        } else {
                            time = timeValue.GetFloat();
                        }
                    }

                    std::string_view type = eventValue["_type"].GetString();

                    rapidjson::Value *data = &eventValue["_data"];
                    saveData->customEventsData->push_back({type, time, data});
                }

                CJDLogger::GetLogger().debug("Parsed %lu custom events", saveData->customEventsData->size());
            }
        }

        cachedSaveData = saveData;

        CJDLogger::GetLogger().debug("Finished reading beatmap data");
        auto stopTime = std::chrono::high_resolution_clock::now();
        CJDLogger::GetLogger().debug("This took %ims", (int) std::chrono::duration_cast<std::chrono::milliseconds>(
                stopTime - startTime).count());

        return saveData;
    } catch (std::exception const& e) {
        CJDLogger::GetLogger().debug("There was an error loading the beatmap through CJD. Cause of error: %s", e.what());
        return nullptr;
    }
}

CustomNoteData* CustomJSONDataCreateBasicNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer, ColorType colorType, NoteCutDirection cutDirection) {
    auto b = CRASH_UNLESS(il2cpp_utils::New<CustomNoteData*>(time, lineIndex, noteLineLayer, noteLineLayer, colorType, cutDirection, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f));

    // too lazy to fix constructor
    b->skipBeforeCutScoring = false;
    b->skipAfterCutScoring = false;

    return b;
}

CustomNoteData* CustomJSONDataCreateBombNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer) {
    auto b = CRASH_UNLESS(il2cpp_utils::New<CustomNoteData*>(time, lineIndex, noteLineLayer, noteLineLayer, ColorType::None, NoteCutDirection::None, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f));

    // too lazy to fix constructor
    b->skipBeforeCutScoring = false;
    b->skipAfterCutScoring = false;

    return b;
}

float GetRealTimeFromBPMTime(float bpmTime, float bpm, float shuffle, float shufflePeriod) {
    float num = bpmTime;
    if (shufflePeriod > 0.0f && (int)(num * (1.0f / shufflePeriod)) % 2 == 1) {
        num += shuffle * shufflePeriod;
    }
    if (bpm > 0.0f) {
        num = num / bpm * 60.0f;
    }
    return num;
}

float ProcessTime(float bpmTime, int &bpmChangesDataIdx, VList<BeatmapDataLoader::BpmChangeData> bpmChangesData, float shuffle, float shufflePeriod) {
    int bpmChangesDataCount = 0;
    while(bpmChangesDataIdx < bpmChangesDataCount - 1 && bpmChangesData[bpmChangesDataIdx + 1].bpmChangeStartBpmTime < bpmTime) {
        bpmChangesDataIdx++;
    }
    BeatmapDataLoader::BpmChangeData bpmChangeData = bpmChangesData[bpmChangesDataIdx];

    float realTime = GetRealTimeFromBPMTime(bpmTime - bpmChangeData.bpmChangeStartBpmTime, bpmChangeData.bpm, shuffle, shufflePeriod);

    return bpmChangeData.bpmChangeStartTime + realTime;
}

template <typename T>
bool TimeCompare(T a, T b) {
    return (a->time < b->time);
}

BeatmapObjectType GetMinTime(BeatmapSaveData::NoteData *note, BeatmapSaveData::WaypointData *waypoint, BeatmapSaveData::ObstacleData *obstacle) {
    float time = std::numeric_limits<float>::infinity();
    BeatmapObjectType type = BeatmapObjectType::None;
    if (note && note->time < time) {
        type = BeatmapObjectType::Note;
        time = note->time;
    }
    if (waypoint && waypoint->time < time) {
        type = BeatmapObjectType::Waypoint;
        time = waypoint->time;
    }
    if (obstacle && obstacle->time < time) {
        type = BeatmapObjectType::Obstacle;
        time = obstacle->time;
    }
    return type;
}

MAKE_HOOK_MATCH(BeatmapDataMirrorTransform_CreateTransformedData, &BeatmapDataMirrorTransform::CreateTransformedData, GlobalNamespace::IReadonlyBeatmapData*, GlobalNamespace::IReadonlyBeatmapData* beatmapData) {
    int numberOfLines = beatmapData->get_numberOfLines();
    BeatmapData* beatmapData2;
    if (il2cpp_utils::AssignableFrom<CustomJSONData::CustomBeatmapData*>(reinterpret_cast<Il2CppObject*>(beatmapData)->klass)) {
        beatmapData2 = reinterpret_cast<CustomJSONData::CustomBeatmapData*>(beatmapData)->BaseCopy(); //CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapData*>(numberOfLines)); // crashes with Tracks
    } else {
        beatmapData2 = BeatmapData::New_ctor(numberOfLines);
    }



    auto beatmapObjectsData = System::Collections::Generic::List_1<GlobalNamespace::BeatmapObjectData*>::New_ctor(beatmapData->get_beatmapObjectsData());
    auto beatmapEventsData = System::Collections::Generic::List_1<GlobalNamespace::BeatmapEventData*>::New_ctor(reinterpret_cast<IEnumerable_1<GlobalNamespace::BeatmapEventData*>*>(beatmapData->get_beatmapEventsData()));

    for (BeatmapObjectData* beatmapObjectData : VList(beatmapObjectsData))
    {
        BeatmapObjectData* copy = beatmapObjectData->GetCopy();
        BeatmapDataMirrorTransform::MirrorTransformBeatmapObject(copy, numberOfLines);
        beatmapData2->AddBeatmapObjectData(copy);
    }
    for (BeatmapEventData* beatmapEventData : VList(beatmapEventsData))
    {
        if (BeatmapEventTypeExtensions::IsRotationEvent(beatmapEventData->type))
        {
            int value = 7 - beatmapEventData->value;
            beatmapData2->AddBeatmapEventData(BeatmapEventData::New_ctor(beatmapEventData->time, beatmapEventData->type, value));
        }
        else
        {
            beatmapData2->AddBeatmapEventData(beatmapEventData);
        }
    }
    using keyValuePairList = System::Collections::Generic::List_1<Il2CppString*>;
    using dict = System::Collections::Generic::Dictionary_2<Il2CppString*, System::Collections::Generic::HashSet_1<GlobalNamespace::BeatmapEventType>*>*;

    // Creating a list from constructor doesn't work
    auto availableSpecialEventsPerKeywordDictionary = reinterpret_cast<dict>(beatmapData->get_availableSpecialEventsPerKeywordDictionary());
    auto availableSpecialEventsPerKeywordDictionaryKeys = keyValuePairList::New_ctor(reinterpret_cast<IEnumerable_1<Il2CppString*>*>(availableSpecialEventsPerKeywordDictionary->get_Keys()));

    for (int i = 0; i < availableSpecialEventsPerKeywordDictionary->get_Count(); i++)
    {
        auto key = availableSpecialEventsPerKeywordDictionaryKeys->get_Item(i);
        auto value = availableSpecialEventsPerKeywordDictionary->get_Item(key);
        beatmapData2->AddAvailableSpecialEventsPerKeyword(key, value);
    }
    CJDLogger::GetLogger().debug("Beatmap klass %s", il2cpp_utils::ClassStandardName(beatmapData2->klass).c_str());
    return reinterpret_cast<IReadonlyBeatmapData *>(beatmapData2);
}

MAKE_HOOK_MATCH(GetBeatmapDataFromBeatmapSaveData, &BeatmapDataLoader::GetBeatmapDataFromBeatmapSaveData, BeatmapData *, BeatmapDataLoader *self,
                List<BeatmapSaveData::NoteData*>* notesSaveDataL,
                List<BeatmapSaveData::WaypointData*>* waypointsSaveDataL,
                List<BeatmapSaveData::ObstacleData*>* obstaclesSaveDataL,
                List<BeatmapSaveData::EventData*>* eventsSaveDataL,
                BeatmapSaveData::SpecialEventKeywordFiltersData* environmentSpecialEventFilterData,
                float startBpm, float shuffle, float shufflePeriod) {
    List_1<BeatmapSaveData::EventData*> *eventsSaveDataList = eventsSaveDataL;
    VList<BeatmapSaveData::NoteData*> notesSaveData(notesSaveDataL);
    VList<BeatmapSaveData::WaypointData*> waypointsSaveData(waypointsSaveDataL);
    VList<BeatmapSaveData::ObstacleData*> obstaclesSaveData(obstaclesSaveDataL);
    VList<BeatmapSaveData::EventData*> eventsSaveData(eventsSaveDataL);

    CJDLogger::GetLogger().debug("Parsing save data");
    auto startTime = std::chrono::high_resolution_clock::now();

    CustomBeatmapData *beatmapData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapData*>(4));
    JSONWrapper *beatmapCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
    beatmapCustomData->value = cachedSaveData->customData;
    beatmapData->customData = beatmapCustomData;
    DocumentWrapper *beatmapDocument = CRASH_UNLESS(il2cpp_utils::New<DocumentWrapper*>());
    beatmapDocument->doc = cachedSaveData->doc;
    beatmapData->doc = beatmapDocument;

    CJDLogger::GetLogger().debug("Parsing events save data");
    VList<BeatmapDataLoader::BpmChangeData> bpmChangesData;
    bpmChangesData.push_back(BeatmapDataLoader::BpmChangeData(0, 0, startBpm));
    BeatmapDataLoader::BpmChangeData bpmChangeData = bpmChangesData[0];
    for (auto *eventData : eventsSaveData) {
        if (BeatmapDataLoader::ConvertFromBeatmapSaveDataBeatmapEventType(eventData->type) == BeatmapEventType::Event10) {
            float time = eventData->time;
            int value = eventData->value;
            float bpmChangeStartTime = bpmChangeData.bpmChangeStartTime + GetRealTimeFromBPMTime(time - bpmChangeData.bpmChangeStartTime, value, shuffle, shufflePeriod);
            bpmChangesData.push_back(BeatmapDataLoader::BpmChangeData(bpmChangeStartTime, time, value));
        }
    }

    CJDLogger::GetLogger().debug("Sorting");
    // Sort by time
    std::sort(notesSaveData.begin(), notesSaveData.end(), TimeCompare<BeatmapSaveData::NoteData*>);
    std::sort(waypointsSaveData.begin(), waypointsSaveData.end(), TimeCompare<BeatmapSaveData::WaypointData*>);
    std::sort(obstaclesSaveData.begin(), obstaclesSaveData.end(), TimeCompare<BeatmapSaveData::ObstacleData*>);

    int notesSaveDataIdx = 0;
    int waypointsSaveDataIdx = 0;
    int obstaclesSaveDataIdx = 0;
    int notesSaveDataCount = notesSaveData.size();
    int obstaclesSaveDataCount = obstaclesSaveData.size();
    int waypointsSaveDataCount = waypointsSaveData.size();

    auto *noteData = (notesSaveDataIdx < notesSaveDataCount) ? (CustomBeatmapSaveData_NoteData*) notesSaveData[notesSaveDataIdx] : nullptr;
    auto *waypointData = (waypointsSaveDataIdx < waypointsSaveDataCount) ? (BeatmapSaveData::WaypointData*) waypointsSaveData[waypointsSaveDataIdx] : nullptr;
    auto *obstacleData = (obstaclesSaveDataIdx < obstaclesSaveDataCount) ? (CustomBeatmapSaveData_ObstacleData*) obstaclesSaveData[obstaclesSaveDataIdx] : nullptr;

    auto const ProcessTime = [&](float bpmTime) {
        int bpmChangesDataIdx = 0;
        while(bpmChangesDataIdx < bpmChangesData.size() - 1 && bpmChangesData[bpmChangesDataIdx + 1].bpmChangeStartBpmTime < bpmTime) {
            bpmChangesDataIdx++;
        }
        BeatmapDataLoader::BpmChangeData bpmChangeData = bpmChangesData[bpmChangesDataIdx];
        return bpmChangeData.bpmChangeStartTime + GetRealTimeFromBPMTime(bpmTime - bpmChangeData.bpmChangeStartBpmTime, bpmChangeData.bpm, shuffle, shufflePeriod);
    };

    CJDLogger::GetLogger().debug("Processing time");
    while (true) {
        BeatmapObjectType nextType = GetMinTime(noteData, waypointData, obstacleData);
        if (nextType == BeatmapObjectType::Note) {
            float time = ProcessTime(noteData->time);
            ColorType colorType = BeatmapDataLoader::ColorTypeFromBeatmapSaveDataNoteType(noteData->type);
            CustomNoteData *customNoteData;
            if (colorType == ColorType::None) {
                customNoteData = CustomJSONDataCreateBombNoteData(time, noteData->lineIndex, noteData->lineLayer);
            } else {
                customNoteData = CustomJSONDataCreateBasicNoteData(time, noteData->lineIndex, noteData->lineLayer, colorType, noteData->cutDirection);
            }

            JSONWrapper *customData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
            customData->value = noteData->customData;
            customNoteData->customData = customData;
            customNoteData->bpm = startBpm;

            beatmapData->AddBeatmapObjectData(customNoteData);
            notesSaveDataIdx++;
            noteData = (notesSaveDataIdx < notesSaveDataCount) ? (CustomBeatmapSaveData_NoteData*) notesSaveData[notesSaveDataIdx] : nullptr;
        } else if (nextType == BeatmapObjectType::Waypoint) {
            float time = ProcessTime(waypointData->time);
            WaypointData *beatmapObjectData = WaypointData::New_ctor(time, waypointData->lineIndex, waypointData->lineLayer, waypointData->offsetDirection);
            beatmapData->AddBeatmapObjectData(beatmapObjectData);
            waypointsSaveDataIdx++;
            waypointData = (waypointsSaveDataIdx < waypointsSaveDataCount) ? (BeatmapSaveData::WaypointData*) waypointsSaveData[waypointsSaveDataIdx] : nullptr;
        } else if (nextType == BeatmapObjectType::Obstacle) {
            float time = ProcessTime(obstacleData->time);

            auto customObstacleData = CRASH_UNLESS(il2cpp_utils::New<CustomObstacleData*>(time, obstacleData->lineIndex, obstacleData->type, GetRealTimeFromBPMTime(obstacleData->duration, startBpm, shuffle, shufflePeriod), obstacleData->width));

            JSONWrapper *customData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
            customData->value = obstacleData->customData;
            customObstacleData->customData = customData;
            customObstacleData->bpm = startBpm;

            beatmapData->AddBeatmapObjectData(customObstacleData);
            obstaclesSaveDataIdx++;
            obstacleData = (obstaclesSaveDataIdx < obstaclesSaveDataCount) ? (CustomBeatmapSaveData_ObstacleData*) obstaclesSaveData[obstaclesSaveDataIdx] : nullptr;
        } else {
            break;
        }
    }

    CJDLogger::GetLogger().debug("Processing events save data");
    for (auto& regularEventData : eventsSaveData) {
        auto *eventData = reinterpret_cast<CustomBeatmapSaveData_EventData*>(regularEventData);
        float realTime = ProcessTime(eventData->time);
        
        BeatmapEventType type = BeatmapDataLoader::ConvertFromBeatmapSaveDataBeatmapEventType(eventData->type);
        CustomBeatmapEventData *beatmapEventData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapEventData*>(realTime, type, eventData->value));

        // Assign custom data
        JSONWrapper *customData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
        customData->value = eventData->customData;
        beatmapEventData->customData = customData;

        beatmapData->AddBeatmapEventData(beatmapEventData);
    }
    if (beatmapData->beatmapEventsData->get_Count() == 0) {
        beatmapData->AddBeatmapEventData(BeatmapEventData::New_ctor(0, BeatmapEventType::Event0, 1));
        beatmapData->AddBeatmapEventData(BeatmapEventData::New_ctor(0, BeatmapEventType::Event4, 1));
    }

    CJDLogger::GetLogger().debug("Parsing environmentSpecialEventFilterData");
    if (environmentSpecialEventFilterData && environmentSpecialEventFilterData->keywords)
    {
        for (auto specialEventsForKeyword : VList(environmentSpecialEventFilterData->keywords))
        {
            if (specialEventsForKeyword->keyword && specialEventsForKeyword->specialEvents)
            {
                auto hashSet = HashSet_1<BeatmapEventType>::New_ctor();
                for (BeatmapSaveData::BeatmapEventType beatmapEventType : VList(specialEventsForKeyword->specialEvents))
                {
                    hashSet->Add(BeatmapDataLoader::ConvertFromBeatmapSaveDataBeatmapEventType(beatmapEventType));
                }
                beatmapData->AddAvailableSpecialEventsPerKeyword(specialEventsForKeyword->keyword, hashSet);
            }
            else
            {
                CJDLogger::GetLogger().error("Missing environmentSerializedName:\"%s\" and/or specialEvents set: %s",
                                             to_utf8(csstrtostr(specialEventsForKeyword->keyword)).c_str(), to_utf8(csstrtostr(specialEventsForKeyword->specialEvents->ToString())).c_str());
            }
        }
    }
    beatmapData->ProcessRemainingData();

    CJDLogger::GetLogger().debug("Sorting events");
    beatmapData->customEventsData = cachedSaveData->customEventsData;
    auto &customEventsData = beatmapData->customEventsData;
    std::sort(customEventsData->begin(), customEventsData->end(), [](CustomEventData& a, CustomEventData& b) {
        return a.time < b.time;
    });
    for (auto& customEventData : *customEventsData) {
        float eventTime = ProcessTime(customEventData.time);
        customEventData.time = eventTime;
    }

    CJDLogger::GetLogger().debug("Finished processing beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::GetLogger().debug("This took %ims", (int) std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return beatmapData;
}

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_Start, &BeatmapObjectCallbackController::Start, void, BeatmapObjectCallbackController *self) {
    BeatmapObjectCallbackController_Start(self);

    for (auto& callbackData : CustomEventCallbacks::customEventCallbacks) {
        callbackData.nextEventIndex = 0;
    }
}

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_LateUpdate, &BeatmapObjectCallbackController::LateUpdate, void, BeatmapObjectCallbackController *self) {
    BeatmapObjectCallbackController_LateUpdate(self);
    auto *customBeatmapData = reinterpret_cast<CustomBeatmapData*>(self->beatmapData);

    auto *customBeatmapDataClass = classof(CustomBeatmapData *);
    if (!customBeatmapData || customBeatmapData->klass != customBeatmapDataClass) {
        return;
    }

    auto &customEventsData = customBeatmapData->customEventsData;

    for (auto& callbackData : CustomEventCallbacks::customEventCallbacks) {
        while (callbackData.nextEventIndex < customEventsData->size()) {
            CustomEventData *customEventData = &(*customEventsData)[callbackData.nextEventIndex];
            if (customEventData->time - callbackData.aheadTime >= self->audioTimeSource->get_songTime()) {
                break;
            }

            if (customEventData->time >= self->spawningStartTime || callbackData.callIfBeforeStartTime) {
                callbackData.callback(self, customEventData);
            }

            callbackData.nextEventIndex++;
        }
    }
}

MAKE_HOOK_MATCH(BeatmapData_AddBeatmapObjectData, &BeatmapData::AddBeatmapObjectData, void, BeatmapData *self, BeatmapObjectData *beatmapObjectData) {
    if (beatmapObjectData->time < self->prevAddedBeatmapObjectDataTime) {
        CJDLogger::GetLogger().info("AddBeatmapObjectData time %f < prev %f", beatmapObjectData->time, self->prevAddedBeatmapObjectDataTime);
    }
    BeatmapData_AddBeatmapObjectData(self, beatmapObjectData);
}



void BeatmapDataLoadedEvent(StandardLevelInfoSaveData *standardLevelInfoSaveData, const std::string &filename, BeatmapData *beatmapData) {
    if (!beatmapData) {
        CJDLogger::GetLogger().warning("Beatmap is null, no custom level data");
        return;
    }

    auto *customSaveData = reinterpret_cast<CustomLevelInfoSaveData *>(standardLevelInfoSaveData);
    auto *customBeatmapData = reinterpret_cast<CustomBeatmapData *>(beatmapData);



    JSONWrapperUTF16 *beatmapCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapperUTF16*>());
    beatmapCustomData->value = customSaveData->customData;

    customBeatmapData->beatmapCustomData = beatmapCustomData;

    JSONWrapperUTF16 *levelCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapperUTF16*>());
    for (int i = 0; i < customSaveData->difficultyBeatmapSets->Length(); i++) {
        StandardLevelInfoSaveData::DifficultyBeatmapSet *beatmapSet = customSaveData->difficultyBeatmapSets->values[i];
        for (int j = 0; j < beatmapSet->difficultyBeatmaps->Length(); j++) {
            auto *customBeatmap = reinterpret_cast<CustomDifficultyBeatmap *>(beatmapSet->difficultyBeatmaps->values[j]);
            if (!customBeatmap || !customBeatmap->beatmapFilename)
                continue;


            std::string beatmapFilename = to_utf8(csstrtostr(customBeatmap->beatmapFilename));
            if (beatmapFilename == filename) {
                levelCustomData->value = customBeatmap->customData;
            }
        }
    }
    customBeatmapData->levelCustomData = levelCustomData;
}

void CustomJSONData::InstallHooks() {
    auto logger = CJDLogger::GetLogger().WithContext("InstallHooks");

    // Install hooks
    INSTALL_HOOK(logger, BeatmapData_AddBeatmapObjectData)
    INSTALL_HOOK(logger, BeatmapObjectCallbackController_Start)
    INSTALL_HOOK(logger, BeatmapObjectCallbackController_LateUpdate)
    INSTALL_HOOK_ORIG(logger, BeatmapSaveData_DeserializeFromJSONString)
    INSTALL_HOOK_ORIG(logger, GetBeatmapDataFromBeatmapSaveData)
    INSTALL_HOOK_ORIG(logger, BeatmapDataMirrorTransform_CreateTransformedData)

    RuntimeSongLoader::API::AddBeatmapDataLoadedEvent(BeatmapDataLoadedEvent);

    custom_types::Register::AutoRegister();
}
