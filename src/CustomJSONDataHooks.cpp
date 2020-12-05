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
#include "System/Comparison_1.hpp"

#include "CustomBeatmapSaveData.h"
#include "CustomBeatmapData.h"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"

#include <chrono>

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

MAKE_HOOK_OFFSETLESS(StandardLevelInfoSaveData_DeserializeFromJSONString, StandardLevelInfoSaveData*, Il2CppString *stringData) {
    return StandardLevelInfoSaveData_DeserializeFromJSONString(stringData);
}

CustomBeatmapSaveData *cachedSaveData;

// This hook loads the json data (with custom data) into a BeatmapSaveData 
MAKE_HOOK_OFFSETLESS(BeatmapSaveData_DeserializeFromJSONString, BeatmapSaveData*, Il2CppString *stringData) {
    CJDLogger::GetLogger().debug("Parsing json");
    // CRASH_UNLESS(nullptr);
    auto startTime = std::chrono::high_resolution_clock::now();


    std::string str = to_utf8(csstrtostr(stringData));
    
    rapidjson::Document doc;
    doc.Parse(str.c_str());

    auto alloc = new rapidjson::MemoryPoolAllocator();

    CJDLogger::GetLogger().debug("Parsing json success");
    
    
    CJDLogger::GetLogger().debug("Parse notes");
    rapidjson::Value& notes_arr = doc["_notes"];

    // Cache class pointer and ctor method info
    Il2CppClass *noteDataClass = classof(CustomBeatmapSaveData_NoteData*);
    const MethodInfo *noteDataCtor = il2cpp_utils::FindMethodUnsafe(noteDataClass, ".ctor", 5);
    List<BeatmapSaveData::NoteData*> *notes = List<BeatmapSaveData::NoteData*>::New_ctor(notes_arr.Size());
    for (rapidjson::SizeType i = 0; i < notes_arr.Size(); i++) {
        rapidjson::Value& note_json = notes_arr[i];

        float time = note_json["_time"].GetFloat();
        int lineIndex = note_json["_lineIndex"].GetInt();
        NoteLineLayer lineLayer = NoteLineLayer(note_json["_lineLayer"].GetInt());
        BeatmapSaveData::NoteType type = BeatmapSaveData::NoteType(note_json["_type"].GetInt());
        NoteCutDirection cutDirection = NoteCutDirection(note_json["_cutDirection"].GetInt());
        auto note = (CustomBeatmapSaveData_NoteData *) il2cpp_functions::object_new(noteDataClass);
        il2cpp_utils::RunMethod<Il2CppObject*, false>(note, noteDataCtor, time, lineIndex, lineLayer, type, cutDirection);
        if (note_json.HasMember("_customData")) {
            note->customData = new rapidjson::Value(note_json["_customData"], *alloc);
        }
        notes->items->values[i] = note;
    }
    notes->size = notes_arr.Size();

    CJDLogger::GetLogger().debug("Parse obstacles");
    rapidjson::Value& obstacles_arr = doc["_obstacles"];

    // Cache class pointer and ctor method info
    Il2CppClass *obstacleDataClass = classof(CustomBeatmapSaveData_ObstacleData*);
    const MethodInfo *obstacleDataCtor = il2cpp_utils::FindMethodUnsafe(obstacleDataClass, ".ctor", 5);
    List<BeatmapSaveData::ObstacleData*> *obstacles = List<BeatmapSaveData::ObstacleData*>::New_ctor(obstacles_arr.Size());
    for (rapidjson::SizeType i = 0; i < obstacles_arr.Size(); i++) {
        rapidjson::Value& obstacle_json = obstacles_arr[i];

        float time = obstacle_json["_time"].GetFloat();
        int lineIndex = obstacle_json["_lineIndex"].GetInt();
        ObstacleType type = ObstacleType(obstacle_json["_type"].GetInt());
        float duration = obstacle_json["_duration"].GetFloat();
        int width = obstacle_json["_width"].GetInt();
        auto obstacle = (CustomBeatmapSaveData_ObstacleData *) il2cpp_functions::object_new(obstacleDataClass);
        il2cpp_utils::RunMethod<Il2CppObject*, false>(obstacle, obstacleDataCtor, time, lineIndex, type, duration, width);
        if (obstacle_json.HasMember("_customData")) {
            obstacle->customData = new rapidjson::Value(obstacle_json["_customData"], *alloc);
        }
        obstacles->items->values[i] = obstacle;
    }
    obstacles->size = obstacles_arr.Size();

    CJDLogger::GetLogger().debug("Parse events");
    // Parse events
    rapidjson::Value& events_arr = doc["_events"];
    List<BeatmapSaveData::EventData*> *events = List<BeatmapSaveData::EventData*>::New_ctor(events_arr.Size());
    for (rapidjson::SizeType i = 0; i < events_arr.Size(); i++) {
        rapidjson::Value& event_json = events_arr[i];
        
        float time = event_json["_time"].GetFloat();
        BeatmapSaveData::BeatmapEventType type = BeatmapSaveData::BeatmapEventType(event_json["_type"].GetInt());
        int value = event_json["_value"].GetInt();
        auto event = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapSaveData_EventData*>(time, type, value));
        if (event_json.HasMember("_customData")) {
            event->customData = new rapidjson::Value(event_json["_customData"], *alloc);
        } 
        events->items->values[i] = event;
    }
    events->size = events_arr.Size();

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
    if (doc.HasMember("_customData")) {
        saveData->customData = new rapidjson::Value(doc["_customData"], *alloc);
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

MAKE_HOOK_OFFSETLESS(GetBeatmapDataFromBeatmapSaveData, BeatmapData *, BeatmapDataLoader *self, List<BeatmapSaveData::NoteData *> *notesSaveData, List<BeatmapSaveData::WaypointData *> *waypointsSaveData, 
    List<BeatmapSaveData::ObstacleData *> *obstaclesSaveData, List<BeatmapSaveData::EventData *> *eventsSaveData, BeatmapSaveData::SpecialEventKeywordFiltersData *evironmentSpecialEventFilterData, float startBpm, float shuffle, float shufflePeriod) {
    
    auto startTime = std::chrono::high_resolution_clock::now();

    CustomBeatmapData *beatmapData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapData*>(4));
    if (cachedSaveData->customData) {
        JSONWrapper *customData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
        customData->value = cachedSaveData->customData;
        beatmapData->customData = customData;
    }

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
    notesSaveData->Sort(il2cpp_utils::MakeDelegate<System::Comparison_1<BeatmapSaveData::NoteData*>*>(classof(System::Comparison_1<BeatmapSaveData::NoteData*>*), (void*)nullptr, 
        +[](BeatmapSaveData::NoteData *x, BeatmapSaveData::NoteData *y) {
            if (x->time >= y->time) {
                return 1;
            }
            return -1;
        }
    ));
    waypointsSaveData->Sort(il2cpp_utils::MakeDelegate<System::Comparison_1<BeatmapSaveData::WaypointData*>*>(classof(System::Comparison_1<BeatmapSaveData::WaypointData*>*), (void*)nullptr, 
        +[](BeatmapSaveData::WaypointData *x, BeatmapSaveData::WaypointData *y) {
            if (x->time >= y->time) {
                return 1;
            }
            return -1;
        }
    ));
    obstaclesSaveData->Sort(il2cpp_utils::MakeDelegate<System::Comparison_1<BeatmapSaveData::ObstacleData*>*>(classof(System::Comparison_1<BeatmapSaveData::ObstacleData*>*), (void*)nullptr, 
        +[](BeatmapSaveData::ObstacleData *x, BeatmapSaveData::ObstacleData *y) {
            if (x->time >= y->time) {
                return 1;
            }
            return -1;
        }
    ));

    int notesSaveDataIdx = 0;
    int obstaclesSaveDataIdx = 0;
    int bpmChangesDataIdx = 0;
    int notesSaveDataCount = notesSaveData->get_Count();
    int obstaclesSaveDataCount = obstaclesSaveData->get_Count();
    int waypointsSaveDataCount = waypointsSaveData->get_Count();

    Il2CppClass *obstacleDataClass = classof(CustomObstacleData*);
    const MethodInfo *obstacleDataCtor = il2cpp_utils::FindMethodUnsafe(obstacleDataClass, ".ctor", 5);
    Il2CppClass *noteDataClass = classof(CustomNoteData*);
    const MethodInfo *noteDataCtor = il2cpp_utils::FindMethodUnsafe(noteDataClass, ".ctor", 11);
    Il2CppClass *jsonWrapperClass = classof(JSONWrapper *);

    while (notesSaveDataIdx < notesSaveDataCount || obstaclesSaveDataIdx < obstaclesSaveDataCount) {
        auto noteData = (notesSaveDataIdx < notesSaveDataCount) ? (CustomBeatmapSaveData_NoteData *) notesSaveData->items->values[notesSaveDataIdx] : nullptr;
        auto waypointData = (notesSaveDataIdx < waypointsSaveDataCount) ? (BeatmapSaveData::WaypointData *) waypointsSaveData->items->values[notesSaveDataIdx] : nullptr;
        auto obstacleData = (obstaclesSaveDataIdx < obstaclesSaveDataCount) ? (CustomBeatmapSaveData_ObstacleData *) obstaclesSaveData->items->values[obstaclesSaveDataIdx] : nullptr;
        if (noteData && (!obstacleData || noteData->time <= obstacleData->time)) {
            float time2 = ProcessTime(self, noteData->time, bpmChangesDataIdx, bpmChangesData, shuffle, shufflePeriod);
            ColorType colorType = BeatmapDataLoader::ConvertFromBeatmapSaveDataNoteType(noteData->type);

            CustomNoteData *beatmapObjectData;
            if (colorType == ColorType::None) {
                beatmapObjectData = CustomJSONDataCreateBombNoteData(noteDataClass, noteDataCtor, time2, noteData->lineIndex, noteData->lineLayer);
            } else {
                beatmapObjectData = CustomJSONDataCreateBasicNoteData(noteDataClass, noteDataCtor, time2, noteData->lineIndex, noteData->lineLayer, colorType, noteData->cutDirection);
            }

            if (noteData->customData) {
                JSONWrapper *customData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapper*>());
                customData->value = noteData->customData;
                beatmapObjectData->customData = customData;
            }
            beatmapObjectData->bpm = startBpm;

            beatmapData->AddBeatmapObjectData(beatmapObjectData);
            notesSaveDataIdx++;
        } else if (obstacleData) {
            float obstacleTime = ProcessTime(self, obstacleData->time, bpmChangesDataIdx, bpmChangesData, shuffle, shufflePeriod);

            auto beatmapObjectData2 = (CustomObstacleData *) il2cpp_functions::object_new(obstacleDataClass);
            il2cpp_utils::RunMethod<Il2CppObject*, false>(beatmapObjectData2, obstacleDataCtor, obstacleTime, obstacleData->lineIndex, obstacleData->type, self->GetRealTimeFromBPMTime(obstacleData->duration, startBpm, shuffle, shufflePeriod), obstacleData->width);

            // TODO: Move this into constructor
            if (obstacleData->customData) {
                JSONWrapper *customData = (JSONWrapper *) il2cpp_functions::object_new(jsonWrapperClass);
                customData->value = obstacleData->customData;
                beatmapObjectData2->customData = customData;
            }
            beatmapObjectData2->bpm = startBpm;

            beatmapData->AddBeatmapObjectData(beatmapObjectData2);
            obstaclesSaveDataIdx++;
        }
    }
    for (int i = 0; i < eventsSaveData->get_Count(); i++) {
        auto *eventData = (CustomBeatmapSaveData_EventData *) eventsSaveData->get_Item(i);
        float time = eventData->time;
        while (bpmChangesDataIdx < bpmChangesData->get_Count() - 1 && bpmChangesData->get_Item(bpmChangesDataIdx + 1).bpmChangeStartBpmTime < time) {
            bpmChangesDataIdx++;
        }
        BeatmapDataLoader::BpmChangeData bpmChangeData2 = bpmChangesData->get_Item(bpmChangesDataIdx);
        float realTime = bpmChangeData2.bpmChangeStartTime + GetRealTimeFromBPMTime(self, time - bpmChangeData2.bpmChangeStartBpmTime, bpmChangeData2.bpm, shuffle, shufflePeriod);
        BeatmapEventType type = BeatmapDataLoader::ConvertFromBeatmapSaveDataBeatmapEventType(eventData->type);
        CustomBeatmapEventData *beatmapEventData = CRASH_UNLESS(il2cpp_utils::New<CustomBeatmapEventData*>(time, type, eventData->value));
        beatmapEventData->customData = eventData->customData;
        beatmapData->AddBeatmapEventData(beatmapEventData);
    }
    if (beatmapData->beatmapEventsData->get_Count() == 0) {
        beatmapData->AddBeatmapEventData(BeatmapEventData::New_ctor(0, BeatmapEventType::Event0, 1));
        beatmapData->AddBeatmapEventData(BeatmapEventData::New_ctor(0, BeatmapEventType::Event4, 1));
    }
    beatmapData->ProcessRemainingData();

    CJDLogger::GetLogger().debug("Finished processing beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::GetLogger().debug("This took %ims", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return beatmapData;
}

void CustomJSONData::InstallHooks() {
    // Install hooks
    INSTALL_HOOK_OFFSETLESS(StandardLevelInfoSaveData_DeserializeFromJSONString, il2cpp_utils::FindMethodUnsafe("", "StandardLevelInfoSaveData", "DeserializeFromJSONString", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapSaveData_DeserializeFromJSONString, il2cpp_utils::FindMethodUnsafe("", "BeatmapSaveData", "DeserializeFromJSONString", 1));
    INSTALL_HOOK_OFFSETLESS(GetBeatmapDataFromBeatmapSaveData, il2cpp_utils::FindMethodUnsafe("", "BeatmapDataLoader", "GetBeatmapDataFromBeatmapSaveData", 8));

    static_assert(sizeof(JSONWrapper) == 24);
    // Register custom tpes
    CRASH_UNLESS(custom_types::Register::RegisterType<JSONWrapper>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomLevelInfoSaveData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomBeatmapSaveData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomBeatmapSaveData_NoteData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomBeatmapSaveData_ObstacleData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomBeatmapSaveData_EventData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomBeatmapData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomBeatmapEventData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomObstacleData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomNoteData>());
    CRASH_UNLESS(custom_types::Register::RegisterType<CustomEventData>());
}