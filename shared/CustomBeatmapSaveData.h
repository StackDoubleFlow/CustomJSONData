#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "GlobalNamespace/BeatmapSaveData.hpp"
#include "GlobalNamespace/BeatmapSaveData_NoteData.hpp"
#include "GlobalNamespace/BeatmapSaveData_ObstacleData.hpp"
#include "GlobalNamespace/BeatmapSaveData_EventData.hpp"
#include "GlobalNamespace/BeatmapSaveData_WaypointData.hpp"
#include "GlobalNamespace/BeatmapSaveData_SpecialEventKeywordFiltersData.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"

#include "CustomEventData.h"
#include "CJDLogger.h"

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapSaveData, GlobalNamespace::BeatmapSaveData,
    DECLARE_CTOR(ctor, 
                 System::Collections::Generic::List_1<GlobalNamespace::BeatmapSaveData::EventData*> *events, 
                 System::Collections::Generic::List_1<GlobalNamespace::BeatmapSaveData::NoteData*> *notes, 
                 System::Collections::Generic::List_1<GlobalNamespace::BeatmapSaveData::WaypointData*> *waypoints, 
                 System::Collections::Generic::List_1<GlobalNamespace::BeatmapSaveData::ObstacleData*> *obstacles,
                 BeatmapSaveData::SpecialEventKeywordFiltersData *specialEventsKeywordFilters);

    DECLARE_SIMPLE_DTOR();

public:
    std::shared_ptr<std::vector<CustomJSONData::CustomEventData>> customEventsData;
    std::shared_ptr<rapidjson::Document> doc;
    std::optional<std::reference_wrapper<rapidjson::Value>> customData;
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapSaveData_NoteData, GlobalNamespace::BeatmapSaveData::NoteData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::NoteLineLayer lineLayer, GlobalNamespace::BeatmapSaveData::NoteType type, GlobalNamespace::NoteCutDirection cutDirection);

public:
    std::optional<std::reference_wrapper<rapidjson::Value>> customData;
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapSaveData_ObstacleData, GlobalNamespace::BeatmapSaveData::ObstacleData,
    DECLARE_CTOR(ctor, float time, int lineIndex, GlobalNamespace::ObstacleType type, float duration, int width);

public:
    std::optional<std::reference_wrapper<rapidjson::Value>> customData;
)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapSaveData_EventData, GlobalNamespace::BeatmapSaveData::EventData,
    DECLARE_CTOR(ctor, float time, GlobalNamespace::BeatmapSaveData::BeatmapEventType type, int value, float floatValue);

public:
    std::optional<std::reference_wrapper<rapidjson::Value>> customData;
)

