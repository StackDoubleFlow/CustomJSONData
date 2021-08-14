#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "songloader/shared/API.hpp"

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
#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "System/Comparison_1.hpp"

#include "CustomLevelInfoSaveData.h"
#include "CustomBeatmapSaveData.h"
#include "CustomBeatmapData.h"
#include "CustomEventData.h"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

#include <chrono>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;

// This is to prevent issues with string limits
std::string to_utf8(std::u16string_view view) {
    char* dat = static_cast<char*>(calloc(view.length() + 1, sizeof(char)));
    std::transform(view.data(), view.data() + view.size(), dat, [](auto utf16_char) {
        return static_cast<char>(utf16_char);
    });
    dat[view.length()] = '\0';
    return {dat};
}

CustomBeatmapSaveData *cachedSaveData;

#define FindMethodGetter(qualifiedTypeName, methodName) \
::il2cpp_utils::il2cpp_type_check::MetadataGetter<#methodName, qualifiedTypeName, decltype(&qualifiedTypeName::methodName)>::get();

// This hook loads the json data (with custom data) into a BeatmapSaveData 
MAKE_HOOK_MATCH(BeatmapSaveData_DeserializeFromJSONString, &GlobalNamespace::BeatmapSaveData::DeserializeFromJSONString, BeatmapSaveData*, Il2CppString *stringData) {
    CJDLogger::GetLogger().debug("Parsing json");
    auto startTime = std::chrono::high_resolution_clock::now();

    std::string str = to_utf8(csstrtostr(stringData));
    
    std::shared_ptr<rapidjson::Document> sharedDoc = std::make_shared<rapidjson::Document>();
    rapidjson::Document& doc = *sharedDoc;
    doc.Parse(str.c_str());

    CJDLogger::GetLogger().debug("Parsing json success");
    
    CJDLogger::GetLogger().debug("Parse notes");
    rapidjson::Value& notesArr = doc["_notes"];

    // Cache class pointer and ctor method info
    Il2CppClass *noteDataClass = classof(CustomBeatmapSaveData_NoteData*);
    const MethodInfo *noteDataCtor = il2cpp_utils::FindMethodUnsafe(noteDataClass, ".ctor", 5);
    VList<BeatmapSaveData::NoteData*> notes(notesArr.Size());
    for (rapidjson::SizeType i = 0; i < notesArr.Size(); i++) {
        rapidjson::Value& note_json = notesArr[i];

        float time = note_json["_time"].GetFloat();
        int lineIndex = note_json["_lineIndex"].GetInt();
        NoteLineLayer lineLayer = NoteLineLayer(note_json["_lineLayer"].GetInt());
        BeatmapSaveData::NoteType type = BeatmapSaveData::NoteType(note_json["_type"].GetInt());
        NoteCutDirection cutDirection = NoteCutDirection(note_json["_cutDirection"].GetInt());
        auto note = (CustomBeatmapSaveData_NoteData *) il2cpp_functions::object_new(noteDataClass);
        il2cpp_utils::RunMethodThrow<void, false>(note, noteDataCtor, time, lineIndex, lineLayer, type, cutDirection);
        if (note_json.HasMember("_customData")) {
            note->customData = note_json["_customData"];
        }
        notes[i] = note;
    }

    CJDLogger::GetLogger().debug("Parse obstacles");
    rapidjson::Value& obstaclesArr = doc["_obstacles"];

    // Cache class pointer and ctor method info
    Il2CppClass *obstacleDataClass = classof(CustomBeatmapSaveData_ObstacleData*);
    const MethodInfo *obstacleDataCtor = il2cpp_utils::FindMethodUnsafe(obstacleDataClass, ".ctor", 5);
    VList<BeatmapSaveData::ObstacleData*> obstacles(obstaclesArr.Size());
    for (rapidjson::SizeType i = 0; i < obstaclesArr.Size(); i++) {
        rapidjson::Value& obstacle_json = obstaclesArr[i];

        float time = obstacle_json["_time"].GetFloat();
        int lineIndex = obstacle_json["_lineIndex"].GetInt();
        ObstacleType type = ObstacleType(obstacle_json["_type"].GetInt());
        float duration = obstacle_json["_duration"].GetFloat();
        int width = obstacle_json["_width"].GetInt();
        auto obstacle = (CustomBeatmapSaveData_ObstacleData *) il2cpp_functions::object_new(obstacleDataClass);
        il2cpp_utils::RunMethodThrow<void, false>(obstacle, obstacleDataCtor, time, lineIndex, type, duration, width);
        if (obstacle_json.HasMember("_customData")) {
            obstacle->customData = obstacle_json["_customData"];
        }
        obstacles[i] = obstacle;
    }

    CJDLogger::GetLogger().debug("Parse events");
    // Parse events
    rapidjson::Value& eventsArr = doc["_events"];
    VList<BeatmapSaveData::EventData*> events(eventsArr.Size());
    CJDLogger::GetLogger().info("eventsSaveData old size: %i", events.size());
    for (rapidjson::SizeType i = 0; i < eventsArr.Size(); i++) {
        rapidjson::Value& event_json = eventsArr[i];
        
        float time = event_json["_time"].GetFloat();
        BeatmapSaveData::BeatmapEventType type = BeatmapSaveData::BeatmapEventType(event_json["_type"].GetInt());
        int value = event_json["_value"].GetInt();
        auto event = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapSaveData_EventData*>(time, type, value));
        if (event_json.HasMember("_customData")) {
            event->customData = event_json["_customData"];
        } 
        events[i] = event;
    }

    CJDLogger::GetLogger().debug("Parse waypoints");
    rapidjson::Value& waypoints_arr = doc["_waypoints"];
    VList<BeatmapSaveData::WaypointData*> waypoints(waypoints_arr.Size());
    for (rapidjson::SizeType i = 0; i < waypoints_arr.Size(); i++) {
        rapidjson::Value& waypoint_json = waypoints_arr[i];
        
        float time = waypoint_json["_time"].GetFloat();
        int lineIndex = waypoint_json["_lineIndex"].GetInt();
        NoteLineLayer lineLayer = NoteLineLayer(waypoint_json["_lineLayer"].GetInt());
        OffsetDirection offsetDirection = OffsetDirection(waypoint_json["_offsetDirection"].GetInt());
        auto waypoint = BeatmapSaveData::WaypointData::New_ctor(time, lineIndex, lineLayer, offsetDirection);
        waypoints[i] = waypoint;
    }

    // TODO: Parse whatever the hell this is
    auto specialEventsKeywordFilters = BeatmapSaveData::SpecialEventKeywordFiltersData::New_ctor(VList<BeatmapSaveData::SpecialEventsForKeyword*>());

    CJDLogger::GetLogger().debug("Parse root");
    auto saveData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapSaveData*>(*events, *notes, *waypoints, *obstacles, specialEventsKeywordFilters));
    CJDLogger::GetLogger().info("eventsSaveDataList pointer right after constructor: %p", saveData->events);
    saveData->doc = sharedDoc;
    saveData->customEventsData = std::shared_ptr<std::vector<CustomEventData>>(new std::vector<CustomJSONData::CustomEventData>());
    if (doc.HasMember("_customData")) {
        saveData->customData = doc["_customData"];
        rapidjson::Value& customData = *saveData->customData;
        
        if (customData.HasMember("_customEvents")) {
            CJDLogger::GetLogger().debug("Parse custom events");

            rapidjson::Value& customEventsArr = customData["_customEvents"];
            for (rapidjson::SizeType i = 0; i < customEventsArr.Size(); i++) {
                rapidjson::Value& eventValue = customEventsArr[i];
                
                // Dammit Reaxt
                if (!eventValue.HasMember("_time")) continue;

                rapidjson::Value& timeValue = eventValue["_time"];
                float time;
                if (timeValue.GetType() == rapidjson::Type::kStringType) {
                    // Reaxt why
                    time = std::stof(timeValue.GetString());
                } else {
                    time = timeValue.GetFloat();
                }

                std::string_view type = eventValue["_type"].GetString();

                rapidjson::Value *data = &eventValue["_data"];
                saveData->customEventsData->push_back({ type, time, data });
            }

            CJDLogger::GetLogger().debug("Parsed %lu custom events", saveData->customEventsData->size());
        }
    }

    cachedSaveData = saveData;

    CJDLogger::GetLogger().debug("Finished reading beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::GetLogger().debug("This took %ims", (int) std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return saveData;
}

