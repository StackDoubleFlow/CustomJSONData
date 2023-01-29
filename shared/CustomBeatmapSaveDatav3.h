#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_ColorNoteData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_SliderData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BurstSliderData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_ObstacleData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_WaypointData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BpmChangeEventData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BombNoteData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BasicEventData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_WaypointData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_ColorBoostEventData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightColorEventBoxGroup.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightRotationEventBoxGroup.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightTranslationEventBox.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightTranslationEventBoxGroup.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightTranslationBaseData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BasicEventTypesWithKeywords.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BasicEventTypesWithKeywords_BasicEventTypesForKeyword.hpp"

#include "BeatmapSaveDataVersion3/BeatmapSaveData_ExecutionTime.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_RotationEventData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_IndexFilter.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_EventBox.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_SliderType.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_TransitionType.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightColorBaseData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_EnvironmentColorType.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightColorEventBox.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightRotationBaseData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_LightRotationEventBox.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_Axis.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BasicEventTypesWithKeywords.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_BasicEventTypesWithKeywords_BasicEventTypesForKeyword.hpp"

#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"

#include "LowLevelUtils.hpp"
#include "CustomEventData.h"
#include "CJDLogger.h"
#include "JSONWrapper.h"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/CustomDifficultyBeatmap.hpp"


namespace CustomJSONData::v2 {
    class CustomBeatmapSaveData;
}


namespace CustomJSONData::v3::Constants {
    // worst naming scheme ever
    static inline const auto beat = "b";
    static inline const auto colorType = "c";
    static inline const auto line = "x";
    static inline const auto layer = "y";
    static inline const auto cutDirection = "d";
    static inline const auto tailBeat = "tb";
    static inline const auto tailLine = "tx";
    static inline const auto tailLayer = "ty";
    static inline const auto eventBoxes = "e";
    static inline const auto groupId = "g";
    static inline const auto indexFilter = "f";
    static inline const auto beatDistributionParam = "w";
    static inline const auto beatDistributionParamType = "d";

    static inline const auto customData = "customData";
}

namespace CustomJSONData::v3 {
    using CustomDataOpt = std::optional<std::reference_wrapper<rapidjson::Value const>>;
    using CustomDataOptUTF16 = std::optional<std::reference_wrapper<ValueUTF16 const>>;

}

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData, BeatmapSaveDataVersion3::BeatmapSaveData,
    DECLARE_FASTER_CTOR(ctor,
                 System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData*>* bpmEvents,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::RotationEventData*>* rotationEvents,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorNoteData*>* colorNotes,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BombNoteData*>* bombNotes,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData*>* obstacles,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::SliderData*>* sliders,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BurstSliderData*>* burstSliders,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::WaypointData*>* waypoints,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData*>* basicBeatmapEvents,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorBoostEventData*>* colorBoostBeatmapEvents,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorEventBoxGroup*>* lightColorEventBoxGroups,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationEventBoxGroup*>* lightRotationEventBoxGroups,
                 ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightTranslationEventBoxGroup*>* lightTranslationEventBoxGroups,
                 ::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords* basicEventTypesWithKeywords,
                 bool useNormalEventsAsCompatibleEvents);

    DECLARE_SIMPLE_DTOR();

public:
    static CustomBeatmapSaveData* Deserialize(std::shared_ptr<rapidjson::Document> sharedDoc);

    static CustomBeatmapSaveData* Convert2_6_0(CustomJSONData::v2::CustomBeatmapSaveData const* beatmap);

    std::shared_ptr<std::vector<CustomJSONData::CustomEventSaveData>> customEventsData;
    std::shared_ptr<rapidjson::Document> doc;
    CustomDataOpt customData;
    CustomDataOptUTF16 beatmapCustomData;
    CustomDataOptUTF16 levelCustomData;

    DECLARE_INSTANCE_FIELD(bool, isV2);
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_ColorNoteData, BeatmapSaveDataVersion3::BeatmapSaveData::ColorNoteData,
                      DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer, ::BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType color, ::GlobalNamespace::NoteCutDirection cutDirection, int angleOffset);
                      DECLARE_SIMPLE_DTOR();

      public:
      DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BombNoteData, BeatmapSaveDataVersion3::BeatmapSaveData::BombNoteData,
      DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer);
      DECLARE_SIMPLE_DTOR();

      public:
      DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_SliderData, BeatmapSaveDataVersion3::BeatmapSaveData::SliderData,
                      DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType, float headBeat, int headLine, int headLayer, float headControlPointLengthMultiplier, ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat, int tailLine, int tailLayer, float tailControlPointLengthMultiplier, ::GlobalNamespace::NoteCutDirection tailCutDirection, ::GlobalNamespace::SliderMidAnchorMode sliderMidAnchorMode);
                      DECLARE_SIMPLE_DTOR();
      public:
      DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BurstSliderData, BeatmapSaveDataVersion3::BeatmapSaveData::BurstSliderData,
                      DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType, float headBeat, int headLine, int headLayer, ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat, int tailLine, int tailLayer, int sliceCount, float squishAmount);
                      DECLARE_SIMPLE_DTOR();
      public:
      DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_ObstacleData, BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData,
      DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer, float duration, int width, int height);


DECLARE_SIMPLE_DTOR();
      public:
      DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);
)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BasicEventData, BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData,
  DECLARE_FASTER_CTOR(ctor, float beat, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType eventType, int value, float floatValue);

  DECLARE_SIMPLE_DTOR();

    public:
    CustomDataOpt customData;
)

namespace CustomJSONData::v3::Parser {
    CustomBeatmapSaveData_BurstSliderData *DeserializeBurstSlider(rapidjson::Value const &val);
    CustomBeatmapSaveData_SliderData *DeserializeSlider(rapidjson::Value const &val);

    CustomBeatmapSaveData_ObstacleData *DeserializeObstacle(rapidjson::Value const &val);

    CustomBeatmapSaveData_BombNoteData *DeserializeBombNote(rapidjson::Value const &val);
    CustomBeatmapSaveData_ColorNoteData *DeserializeColorNote(rapidjson::Value const &val);

    extern UnorderedEventCallback<v3::CustomBeatmapSaveData*> ParsedEvent;
}

namespace CustomJSONData {
    inline static std::optional<v3::CustomBeatmapSaveData*> GetBeatmapSaveData(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap)
    {
        auto customDifficultyBeatmap = il2cpp_utils::try_cast<GlobalNamespace::CustomDifficultyBeatmap>(difficultyBeatmap);
        if (!customDifficultyBeatmap) return std::nullopt;

        return il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(customDifficultyBeatmap.value()->beatmapSaveData);
    }
}