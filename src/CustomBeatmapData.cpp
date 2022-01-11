#include "CustomBeatmapData.h"

using namespace GlobalNamespace;

DEFINE_TYPE(CustomJSONData, CustomBeatmapData);

void CustomJSONData::CustomBeatmapData::ctor(int numberOfLines) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe("", "BeatmapData", ".ctor", 1);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, numberOfLines));
    INVOKE_CTOR();
    this->prevAddedBeatmapObjectDataTime = -std::numeric_limits<float>::infinity();
}


CustomJSONData::CustomBeatmapData *CustomJSONData::CustomBeatmapData::BaseCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomBeatmapData*>((int) this->beatmapLinesData.Length()));

    copy->customEventsData = customEventsData;

    // copy the rest
    copy->doc = this->doc;
    copy->customData = customData;
    copy->beatmapCustomData = beatmapCustomData;
    copy->levelCustomData = levelCustomData;

    return copy;
}

BeatmapData *CustomJSONData::CustomBeatmapData::GetCopy() {
    auto copy = BaseCopy();

    BeatmapData::CopyBeatmapObjects(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);
    BeatmapData::CopyBeatmapEvents(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);
    BeatmapData::CopyAvailableSpecialEventsPerKeywordDictionary(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);

    return copy;
}




BeatmapData *CustomJSONData::CustomBeatmapData::GetCopyWithoutBeatmapObjects() {
    auto copy = BaseCopy();

    BeatmapData::CopyBeatmapEvents(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);
    BeatmapData::CopyAvailableSpecialEventsPerKeywordDictionary(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);

    return copy;
}

BeatmapData *CustomJSONData::CustomBeatmapData::GetCopyWithoutEvents() {
    auto copy = BaseCopy();

    BeatmapData::CopyBeatmapObjects(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);
    BeatmapData::CopyAvailableSpecialEventsPerKeywordDictionary(reinterpret_cast<IReadonlyBeatmapData*>(this), copy);

    return copy;
}

DEFINE_TYPE(CustomJSONData, CustomBeatmapEventData);

void CustomJSONData::CustomBeatmapEventData::ctor(float time, BeatmapEventType type, int value, float floatValue) {
    static auto BeatmapEventData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(BeatmapEventData*), ".ctor", 3));
    BeatmapEventData* instance = this;
    il2cpp_utils::RunMethodRethrow<void, false>(instance, BeatmapEventData_Ctor, time, type, value, floatValue);
    INVOKE_CTOR();
}

DEFINE_TYPE(CustomJSONData, CustomObstacleData);

void CustomJSONData::CustomObstacleData::ctor(float time, int lineIndex, ObstacleType obstacleType, float duration, int width) {
    static auto NoteData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(ObstacleData*), ".ctor", 5));
    ObstacleData* instance = this;
    il2cpp_utils::RunMethodThrow<void, false>(instance, NoteData_Ctor, time, lineIndex, obstacleType, duration, width);
    INVOKE_CTOR();
}

BeatmapObjectData *CustomJSONData::CustomObstacleData::GetCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomObstacleData*>(this->time, this->lineIndex, this->obstacleType, this->duration, this->width));
    copy->customData = this->customData;
    copy->bpm = this->bpm;
    return copy;
}

DEFINE_TYPE(CustomJSONData, CustomNoteData);

void CustomJSONData::CustomNoteData::ctor(float time, int lineIndex, NoteLineLayer noteLineLayer, NoteLineLayer beforeJumpNoteLineLayer, ColorType colorType, NoteCutDirection cutDirection, float timeToNextColorNote, float timeToPrevColorNote, int flipLineIndex, float flipYSide, float duration) {
    static auto NoteData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(NoteData*), ".ctor", 13));
    NoteData* instance = this;
    il2cpp_utils::RunMethodRethrow<void, false>(instance, NoteData_Ctor, time, lineIndex, noteLineLayer, beforeJumpNoteLineLayer, colorType, cutDirection, timeToNextColorNote, timeToPrevColorNote, flipLineIndex, flipYSide, duration, false, false);
    INVOKE_CTOR();
}

BeatmapObjectData *CustomJSONData::CustomNoteData::GetCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomNoteData*>(
            this->time, this->lineIndex, this->noteLineLayer, this->beforeJumpNoteLineLayer,
            this->colorType, this->cutDirection, this->timeToNextColorNote,
            this->timeToPrevColorNote, this->flipLineIndex, this->flipYSide,
            this->duration));
    copy->customData = this->customData;
    copy->bpm = this->bpm;
    copy->skipBeforeCutScoring = this->skipBeforeCutScoring;
    copy->skipAfterCutScoring = this->skipAfterCutScoring;
    return copy;
}

DEFINE_TYPE(CustomJSONData, CustomWaypointData);

void CustomJSONData::CustomWaypointData::ctor(float time, int lineIndex, GlobalNamespace::NoteLineLayer noteLineLayer, GlobalNamespace::OffsetDirection offsetDirection) {
    static auto NoteData_Ctor = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(WaypointData*), ".ctor", 4));
    WaypointData* instance = this;
    il2cpp_utils::RunMethodRethrow<void, false>(instance, NoteData_Ctor, time, lineIndex, noteLineLayer, offsetDirection);
    INVOKE_CTOR();
}

BeatmapObjectData *CustomJSONData::CustomWaypointData::GetCopy() {
    auto copy = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomWaypointData*>(this->time, this->lineIndex, this->noteLineLayer, this->offsetDirection));
    copy->bpm = this->bpm;
    return copy;
}