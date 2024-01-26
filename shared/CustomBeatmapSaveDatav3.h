#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData.hpp"
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
static inline auto const beat = "b";
static inline auto const colorType = "c";
static inline auto const line = "x";
static inline auto const layer = "y";
static inline auto const cutDirection = "d";
static inline auto const tailBeat = "tb";
static inline auto const tailLine = "tx";
static inline auto const tailLayer = "ty";
static inline auto const eventBoxes = "e";
static inline auto const groupId = "g";
static inline auto const indexFilter = "f";
static inline auto const beatDistributionParam = "w";
static inline auto const beatDistributionParamType = "d";

static inline auto const customData = "customData";
} // namespace CustomJSONData::v3::Constants

namespace CustomJSONData::v3 {
using CustomDataOpt = std::optional<std::reference_wrapper<rapidjson::Value const>>;
using CustomDataOptUTF16 = std::optional<std::reference_wrapper<ValueUTF16 const>>;

} // namespace CustomJSONData::v3

DECLARE_CLASS_CODEGEN(
    CustomJSONData::v3, CustomBeatmapSaveData, BeatmapSaveDataVersion3::BeatmapSaveData,
    DECLARE_FASTER_CTOR(
        ctor,
        System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData*>*
            bpmEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::RotationEventData*>*
            rotationEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorNoteData*>* colorNotes,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BombNoteData*>* bombNotes,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData*>* obstacles,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::SliderData*>* sliders,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BurstSliderData*>*
            burstSliders,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::WaypointData*>* waypoints,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData*>*
            basicBeatmapEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorBoostEventData*>*
            colorBoostBeatmapEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorEventBoxGroup*>*
            lightColorEventBoxGroups,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationEventBoxGroup*>*
            lightRotationEventBoxGroups,
        ::System::Collections::Generic::List_1<
            ::BeatmapSaveDataVersion3::BeatmapSaveData::LightTranslationEventBoxGroup*>* lightTranslationEventBoxGroups,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::FxEventBoxGroup*>*
            lightFxEventBoxGroups,
        BeatmapSaveData::FxEventsCollection* eventsCollection,
        ::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords* basicEventTypesWithKeywords,
        bool useNormalEventsAsCompatibleEvents);

    DECLARE_SIMPLE_DTOR();

    public
    : static CustomBeatmapSaveData * Deserialize(std::shared_ptr<rapidjson::Document> const& sharedDoc);

    static CustomBeatmapSaveData * Convert2_6_0(CustomJSONData::v2::CustomBeatmapSaveData * beatmap);

    std::shared_ptr<std::vector<CustomJSONData::CustomEventSaveData>> customEventsData;
    std::shared_ptr<rapidjson::Document> doc; CustomDataOpt customData; CustomDataOptUTF16 beatmapCustomData;
    CustomDataOptUTF16 levelCustomData;

    DECLARE_INSTANCE_FIELD(bool, isV2);)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_ColorNoteData,
                      BeatmapSaveDataVersion3::BeatmapSaveData::ColorNoteData,
                      DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer,
                                          ::BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType color,
                                          ::GlobalNamespace::NoteCutDirection cutDirection, int angleOffset);
                      DECLARE_SIMPLE_DTOR();

                      public
                      : DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BombNoteData,
                      BeatmapSaveDataVersion3::BeatmapSaveData::BombNoteData,
                      DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer);
                      DECLARE_SIMPLE_DTOR();

                      public
                      : DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_SliderData,
                      BeatmapSaveDataVersion3::BeatmapSaveData::SliderData,
                      DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType,
                                          float headBeat, int headLine, int headLayer,
                                          float headControlPointLengthMultiplier,
                                          ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat,
                                          int tailLine, int tailLayer, float tailControlPointLengthMultiplier,
                                          ::GlobalNamespace::NoteCutDirection tailCutDirection,
                                          ::GlobalNamespace::SliderMidAnchorMode sliderMidAnchorMode);
                      DECLARE_SIMPLE_DTOR(); public
                      : DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BurstSliderData,
                      BeatmapSaveDataVersion3::BeatmapSaveData::BurstSliderData,
                      DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType,
                                          float headBeat, int headLine, int headLayer,
                                          ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat,
                                          int tailLine, int tailLayer, int sliceCount, float squishAmount);
                      DECLARE_SIMPLE_DTOR(); public
                      : DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);)

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_ObstacleData,
                      BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData,
                      DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer, float duration, int width, int height);

                      DECLARE_SIMPLE_DTOR(); public
                      : DECLARE_INSTANCE_FIELD(JSONWrapper*, customData);)

DECLARE_CLASS_CODEGEN(
    CustomJSONData::v3, CustomBeatmapSaveData_BasicEventData, BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData,
    DECLARE_FASTER_CTOR(ctor, float beat,
                        BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType eventType, int value,
                        float floatValue);

    DECLARE_SIMPLE_DTOR();

    public
    : CustomDataOpt customData;)

namespace CustomJSONData::v3::Parser {
CustomBeatmapSaveData_BurstSliderData* DeserializeBurstSlider(rapidjson::Value const& val);
CustomBeatmapSaveData_SliderData* DeserializeSlider(rapidjson::Value const& val);

CustomBeatmapSaveData_ObstacleData* DeserializeObstacle(rapidjson::Value const& val);

CustomBeatmapSaveData_BombNoteData* DeserializeBombNote(rapidjson::Value const& val);
CustomBeatmapSaveData_ColorNoteData* DeserializeColorNote(rapidjson::Value const& val);

extern UnorderedEventCallback<v3::CustomBeatmapSaveData*> ParsedEvent;
} // namespace CustomJSONData::v3::Parser

namespace CustomJSONData {
inline static std::optional<v3::CustomBeatmapSaveData*>
GetBeatmapSaveData(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap) {
  auto customDifficultyBeatmap = il2cpp_utils::try_cast<GlobalNamespace::CustomDifficultyBeatmap>(difficultyBeatmap);
  if (!customDifficultyBeatmap) {
    return std::nullopt;
  }

  return il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(customDifficultyBeatmap.value()->beatmapSaveData);
}
} // namespace CustomJSONData