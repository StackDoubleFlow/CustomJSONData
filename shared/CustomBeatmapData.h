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

    DECLARE_OVERRIDE_METHOD(BeatmapData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapData", "GetCopy"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    // std::vector<CustomJSONData::CustomEventData>
    DECLARE_INSTANCE_FIELD(void *, customEventsData);

    REGISTER_FUNCTION(CustomBeatmapData,
        CJDLogger::GetLogger().debug("Registering CustomBeatmapData!");

        // Register constructor
        REGISTER_METHOD(ctor);

        // Register methods
        REGISTER_METHOD(GetCopy);

        // Register fields
        REGISTER_FIELD(customData);
        REGISTER_FIELD(customEventsData);
    )
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapEventData, GlobalNamespace::BeatmapEventData,
    DECLARE_CTOR(ctor, float time, GlobalNamespace::BeatmapEventType type, int value);

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);

    REGISTER_FUNCTION(CustomBeatmapEventData,
        CJDLogger::GetLogger().debug("Registering CustomBeatmapEventData!");

        // Register constructor
        REGISTER_METHOD(ctor);
        
        // Register fields
        REGISTER_FIELD(customData);
    )
)


DECLARE_CLASS_CODEGEN(CustomJSONData, CustomObstacleData, GlobalNamespace::ObstacleData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::ObstacleType obstacleType, float duration, int width);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapObjectData", "GetCopy"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);

    REGISTER_FUNCTION(CustomObstacleData,
        CJDLogger::GetLogger().debug("Registering CustomObstacleData!");

        // Register constructor
        REGISTER_METHOD(ctor);

        // Register methods
        REGISTER_METHOD(GetCopy);

        // Register fields
        REGISTER_FIELD(customData);
        REGISTER_FIELD(bpm);
    )
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomNoteData, GlobalNamespace::NoteData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::NoteLineLayer noteLineLayer, GlobalNamespace::NoteLineLayer startNoteLineLayer, GlobalNamespace::ColorType colorType, GlobalNamespace::NoteCutDirection cutDirection, float timeToNextColorNote, float timeToPrevColorNote, int flipLineIndex, float flipYSide, float duration);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapObjectData", "GetCopy"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);

    REGISTER_FUNCTION(CustomNoteData,
        CJDLogger::GetLogger().debug("Registering CustomNoteData!");

        // Register constructor
        REGISTER_METHOD(ctor);

        // Register methods
        REGISTER_METHOD(GetCopy);

        // Register fields
        REGISTER_FIELD(customData);
        REGISTER_FIELD(bpm);
    )
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomWaypointData, GlobalNamespace::WaypointData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::NoteLineLayer noteLineLayer, GlobalNamespace::OffsetDirection offsetDirection);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapObjectData", "GetCopy"));

    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);

    REGISTER_FUNCTION(CustomNoteData,
        CJDLogger::GetLogger().debug("Registering CustomWaypointData!");

        // Register constructor
        REGISTER_METHOD(ctor);

        // Register methods
        REGISTER_METHOD(GetCopy);

        // Register fields
        REGISTER_FIELD(bpm);
    )
)
