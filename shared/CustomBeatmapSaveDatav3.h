#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "BeatmapSaveDataVersion3/ColorNoteData.hpp"
#include "BeatmapSaveDataVersion3/BurstSliderData.hpp"
#include "BeatmapSaveDataVersion3/BombNoteData.hpp"
#include "BeatmapSaveDataVersion3/BurstSliderData.hpp"
#include "BeatmapSaveDataVersion3/SliderData.hpp"
#include "BeatmapSaveDataVersion3/SliderType.hpp"
#include "BeatmapSaveDataVersion3/ObstacleData.hpp"
#include "BeatmapSaveDataVersion3/WaypointData.hpp"
#include "BeatmapSaveDataVersion3/BasicEventData.hpp"
#include "BeatmapSaveDataVersion3/BpmChangeEventData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveDataItem.hpp"
#include "BeatmapSaveDataVersion3/LightColorBaseData.hpp"
#include "BeatmapSaveDataVersion3/IndexFilter.hpp"
#include "BeatmapSaveDataVersion3/EventBox.hpp"
#include "BeatmapSaveDataVersion3/LightRotationEventBox.hpp"
#include "BeatmapSaveDataVersion3/LightTranslationEventBox.hpp"
#include "BeatmapSaveDataVersion3/LightColorEventBox.hpp"
#include "BeatmapSaveDataVersion3/LightRotationBaseData.hpp"
#include "BeatmapSaveDataVersion3/LightColorBaseData.hpp"
#include "BeatmapSaveDataVersion3/LightTranslationBaseData.hpp"
#include "BeatmapSaveDataVersion3/IntFxEventBaseData.hpp"
#include "BeatmapSaveDataVersion3/FloatFxEventBaseData.hpp"
#include "BeatmapSaveDataVersion3/FxEventsCollection.hpp"
#include "BeatmapSaveDataVersion3/FxEventBox.hpp"
#include "BeatmapSaveDataVersion3/FxEventBoxGroup.hpp"
#include "BeatmapSaveDataVersion3/FxEventType.hpp"
#include "BeatmapSaveDataVersion3/ColorBoostEventData.hpp"
#include "BeatmapSaveDataVersion3/RotationEventData.hpp"
#include "BeatmapSaveDataVersion3/RotationEventData.hpp"

#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"

#include "LowLevelUtils.hpp"
#include "CustomEventData.h"
#include "CJDLogger.h"
#include "JSONWrapper.h"
#include "songcore/shared/CustomJSONData.hpp"

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
using CustomDataOptUTF16 = std::optional<std::reference_wrapper<SongCore::CustomJSONData::ValueUTF16 const>>;

} // namespace CustomJSONData::v3

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData, BeatmapSaveDataVersion3::BeatmapSaveData) {
  DECLARE_FASTER_CTOR(
      ctor, System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BpmChangeEventData*> * bpmEvents,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::RotationEventData*> * rotationEvents,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::ColorNoteData*> * colorNotes,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BombNoteData*> * bombNotes,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::ObstacleData*> * obstacles,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::SliderData*> * sliders,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BurstSliderData*> * burstSliders,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::WaypointData*> * waypoints,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BasicEventData*> * basicBeatmapEvents,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::ColorBoostEventData*> * colorBoostBeatmapEvents,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::LightColorEventBoxGroup*> *
          lightColorEventBoxGroups,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::LightRotationEventBoxGroup*> *
          lightRotationEventBoxGroups,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::LightTranslationEventBoxGroup*> *
          lightTranslationEventBoxGroups,
      ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::FxEventBoxGroup*> * lightFxEventBoxGroups,
      BeatmapSaveDataVersion3::FxEventsCollection * eventsCollection,
      BeatmapSaveDataCommon::BasicEventTypesWithKeywords * basicEventTypesWithKeywords,
      bool useNormalEventsAsCompatibleEvents);

  DECLARE_SIMPLE_DTOR();

public:
  static CustomBeatmapSaveData* Deserialize(std::shared_ptr<rapidjson::Document> const& sharedDoc);
  static CustomBeatmapSaveData* Convert2_6_0(CustomJSONData::v2::CustomBeatmapSaveData * beatmap);

  std::shared_ptr<std::vector<CustomJSONData::CustomEventSaveData>> customEventsData;
  std::shared_ptr<rapidjson::Document> doc;
  CustomDataOpt customData;
  CustomDataOptUTF16 beatmapCustomData;
  CustomDataOptUTF16 levelCustomData;

  DECLARE_INSTANCE_FIELD(bool, isV2);
};

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_ColorNoteData, BeatmapSaveDataVersion3::ColorNoteData) {
  DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer, BeatmapSaveDataCommon::NoteColorType color,
                      BeatmapSaveDataCommon::NoteCutDirection cutDirection, int angleOffset);
  DECLARE_SIMPLE_DTOR();