CustomNoteData* CustomJSONDataCreateBasicNoteData(Il2CppClass *noteDataClass, const MethodInfo *noteDataCtor, float time, int lineIndex, NoteLineLayer noteLineLayer, ColorType colorType, NoteCutDirection cutDirection) {
    auto noteData = (CustomNoteData *) il2cpp_functions::object_new(noteDataClass);
    il2cpp_utils::RunMethodThrow<void, false>(noteData, noteDataCtor, time, lineIndex, noteLineLayer, noteLineLayer, colorType, cutDirection, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f);
    return noteData;
}

CustomNoteData* CustomJSONDataCreateBombNoteData(Il2CppClass *noteDataClass, const MethodInfo *noteDataCtor, float time, int lineIndex, NoteLineLayer noteLineLayer) {
    auto noteData = (CustomNoteData *) il2cpp_functions::object_new(noteDataClass);
    il2cpp_utils::RunMethodThrow<void, false>(noteData, noteDataCtor, time, lineIndex, noteLineLayer, noteLineLayer, ColorType::None, NoteCutDirection::None, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f);
    return noteData;
}

float GetRealTimeFromBPMTime(float bpmTime, float bpm, float shuffle, float shufflePeriod) {
    float num = bpmTime;
    if (shufflePeriod > 0 && (int)(num * (1 / shufflePeriod)) % 2 == 1) {
        num += shuffle * shufflePeriod;
    }
    if (bpm > 0) {
        num = num / bpm * 60;
    }
    return num;
}

