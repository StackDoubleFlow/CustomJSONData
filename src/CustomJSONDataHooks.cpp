#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

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

#include <chrono>

using namespace System;
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

// This hook loads the json data (with custom data) into a BeatmapSaveData 
MAKE_HOOK_OFFSETLESS(BeatmapSaveData_DeserializeFromJSONString, BeatmapSaveData*, Il2CppString *stringData) {
    CJDLogger::GetLogger().debug("Parsing json");
    // CRASH_UNLESS(nullptr);
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
    List<BeatmapSaveData::NoteData*> *notes = List<BeatmapSaveData::NoteData*>::New_ctor(notesArr.Size());
    for (rapidjson::SizeType i = 0; i < notesArr.Size(); i++) {
        rapidjson::Value& note_json = notesArr[i];

        float time = note_json["_time"].GetFloat();
        int lineIndex = note_json["_lineIndex"].GetInt();
        NoteLineLayer lineLayer = NoteLineLayer(note_json["_lineLayer"].GetInt());
        BeatmapSaveData::NoteType type = BeatmapSaveData::NoteType(note_json["_type"].GetInt());
        NoteCutDirection cutDirection = NoteCutDirection(note_json["_cutDirection"].GetInt());
        auto note = (CustomBeatmapSaveData_NoteData *) il2cpp_functions::object_new(noteDataClass);
        il2cpp_utils::RunMethod<Il2CppObject*, false>(note, noteDataCtor, time, lineIndex, lineLayer, type, cutDirection);
        if (note_json.HasMember("_customData")) {
            note->customData = note_json["_customData"];
        }
        notes->items->values[i] = note;
    }
    notes->size = notesArr.Size();

    CJDLogger::GetLogger().debug("Parse obstacles");
    rapidjson::Value& obstaclesArr = doc["_obstacles"];

    // Cache class pointer and ctor method info
    Il2CppClass *obstacleDataClass = classof(CustomBeatmapSaveData_ObstacleData*);
    const MethodInfo *obstacleDataCtor = il2cpp_utils::FindMethodUnsafe(obstacleDataClass, ".ctor", 5);
    List<BeatmapSaveData::ObstacleData*> *obstacles = List<BeatmapSaveData::ObstacleData*>::New_ctor(obstaclesArr.Size());
    for (rapidjson::SizeType i = 0; i < obstaclesArr.Size(); i++) {
        rapidjson::Value& obstacle_json = obstaclesArr[i];

        float time = obstacle_json["_time"].GetFloat();
        int lineIndex = obstacle_json["_lineIndex"].GetInt();
        ObstacleType type = ObstacleType(obstacle_json["_type"].GetInt());
        float duration = obstacle_json["_duration"].GetFloat();
        int width = obstacle_json["_width"].GetInt();
        auto obstacle = (CustomBeatmapSaveData_ObstacleData *) il2cpp_functions::object_new(obstacleDataClass);
        il2cpp_utils::RunMethod<Il2CppObject*, false>(obstacle, obstacleDataCtor, time, lineIndex, type, duration, width);
        if (obstacle_json.HasMember("_customData")) {
            obstacle->customData = obstacle_json["_customData"];
        }
        obstacles->items->values[i] = obstacle;
    }
    obstacles->size = obstaclesArr.Size();

    CJDLogger::GetLogger().debug("Parse events");
    // Parse events
    rapidjson::Value& eventsArr = doc["_events"];
    List<BeatmapSaveData::EventData*> *events = List<BeatmapSaveData::EventData*>::New_ctor(eventsArr.Size());
    for (rapidjson::SizeType i = 0; i < eventsArr.Size(); i++) {
        rapidjson::Value& event_json = eventsArr[i];
        
        float time = event_json["_time"].GetFloat();
        BeatmapSaveData::BeatmapEventType type = BeatmapSaveData::BeatmapEventType(event_json["_type"].GetInt());
        int value = event_json["_value"].GetInt();
        auto event = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapSaveData_EventData*>(time, type, value));
        if (event_json.HasMember("_customData")) {
            event->customData = event_json["_customData"];
        } 
        events->items->values[i] = event;
    }
    events->size = eventsArr.Size();

    CJDLogger::GetLogger().debug("Parse waypoints");
    rapidjson::Value& waypoints_arr = doc["_waypoints"];
    List<BeatmapSaveData::WaypointData*> *waypoints = List<BeatmapSaveData::WaypointData*>::New_ctor(waypoints_arr.Size());
    for (rapidjson::SizeType i = 0; i < waypoints_arr.Size(); i++) {
        rapidjson::Value& waypoint_json = waypoints_arr[i];
        
        float time = waypoint_json["_time"].GetFloat();
        int lineIndex = waypoint_json["_lineIndex"].GetInt();
        NoteLineLayer lineLayer = NoteLineLayer(waypoint_json["_lineLayer"].GetInt());
        OffsetDirection offsetDirection = OffsetDirection(waypoint_json["_offsetDirection"].GetInt());
        auto waypoint = BeatmapSaveData::WaypointData::New_ctor(time, lineIndex, lineLayer, offsetDirection);
        waypoints->items->values[i] = waypoint;
    }
    waypoints->size = waypoints_arr.Size();

    // TODO: Parse whatever the hell this is
    auto specialEventsKeywordFilters = BeatmapSaveData::SpecialEventKeywordFiltersData::New_ctor(List<BeatmapSaveData::SpecialEventsForKeyword*>::New_ctor());

    CJDLogger::GetLogger().debug("Parse root");
    auto saveData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapSaveData*>(events, notes, waypoints, obstacles, specialEventsKeywordFilters));
    saveData->doc = sharedDoc;
    saveData->customEventsData = new std::vector<CustomJSONData::CustomEventData>();
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
        }
    }

    cachedSaveData = saveData;

    CJDLogger::GetLogger().debug("Finished reading beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::GetLogger().debug("This took %ims", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return saveData;
}

