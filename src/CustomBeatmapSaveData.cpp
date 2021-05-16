#include "CustomBeatmapSaveData.h"

using namespace GlobalNamespace;

void CustomJSONData::CustomBeatmapSaveData::ctor(List<BeatmapSaveData::EventData*>* events, 
                                                 List<BeatmapSaveData::NoteData*>* notes, 
                                                 List<BeatmapSaveData::WaypointData*>* waypoints, 
                                                 List<BeatmapSaveData::ObstacleData*>* obstacles,
                                                 BeatmapSaveData::SpecialEventKeywordFiltersData *specialEventsKeywordFilters) {
    INVOKE_CTOR();
    this->events = events;
    this->notes = notes;
    this->waypoints = waypoints;
    this->obstacles = obstacles;
    this->specialEventsKeywordFilters = specialEventsKeywordFilters;
}

DEFINE_TYPE(CustomJSONData::CustomBeatmapSaveData);

void CustomJSONData::CustomBeatmapSaveData_NoteData::ctor(float time, int lineIndex, NoteLineLayer lineLayer, BeatmapSaveData::NoteType type, NoteCutDirection cutDirection) {
    INVOKE_CTOR();
    this->time = time;
    this->lineIndex = lineIndex;
    this->lineLayer = lineLayer;
    this->type = type;
    this->cutDirection = cutDirection;
}

DEFINE_TYPE(CustomJSONData::CustomBeatmapSaveData_NoteData);

void CustomJSONData::CustomBeatmapSaveData_ObstacleData::ctor(float time, int lineIndex, ObstacleType type, float duration, int width) {
    INVOKE_CTOR();
    this->time = time;
    this->lineIndex = lineIndex;
    this->type = type;
    this->duration = duration;
    this->width = width;
}

DEFINE_TYPE(CustomJSONData::CustomBeatmapSaveData_ObstacleData);

void CustomJSONData::CustomBeatmapSaveData_EventData::ctor(float time, BeatmapSaveData::BeatmapEventType type, int value) {
    INVOKE_CTOR();
    this->time = time;
    this->type = type;
    this->value = value;
}

DEFINE_TYPE(CustomJSONData::CustomBeatmapSaveData_EventData);