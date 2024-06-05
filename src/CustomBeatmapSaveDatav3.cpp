#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapSaveDatav2.h"
#include "BeatmapFieldUtils.hpp"

#include "JsonUtils.h"
#include "VList.h"

#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "GlobalNamespace/IndexFilter.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/LightTranslationBeatmapEventDataBox.hpp"

#include "cpp-semver/shared/cpp-semver.hpp"
#include "GlobalNamespace/NoteLineLayer.hpp"
#include "GlobalNamespace/EaseType.hpp"

#include <numeric>
#include <type_traits>

using namespace GlobalNamespace;
using namespace BeatmapSaveDataVersion3;
using namespace CustomJSONData::v3;

template<typename T>
using SList = System::Collections::Generic::List_1<T>;

UnorderedEventCallback<CustomJSONData::v3::CustomBeatmapSaveData*> CustomJSONData::v3::Parser::ParsedEvent;

DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_ColorNoteData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_BombNoteData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_ObstacleData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_BurstSliderData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_SliderData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_BasicEventData);



// do not use safeptr allocs
#define SORT_BEATMAP(Type, list)                                                                                       \
  std::stable_sort(ListW<Type>(list).begin(), ListW<Type>(list).end(), TimeCompare<Type>);

#define SAFEPTR_VLIST_DEFAULT_SORTED(type, name, ...)                                                                  \
  ListW<type> name(__VA_ARGS__);                 \
  SORT_BEATMAP(type, name)

static inline std::array<float, 8> const spawnRotations{ -60.0F, -45.0F, -30.0F, -15.0F, 15.0F, 30.0F, 45.0F, 60.0F };

template <typename T>
  requires(std::is_pointer_v<T> &&
           std::is_convertible_v<T, BeatmapSaveDataVersion3::BeatmapSaveDataItem*>)
constexpr bool TimeCompare(T const& a, T const& b) {
  return (a->b < b->b);
}

template <typename T>
  requires(std::is_pointer_v<T> &&
           std::is_convertible_v<T, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveDataItem*>)
constexpr bool TimeCompare(T const& a, T const& b) {
  return (a->time < b->time);
}

constexpr bool TimeCompare(BeatmapSaveDataVersion2_6_0AndEarlier::SliderData* const& a,
                           BeatmapSaveDataVersion2_6_0AndEarlier::SliderData* const& b) {
  return (a->_headTime < b->_headTime);
}

static CustomDataOpt GetCustomData(rapidjson::Value const& doc) {
  auto customDataIt = doc.FindMember(Constants::customData);
  if (customDataIt != doc.MemberEnd() && customDataIt->value.IsObject()) {
    return customDataIt->value;
  }

  return std::nullopt;
}

void CustomBeatmapSaveData::ctor(
    System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BpmChangeEventData*>* bpmEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::RotationEventData*>*
        rotationEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::ColorNoteData*>* colorNotes,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BombNoteData*>* bombNotes,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::ObstacleData*>* obstacles,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::SliderData*>* sliders,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BurstSliderData*>* burstSliders,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::WaypointData*>* waypoints,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BasicEventData*>*
        basicBeatmapEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::ColorBoostEventData*>*
        colorBoostBeatmapEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::LightColorEventBoxGroup*>*
        lightColorEventBoxGroups,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::LightRotationEventBoxGroup*>*
        lightRotationEventBoxGroups,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::LightTranslationEventBoxGroup*>*
        lightTranslationEventBoxGroups,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::FxEventBoxGroup*>*
        lightFxEventBoxGroups,
    BeatmapSaveDataVersion3::FxEventsCollection* eventsCollection,
    BeatmapSaveDataCommon::BasicEventTypesWithKeywords* basicEventTypesWithKeywords,
    bool useNormalEventsAsCompatibleEvents) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData*), ".ctor", 17);
  il2cpp_utils::RunMethodRethrow(
      this, ctor, bpmEvents, rotationEvents, colorNotes, bombNotes, obstacles, sliders, burstSliders, waypoints,
      basicBeatmapEvents, colorBoostBeatmapEvents, lightColorEventBoxGroups, lightRotationEventBoxGroups,
      lightTranslationEventBoxGroups, lightFxEventBoxGroups, eventsCollection, basicEventTypesWithKeywords, useNormalEventsAsCompatibleEvents);
}

void CustomBeatmapSaveData_ColorNoteData::ctor(float beat, int line, int layer,
                                               ::BeatmapSaveDataCommon::NoteColorType color,
                                               ::BeatmapSaveDataCommon::NoteCutDirection cutDirection, int angleOffset) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveDataVersion3::ColorNoteData*), ".ctor", 6);
  il2cpp_utils::RunMethodRethrow(this, ctor, beat, line, layer, color, cutDirection, angleOffset);
}

void CustomBeatmapSaveData_BombNoteData::ctor(float beat, int line, int layer) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveDataVersion3::BombNoteData*), ".ctor", 3);
  il2cpp_utils::RunMethodRethrow(this, ctor, beat, line, layer);
}

void CustomBeatmapSaveData_SliderData::ctor(BeatmapSaveDataCommon::NoteColorType colorType,
                                            float headBeat, int headLine, int headLayer,
                                            float headControlPointLengthMultiplier,
                                            ::BeatmapSaveDataCommon::NoteCutDirection headCutDirection, float tailBeat,
                                            int tailLine, int tailLayer, float tailControlPointLengthMultiplier,
                                            ::BeatmapSaveDataCommon::NoteCutDirection tailCutDirection,
                                            ::BeatmapSaveDataCommon::SliderMidAnchorMode sliderMidAnchorMode) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveDataVersion3::SliderData*), ".ctor", 12);
  il2cpp_utils::RunMethodRethrow(
      this, ctor, colorType, headBeat, headLine, headLayer, headControlPointLengthMultiplier, headCutDirection,
      tailBeat, tailLine, tailLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode);
}