public:
  CustomDataOpt customData;
};

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BombNoteData, BeatmapSaveDataVersion3::BombNoteData) {
  DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer);
  DECLARE_SIMPLE_DTOR();

public:
  CustomDataOpt customData;
};

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_SliderData, BeatmapSaveDataVersion3::SliderData) {
  DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataCommon::NoteColorType colorType, float headBeat, int headLine, int headLayer,
                      float headControlPointLengthMultiplier, BeatmapSaveDataCommon::NoteCutDirection headCutDirection,
                      float tailBeat, int tailLine, int tailLayer, float tailControlPointLengthMultiplier,
                      BeatmapSaveDataCommon::NoteCutDirection tailCutDirection,
                      BeatmapSaveDataCommon::SliderMidAnchorMode sliderMidAnchorMode);
  DECLARE_SIMPLE_DTOR();

public:
  CustomDataOpt customData;
};

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BurstSliderData,
                      BeatmapSaveDataVersion3::BurstSliderData) {
  DECLARE_FASTER_CTOR(ctor, BeatmapSaveDataCommon::NoteColorType colorType, float headBeat, int headLine, int headLayer,
                      BeatmapSaveDataCommon::NoteCutDirection headCutDirection, float tailBeat, int tailLine,
                      int tailLayer, int sliceCount, float squishAmount);
  DECLARE_SIMPLE_DTOR();

public:
  CustomDataOpt customData;
};

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_ObstacleData, BeatmapSaveDataVersion3::ObstacleData) {
  DECLARE_FASTER_CTOR(ctor, float beat, int line, int layer, float duration, int width, int height);

  DECLARE_SIMPLE_DTOR();

public:
  CustomDataOpt customData;
};

DECLARE_CLASS_CODEGEN(CustomJSONData::v3, CustomBeatmapSaveData_BasicEventData,
                      BeatmapSaveDataVersion3::BasicEventData) {
  DECLARE_FASTER_CTOR(ctor, float beat, BeatmapSaveDataCommon::BeatmapEventType eventType, int value, float floatValue);

  DECLARE_SIMPLE_DTOR();

public:
  CustomDataOpt customData;
};

// clang-format on

namespace CustomJSONData::v3::Parser {
CustomBeatmapSaveData_BurstSliderData* DeserializeBurstSlider(rapidjson::Value const& val);
CustomBeatmapSaveData_SliderData* DeserializeSlider(rapidjson::Value const& val);

CustomBeatmapSaveData_ObstacleData* DeserializeObstacle(rapidjson::Value const& val);

CustomBeatmapSaveData_BombNoteData* DeserializeBombNote(rapidjson::Value const& val);
CustomBeatmapSaveData_ColorNoteData* DeserializeColorNote(rapidjson::Value const& val);

extern UnorderedEventCallback<v3::CustomBeatmapSaveData*> ParsedEvent;
} // namespace CustomJSONData::v3::Parser
