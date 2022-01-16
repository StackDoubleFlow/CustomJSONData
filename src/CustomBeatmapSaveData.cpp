#include "CustomBeatmapSaveData.h"

using namespace GlobalNamespace;

void CustomJSONData::CustomBeatmapSaveData::ctor(List<BeatmapSaveData::EventData*>* events, 
                                                 List<BeatmapSaveData::NoteData*>* notes, 
                                                 List<BeatmapSaveData::WaypointData*>* waypoints, 
                                                 List<BeatmapSaveData::ObstacleData*>* obstacles,
                                                 BeatmapSaveData::SpecialEventKeywordFiltersData *specialEventsKeywordFilters) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData*), ".ctor", 5);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, events, notes, waypoints, obstacles, specialEventsKeywordFilters));
    INVOKE_CTOR();
//    this->events = events;
//    this->notes = notes;
//    this->waypoints = waypoints;
//    this->obstacles = obstacles;
//    this->specialEventsKeywordFilters = specialEventsKeywordFilters;
}

DEFINE_TYPE(CustomJSONData, CustomBeatmapSaveData);

void CustomJSONData::CustomBeatmapSaveData_NoteData::ctor(float time, int lineIndex, NoteLineLayer lineLayer, BeatmapSaveData::NoteType type, NoteCutDirection cutDirection) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::NoteData*), ".ctor", 5);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, time, lineIndex, lineLayer, type, cutDirection));
    INVOKE_CTOR();
//    this->time = time;
//    this->lineIndex = lineIndex;
//    this->lineLayer = lineLayer;
//    this->type = type;
//    this->cutDirection = cutDirection;
}

DEFINE_TYPE(CustomJSONData, CustomBeatmapSaveData_NoteData);

void CustomJSONData::CustomBeatmapSaveData_ObstacleData::ctor(float time, int lineIndex, ObstacleType type, float duration, int width) {
    INVOKE_CTOR();
    this->time = time;
    this->lineIndex = lineIndex;
    this->type = type;
    this->duration = duration;
    this->width = width;
}

DEFINE_TYPE(CustomJSONData, CustomBeatmapSaveData_ObstacleData);

void CustomJSONData::CustomBeatmapSaveData_EventData::ctor(float time, BeatmapSaveData::BeatmapEventType type, int value, float floatValue) {
    INVOKE_CTOR();
    this->time = time;
    this->type = type;
    this->value = value;
    this->floatValue = floatValue;
}

DEFINE_TYPE(CustomJSONData, CustomBeatmapSaveData_EventData);