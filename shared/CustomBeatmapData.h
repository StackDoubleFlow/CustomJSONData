#pragma once

#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/ObstacleData.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/WaypointData.hpp"
#include "System/Object.hpp"

#include "CustomEventData.h"
#include "JSONWrapper.h"
#include "CJDLogger.h"

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapData, GlobalNamespace::BeatmapData,
    DECLARE_CTOR(ctor, int numberOfLines);

    DECLARE_OVERRIDE_METHOD(BeatmapData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapData", "GetCopy"));
    DECLARE_OVERRIDE_METHOD(BeatmapData*, GetCopyWithoutBeatmapObjects, il2cpp_utils::FindMethod("", "BeatmapData", "GetCopyWithoutBeatmapObjects"));
    DECLARE_OVERRIDE_METHOD(BeatmapData*, GetCopyWithoutEvents, il2cpp_utils::FindMethod("", "BeatmapData", "GetCopyWithoutEvents"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    DECLARE_INSTANCE_FIELD(CustomJSONData::DocumentWrapper*, doc);
    // TODO: Clean this up in a Finalize method
    // std::vector<CustomJSONData::CustomEventData>
    DECLARE_INSTANCE_FIELD(void*, customEventsData);
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapEventData, GlobalNamespace::BeatmapEventData,
    DECLARE_CTOR(ctor, float time, GlobalNamespace::BeatmapEventType type, int value);

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
)


DECLARE_CLASS_CODEGEN(CustomJSONData, CustomObstacleData, GlobalNamespace::ObstacleData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::ObstacleType obstacleType, float duration, int width);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapObjectData", "GetCopy"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomNoteData, GlobalNamespace::NoteData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::NoteLineLayer noteLineLayer, GlobalNamespace::NoteLineLayer startNoteLineLayer, GlobalNamespace::ColorType colorType, GlobalNamespace::NoteCutDirection cutDirection, float timeToNextColorNote, float timeToPrevColorNote, int flipLineIndex, float flipYSide, float duration);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapObjectData", "GetCopy"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomWaypointData, GlobalNamespace::WaypointData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::NoteLineLayer noteLineLayer, GlobalNamespace::OffsetDirection offsetDirection);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapObjectData", "GetCopy"));

    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);
)