void CustomBeatmapSaveData_BurstSliderData::ctor(BeatmapSaveDataCommon::NoteColorType colorType,
                                                 float headBeat, int headLine, int headLayer,
                                                 ::BeatmapSaveDataCommon::NoteCutDirection headCutDirection, float tailBeat,
                                                 int tailLine, int tailLayer, int sliceCount, float squishAmount) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveDataVersion3::BurstSliderData*), ".ctor", 10);
  il2cpp_utils::RunMethodRethrow(this, ctor, colorType, headBeat, headLine, headLayer, headCutDirection, tailBeat,
                                       tailLine, tailLayer, sliceCount, squishAmount);
}

void CustomBeatmapSaveData_ObstacleData::ctor(float beat, int line, int layer, float duration, int width, int height) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveDataVersion3::ObstacleData*), ".ctor", 6);
  il2cpp_utils::RunMethodRethrow(this, ctor, beat, line, layer, duration, width, height);
}

void CustomJSONData::v3::CustomBeatmapSaveData_BasicEventData::ctor(
    float time, BeatmapSaveDataCommon::BeatmapEventType type, int value,
    float floatValue) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveDataVersion3::BasicEventData*), ".ctor", 4);
  il2cpp_utils::RunMethodRethrow(this, ctor, time, type, value, floatValue);
}



CustomBeatmapSaveData_ColorNoteData* CustomJSONData::v3::Parser::DeserializeColorNote(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int line = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int layer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  BeatmapSaveDataCommon::NoteColorType color = NEJSON::ReadOptionalInt(val, Constants::colorType).value_or(0);
  BeatmapSaveDataCommon::NoteCutDirection cutDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);
  int angleOffset = NEJSON::ReadOptionalInt(val, "a").value_or(0);
  CustomDataOpt customData = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* note = CustomBeatmapSaveData_ColorNoteData::New_ctor(beat, line, layer, color, cutDirection, angleOffset);
  note->customData = customData;

  return note;
}

CustomBeatmapSaveData_BombNoteData* CustomJSONData::v3::Parser::DeserializeBombNote(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int line = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int layer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* bomb = CustomBeatmapSaveData_BombNoteData::New_ctor(beat, line, layer);
  bomb->customData = data;

  return bomb;
}

CustomBeatmapSaveData_ObstacleData* CustomJSONData::v3::Parser::DeserializeObstacle(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int line = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int layer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  float duration = NEJSON::ReadOptionalFloat(val, "d").value_or(0);
  int width = NEJSON::ReadOptionalInt(val, "w").value_or(0);
  int height = NEJSON::ReadOptionalInt(val, "h").value_or(0);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* obstacle = CustomBeatmapSaveData_ObstacleData::New_ctor(beat, line, layer, duration, width, height);
  obstacle->customData = data;
  return obstacle;
}

CustomBeatmapSaveData_SliderData* CustomJSONData::v3::Parser::DeserializeSlider(rapidjson::Value const& val) {

  BeatmapSaveDataCommon::NoteColorType color = NEJSON::ReadOptionalInt(val, Constants::colorType).value_or(0);
  float headBeat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int headLine = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int headLayer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  BeatmapSaveDataCommon::NoteCutDirection headCutDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);
  float tailBeat = NEJSON::ReadOptionalFloat(val, Constants::tailBeat).value_or(0);
  int tailLine = NEJSON::ReadOptionalInt(val, Constants::tailLine).value_or(0);
  int tailLayer = NEJSON::ReadOptionalInt(val, Constants::tailLayer).value_or(0);
  int sliceCount = NEJSON::ReadOptionalInt(val, "sc").value_or(0);
  float squishAmount = NEJSON::ReadOptionalFloat(val, "s").value_or(0);

  float headControlPointLengthMultiplier = NEJSON::ReadOptionalFloat(val, "mu").value_or(0);
  float tailControlPointLengthMultiplier = NEJSON::ReadOptionalFloat(val, "tmu").value_or(0);
  BeatmapSaveDataCommon::NoteCutDirection tailCutDirection = NEJSON::ReadOptionalInt(val, "tc").value_or(0);
  BeatmapSaveDataCommon::SliderMidAnchorMode sliderMidAnchorMode = NEJSON::ReadOptionalInt(val, "m").value_or(0);

  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* slider = CustomBeatmapSaveData_SliderData::New_ctor(
      color, headBeat, headLine, headLayer, headControlPointLengthMultiplier, headCutDirection, tailBeat, tailLine,
      tailLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode);

  slider->customData = data;

  return slider;
}

CustomJSONData::v3::CustomBeatmapSaveData_BurstSliderData*
CustomJSONData::v3::Parser::DeserializeBurstSlider(rapidjson::Value const& val) {
  BeatmapSaveDataCommon::NoteColorType color = NEJSON::ReadOptionalInt(val, Constants::colorType).value_or(0);
  float headBeat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int headLine = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int headLayer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  BeatmapSaveDataCommon::NoteCutDirection headCutDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);
  float tailBeat = NEJSON::ReadOptionalFloat(val, Constants::tailBeat).value_or(0);
  int tailLine = NEJSON::ReadOptionalInt(val, Constants::tailLine).value_or(0);
  int tailLayer = NEJSON::ReadOptionalInt(val, Constants::tailLayer).value_or(0);
  int sliceCount = NEJSON::ReadOptionalInt(val, "sc").value_or(0);
  float squishAmount = NEJSON::ReadOptionalFloat(val, "s").value_or(0);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("DeserializeBurstSlider customData {}", data.has_value());

  auto* slider = CustomBeatmapSaveData_BurstSliderData::New_ctor(
      color, headBeat, headLine, headLayer, headCutDirection, tailBeat, tailLine, tailLayer, sliceCount, squishAmount);

  slider->customData = data;

  return slider;
}

