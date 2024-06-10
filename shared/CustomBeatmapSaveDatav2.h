#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/SpecialEventKeywordFiltersData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/SpecialEventsForKeyword.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/NoteData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/SliderData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/EventData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/ObstacleData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/WaypointData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/ObstacleType.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/ColorType.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/NoteType.hpp"

#include "BeatmapSaveDataCommon/BeatmapEventType.hpp"
#include "BeatmapSaveDataCommon/NoteCutDirection.hpp"
#include "BeatmapSaveDataCommon/SliderMidAnchorMode.hpp"
#include "BeatmapSaveDataCommon/NoteLineLayer.hpp"
#include "BeatmapSaveDataCommon/OffsetDirection.hpp"
#include "BeatmapSaveDataCommon/Axis.hpp"
#include "BeatmapSaveDataCommon/BasicEventTypesWithKeywords.hpp"
#include "BeatmapSaveDataCommon/ExecutionTime.hpp"
#include "BeatmapSaveDataCommon/BasicEventTypesWithKeywords.hpp"

#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"

#include "JSONWrapper.h"

#include "LowLevelUtils.hpp"
#include "CustomEventData.h"
#include "CJDLogger.h"

// clang-format off

namespace CustomJSONData::v2 {
using CustomDataOpt = std::optional<std::reference_wrapper<rapidjson::Value const>>;
using CustomDataOptUTF16 = std::optional<std::reference_wrapper<SongCore::CustomJSONData::ValueUTF16 const>>;

} // namespace CustomJSONData::v3
DECLARE_CLASS_CODEGEN(CustomJSONData::v2, CustomBeatmapSaveData, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData,
    DECLARE_FASTER_CTOR(
        ctor,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::EventData*>*
            events,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::NoteData*>* notes,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::SliderData*>*
            sliders,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::WaypointData*>*
            waypoints,  
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleData*>*
            obstacles,
        BeatmapSaveDataVersion2_6_0AndEarlier::SpecialEventKeywordFiltersData* specialEventsKeywordFilters);

    DECLARE_SIMPLE_DTOR();

    public:
        static CustomBeatmapSaveData * Deserialize(std::shared_ptr<rapidjson::Document> const& sharedDoc);

    std::shared_ptr<std::vector<CustomJSONData::CustomEventSaveData>> customEventsData;
    std::shared_ptr<rapidjson::Document> doc; 
    CustomDataOpt customData;
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v2, CustomBeatmapSaveData_NoteData, BeatmapSaveDataVersion2_6_0AndEarlier::NoteData,
    DECLARE_FASTER_CTOR(ctor, float time, int lineIndex, BeatmapSaveDataCommon::NoteLineLayer lineLayer,
                        BeatmapSaveDataVersion2_6_0AndEarlier::NoteType type,
                        BeatmapSaveDataCommon::NoteCutDirection cutDirection);

    DECLARE_SIMPLE_DTOR();

                            public:
                CustomDataOpt customData;
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v2, CustomBeatmapSaveData_SliderData, BeatmapSaveDataVersion2_6_0AndEarlier::SliderData,
    DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataVersion2_6_0AndEarlier::ColorType colorType,
                        float headTime, int headLineIndex, BeatmapSaveDataCommon::NoteLineLayer headLineLayer,
                        float headControlPointLengthMultiplier,
                        BeatmapSaveDataCommon::NoteCutDirection headCutDirection, float tailTime, int tailLineIndex,
                        BeatmapSaveDataCommon::NoteLineLayer tailLineLayer, float tailControlPointLengthMultiplier,
                        BeatmapSaveDataCommon::NoteCutDirection tailCutDirection,
                        BeatmapSaveDataCommon::SliderMidAnchorMode sliderMidAnchorMode);

            DECLARE_SIMPLE_DTOR();

            public:
                CustomDataOpt customData;
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v2, CustomBeatmapSaveData_ObstacleData, BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleData,
    DECLARE_FASTER_CTOR(ctor, float time, int lineIndex,
                        BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleType type,
                        float duration, int width);

                DECLARE_SIMPLE_DTOR();

                public:
        CustomDataOpt customData;
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v2, CustomBeatmapSaveData_EventData, BeatmapSaveDataVersion2_6_0AndEarlier::EventData,
    DECLARE_FASTER_CTOR(ctor, float time,
                        BeatmapSaveDataCommon::BeatmapEventType type,
                        int value, float floatValue);

    DECLARE_SIMPLE_DTOR();

                    public:
        CustomDataOpt customData;
)

// clang-format on