float ProcessTime(float bpmTime, int &bpmChangesDataIdx, VList<BeatmapDataLoader::BpmChangeData> bpmChangesData, float shuffle, float shufflePeriod) {
    int bpmChangesDataCount = bpmChangesData.size();
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

    auto startTime = std::chrono::high_resolution_clock::now();

    CustomBeatmapData *beatmapData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapData*>(4));
    JSONWrapper *beatmapCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
    beatmapCustomData->value = cachedSaveData->customData;
    beatmapData->customData = beatmapCustomData;
    DocumentWrapper *beatmapDocument = CRASH_UNLESS(il2cpp_utils::New<DocumentWrapper*>());
    beatmapDocument->doc = cachedSaveData->doc;
    beatmapData->doc = beatmapDocument;

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

    // Sort by time
    std::sort(notesSaveData.begin(), notesSaveData.end(), TimeCompare<BeatmapSaveData::NoteData*>);
    std::sort(waypointsSaveData.begin(), waypointsSaveData.end(), TimeCompare<BeatmapSaveData::WaypointData*>);
    std::sort(obstaclesSaveData.begin(), obstaclesSaveData.end(), TimeCompare<BeatmapSaveData::ObstacleData*>);

    int notesSaveDataIdx = 0;
    int waypointsSaveDataIdx = 0;
    int obstaclesSaveDataIdx = 0;
    int bpmChangesDataIdx = 0;
    int notesSaveDataCount = notesSaveData.size();
    int obstaclesSaveDataCount = obstaclesSaveData.size();
    int waypointsSaveDataCount = waypointsSaveData.size();

    // Cache il2cpp classes and methods
    Il2CppClass *obstacleDataClass = classof(CustomObstacleData*);
    Il2CppClass *noteDataClass = classof(CustomNoteData*);
    Il2CppClass *jsonWrapperClass = classof(JSONWrapper *);
    const MethodInfo *obstacleDataCtor = il2cpp_utils::FindMethodUnsafe(obstacleDataClass, ".ctor", 5);
    const MethodInfo *noteDataCtor = il2cpp_utils::FindMethodUnsafe(noteDataClass, ".ctor", 11);

    auto *noteData = (notesSaveDataIdx < notesSaveDataCount) ? (CustomBeatmapSaveData_NoteData*) notesSaveData[notesSaveDataIdx] : nullptr;
    auto *waypointData = (waypointsSaveDataIdx < waypointsSaveDataCount) ? (BeatmapSaveData::WaypointData*) waypointsSaveData[waypointsSaveDataIdx] : nullptr;
    auto *obstacleData = (obstaclesSaveDataIdx < obstaclesSaveDataCount) ? (CustomBeatmapSaveData_ObstacleData*) obstaclesSaveData[obstaclesSaveDataIdx] : nullptr;

    auto ProcessTime = [&](float bpmTime) {
        for (; bpmChangesDataIdx < bpmChangesData.size() - 1 && bpmChangesData[bpmChangesDataIdx + 1].bpmChangeStartBpmTime < bpmTime; bpmChangesDataIdx++)	{}
        BeatmapDataLoader::BpmChangeData bpmChangeData = bpmChangesData[bpmChangesDataIdx];
        return bpmChangeData.bpmChangeStartTime + GetRealTimeFromBPMTime(bpmTime - bpmChangeData.bpmChangeStartBpmTime, bpmChangeData.bpm, shuffle, shufflePeriod);
    };

    while (true) {
        BeatmapObjectType nextType = GetMinTime(noteData, waypointData, obstacleData);
        if (nextType == BeatmapObjectType::Note) {
            float time = ProcessTime(noteData->time);
            ColorType colorType = BeatmapDataLoader::ConvertFromBeatmapSaveDataNoteType(noteData->type);
            CustomNoteData *customNoteData;
            if (colorType == ColorType::None) {
                customNoteData = CustomJSONDataCreateBombNoteData(noteDataClass, noteDataCtor, time, noteData->lineIndex, noteData->lineLayer);
            } else {
                customNoteData = CustomJSONDataCreateBasicNoteData(noteDataClass, noteDataCtor, time, noteData->lineIndex, noteData->lineLayer, colorType, noteData->cutDirection);
            }

            JSONWrapper *customData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
            customData->value = noteData->customData;
            customNoteData->customData = customData;
            customNoteData->bpm = startBpm;

            beatmapData->AddBeatmapObjectData(customNoteData);
            notesSaveDataIdx++;
            noteData = (notesSaveDataIdx < notesSaveDataCount) ? (CustomBeatmapSaveData_NoteData*) notesSaveData[notesSaveDataIdx] : nullptr;
        } else if (nextType == BeatmapObjectType::Waypoint) {
            float time = ProcessTime(noteData->time);
            WaypointData *beatmapObjectData = WaypointData::New_ctor(time, waypointData->lineIndex, waypointData->lineLayer, waypointData->offsetDirection);
            beatmapData->AddBeatmapObjectData(beatmapObjectData);
            waypointsSaveDataIdx++;
            waypointData = (waypointsSaveDataIdx < waypointsSaveDataCount) ? (BeatmapSaveData::WaypointData*) waypointsSaveData[waypointsSaveDataIdx] : nullptr;
        } else if (nextType == BeatmapObjectType::Obstacle) {
            float time = ProcessTime(obstacleData->time);

            auto customObstacleData = (CustomObstacleData*) il2cpp_functions::object_new(obstacleDataClass);
            il2cpp_utils::RunMethodThrow<void, false>(customObstacleData, obstacleDataCtor, time, obstacleData->lineIndex, obstacleData->type, GetRealTimeFromBPMTime(obstacleData->duration, startBpm, shuffle, shufflePeriod), obstacleData->width);

            JSONWrapper *customData = (JSONWrapper*) il2cpp_functions::object_new(jsonWrapperClass);
            customData->__ctor();
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

    for (auto& regularEventData : eventsSaveData) {
        auto *eventData = reinterpret_cast<CustomBeatmapSaveData_EventData*>(regularEventData);
        float time = eventData->time;
        while (bpmChangesDataIdx < bpmChangesData.size() - 1 && bpmChangesData[bpmChangesDataIdx + 1].bpmChangeStartBpmTime < time) {
            bpmChangesDataIdx++;
        }
        BeatmapDataLoader::BpmChangeData bpmChangeData2 = bpmChangesData[bpmChangesDataIdx];
        float realTime = bpmChangeData2.bpmChangeStartTime + GetRealTimeFromBPMTime(time - bpmChangeData2.bpmChangeStartBpmTime, bpmChangeData2.bpm, shuffle, shufflePeriod);
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
    beatmapData->ProcessRemainingData();

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



MAKE_HOOK_MATCH(StandardLevelInfoSaveData_DeserializeFromJSONString, &StandardLevelInfoSaveData::DeserializeFromJSONString, StandardLevelInfoSaveData*, Il2CppString *stringData) {
    auto *original = StandardLevelInfoSaveData_DeserializeFromJSONString(stringData);
    
    ::Array<StandardLevelInfoSaveData::DifficultyBeatmapSet*> *customBeatmapSets = 
        ::Array<StandardLevelInfoSaveData::DifficultyBeatmapSet*>::NewLength(original->difficultyBeatmapSets->Length());

    CustomLevelInfoSaveData *customSaveData = CRASH_UNLESS(il2cpp_utils::New<CustomLevelInfoSaveData*>(original->songName, 
        original->songSubName, original->songAuthorName, original->levelAuthorName, original->beatsPerMinute, original->songTimeOffset, 
        original->shuffle, original->shufflePeriod, original->previewStartTime, original->previewDuration, original->songFilename, 
        original->coverImageFilename, original->environmentName, original->allDirectionsEnvironmentName, customBeatmapSets));
    
    std::string str = to_utf8(csstrtostr(stringData));
    
    std::shared_ptr<rapidjson::Document> sharedDoc = std::make_shared<rapidjson::Document>();
    customSaveData->doc = sharedDoc;

    rapidjson::Document& doc = *sharedDoc;
    doc.Parse(str.c_str());

    if (doc.HasMember("_customData")) {
        customSaveData->customData = doc["_customData"];
    }

    rapidjson::Value& beatmapSetsArr = doc["_difficultyBeatmapSets"];

    for (rapidjson::SizeType i = 0; i < beatmapSetsArr.Size(); i++) {
        rapidjson::Value& beatmapSetJson = beatmapSetsArr[i];
        StandardLevelInfoSaveData::DifficultyBeatmapSet *standardBeatmapSet = original->difficultyBeatmapSets->values[i];
        ::Array<StandardLevelInfoSaveData::DifficultyBeatmap*> *customBeatmaps = 
            ::Array<StandardLevelInfoSaveData::DifficultyBeatmap*>::NewLength(standardBeatmapSet->difficultyBeatmaps->Length());

        for (rapidjson::SizeType j = 0; j < standardBeatmapSet->difficultyBeatmaps->Length(); j++) {
            rapidjson::Value& difficultyBeatmapJson = beatmapSetJson["_difficultyBeatmaps"][j];
            StandardLevelInfoSaveData::DifficultyBeatmap *standardBeatmap = standardBeatmapSet->difficultyBeatmaps->values[j];

            CustomDifficultyBeatmap *customBeatmap = CRASH_UNLESS(il2cpp_utils::New<CustomDifficultyBeatmap*>(standardBeatmap->difficulty, 
                standardBeatmap->difficultyRank, standardBeatmap->beatmapFilename, standardBeatmap->noteJumpMovementSpeed, standardBeatmap->noteJumpStartBeatOffset));
            
            if (difficultyBeatmapJson.HasMember("_customData")) {
                customBeatmap->customData = difficultyBeatmapJson["_customData"];
            }

            customBeatmaps->values[j] = customBeatmap;
        }

        customBeatmapSets->values[i] = StandardLevelInfoSaveData::DifficultyBeatmapSet::New_ctor(standardBeatmapSet->beatmapCharacteristicName, customBeatmaps);
    }

    return customSaveData;
}


MAKE_HOOK_MATCH(BeatmapData_AddBeatmapObjectData, &BeatmapData::AddBeatmapObjectData, void, BeatmapData *self, BeatmapObjectData *beatmapObjectData) {
    if (beatmapObjectData->time < self->prevAddedBeatmapObjectDataTime) {
        CJDLogger::GetLogger().info("AddBeatmapObjectData time %f < prev %f", beatmapObjectData->time, self->prevAddedBeatmapObjectDataTime);
    }
    BeatmapData_AddBeatmapObjectData(self, beatmapObjectData);
}

void BeatmapDataLoadedEvent(StandardLevelInfoSaveData *standardLevelInfoSaveData, const std::string &filename, BeatmapData *beatmapData) {
    auto *customSaveData = reinterpret_cast<CustomLevelInfoSaveData *>(standardLevelInfoSaveData);
    auto *customBeatmapData = reinterpret_cast<CustomBeatmapData *>(beatmapData);

    JSONWrapper *beatmapCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
    beatmapCustomData->value = customSaveData->customData;
    customBeatmapData->beatmapCustomData = beatmapCustomData;

    JSONWrapper *levelCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
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
    INSTALL_HOOK(logger, StandardLevelInfoSaveData_DeserializeFromJSONString)
    INSTALL_HOOK_ORIG(logger, BeatmapSaveData_DeserializeFromJSONString)
    INSTALL_HOOK_ORIG(logger, GetBeatmapDataFromBeatmapSaveData)

    RuntimeSongLoader::API::AddBeatmapDataLoadedEvent(BeatmapDataLoadedEvent);

    custom_types::Register::AutoRegister();
}