namespace {
auto DeserializeBpmChangeEventData(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  float bpm = NEJSON::ReadOptionalFloat(val, "m").value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveDataVersion3::BpmChangeEventData*>(beat, bpm);
}

auto DeserializeRotation(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  BeatmapSaveDataCommon::ExecutionTime executionTime = NEJSON::ReadOptionalInt(val, "e").value_or(0);
  float rotation = NEJSON::ReadOptionalFloat(val, "r").value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveDataVersion3::RotationEventData*>(beat, executionTime, rotation);
}

auto DeserializeWaypoint(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int line = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int layer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  BeatmapSaveDataCommon::OffsetDirection offsetDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveDataVersion3::WaypointData*>(beat, line, layer, offsetDirection);
}

auto DeserializeBasicEvent(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  BeatmapSaveDataCommon::BeatmapEventType eventType =
      NEJSON::ReadOptionalInt(val, "et").value_or(0);
  int value = NEJSON::ReadOptionalInt(val, "i").value_or(0);
  float floatValue = NEJSON::ReadOptionalFloat(val, "f").value_or(0);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* event = CustomBeatmapSaveData_BasicEventData::New_ctor(beat, eventType, value, floatValue);
  event->customData = data;
  return event;
}

auto DeserializeColorBoostEventData(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  bool boost = NEJSON::ReadOptionalBool(val, "o").value_or(false);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* event = CustomJSONData::NewFast<BeatmapSaveDataVersion3::ColorBoostEventData*>(beat, boost);
  //    event->customData = CustomJSONData::JSONObjectOrNull(data);
  return event;
}

auto DeserializeIndexFilter(rapidjson::Value const& val) {
  BeatmapSaveDataCommon::IndexFilterType type = NEJSON::ReadOptionalInt(val, "f").value_or(0);
  int param0 = NEJSON::ReadOptionalInt(val, "p").value_or(0);
  int param1 = NEJSON::ReadOptionalInt(val, "t").value_or(0);
  bool reversed = NEJSON::ReadOptionalBool(val, "r").value_or(false);
  BeatmapSaveDataCommon::IndexFilterRandomType random = NEJSON::ReadOptionalInt(val, "n").value_or(0);
  int seed = NEJSON::ReadOptionalInt(val, "s").value_or(0);
  int chunks = NEJSON::ReadOptionalInt(val, "c").value_or(0);
  float limit = NEJSON::ReadOptionalFloat(val, "l").value_or(0);
  BeatmapSaveDataCommon::IndexFilterLimitAlsoAffectsType limitAlsoAffectsType = NEJSON::ReadOptionalInt(val, "d").value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveDataVersion3::IndexFilter*>(type, param0, param1, reversed, random, seed, chunks,
                                                                limit, limitAlsoAffectsType);
}

auto DeserializeLightColorEventBoxGroup(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int groupId = NEJSON::ReadOptionalInt(val, Constants::groupId).value_or(0);

  auto eventBoxesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightColorEventBox*>(
      val, Constants::eventBoxes, [](rapidjson::Value const& arrIt) {
        /* nullable */
        BeatmapSaveDataVersion3::IndexFilter* indexFilter =
            NEJSON::ReadOptionalType<BeatmapSaveDataVersion3::IndexFilter*>(
                arrIt, Constants::indexFilter,
                [](rapidjson::Value const& it) { return DeserializeIndexFilter(it); })
                .value_or(nullptr);

        float beatDistributionParam = NEJSON::ReadOptionalFloat(arrIt, Constants::beatDistributionParam).value_or(0);
        BeatmapSaveDataCommon::DistributionParamType beatDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, Constants::beatDistributionParamType).value_or(0);

        float brightnessDistributionParam = NEJSON::ReadOptionalFloat(arrIt, "r").value_or(0);

        bool brightnessDistributionShouldAffectFirstBaseEvent = NEJSON::ReadOptionalBool(arrIt, "b").value_or(false);

        BeatmapSaveDataCommon::DistributionParamType brightnessDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, "t").value_or(0);

        BeatmapSaveDataCommon::EaseType brightnessDistributionEaseType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        auto lightColorBaseDataListVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightColorBaseData*>(
            arrIt, "e", [](rapidjson::Value const& arrIt) {
              float lightBeat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
              BeatmapSaveDataVersion3::TransitionType transitionType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);
              BeatmapSaveDataCommon::EnvironmentColorType colorType =
                  NEJSON::ReadOptionalInt(arrIt, Constants::colorType).value_or(0);
              float brightness = NEJSON::ReadOptionalFloat(arrIt, "s").value_or(0);
              int strobeFrequency = NEJSON::ReadOptionalInt(arrIt, "f").value_or(0);
              float strobeBrightness = NEJSON::ReadOptionalFloat(arrIt, "sb").value_or(0);
              bool strobeFade = NEJSON::ReadOptionalBool(arrIt, "sf").value_or(false);

              return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightColorBaseData*>(
                  lightBeat, transitionType.value__, colorType.value__, brightness, strobeFrequency, strobeBrightness, strobeFade);
            });
        ListW<BeatmapSaveDataVersion3::LightColorBaseData*> lightColorBaseDataList = CustomJSONData::SpanToSystemList(lightColorBaseDataListVec);

        return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightColorEventBox*>(
            indexFilter, beatDistributionParam, beatDistributionParamType, brightnessDistributionParam,
            brightnessDistributionShouldAffectFirstBaseEvent, brightnessDistributionParamType.value__,
            brightnessDistributionEaseType.value__, lightColorBaseDataList.getPtr());
      });

  ListW<BeatmapSaveDataVersion3::LightColorEventBox*> eventBoxes =
                        CustomJSONData::SpanToSystemList(eventBoxesVec);

  return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightColorEventBoxGroup*>(beat, groupId, eventBoxes.getPtr());
}

