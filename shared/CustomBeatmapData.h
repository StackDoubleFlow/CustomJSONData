#pragma once

#include "custom-types/shared/macros.hpp"

#include "System/Func_2.hpp"


#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/ObstacleData.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/WaypointData.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "System/Object.hpp"

#include "CustomEventData.h"
#include "JSONWrapper.h"
#include "CJDLogger.h"

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapData, GlobalNamespace::BeatmapData,
    DECLARE_CTOR(ctor, int numberOfLines);
    DECLARE_SIMPLE_DTOR();

    DECLARE_OVERRIDE_METHOD(BeatmapData*, GetFilteredCopy, il2cpp_utils::FindMethod("", "BeatmapData", "GetFilteredCopy"), System::Func_2<::GlobalNamespace::BeatmapDataItem*, ::GlobalNamespace::BeatmapDataItem*>* processDataItem);
    DECLARE_OVERRIDE_METHOD(BeatmapData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapData", "GetCopy"));

    DECLARE_INSTANCE_METHOD(CustomBeatmapData*, BaseCopy);

public:
    static System::Type* GetCustomType(Il2CppObject* obj);

    void InsertCustomEventData(CustomEventData* customEventData);

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper*, customData);
    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapperUTF16*, beatmapCustomData);
    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapperUTF16*, levelCustomData);
    DECLARE_INSTANCE_FIELD(CustomJSONData::DocumentWrapper*, doc);
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapEventData, GlobalNamespace::BasicBeatmapEventData,
    DECLARE_CTOR(ctor, float time, ::GlobalNamespace::BasicBeatmapEventType basicBeatmapEventType, int value, float floatValue);

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
)


DECLARE_CLASS_CODEGEN(CustomJSONData, CustomObstacleData, GlobalNamespace::ObstacleData,
    DECLARE_CTOR(ctor, float time, int lineIndex, ::GlobalNamespace::NoteLineLayer lineLayer, float duration, int width, int height);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomNoteData, GlobalNamespace::NoteData,
    DECLARE_CTOR(ctor, float time, int lineIndex, ::GlobalNamespace::NoteLineLayer noteLineLayer, ::GlobalNamespace::NoteLineLayer beforeJumpNoteLineLayer, ::GlobalNamespace::NoteData::GameplayType gameplayType, ::GlobalNamespace::NoteData::ScoringType scoringType, ::GlobalNamespace::ColorType colorType, ::GlobalNamespace::NoteCutDirection cutDirection, float timeToNextColorNote, float timeToPrevColorNote, int flipLineIndex, float flipYSide, float cutDirectionAngleOffset, float cutSfxVolumeMultiplier);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomSliderData, GlobalNamespace::SliderData,
                      DECLARE_CTOR(ctor, GlobalNamespace::SliderData::Type sliderType, ::GlobalNamespace::ColorType colorType,
                                   bool hasHeadNote, float headTime, int headLineIndex, ::GlobalNamespace::NoteLineLayer headLineLayer,
                                   ::GlobalNamespace::NoteLineLayer headBeforeJumpLineLayer, float headControlPointLengthMultiplier,
                                   ::GlobalNamespace::NoteCutDirection headCutDirection, float headCutDirectionAngleOffset,
                                   bool hasTailNote, float tailTime, int tailLineIndex, ::GlobalNamespace::NoteLineLayer tailLineLayer,
                                   ::GlobalNamespace::NoteLineLayer tailBeforeJumpLineLayer, float tailControlPointLengthMultiplier,
                                   ::GlobalNamespace::NoteCutDirection tailCutDirection, float tailCutDirectionAngleOffset,
                                   ::GlobalNamespace::SliderMidAnchorMode midAnchorMode, int sliceCount, float squishAmount);

                      DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

                      DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper *, customData);
                      // Used for Noodle Extensions
                      DECLARE_INSTANCE_FIELD(float, bpm);
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomWaypointData, GlobalNamespace::WaypointData,
    DECLARE_CTOR(ctor, float time, int lineIndex, ::GlobalNamespace::NoteLineLayer lineLayer, ::GlobalNamespace::OffsetDirection offsetDirection);

    DECLARE_OVERRIDE_METHOD(BeatmapObjectData *, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);
)
