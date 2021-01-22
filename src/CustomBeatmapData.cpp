#include "CustomBeatmapData.h"

using namespace GlobalNamespace;

DEFINE_CLASS(CustomJSONData::CustomBeatmapData);

void CustomJSONData::CustomBeatmapData::ctor(int numberOfLines) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe("", "BeatmapData", ".ctor", 1);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, numberOfLines));
    this->customData = nullptr;
}

BeatmapData *CustomJSONData::CustomBeatmapData::GetCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomBeatmapData*>((int) this->beatmapLinesData->Length()));

    this->CopyBeatmapObjects(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);
    this->CopyBeatmapEvents(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);
    this->CopyAvailableSpecialEventsPerKeywordDictionary(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);
    copy->customData = this->customData;
    copy->customEventsData = this->customEventsData;

    return copy;
}

void CustomJSONData::CustomBeatmapData::Finalize() {
    // delete this->customData;
}

DEFINE_CLASS(CustomJSONData::CustomBeatmapEventData);

void CustomJSONData::CustomBeatmapEventData::ctor(float time, BeatmapEventType type, int value) {
    this->time = time;
    this->type = type;
    this->value = value;
}

void CustomJSONData::CustomBeatmapEventData::Finalize() {
    // delete this->customData;
}

DEFINE_CLASS(CustomJSONData::CustomObstacleData);

void CustomJSONData::CustomObstacleData::ctor(float time, int lineIndex, ObstacleType obstacleType, float duration, int width) {
    this->time = time;
    this->lineIndex = lineIndex;
    this->obstacleType = obstacleType;
    this->duration = duration;
    this->width = width;
}

BeatmapObjectData *CustomJSONData::CustomObstacleData::GetCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomObstacleData*>(this->time, this->lineIndex, this->obstacleType, this->duration, this->width));
    copy->customData = this->customData;
    copy->bpm = this->bpm;
    return copy;
}

void CustomJSONData::CustomObstacleData::Finalize() {
    // delete this->customData;
    // CJDLogger::GetLogger().debug("CustomObstacleData::Finalize");
}

DEFINE_CLASS(CustomJSONData::CustomNoteData);

void CustomJSONData::CustomNoteData::ctor(float time, int lineIndex, NoteLineLayer noteLineLayer, NoteLineLayer startNoteLineLayer, ColorType colorType, NoteCutDirection cutDirection, float timeToNextColorNote, float timeToPrevColorNote, int flipLineIndex, float flipYSide, float duration) {
    this->time = time;
    this->lineIndex = lineIndex;
    this->colorType = colorType;
    this->cutDirection = cutDirection;
    this->timeToNextColorNote = timeToNextColorNote;
    this->timeToPrevColorNote = timeToPrevColorNote;
    this->noteLineLayer = noteLineLayer;
    this->startNoteLineLayer = startNoteLineLayer;
    this->flipLineIndex = flipLineIndex;
    this->flipYSide = flipYSide;
    this->duration = duration;
    this->customData = nullptr;
}

BeatmapObjectData *CustomJSONData::CustomNoteData::GetCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomNoteData*>(this->time, this->lineIndex, this->noteLineLayer, this->startNoteLineLayer, 
        this->colorType, this->cutDirection, this->timeToNextColorNote, this->timeToPrevColorNote, this->flipLineIndex, this->flipYSide, this->duration));
    copy->customData = this->customData;
    copy->bpm = this->bpm;
    return copy;
}

void CustomJSONData::CustomNoteData::Finalize() {
    // delete this->customData;
    // CJDLogger::GetLogger().debug("CustomNoteData::Finalize");
}

DEFINE_CLASS(CustomJSONData::CustomWaypointData);

void CustomJSONData::CustomWaypointData::ctor(float time, int lineIndex, GlobalNamespace::NoteLineLayer noteLineLayer, GlobalNamespace::OffsetDirection offsetDirection) {
    this->time = time;
    this->lineIndex = lineIndex;
    this->noteLineLayer = noteLineLayer;
    this->offsetDirection = offsetDirection;
}

BeatmapObjectData *CustomJSONData::CustomWaypointData::GetCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomWaypointData*>(this->time, this->lineIndex, this->noteLineLayer, this->offsetDirection));
    copy->bpm = this->bpm;
    return copy;
}

void CustomJSONData::CustomWaypointData::Finalize() {
    // CJDLogger::GetLogger().debug("CustomWaypointData::Finalize");
}