auto DeserializeLightRotationEventBoxGroup(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int groupId = NEJSON::ReadOptionalInt(val, Constants::groupId).value_or(0);

  auto eventBoxesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightRotationEventBox*>(
      val, Constants::eventBoxes, [](rapidjson::Value const& arrIt) {
        /* nullable */
        BeatmapSaveDataVersion3::IndexFilter* indexFilter =
            NEJSON::ReadOptionalType<BeatmapSaveDataVersion3::IndexFilter*>(
                arrIt, Constants::indexFilter, [](rapidjson::Value const& it) { return DeserializeIndexFilter(it); })
                .value_or(nullptr);

        float beatDistributionParam = NEJSON::ReadOptionalFloat(arrIt, Constants::beatDistributionParam).value_or(0);

        BeatmapSaveDataCommon::DistributionParamType beatDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, Constants::beatDistributionParamType).value_or(0);

        float rotationDistributionParam = NEJSON::ReadOptionalFloat(arrIt, "s").value_or(0);
        BeatmapSaveDataCommon::DistributionParamType rotationDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, "t").value_or(0);
        bool rotationDistributionShouldAffectFirstBaseEvent = NEJSON::ReadOptionalBool(arrIt, "b").value_or(false);
        BeatmapSaveDataCommon::Axis axis = NEJSON::ReadOptionalInt(arrIt, "a").value_or(0);
        bool flipRotation = NEJSON::ReadOptionalBool(arrIt, "r").value_or(false);
        EaseType rotationDistributionEaseType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        auto lightTranslationBaseDataListVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightRotationBaseData*>(
            arrIt, "l", [](rapidjson::Value const& arrIt) {
              float lightBeat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
              bool usePreviousEventRotationValue = NEJSON::ReadOptionalBool(arrIt, "p").value_or(false);
              BeatmapSaveDataCommon::EaseType easeType = NEJSON::ReadOptionalInt(arrIt, "e").value_or(0);
              int loopsCount = NEJSON::ReadOptionalInt(arrIt, "l").value_or(0);
              float rotation = NEJSON::ReadOptionalFloat(arrIt, "r").value_or(0);
              BeatmapSaveDataCommon::RotationDirection rotationDirection =
                  NEJSON::ReadOptionalInt(arrIt, "o").value_or(0);

              return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightRotationBaseData*>(
                  lightBeat, usePreviousEventRotationValue, easeType.value__, loopsCount, rotation,
                  rotationDirection.value__);
            });

        ListW<BeatmapSaveDataVersion3::LightRotationBaseData*> lightTranslationBaseDataList = CustomJSONData::SpanToSystemList(lightTranslationBaseDataListVec);

        return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightRotationEventBox*>(
            indexFilter, beatDistributionParam, beatDistributionParamType, rotationDistributionParam,
            rotationDistributionParamType.value__, rotationDistributionShouldAffectFirstBaseEvent,
            rotationDistributionEaseType.value__, axis.value__, flipRotation, lightTranslationBaseDataList.getPtr());
      });
  ListW<BeatmapSaveDataVersion3::LightRotationEventBox*> eventBoxes =
                        CustomJSONData::SpanToSystemList(eventBoxesVec);

  return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightRotationEventBoxGroup*>(beat, groupId, eventBoxes.getPtr());
}

auto DeserializeLightTranslationEventBoxGroup(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int groupId = NEJSON::ReadOptionalInt(val, Constants::groupId).value_or(0);

  auto eventBoxesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightTranslationEventBox*>(
      val, Constants::eventBoxes, [](rapidjson::Value const& arrIt) {
        /* nullable */
        BeatmapSaveDataVersion3::IndexFilter* indexFilter =
            NEJSON::ReadOptionalType < BeatmapSaveDataVersion3::IndexFilter * >(
                                                                          arrIt, Constants::indexFilter,
                                                                          [](rapidjson::Value const& it) {
                                                                            return DeserializeIndexFilter(it);
                                                                          })
                                                                          .value_or(nullptr);

        float beatDistributionParam = NEJSON::ReadOptionalFloat(arrIt, Constants::beatDistributionParam).value_or(0);

        BeatmapSaveDataCommon::DistributionParamType beatDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, Constants::beatDistributionParamType).value_or(0);

        float gapDistributionParam = NEJSON::ReadOptionalFloat(arrIt, "s").value_or(0);

        BeatmapSaveDataCommon::DistributionParamType gapDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, "t").value_or(0);

        bool gapDistributionShouldAffectFirstBaseEvent = NEJSON::ReadOptionalBool(arrIt, "b").value_or(false);

        BeatmapSaveDataCommon::Axis axis = NEJSON::ReadOptionalInt(arrIt, "a").value_or(0);

        bool flipTranslation = NEJSON::ReadOptionalBool(arrIt, "r").value_or(false);

        EaseType gapDistributionEaseType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        auto lightTranslationBaseDataListVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightTranslationBaseData*>(
            arrIt, "l", [](rapidjson::Value const& arrIt) {
              float lightBeat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
              bool usePreviousEventTransitionValue = NEJSON::ReadOptionalBool(arrIt, "p").value_or(false);
              BeatmapSaveDataCommon::EaseType easeType = NEJSON::ReadOptionalInt(arrIt, "e").value_or(0);
              float translation = NEJSON::ReadOptionalFloat(arrIt, "t").value_or(0);

              return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightTranslationBaseData*>(
                  lightBeat, usePreviousEventTransitionValue, easeType.value__, translation);
            });

        ListW<BeatmapSaveDataVersion3::LightTranslationBaseData*> lightTranslationBaseDataList =
                              CustomJSONData::SpanToSystemList(lightTranslationBaseDataListVec);

        return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightTranslationEventBox*>(
            indexFilter, beatDistributionParam, beatDistributionParamType.value__, gapDistributionParam,
            gapDistributionParamType.value__, gapDistributionShouldAffectFirstBaseEvent, gapDistributionEaseType.value__,
            axis, flipTranslation, lightTranslationBaseDataList.getPtr());
      });
  ListW<BeatmapSaveDataVersion3::LightTranslationEventBox*> eventBoxes = 
                        CustomJSONData::SpanToSystemList(eventBoxesVec);

  return CustomJSONData::NewFast<BeatmapSaveDataVersion3::LightTranslationEventBoxGroup*>(beat, groupId, eventBoxes.getPtr());
}