CustomNoteData* CustomJSONDataCreateBasicNoteData(Il2CppClass *noteDataClass, const MethodInfo *noteDataCtor, float time, int lineIndex, NoteLineLayer noteLineLayer, ColorType colorType, NoteCutDirection cutDirection) {
    auto noteData = (CustomNoteData *) il2cpp_functions::object_new(noteDataClass);
    il2cpp_utils::RunMethod<Il2CppObject*, false>(noteData, noteDataCtor, time, lineIndex, noteLineLayer, noteLineLayer, colorType, cutDirection, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f);
    return noteData;
}

CustomNoteData* CustomJSONDataCreateBombNoteData(Il2CppClass *noteDataClass, const MethodInfo *noteDataCtor, float time, int lineIndex, NoteLineLayer noteLineLayer) {
    auto noteData = (CustomNoteData *) il2cpp_functions::object_new(noteDataClass);
    il2cpp_utils::RunMethod<Il2CppObject*, false>(noteData, noteDataCtor, time, lineIndex, noteLineLayer, noteLineLayer, ColorType::None, NoteCutDirection::None, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f);
    return noteData;
}

const MethodInfo *getRealTimeFromBPMTimeMethodInfo;
float GetRealTimeFromBPMTime(BeatmapDataLoader *self, float bmpTime, float bpm, float shuffle, float shufflePeriod) {
    if (!getRealTimeFromBPMTimeMethodInfo) {
        Il2CppClass *beatmapDataLoaderClass = classof(BeatmapDataLoader*);
        getRealTimeFromBPMTimeMethodInfo = il2cpp_utils::FindMethodUnsafe(beatmapDataLoaderClass, "GetRealTimeFromBPMTime", 4);
    }
    auto realTimeOptional = il2cpp_utils::RunMethod<float, false>(self, getRealTimeFromBPMTimeMethodInfo, bmpTime, bpm, shuffle, shufflePeriod);
    return CRASH_UNLESS(realTimeOptional);
}

