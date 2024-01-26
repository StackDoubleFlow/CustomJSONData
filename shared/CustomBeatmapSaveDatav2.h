#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveData.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"

#include "LowLevelUtils.hpp"
#include "CustomEventData.h"
#include "CJDLogger.h"

DECLARE_CLASS_CODEGEN&(
    CustomJSONData::v2, CustomBeatmapSaveData, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData,
    DECLARE_FASTER_CTOR(
        ctor,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::EventData*>*
            events,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteData*>* notes,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SliderData*>*
            sliders,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::WaypointData*>*
            waypoints,
        System::Collections::Generic::List_1<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleData*>*
            obstacles,
        BeatmapSaveData::SpecialEventKeywordFiltersData* specialEventsKeywordFilters);

    DECLARE_SIMPLE_DTOR();

    public
    : static CustomBeatmapSaveData * Deserialize(std::shared_ptr<rapidjson::Document> sharedDoc);

    std::shared_ptr<std::vector<CustomJSONData::CustomEventSaveData>> customEventsData;
    std::shared_ptr<rapidjson::Document> doc; std::optional<std::reference_wrapper<const rapidjson::Value>> customData;)

    DECLARE_CLASS_CODEGEN(CustomJSONData::v2, CustomBeatmapSaveData_NoteData,
                          BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteData,
                          DECLARE_FASTER_CTOR(ctor, float time, int lineIndex, GlobalNamespace::NoteLineLayer lineLayer,
                                              BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteType type,
                                              GlobalNamespace::NoteCutDirection cutDirection);

                          DECLARE_SIMPLE_DTOR();

                          public
                          : std::optional<std::reference_wrapper<const rapidjson::Value>>
                              customData;)

        DECLARE_CLASS_CODEGEN(
            CustomJSONData::v2, CustomBeatmapSaveData_SliderData,
            BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SliderData,
            DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ColorType colorType,
                                float headTime, int headLineIndex, ::GlobalNamespace::NoteLineLayer headLineLayer,
                                float headControlPointLengthMultiplier,
                                ::GlobalNamespace::NoteCutDirection headCutDirection, float tailTime, int tailLineIndex,
                                ::GlobalNamespace::NoteLineLayer tailLineLayer, float tailControlPointLengthMultiplier,
                                ::GlobalNamespace::NoteCutDirection tailCutDirection,
                                ::GlobalNamespace::SliderMidAnchorMode sliderMidAnchorMode);

            DECLARE_SIMPLE_DTOR();

            public
            : std::optional<std::reference_wrapper<const rapidjson::Value>>
                customData;)

            DECLARE_CLASS_CODEGEN(
                CustomJSONData::v2, CustomBeatmapSaveData_ObstacleData,
                BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleData,
                DECLARE_FASTER_CTOR(ctor, float time, int lineIndex,
                                    BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType type,
                                    float duration, int width);

                DECLARE_SIMPLE_DTOR();

                public
                : std::optional<std::reference_wrapper<const rapidjson::Value>>
                    customData;)

                DECLARE_CLASS_CODEGEN(
                    CustomJSONData::v2, CustomBeatmapSaveData_EventData,
                    BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::EventData,
                    DECLARE_FASTER_CTOR(ctor, float time,
                                        BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType type,
                                        int value, float floatValue);

                    DECLARE_SIMPLE_DTOR();

                    public
                    : std::optional<std::reference_wrapper<const rapidjson::Value>>
                        customData;)