BeatmapSaveDataVersion3::FxEventsCollection* DeserializeFxEventCollection(rapidjson::Document const& doc) {
  auto it = doc.FindMember("_fxEventsCollection");
  if (it == doc.MemberEnd()) {
    return nullptr;
  }

  auto const& val = it->value;

  auto* result = BeatmapSaveDataVersion3::FxEventsCollection::New_ctor();
  ListW<BeatmapSaveDataVersion3::IntFxEventBaseData*> intEventsList = result->_il;
  ListW<BeatmapSaveDataVersion3::FloatFxEventBaseData*> floatEventsList = result->_fl;

  auto intEventsListVec =
      NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::IntFxEventBaseData*>(val, "_il", [](rapidjson::Value const& arrIt) {
        float beat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
        bool usePreviousEventValue = NEJSON::ReadOptionalBool(arrIt, "p").value_or(false);
        int value = NEJSON::ReadOptionalInt(arrIt, "v").value_or(0);

        auto* fx = BeatmapSaveDataVersion3::IntFxEventBaseData::New_ctor(beat, value);
        fx->p = usePreviousEventValue ? 1 : 0;

        return fx;
      });
  auto floatEventsListVec =
      NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::FloatFxEventBaseData*>(val, "_fl", [](rapidjson::Value const& arrIt) {
        float beat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
        bool usePreviousEventValue = NEJSON::ReadOptionalBool(arrIt, "p").value_or(false);
        float value = NEJSON::ReadOptionalFloat(arrIt, "v").value_or(0);
        BeatmapSaveDataCommon::EaseType easeType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        auto* fx = BeatmapSaveDataVersion3::FloatFxEventBaseData::New_ctor(beat, usePreviousEventValue, value, easeType);

        return fx;
      });

  intEventsList.insert_range(intEventsListVec);
  floatEventsList.insert_range(floatEventsListVec);

  return result;
}

BeatmapSaveDataVersion3::FxEventBoxGroup* DeserializeFxEventBoxGroupArray(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int groupId = NEJSON::ReadOptionalInt(val, Constants::groupId).value_or(0);
  BeatmapSaveDataVersion3::FxEventType type = NEJSON::ReadOptionalInt(val, "t").value_or(0);

  auto eventBoxesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::FxEventBox*>(
      val, Constants::eventBoxes, [](rapidjson::Value const& arrIt) {
        /* nullable */
        BeatmapSaveDataVersion3::IndexFilter* indexFilter =
            NEJSON::ReadOptionalType<BeatmapSaveDataVersion3::IndexFilter*>(
                arrIt, Constants::indexFilter, [](rapidjson::Value const& it) { return DeserializeIndexFilter(it); })
                .value_or(nullptr);

        float beatDistributionParam = NEJSON::ReadOptionalFloat(arrIt, Constants::beatDistributionParam).value_or(0);

        BeatmapSaveDataCommon::DistributionParamType beatDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, Constants::beatDistributionParamType).value_or(0);

        float vfxDistributionParam = NEJSON::ReadOptionalFloat(arrIt, "s").value_or(0);

        BeatmapSaveDataCommon::DistributionParamType vfxDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, "t").value_or(0);

        bool vfxDistributionShouldAffectFirstBaseEvent = NEJSON::ReadOptionalBool(arrIt, "b").value_or(false);

        BeatmapSaveDataCommon::EaseType vfxDistributionEaseType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        std::vector<int> vfxBaseDataList =
            NEJSON::ReadArrayOrEmpty<int>(arrIt, "l", [](rapidjson::Value const& arrIt) { return arrIt.GetInt(); });

        return BeatmapSaveDataVersion3::FxEventBox::New_ctor(indexFilter, beatDistributionParam, beatDistributionParamType,
                                                     vfxDistributionParam, vfxDistributionParamType,
                                                     vfxDistributionEaseType, vfxDistributionShouldAffectFirstBaseEvent,
                                                     CustomJSONData::SpanToSystemList(vfxBaseDataList));
      });
  ListW<BeatmapSaveDataVersion3::FxEventBox*> eventBoxes = CustomJSONData::SpanToSystemList(eventBoxesVec);

  return BeatmapSaveDataVersion3::FxEventBoxGroup::New_ctor(beat, groupId, type, eventBoxes.getPtr());
}

auto DeserializeBasicEventTypesForKeyword(rapidjson::Value const& val) {
  std::string_view keyword = NEJSON::ReadOptionalString(val, "k").value_or("");
  auto eventTypesVec =
      NEJSON::ReadArrayOrEmpty<BeatmapSaveDataCommon::BeatmapEventType>(
          val, "e",
          [](rapidjson::Value const& it) -> BeatmapSaveDataCommon::BeatmapEventType {
            return it.GetInt();
          });

  ListW<BeatmapSaveDataCommon::BeatmapEventType> eventTypes =
                        CustomJSONData::SpanToSystemList(eventTypesVec);

  return BeatmapSaveDataCommon::BasicEventTypesWithKeywords::BasicEventTypesForKeyword::New_ctor(keyword,
                                                                    eventTypes.getPtr());
}

auto DeserializeCustomEvent(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  std::string_view type = NEJSON::ReadOptionalString(val, "t").value_or("");
  rapidjson::Value const* data = NEJSON::ReadOptionalValuePtr(val, "d").value_or(nullptr);

  return CustomJSONData::CustomEventSaveData(type, beat, data);
}
} // namespace