float ProcessTime(BeatmapDataLoader *self, float bpmTime, int &bpmChangesDataIdx, List<BeatmapDataLoader::BpmChangeData> *bpmChangesData, float shuffle, float shufflePeriod) {
    int bpmChangesDataCount = bpmChangesData->size;
    while(bpmChangesDataIdx < bpmChangesDataCount - 1 && bpmChangesData->items->values[bpmChangesDataIdx + 1].bpmChangeStartBpmTime < bpmTime) {
        bpmChangesDataIdx++;
    }
    BeatmapDataLoader::BpmChangeData bpmChangeData = bpmChangesData->items->values[bpmChangesDataIdx];

    float realTime = GetRealTimeFromBPMTime(self, bpmTime - bpmChangeData.bpmChangeStartBpmTime, bpmChangeData.bpm, shuffle, shufflePeriod);

    return bpmChangeData.bpmChangeStartTime + realTime;
}

template <typename T>
bool TimeCompare(T a, T b) {
    return (a->time < b->time);
}

MAKE_HOOK_OFFSETLESS(GetBeatmapDataFromBeatmapSaveData, BeatmapData *, BeatmapDataLoader *self, List<BeatmapSaveData::NoteData *> *notesSaveData, List<BeatmapSaveData::WaypointData *> *waypointsSaveData, 
    List<BeatmapSaveData::ObstacleData *> *obstaclesSaveData, List<BeatmapSaveData::EventData *> *eventsSaveData, BeatmapSaveData::SpecialEventKeywordFiltersData *evironmentSpecialEventFilterData, float startBpm, float shuffle, float shufflePeriod) {
    // il2cpp_functions

    auto startTime = std::chrono::high_resolution_clock::now();

    CustomBeatmapData *beatmapData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapData*>(4));
    JSONWrapper *beatmapCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
    beatmapCustomData->value = cachedSaveData->customData;
    beatmapData->customData = beatmapCustomData;
    DocumentWrapper *beatmapDocument = CRASH_UNLESS(il2cpp_utils::New<DocumentWrapper*>());
    beatmapDocument->doc = cachedSaveData->doc;
    beatmapData->doc = beatmapDocument;

    List<BeatmapDataLoader::BpmChangeData> *bpmChangesData = List<BeatmapDataLoader::BpmChangeData>::New_ctor();
    bpmChangesData->Add(BeatmapDataLoader::BpmChangeData(0, 0, startBpm));
    BeatmapDataLoader::BpmChangeData bpmChangeData = bpmChangesData->items->values[0];
    for (int i = 0; i < eventsSaveData->get_Count(); i++) {
        BeatmapSaveData::EventData *eventData = eventsSaveData->items->values[i];
        if (BeatmapDataLoader::ConvertFromBeatmapSaveDataBeatmapEventType(eventData->type) == BeatmapEventType::Event10) {
            float time = eventData->time;
            int value = eventData->value;
            float bpmChangeStartTime = bpmChangeData.bpmChangeStartTime + GetRealTimeFromBPMTime(self, time - bpmChangeData.bpmChangeStartTime, value, shuffle, shufflePeriod);
            bpmChangesData->Add(BeatmapDataLoader::BpmChangeData(bpmChangeStartTime, time, value));
        }
    }

    // Sort by time
    std::sort(notesSaveData->items->values, notesSaveData->items->values + notesSaveData->items->Length(), TimeCompare<BeatmapSaveData::NoteData*>);
    std::sort(waypointsSaveData->items->values, waypointsSaveData->items->values + waypointsSaveData->items->Length(), TimeCompare<BeatmapSaveData::WaypointData*>);
    std::sort(obstaclesSaveData->items->values, obstaclesSaveData->items->values + obstaclesSaveData->items->Length(), TimeCompare<BeatmapSaveData::ObstacleData*>);

    int notesSaveDataIdx = 0;
    int obstaclesSaveDataIdx = 0;
    int bpmChangesDataIdx = 0;
    int notesSaveDataCount = notesSaveData->get_Count();
    int obstaclesSaveDataCount = obstaclesSaveData->get_Count();
    int waypointsSaveDataCount = waypointsSaveData->get_Count();

    // Cache il2cpp classes and methods
    Il2CppClass *obstacleDataClass = classof(CustomObstacleData*);
    Il2CppClass *noteDataClass = classof(CustomNoteData*);
    Il2CppClass *jsonWrapperClass = classof(JSONWrapper *);
    const MethodInfo *obstacleDataCtor = il2cpp_utils::FindMethodUnsafe(obstacleDataClass, ".ctor", 5);
    const MethodInfo *noteDataCtor = il2cpp_utils::FindMethodUnsafe(noteDataClass, ".ctor", 11);

    while (notesSaveDataIdx < notesSaveDataCount || obstaclesSaveDataIdx < obstaclesSaveDataCount) {
        auto noteData = (notesSaveDataIdx < notesSaveDataCount) ? (CustomBeatmapSaveData_NoteData*) notesSaveData->items->values[notesSaveDataIdx] : nullptr;
        auto waypointData = (notesSaveDataIdx < waypointsSaveDataCount) ? (BeatmapSaveData::WaypointData*) waypointsSaveData->items->values[notesSaveDataIdx] : nullptr;
        auto obstacleData = (obstaclesSaveDataIdx < obstaclesSaveDataCount) ? (CustomBeatmapSaveData_ObstacleData*) obstaclesSaveData->items->values[obstaclesSaveDataIdx] : nullptr;
        if (noteData && (!obstacleData || noteData->time <= obstacleData->time)) {
            float time = ProcessTime(self, noteData->time, bpmChangesDataIdx, bpmChangesData, shuffle, shufflePeriod);
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
        } else if (obstacleData) {
            float obstacleTime = ProcessTime(self, obstacleData->time, bpmChangesDataIdx, bpmChangesData, shuffle, shufflePeriod);

            auto beatmapObjectData2 = (CustomObstacleData*) il2cpp_functions::object_new(obstacleDataClass);
            il2cpp_utils::RunMethod<Il2CppObject*, false>(beatmapObjectData2, obstacleDataCtor, obstacleTime, obstacleData->lineIndex, obstacleData->type, self->GetRealTimeFromBPMTime(obstacleData->duration, startBpm, shuffle, shufflePeriod), obstacleData->width);

            // TODO: Move this into constructor
            JSONWrapper *customData = (JSONWrapper*) il2cpp_functions::object_new(jsonWrapperClass);
            customData->__ctor();
            customData->value = obstacleData->customData;
            beatmapObjectData2->customData = customData;
            beatmapObjectData2->bpm = startBpm;

            beatmapData->AddBeatmapObjectData(beatmapObjectData2);
            obstaclesSaveDataIdx++;
        }
    }

    for (int i = 0; i < eventsSaveData->items->Length(); i++) {
        auto *eventData = (CustomBeatmapSaveData_EventData*) eventsSaveData->items->values[i];
        float time = eventData->time;
        while (bpmChangesDataIdx < bpmChangesData->get_Count() - 1 && bpmChangesData->get_Item(bpmChangesDataIdx + 1).bpmChangeStartBpmTime < time) {
            bpmChangesDataIdx++;
        }
        BeatmapDataLoader::BpmChangeData bpmChangeData2 = bpmChangesData->get_Item(bpmChangesDataIdx);
        float realTime = bpmChangeData2.bpmChangeStartTime + GetRealTimeFromBPMTime(self, time - bpmChangeData2.bpmChangeStartBpmTime, bpmChangeData2.bpm, shuffle, shufflePeriod);
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
    std::vector<CustomEventData> *customEventsData = (std::vector<CustomEventData>*) beatmapData->customEventsData;
    std::sort(customEventsData->begin(), customEventsData->end(), [](CustomEventData& a, CustomEventData& b) {
        return a.time < b.time;
    });
    for (auto& customEventData : *customEventsData) {
        float eventTime = ProcessTime(self, customEventData.time, bpmChangesDataIdx, bpmChangesData, shuffle, shufflePeriod);
        customEventData.time = eventTime;
    }

    CJDLogger::GetLogger().debug("Finished processing beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::GetLogger().debug("This took %ims", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return beatmapData;
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectCallbackController_Start, void, BeatmapObjectCallbackController *self) {
    BeatmapObjectCallbackController_Start(self);

    for (auto& callbackData : CustomEventCallbacks::customEventCallbacks) {
        callbackData.nextEventIndex = 0;
    }
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectCallbackController_LateUpdate, void, BeatmapObjectCallbackController *self) {
    BeatmapObjectCallbackController_LateUpdate(self);

    auto *customBeatmapData = ((CustomBeatmapData*) self->beatmapData);
    auto *customEventsData = (std::vector<CustomEventData>*) customBeatmapData->customEventsData;
    
    for (auto& callbackData : CustomEventCallbacks::customEventCallbacks) {
        while (callbackData.nextEventIndex < customEventsData->size()) {
            CustomEventData *customEventData = &(*customEventsData)[callbackData.nextEventIndex];
            if (customEventData->time - callbackData.aheadTime >= self->audioTimeSource->get_songTime()) {
                break;
            }

            if (customEventData->time >= self->spawningStartTime || callbackData.callIfBeforeStartTime) {
                callbackData.callback(customEventData);
            }

            callbackData.nextEventIndex++;
        }
    }
}

MAKE_HOOK_OFFSETLESS(StandardLevelInfoSaveData_DeserializeFromJSONString, StandardLevelInfoSaveData*, Il2CppString *stringData) {
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


MAKE_HOOK_OFFSETLESS(BeatmapData_AddBeatmapObjectData, void, BeatmapData *self, BeatmapObjectData *beatmapObjectData) {
    if (beatmapObjectData->time < self->prevAddedBeatmapObjectDataTime) {
        CJDLogger::GetLogger().info("AddBeatmapObjectData time %f < prev %f", beatmapObjectData->time, self->prevAddedBeatmapObjectDataTime);
    }
    BeatmapData_AddBeatmapObjectData(self, beatmapObjectData);
}

void CustomJSONData::InstallHooks() {
    auto logger = CJDLogger::GetLogger().WithContext("InstallHooks");

    // Install hooks
    INSTALL_HOOK_OFFSETLESS(logger, BeatmapData_AddBeatmapObjectData, il2cpp_utils::FindMethodUnsafe("", "BeatmapData", "AddBeatmapObjectData", 1));
    INSTALL_HOOK_OFFSETLESS(logger, BeatmapObjectCallbackController_Start, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectCallbackController", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(logger, BeatmapObjectCallbackController_LateUpdate, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectCallbackController", "LateUpdate", 0));
    INSTALL_HOOK_OFFSETLESS(logger, StandardLevelInfoSaveData_DeserializeFromJSONString, il2cpp_utils::FindMethodUnsafe("", "StandardLevelInfoSaveData", "DeserializeFromJSONString", 1));
    INSTALL_HOOK_ORIG(logger, BeatmapSaveData_DeserializeFromJSONString, il2cpp_utils::FindMethodUnsafe("", "BeatmapSaveData", "DeserializeFromJSONString", 1));
    INSTALL_HOOK_ORIG(logger, GetBeatmapDataFromBeatmapSaveData, il2cpp_utils::FindMethodUnsafe("", "BeatmapDataLoader", "GetBeatmapDataFromBeatmapSaveData", 8));

    // Register custom types
    custom_types::Register::RegisterTypes<JSONWrapper,
                                          DocumentWrapper,
                                          CustomLevelInfoSaveData,
                                          CustomDifficultyBeatmap,
                                          CustomBeatmapSaveData,
                                          CustomBeatmapSaveData_NoteData,
                                          CustomBeatmapSaveData_ObstacleData,
                                          CustomBeatmapSaveData_EventData,
                                          CustomBeatmapData,
                                          CustomBeatmapEventData,
                                          CustomObstacleData,
                                          CustomNoteData>();

}