CustomJSONData::v3::CustomBeatmapSaveData*
CustomJSONData::v3::CustomBeatmapSaveData::Deserialize(std::shared_ptr<rapidjson::Document> const& sharedDoc) {
  using namespace Parser;

  auto const& doc = *sharedDoc;

  auto bpmEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::BpmChangeEventData*>(
      doc, "bpmEvents", [](rapidjson::Value const& arr) { return DeserializeBpmChangeEventData(arr); });

  auto rotationEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::RotationEventData*>(
      doc, "rotationEvents", [](rapidjson::Value const& arr) { return DeserializeRotation(arr); });

  auto colorNotesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::ColorNoteData*>(
      doc, "colorNotes", [](rapidjson::Value const& arr) { return DeserializeColorNote(arr); });

  auto bombNotesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::BombNoteData*>(
      doc, "bombNotes", [](rapidjson::Value const& arr) { return DeserializeBombNote(arr); });

  auto obstaclesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::ObstacleData*>(
      doc, "obstacles", [](rapidjson::Value const& arr) { return DeserializeObstacle(arr); });

  auto slidersVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::SliderData*>(
      doc, "sliders", [](rapidjson::Value const& arr) { return DeserializeSlider(arr); });

  auto burstSlidersVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::BurstSliderData*>(
      doc, "burstSliders", [](rapidjson::Value const& arr) { return DeserializeBurstSlider(arr); });

  auto waypointsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::WaypointData*>(
      doc, "waypoints", [](rapidjson::Value const& arr) { return DeserializeWaypoint(arr); });

  auto basicBeatmapEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::BasicEventData*>(
      doc, "basicBeatmapEvents", [](rapidjson::Value const& arr) { return DeserializeBasicEvent(arr); });

  auto colorBoostBeatmapEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::ColorBoostEventData*>(
      doc, "colorBoostBeatmapEvents", [](rapidjson::Value const& arr) { return DeserializeColorBoostEventData(arr); });

  auto lightColorEventBoxGroupsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightColorEventBoxGroup*>(
      doc, "lightColorEventBoxGroups",
      [](rapidjson::Value const& arr) { return DeserializeLightColorEventBoxGroup(arr); });

  auto lightRotationEventBoxGroupsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightRotationEventBoxGroup*>(
      doc, "lightRotationEventBoxGroups",
      [](rapidjson::Value const& arr) { return DeserializeLightRotationEventBoxGroup(arr); });

  auto lightTranslationEventBoxGroupsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::LightTranslationEventBoxGroup*>(
      doc, "lightTranslationEventBoxGroups",
      [](rapidjson::Value const& arr) { return DeserializeLightTranslationEventBoxGroup(arr); });

  auto vfxEventBoxGroupsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion3::FxEventBoxGroup*>(
      doc, "vfxEventBoxGroups", [](rapidjson::Value const& arr) { return DeserializeFxEventBoxGroupArray(arr); });

  auto *fxEventsCollection = DeserializeFxEventCollection(doc);

  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::BpmChangeEventData*, bpmEvents,
                                   CustomJSONData::SpanToSystemList(bpmEventsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::RotationEventData*, rotationEvents,
                               CustomJSONData::SpanToSystemList(rotationEventsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::ColorNoteData*, colorNotes,
                               CustomJSONData::SpanToSystemList(colorNotesVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::BombNoteData*, bombNotes,
                               CustomJSONData::SpanToSystemList(bombNotesVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::ObstacleData*, obstacles,
                               CustomJSONData::SpanToSystemList(obstaclesVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::SliderData*, sliders, 
                               CustomJSONData::SpanToSystemList(slidersVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::BurstSliderData*, burstSliders,
                               CustomJSONData::SpanToSystemList(burstSlidersVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::WaypointData*, waypoints,
                               CustomJSONData::SpanToSystemList(waypointsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::BasicEventData*, basicBeatmapEvents,
                               CustomJSONData::SpanToSystemList(basicBeatmapEventsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::ColorBoostEventData*, colorBoostBeatmapEvents,
                               CustomJSONData::SpanToSystemList(colorBoostBeatmapEventsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::LightColorEventBoxGroup*, lightColorEventBoxGroups,
                               CustomJSONData::SpanToSystemList(lightColorEventBoxGroupsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::LightRotationEventBoxGroup*, lightRotationEventBoxGroups,
                               CustomJSONData::SpanToSystemList(lightRotationEventBoxGroupsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(LightTranslationEventBoxGroup*, lightTranslationEventBoxGroups,
                               CustomJSONData::SpanToSystemList(lightTranslationEventBoxGroupsVec));
  SAFEPTR_VLIST_DEFAULT_SORTED(BeatmapSaveDataVersion3::FxEventBoxGroup*, vfxEventBoxGroups,
                               CustomJSONData::SpanToSystemList(vfxEventBoxGroupsVec));

  auto basicEventTypesForKeyword = ListW<BeatmapSaveDataCommon::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>::New();
  bool useNormalEventsAsCompatibleEvents =
      NEJSON::ReadOptionalBool(doc, "useNormalEventsAsCompatibleEvents").value_or(false);

  auto basicEventTypesWithKeywordsIt = doc.FindMember("basicEventTypesWithKeywords");
  if (basicEventTypesWithKeywordsIt != doc.MemberEnd()) {
    auto dIt = basicEventTypesWithKeywordsIt->value.FindMember("d");

    if (dIt != basicEventTypesWithKeywordsIt->value.MemberEnd()) {
      auto arr = dIt->value.GetArray();

      basicEventTypesForKeyword->EnsureCapacity(arr.Size());

      for (auto const& it : arr) {
        basicEventTypesForKeyword.push_back(DeserializeBasicEventTypesForKeyword(it));
      }
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse bpm events {}", bpmEvents.size());
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse notes {}", colorNotes.size());

  auto customEvents = std::make_shared<std::vector<CustomJSONData::CustomEventSaveData>>();
  CustomDataOpt dataOpt = GetCustomData(doc);

  if (dataOpt) {
    auto customEventsIt = dataOpt->get().FindMember("customEvents");
    if (customEventsIt != dataOpt->get().MemberEnd() && !customEventsIt->value.IsNull()) {
      customEvents->reserve(customEventsIt->value.GetArray().Size());
      for (auto const& it : customEventsIt->value.GetArray()) {
        customEvents->emplace_back(DeserializeCustomEvent(it));
      }
    }
  }

  CJDLogger::Logger.info("Size {} empty {}", bpmEvents.size(), bpmEvents.empty());
  if (!bpmEvents.empty()) CRASH_UNLESS(bpmEvents.front() != nullptr);

  auto* beatmap = CustomBeatmapSaveData::New_ctor(
      bpmEvents.getPtr(), rotationEvents.getPtr(), colorNotes.getPtr(), bombNotes.getPtr(), obstacles.getPtr(),
      sliders.getPtr(), burstSliders.getPtr(), waypoints.getPtr(), basicBeatmapEvents.getPtr(),
      colorBoostBeatmapEvents.getPtr(), lightColorEventBoxGroups.getPtr(), lightRotationEventBoxGroups.getPtr(),
      lightTranslationEventBoxGroups, vfxEventBoxGroups.getPtr(), fxEventsCollection,
      BeatmapSaveDataCommon::BasicEventTypesWithKeywords::New_ctor(basicEventTypesForKeyword.getPtr()),useNormalEventsAsCompatibleEvents);

  beatmap->customData = dataOpt;
  beatmap->doc = sharedDoc;
  beatmap->customEventsData = customEvents;

  Parser::ParsedEvent.invoke(beatmap);
  return beatmap;
}

constexpr BeatmapSaveDataCommon::NoteColorType
GetNoteColorType(BeatmapSaveDataVersion2_6_0AndEarlier::NoteType noteType) {
  return noteType == BeatmapSaveDataVersion2_6_0AndEarlier::NoteType::NoteB
             ? BeatmapSaveDataCommon::NoteColorType::ColorB
             : BeatmapSaveDataCommon::NoteColorType::ColorA;
}

constexpr BeatmapSaveDataCommon::NoteColorType
GetNoteColorType(BeatmapSaveDataVersion2_6_0AndEarlier::ColorType colorType) {
  return colorType == BeatmapSaveDataVersion2_6_0AndEarlier::ColorType::ColorB
             ? BeatmapSaveDataCommon::NoteColorType::ColorB
             : BeatmapSaveDataCommon::NoteColorType::ColorA;
}

constexpr int
GetHeightForObstacleType(BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleType obstacleType) {
  return obstacleType != BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleType::Top ? 5 : 3;
}

constexpr int
GetLayerForObstacleType(BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleType obstacleType) {
  return obstacleType != BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleType::Top ? 0 : 2;
}

constexpr float SpawnRotationForEventValue(int index) {
  if (index >= 0 && index < spawnRotations.size()) {
    return spawnRotations[index];
  }

  return 0.0F;
}

CustomBeatmapSaveData* CustomBeatmapSaveData::Convert2_6_0(CustomJSONData::v2::CustomBeatmapSaveData* beatmap) {
  auto startTime = std::chrono::high_resolution_clock::now();
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Initiating converting 2.0.0 to 3.0.0 map");

  auto colorNotes = ListW<BeatmapSaveDataVersion3::ColorNoteData*>::New(beatmap->notes->_size);
  auto bombNotes = ListW<BeatmapSaveDataVersion3::BombNoteData*> ::New(beatmap->notes->_size);
  auto sliders = ListW<BeatmapSaveDataVersion3::SliderData*>::New(beatmap->sliders->_size);
  auto obstacles = ListW<BeatmapSaveDataVersion3::ObstacleData*>::New(beatmap->obstacles->_size);
  auto waypoints = ListW<BeatmapSaveDataVersion3::WaypointData*> ::New(beatmap->waypoints->_size);

  auto colorBoosts = ListW<BeatmapSaveDataVersion3::ColorBoostEventData*>::New();
  auto rotationEvents = ListW<BeatmapSaveDataVersion3::RotationEventData*>::New();
  auto bpmChanges = ListW<BeatmapSaveDataVersion3::BpmChangeEventData*>::New();
  auto basicEvents = ListW<BeatmapSaveDataVersion3::BasicEventData*>::New();

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Sorting");

  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::NoteData*, beatmap->notes)
  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleData*, beatmap->obstacles)
  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::WaypointData*, beatmap->waypoints)
  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::EventData*, beatmap->events)
  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::EventData*, beatmap->sliders)

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting notes");
  for (auto const& n : VList <BeatmapSaveDataVersion2_6_0AndEarlier::NoteData*>(beatmap->notes)) {
    auto* customN = il2cpp_utils::cast<CustomJSONData::v2::CustomBeatmapSaveData_NoteData>(n);

    if (n->type == BeatmapSaveDataVersion2_6_0AndEarlier::NoteType::Bomb) {
      auto* newNote = CustomBeatmapSaveData_BombNoteData::New_ctor(n->time, n->lineIndex, n->lineLayer.value__);
      newNote->customData = customN->customData;

      bombNotes.push_back(newNote);
    } else {
      auto* newNote = CustomBeatmapSaveData_ColorNoteData::New_ctor(n->time, n->lineIndex, n->lineLayer.value__,
                                                                    GetNoteColorType(n->type), n->cutDirection, 0);
      newNote->customData = customN->customData;
      colorNotes.push_back(newNote);
    }
  }
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converted {} notes and {} bombs", colorNotes.size(), bombNotes.size());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting obstacles");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleData * >(beatmap->obstacles)) {
    auto* customN = il2cpp_utils::cast<CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData>(n);

    auto* obstacle =
        CustomBeatmapSaveData_ObstacleData::New_ctor(n->time, n->lineIndex, GetLayerForObstacleType(n->type),
                                                     n->duration, n->width, GetHeightForObstacleType(n->type));

    obstacle->customData = customN->customData;

    obstacles.push_back(obstacle);
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting Sliders");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::SliderData*>(beatmap->sliders)) {
    auto* customN = il2cpp_utils::cast<CustomJSONData::v2::CustomBeatmapSaveData_SliderData>(n);

    auto* slider = CustomBeatmapSaveData_SliderData::New_ctor(
        GetNoteColorType(n->colorType), n->_headTime, n->headLineIndex, static_cast<int>(n->_headLineLayer.value__),
        n->headControlPointLengthMultiplier, n->headCutDirection, n->tailTime, n->tailLineIndex,
        static_cast<int>(n->tailLineLayer.value__), n->tailControlPointLengthMultiplier, n->tailCutDirection,
        n->sliderMidAnchorMode);

    slider->customData = customN->customData;

    sliders.push_back(slider);
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting waypoints");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::WaypointData*>(beatmap->waypoints)) {
    auto* customN = il2cpp_utils::cast<CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData>(n);

    auto* waypoint =
        CustomJSONData::NewFast<WaypointData*>(n->time, n->lineIndex, n->lineLayer.value__, n->offsetDirection);

    waypoints.push_back(waypoint);
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting events");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::EventData * >(beatmap->events)) {
    auto* customN = il2cpp_utils::cast<CustomJSONData::v2::CustomBeatmapSaveData_EventData>(n);

    switch (customN->type) {
    case BeatmapSaveDataCommon::BeatmapEventType::Event5: {
      auto* boost = CustomJSONData::NewFast<ColorBoostEventData*>(n->time, n->value == 1);
      // TODO: Custom data
      colorBoosts.push_back(boost);
      continue;
    }
    case BeatmapSaveDataCommon::BeatmapEventType::Event14:
    case BeatmapSaveDataCommon::BeatmapEventType::Event15: {
      auto* rotation = CustomJSONData::NewFast<RotationEventData*>(
          n->time,
          n->type == BeatmapSaveDataCommon::BeatmapEventType::Event14
              ? BeatmapSaveDataCommon::ExecutionTime::Early.value__
              : BeatmapSaveDataCommon::ExecutionTime::Late.value__,
          SpawnRotationForEventValue(n->value));
      // TODO: Custom data
      rotationEvents.push_back(rotation);
      continue;
    }
    case BeatmapSaveDataCommon::BeatmapEventType::BpmChange:
      // TODO: Custom data!
      bpmChanges.push_back(CustomJSONData::NewFast<BpmChangeEventData*>(n->time, n->floatValue));
      continue;
    default:
      auto* event = v3::CustomBeatmapSaveData_BasicEventData::New_ctor(n->time, n->type, n->value, n->floatValue);
      event->customData = customN->customData;
      basicEvents.push_back(event);
      continue;
    }
  }

  auto* keywordsInnerList =
      System::Collections::Generic::List_1<BeatmapSaveDataCommon::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>::
          New_ctor(beatmap->specialEventsKeywordFilters->keywords->get_Count());
  VList<BeatmapSaveDataCommon::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*> keywords(
      keywordsInnerList);
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting specialEventsKeywordFilters {} with size {}",
                                          fmt::ptr(keywordsInnerList), keywords.size());

  for (auto const& n : VList < BeatmapSaveDataVersion2_6_0AndEarlier::SpecialEventsForKeyword *
                                   >(beatmap->specialEventsKeywordFilters->keywords)) {
    keywords.push_back(
        CustomJSONData::NewFast<BeatmapSaveDataCommon::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>(
            n->keyword, n->specialEvents));
  }

  auto* basicEventTypesWithKeywords =
      CustomJSONData::NewFast<BeatmapSaveDataCommon::BasicEventTypesWithKeywords*>(keywordsInnerList);

  colorNotes->TrimExcess();
  bombNotes->TrimExcess();

  auto* v3beatmap = CustomBeatmapSaveData::New_ctor(
      bpmChanges, rotationEvents, colorNotes, bombNotes, obstacles, sliders,
      SList<BeatmapSaveDataVersion3::BurstSliderData*>::New_ctor(), waypoints, basicEvents, colorBoosts,
      SList<BeatmapSaveDataVersion3::LightColorEventBoxGroup*>::New_ctor(),
      SList<BeatmapSaveDataVersion3::LightRotationEventBoxGroup*>::New_ctor(),
      SList<BeatmapSaveDataVersion3::LightTranslationEventBoxGroup*>::New_ctor(),
      SList<BeatmapSaveDataVersion3::FxEventBoxGroup*>::New_ctor(), BeatmapSaveDataVersion3::FxEventsCollection::New_ctor(),
      basicEventTypesWithKeywords, true);

  v3beatmap->customEventsData = beatmap->customEventsData;
  v3beatmap->doc = beatmap->doc;
  v3beatmap->customData = beatmap->customData;

  CJDLogger::Logger.fmtLog<LogLevel::DBG>(
      "beatmap eventkeywords {} vs our {} and finally items {}", fmt::ptr(v3beatmap->basicEventTypesWithKeywords->d),
      fmt::ptr(basicEventTypesWithKeywords->d), fmt::ptr(basicEventTypesWithKeywords->d->_items.convert()));

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished converting 2.0.0 to 3.0.0 map");
  auto stopTime = std::chrono::high_resolution_clock::now();

  CJDLogger::Logger.fmtLog<LogLevel::DBG>(
      "This took {}ms",
      static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count()));

  return v3beatmap;
}
