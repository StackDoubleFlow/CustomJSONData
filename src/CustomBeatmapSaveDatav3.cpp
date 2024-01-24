#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapSaveDatav2.h"
#include "BeatmapFieldUtils.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"

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

#define SAFEPTR_VLIST_DEFAULT(type, name, ...)                                                                         \
  SafePtr<System::Collections::Generic::List_1<type>> name##Ptr(__VA_ARGS__);                                          \
  VList<type> name(static_cast<System::Collections::Generic::List_1<type>*>(name##Ptr));

//#define SAFEPTR_VLIST_ARG(type, name, ...) \
//    SafePtr<System::Collections::Generic::List_1<type>> name##Ptr(System::Collections::Generic::List_1<type>::New_ctor(__VA_ARGS__)); \
//    VList<type> name(static_cast<System::Collections::Generic::List_1<type>*>(name##Ptr));
//
// #define SAFEPTR_VLIST(type, name) \
//    SafePtr<System::Collections::Generic::List_1<type>>
 //    name##Ptr(System::Collections::Generic::List_1<type>::New_ctor(0)); \
 VList<type>
//    name(static_cast<System::Collections::Generic::List_1<type>*>(name##Ptr));

#define SAFEPTR_VLIST_ARG(type, name, ...)                                                                             \
  VList<type> name(CustomJSONData::NewFast<System::Collections::Generic::List_1<type>*>(__VA_ARGS__));

#define SAFEPTR_VLIST(type, name) SAFEPTR_VLIST_ARG(type, name)

void CustomBeatmapSaveData::ctor(
    System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData*>* bpmEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::RotationEventData*>*
        rotationEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorNoteData*>* colorNotes,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BombNoteData*>* bombNotes,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData*>* obstacles,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::SliderData*>* sliders,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BurstSliderData*>* burstSliders,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::WaypointData*>* waypoints,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData*>*
        basicBeatmapEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorBoostEventData*>*
        colorBoostBeatmapEvents,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorEventBoxGroup*>*
        lightColorEventBoxGroups,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationEventBoxGroup*>*
        lightRotationEventBoxGroups,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightTranslationEventBoxGroup*>*
        lightTranslationEventBoxGroups,
    ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::FxEventBoxGroup*>*
        lightFxEventBoxGroups,
    BeatmapSaveData::FxEventsCollection* eventsCollection,
    ::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords* basicEventTypesWithKeywords,
    bool useNormalEventsAsCompatibleEvents) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData*), ".ctor", 17);
  il2cpp_utils::RunMethodRethrow(
      this, ctor, bpmEvents, rotationEvents, colorNotes, bombNotes, obstacles, sliders, burstSliders, waypoints,
      basicBeatmapEvents, colorBoostBeatmapEvents, lightColorEventBoxGroups, lightRotationEventBoxGroups,
      lightTranslationEventBoxGroups, lightFxEventBoxGroups, eventsCollection, basicEventTypesWithKeywords, useNormalEventsAsCompatibleEvents);
}

void CustomBeatmapSaveData_ColorNoteData::ctor(float beat, int line, int layer,
                                               ::BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType color,
                                               ::GlobalNamespace::NoteCutDirection cutDirection, int angleOffset) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::ColorNoteData*), ".ctor", 6);
  il2cpp_utils::RunMethodRethrow(this, ctor, beat, line, layer, color, cutDirection, angleOffset);
}

void CustomBeatmapSaveData_BombNoteData::ctor(float beat, int line, int layer) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::BombNoteData*), ".ctor", 3);
  il2cpp_utils::RunMethodRethrow(this, ctor, beat, line, layer);
}

void CustomBeatmapSaveData_SliderData::ctor(BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType,
                                            float headBeat, int headLine, int headLayer,
                                            float headControlPointLengthMultiplier,
                                            ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat,
                                            int tailLine, int tailLayer, float tailControlPointLengthMultiplier,
                                            ::GlobalNamespace::NoteCutDirection tailCutDirection,
                                            ::GlobalNamespace::SliderMidAnchorMode sliderMidAnchorMode) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::SliderData*), ".ctor", 12);
  il2cpp_utils::RunMethodRethrow(
      this, ctor, colorType, headBeat, headLine, headLayer, headControlPointLengthMultiplier, headCutDirection,
      tailBeat, tailLine, tailLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode);
}

void CustomBeatmapSaveData_BurstSliderData::ctor(BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType,
                                                 float headBeat, int headLine, int headLayer,
                                                 ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat,
                                                 int tailLine, int tailLayer, int sliceCount, float squishAmount) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::BurstSliderData*), ".ctor", 10);
  il2cpp_utils::RunMethodRethrow(this, ctor, colorType, headBeat, headLine, headLayer, headCutDirection, tailBeat,
                                       tailLine, tailLayer, sliceCount, squishAmount);
}

void CustomBeatmapSaveData_ObstacleData::ctor(float beat, int line, int layer, float duration, int width, int height) {
  INVOKE_CTOR();
  static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::ObstacleData*), ".ctor", 6);
  il2cpp_utils::RunMethodRethrow(this, ctor, beat, line, layer, duration, width, height);
}

void CustomJSONData::v3::CustomBeatmapSaveData_BasicEventData::ctor(
    float time, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType type, int value,
    float floatValue) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::BasicEventData*), ".ctor", 4);
  il2cpp_utils::RunMethodRethrow(this, ctor, time, type, value, floatValue);
}

inline CustomDataOpt GetCustomData(rapidjson::Value const& doc) {
  auto customDataIt = doc.FindMember(Constants::customData);
  if (customDataIt != doc.MemberEnd() && customDataIt->value.IsObject()) {
    return customDataIt->value;
  }

  return std::nullopt;
}

CustomBeatmapSaveData_ColorNoteData* CustomJSONData::v3::Parser::DeserializeColorNote(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int line = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int layer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  BeatmapSaveData::NoteColorType color = NEJSON::ReadOptionalInt(val, Constants::colorType).value_or(0);
  NoteCutDirection cutDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);
  int angleOffset = NEJSON::ReadOptionalInt(val, "a").value_or(0);
  CustomDataOpt customData = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* note = CustomBeatmapSaveData_ColorNoteData::New_ctor(beat, line, layer, color, cutDirection, angleOffset);
  note->customData = CustomJSONData::JSONWrapperOrNull(customData);

  return note;
}

CustomBeatmapSaveData_BombNoteData* CustomJSONData::v3::Parser::DeserializeBombNote(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int line = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int layer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* bomb = CustomBeatmapSaveData_BombNoteData::New_ctor(beat, line, layer);
  bomb->customData = CustomJSONData::JSONWrapperOrNull(data);

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
  obstacle->customData = CustomJSONData::JSONWrapperOrNull(data);
  return obstacle;
}

CustomBeatmapSaveData_SliderData* CustomJSONData::v3::Parser::DeserializeSlider(rapidjson::Value const& val) {

  BeatmapSaveData::NoteColorType color = NEJSON::ReadOptionalInt(val, Constants::colorType).value_or(0);
  float headBeat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int headLine = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int headLayer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  NoteCutDirection headCutDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);
  float tailBeat = NEJSON::ReadOptionalFloat(val, Constants::tailBeat).value_or(0);
  int tailLine = NEJSON::ReadOptionalInt(val, Constants::tailLine).value_or(0);
  int tailLayer = NEJSON::ReadOptionalInt(val, Constants::tailLayer).value_or(0);
  int sliceCount = NEJSON::ReadOptionalInt(val, "sc").value_or(0);
  float squishAmount = NEJSON::ReadOptionalFloat(val, "s").value_or(0);

  float headControlPointLengthMultiplier = NEJSON::ReadOptionalFloat(val, "mu").value_or(0);
  float tailControlPointLengthMultiplier = NEJSON::ReadOptionalFloat(val, "tmu").value_or(0);
  NoteCutDirection tailCutDirection = NEJSON::ReadOptionalInt(val, "tc").value_or(0);
  SliderMidAnchorMode sliderMidAnchorMode = NEJSON::ReadOptionalInt(val, "m").value_or(0);

  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* slider = CustomBeatmapSaveData_SliderData::New_ctor(
      color, headBeat, headLine, headLayer, headControlPointLengthMultiplier, headCutDirection, tailBeat, tailLine,
      tailLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode);

  slider->customData = CustomJSONData::JSONWrapperOrNull(data);

  return slider;
}

CustomJSONData::v3::CustomBeatmapSaveData_BurstSliderData*
CustomJSONData::v3::Parser::DeserializeBurstSlider(rapidjson::Value const& val) {
  BeatmapSaveData::NoteColorType color = NEJSON::ReadOptionalInt(val, Constants::colorType).value_or(0);
  float headBeat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int headLine = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int headLayer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  NoteCutDirection headCutDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);
  float tailBeat = NEJSON::ReadOptionalFloat(val, Constants::tailBeat).value_or(0);
  int tailLine = NEJSON::ReadOptionalInt(val, Constants::tailLine).value_or(0);
  int tailLayer = NEJSON::ReadOptionalInt(val, Constants::tailLayer).value_or(0);
  int sliceCount = NEJSON::ReadOptionalInt(val, "sc").value_or(0);
  float squishAmount = NEJSON::ReadOptionalFloat(val, "s").value_or(0);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* slider = CustomBeatmapSaveData_BurstSliderData::New_ctor(
      color, headBeat, headLine, headLayer, headCutDirection, tailBeat, tailLine, tailLayer, sliceCount, squishAmount);

  slider->customData = CustomJSONData::JSONWrapperOrNull(data);

  return slider;
}

namespace {
auto DeserializeBpmChangeEventData(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  float bpm = NEJSON::ReadOptionalFloat(val, "m").value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveData::BpmChangeEventData*>(beat, bpm);
}

auto DeserializeRotation(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  BeatmapSaveData::ExecutionTime executionTime = NEJSON::ReadOptionalInt(val, "e").value_or(0);
  float rotation = NEJSON::ReadOptionalFloat(val, "r").value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveData::RotationEventData*>(beat, executionTime, rotation);
}

auto DeserializeWaypoint(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int line = NEJSON::ReadOptionalInt(val, Constants::line).value_or(0);
  int layer = NEJSON::ReadOptionalInt(val, Constants::layer).value_or(0);
  OffsetDirection offsetDirection = NEJSON::ReadOptionalInt(val, Constants::cutDirection).value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveData::WaypointData*>(beat, line, layer, offsetDirection);
}

auto DeserializeBasicEvent(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType eventType =
      NEJSON::ReadOptionalInt(val, "et").value_or(0);
  int value = NEJSON::ReadOptionalInt(val, "i").value_or(0);
  float floatValue = NEJSON::ReadOptionalFloat(val, "f").value_or(0);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* event = CustomBeatmapSaveData_BasicEventData::New_ctor(beat, eventType, value, floatValue);
  event->customData = CustomJSONData::JSONObjectOrNull(data);
  return event;
}

static auto DeserializeColorBoostEventData(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  bool boost = NEJSON::ReadOptionalBool(val, "o").value_or(false);
  CustomDataOpt data = NEJSON::ReadOptionalValue(val, Constants::customData);

  auto* event = CustomJSONData::NewFast<BeatmapSaveData::ColorBoostEventData*>(beat, boost);
  //    event->customData = CustomJSONData::JSONObjectOrNull(data);
  return event;
}

auto DeserializeIndexFilter(rapidjson::Value const& val) {
  BeatmapSaveData::IndexFilter::IndexFilterType type = NEJSON::ReadOptionalInt(val, "f").value_or(0);
  int param0 = NEJSON::ReadOptionalInt(val, "p").value_or(0);
  int param1 = NEJSON::ReadOptionalInt(val, "t").value_or(0);
  bool reversed = NEJSON::ReadOptionalBool(val, "r").value_or(false);
  BeatmapSaveData::IndexFilterRandomType random = NEJSON::ReadOptionalInt(val, "n").value_or(0);
  int seed = NEJSON::ReadOptionalInt(val, "s").value_or(0);
  int chunks = NEJSON::ReadOptionalInt(val, "c").value_or(0);
  float limit = NEJSON::ReadOptionalFloat(val, "l").value_or(0);
  BeatmapSaveData::IndexFilterLimitAlsoAffectsType limitAlsoAffectsType = NEJSON::ReadOptionalInt(val, "d").value_or(0);

  return CustomJSONData::NewFast<BeatmapSaveData::IndexFilter*>(type, param0, param1, reversed, random, seed, chunks,
                                                                limit, limitAlsoAffectsType);
}

auto DeserializeLightColorEventBoxGroup(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int groupId = NEJSON::ReadOptionalInt(val, Constants::groupId).value_or(0);

  auto eventBoxesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightColorEventBox*>(
      val, Constants::eventBoxes, [](rapidjson::Value const& arrIt) {
        /* nullable */
        BeatmapSaveData::IndexFilter* indexFilter =
            NEJSON::ReadOptionalType<BeatmapSaveData::IndexFilter*>(
                arrIt, Constants::indexFilter,
                [](rapidjson::Value const& it) { return DeserializeIndexFilter(it); })
                .value_or(nullptr);

        float beatDistributionParam = NEJSON::ReadOptionalFloat(arrIt, Constants::beatDistributionParam).value_or(0);
        BeatmapSaveData::EventBox::DistributionParamType beatDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, Constants::beatDistributionParamType).value_or(0);

        float brightnessDistributionParam = NEJSON::ReadOptionalFloat(arrIt, "r").value_or(0);

        bool brightnessDistributionShouldAffectFirstBaseEvent = NEJSON::ReadOptionalBool(arrIt, "b").value_or(false);

        BeatmapSaveData::EventBox::DistributionParamType brightnessDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, "t").value_or(0);

        EaseType brightnessDistributionEaseType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        auto lightColorBaseDataListVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightColorBaseData*>(
            arrIt, "e", [](rapidjson::Value const& arrIt) {
              float lightBeat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
              BeatmapSaveData::TransitionType transitionType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);
              BeatmapSaveData::EnvironmentColorType colorType =
                  NEJSON::ReadOptionalInt(arrIt, Constants::colorType).value_or(0);
              float brightness = NEJSON::ReadOptionalFloat(arrIt, "s").value_or(0);
              int strobeFrequency = NEJSON::ReadOptionalInt(arrIt, "f").value_or(0);

              return CustomJSONData::NewFast<BeatmapSaveData::LightColorBaseData*>(
                  lightBeat, transitionType.value__, colorType.value__, brightness, strobeFrequency);
            });
        SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightColorBaseData*, lightColorBaseDataList,
                              CustomJSONData::SpanToSystemList(lightColorBaseDataListVec));

        return CustomJSONData::NewFast<BeatmapSaveData::LightColorEventBox*>(
            indexFilter, beatDistributionParam, beatDistributionParamType, brightnessDistributionParam,
            brightnessDistributionShouldAffectFirstBaseEvent, brightnessDistributionParamType.value__,
            brightnessDistributionEaseType.value__, lightColorBaseDataList.getPtr());
      });

  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightColorEventBox*, eventBoxes,
                        CustomJSONData::SpanToSystemList(eventBoxesVec));

  return CustomJSONData::NewFast<BeatmapSaveData::LightColorEventBoxGroup*>(beat, groupId, eventBoxes.getPtr());
}

auto DeserializeLightRotationEventBoxGroup(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int groupId = NEJSON::ReadOptionalInt(val, Constants::groupId).value_or(0);

  auto eventBoxesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightRotationEventBox*>(
      val, Constants::eventBoxes, [](rapidjson::Value const& arrIt) {
        /* nullable */
        BeatmapSaveData::IndexFilter* indexFilter =
            NEJSON::ReadOptionalType<BeatmapSaveData::IndexFilter*>(
                arrIt, Constants::indexFilter, [](rapidjson::Value const& it) { return DeserializeIndexFilter(it); })
                .value_or(nullptr);

        float beatDistributionParam = NEJSON::ReadOptionalFloat(arrIt, Constants::beatDistributionParam).value_or(0);

        BeatmapSaveData::EventBox::DistributionParamType beatDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, Constants::beatDistributionParamType).value_or(0);

        float rotationDistributionParam = NEJSON::ReadOptionalFloat(arrIt, "s").value_or(0);
        BeatmapSaveData::EventBox::DistributionParamType rotationDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, "t").value_or(0);
        bool rotationDistributionShouldAffectFirstBaseEvent = NEJSON::ReadOptionalBool(arrIt, "b").value_or(false);
        BeatmapSaveData::Axis axis = NEJSON::ReadOptionalInt(arrIt, "a").value_or(0);
        bool flipRotation = NEJSON::ReadOptionalBool(arrIt, "r").value_or(false);
        EaseType rotationDistributionEaseType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        auto lightTranslationBaseDataListVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightRotationBaseData*>(
            arrIt, "l", [](rapidjson::Value const& arrIt) {
              float lightBeat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
              bool usePreviousEventRotationValue = NEJSON::ReadOptionalBool(arrIt, "p").value_or(false);
              BeatmapSaveData::EaseType easeType = NEJSON::ReadOptionalInt(arrIt, "e").value_or(0);
              int loopsCount = NEJSON::ReadOptionalInt(arrIt, "l").value_or(0);
              float rotation = NEJSON::ReadOptionalFloat(arrIt, "r").value_or(0);
              BeatmapSaveData::LightRotationBaseData::RotationDirection rotationDirection =
                  NEJSON::ReadOptionalInt(arrIt, "o").value_or(0);

              return CustomJSONData::NewFast<BeatmapSaveData::LightRotationBaseData*>(
                  lightBeat, usePreviousEventRotationValue, easeType.value__, loopsCount, rotation,
                  rotationDirection.value__);
            });

        SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightRotationBaseData*, lightTranslationBaseDataList,
                              CustomJSONData::SpanToSystemList(lightTranslationBaseDataListVec));

        return CustomJSONData::NewFast<BeatmapSaveData::LightRotationEventBox*>(
            indexFilter, beatDistributionParam, beatDistributionParamType, rotationDistributionParam,
            rotationDistributionParamType.value__, rotationDistributionShouldAffectFirstBaseEvent,
            rotationDistributionEaseType.value__, axis.value__, flipRotation, lightTranslationBaseDataList.getPtr());
      });
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightRotationEventBox*, eventBoxes,
                        CustomJSONData::SpanToSystemList(eventBoxesVec));

  return CustomJSONData::NewFast<BeatmapSaveData::LightRotationEventBoxGroup*>(beat, groupId, eventBoxes.getPtr());
}

auto DeserializeLightTranslationEventBoxGroup(rapidjson::Value const& val) {
  float beat = NEJSON::ReadOptionalFloat(val, Constants::beat).value_or(0);
  int groupId = NEJSON::ReadOptionalInt(val, Constants::groupId).value_or(0);

  auto eventBoxesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightTranslationEventBox*>(
      val, Constants::eventBoxes, [](rapidjson::Value const& arrIt) {
        /* nullable */
        BeatmapSaveData::IndexFilter* indexFilter =
            NEJSON::ReadOptionalType < BeatmapSaveData::IndexFilter * >(
                                                                          arrIt, Constants::indexFilter,
                                                                          [](rapidjson::Value const& it) {
                                                                            return DeserializeIndexFilter(it);
                                                                          })
                                                                          .value_or(nullptr);

        float beatDistributionParam = NEJSON::ReadOptionalFloat(arrIt, Constants::beatDistributionParam).value_or(0);

        BeatmapSaveData::EventBox::DistributionParamType beatDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, Constants::beatDistributionParamType).value_or(0);

        float gapDistributionParam = NEJSON::ReadOptionalFloat(arrIt, "s").value_or(0);

        BeatmapSaveData::EventBox::DistributionParamType gapDistributionParamType =
            NEJSON::ReadOptionalInt(arrIt, "t").value_or(0);

        bool gapDistributionShouldAffectFirstBaseEvent = NEJSON::ReadOptionalBool(arrIt, "b").value_or(false);

        BeatmapSaveData::Axis axis = NEJSON::ReadOptionalInt(arrIt, "a").value_or(0);

        bool flipTranslation = NEJSON::ReadOptionalBool(arrIt, "r").value_or(false);

        EaseType gapDistributionEaseType = NEJSON::ReadOptionalInt(arrIt, "i").value_or(0);

        auto lightTranslationBaseDataListVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightTranslationBaseData*>(
            arrIt, "l", [](rapidjson::Value const& arrIt) {
              float lightBeat = NEJSON::ReadOptionalFloat(arrIt, Constants::beat).value_or(0);
              bool usePreviousEventTransitionValue = NEJSON::ReadOptionalBool(arrIt, "p").value_or(false);
              BeatmapSaveData::EaseType easeType = NEJSON::ReadOptionalInt(arrIt, "e").value_or(0);
              float translation = NEJSON::ReadOptionalFloat(arrIt, "t").value_or(0);

              return CustomJSONData::NewFast<BeatmapSaveData::LightTranslationBaseData*>(
                  lightBeat, usePreviousEventTransitionValue, easeType.value__, translation);
            });

        SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightTranslationBaseData*, lightTranslationBaseDataList,
                              CustomJSONData::SpanToSystemList(lightTranslationBaseDataListVec));

        return CustomJSONData::NewFast<BeatmapSaveData::LightTranslationEventBox*>(
            indexFilter, beatDistributionParam, beatDistributionParamType.value__, gapDistributionParam,
            gapDistributionParamType.value__, gapDistributionShouldAffectFirstBaseEvent, gapDistributionEaseType.value__,
            axis, flipTranslation, lightTranslationBaseDataList.getPtr());
      });
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightTranslationEventBox*, eventBoxes,
                        CustomJSONData::SpanToSystemList(eventBoxesVec));

  return CustomJSONData::NewFast<BeatmapSaveData::LightTranslationEventBoxGroup*>(beat, groupId, eventBoxes.getPtr());
}

auto DeserializeBasicEventTypesForKeyword(rapidjson::Value const& val) {
  std::string_view keyword = NEJSON::ReadOptionalString(val, "k").value_or("");
  auto eventTypesVec =
      NEJSON::ReadArrayOrEmpty<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType>(
          val, "e",
          [](rapidjson::Value const& it) -> BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType {
            return it.GetInt();
          });

  SAFEPTR_VLIST_DEFAULT(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType, eventTypes,
                        CustomJSONData::SpanToSystemList(eventTypesVec));

  return BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword::New_ctor(keyword,
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

  auto bpmEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::BpmChangeEventData*>(
      doc, "bpmEvents", [](rapidjson::Value const& arr) { return DeserializeBpmChangeEventData(arr); });

  auto rotationEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::RotationEventData*>(
      doc, "rotationEvents", [](rapidjson::Value const& arr) { return DeserializeRotation(arr); });

  auto colorNotesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::ColorNoteData*>(
      doc, "colorNotes", [](rapidjson::Value const& arr) { return DeserializeColorNote(arr); });

  auto bombNotesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::BombNoteData*>(
      doc, "bombNotes", [](rapidjson::Value const& arr) { return DeserializeBombNote(arr); });

  auto obstaclesVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::ObstacleData*>(
      doc, "obstacles", [](rapidjson::Value const& arr) { return DeserializeObstacle(arr); });

  auto slidersVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::SliderData*>(
      doc, "sliders", [](rapidjson::Value const& arr) { return DeserializeSlider(arr); });

  auto burstSlidersVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::BurstSliderData*>(
      doc, "burstSliders", [](rapidjson::Value const& arr) { return DeserializeBurstSlider(arr); });

  auto waypointsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::WaypointData*>(
      doc, "waypoints", [](rapidjson::Value const& arr) { return DeserializeWaypoint(arr); });

  auto basicBeatmapEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::BasicEventData*>(
      doc, "basicBeatmapEvents", [](rapidjson::Value const& arr) { return DeserializeBasicEvent(arr); });

  auto colorBoostBeatmapEventsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::ColorBoostEventData*>(
      doc, "colorBoostBeatmapEvents", [](rapidjson::Value const& arr) { return DeserializeColorBoostEventData(arr); });

  auto lightColorEventBoxGroupsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightColorEventBoxGroup*>(
      doc, "lightColorEventBoxGroups",
      [](rapidjson::Value const& arr) { return DeserializeLightColorEventBoxGroup(arr); });

  auto lightRotationEventBoxGroupsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightRotationEventBoxGroup*>(
      doc, "lightRotationEventBoxGroups",
      [](rapidjson::Value const& arr) { return DeserializeLightRotationEventBoxGroup(arr); });

  auto lightTranslationEventBoxGroupsVec = NEJSON::ReadArrayOrEmpty<BeatmapSaveData::LightTranslationEventBoxGroup*>(
      doc, "lightTranslationEventBoxGroups",
      [](rapidjson::Value const& arr) { return DeserializeLightTranslationEventBoxGroup(arr); });

  static_assert(false, "Must update this to parse fx events");

  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::BpmChangeEventData*, bpmEvents,
                        CustomJSONData::SpanToSystemList(bpmEventsVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::RotationEventData*, rotationEvents,
                        CustomJSONData::SpanToSystemList(rotationEventsVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::ColorNoteData*, colorNotes, CustomJSONData::SpanToSystemList(colorNotesVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::BombNoteData*, bombNotes, CustomJSONData::SpanToSystemList(bombNotesVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::ObstacleData*, obstacles, CustomJSONData::SpanToSystemList(obstaclesVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::SliderData*, sliders, CustomJSONData::SpanToSystemList(slidersVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::BurstSliderData*, burstSliders,
                        CustomJSONData::SpanToSystemList(burstSlidersVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::WaypointData*, waypoints, CustomJSONData::SpanToSystemList(waypointsVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::BasicEventData*, basicBeatmapEvents,
                        CustomJSONData::SpanToSystemList(basicBeatmapEventsVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::ColorBoostEventData*, colorBoostBeatmapEvents,
                        CustomJSONData::SpanToSystemList(colorBoostBeatmapEventsVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightColorEventBoxGroup*, lightColorEventBoxGroups,
                        CustomJSONData::SpanToSystemList(lightColorEventBoxGroupsVec));
  SAFEPTR_VLIST_DEFAULT(BeatmapSaveData::LightRotationEventBoxGroup*, lightRotationEventBoxGroups,
                        CustomJSONData::SpanToSystemList(lightRotationEventBoxGroupsVec));
  SAFEPTR_VLIST_DEFAULT(LightTranslationEventBoxGroup*, lightTranslationEventBoxGroups,
                        CustomJSONData::SpanToSystemList(lightTranslationEventBoxGroupsVec));
  SAFEPTR_VLIST(BasicEventTypesWithKeywords::BasicEventTypesForKeyword*, basicEventTypesForKeyword);
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

  auto* beatmap = CustomBeatmapSaveData::New_ctor(
      bpmEvents.getPtr(), rotationEvents.getPtr(), colorNotes.getPtr(), bombNotes.getPtr(),
      obstacles.getPtr(), sliders.getPtr(), burstSliders.getPtr(), waypoints.getPtr(),
      basicBeatmapEvents.getPtr(), colorBoostBeatmapEvents.getPtr(), lightColorEventBoxGroups.getPtr(),
      lightRotationEventBoxGroups.getPtr(), lightTranslationEventBoxGroups,
      BasicEventTypesWithKeywords::New_ctor(basicEventTypesForKeyword.getPtr()), useNormalEventsAsCompatibleEvents);

  beatmap->customData = CustomJSONData::JSONObjectOrNull(dataOpt);
  beatmap->doc = sharedDoc;
  beatmap->customEventsData = customEvents;

  Parser::ParsedEvent.invoke(beatmap);
  return beatmap;
}

static inline const std::array<float, 8> spawnRotations{ -60.0f, -45.0f, -30.0f, -15.0f, 15.0f, 30.0f, 45.0f, 60.0f };

template <typename T> constexpr bool TimeCompare(T const& a, T const& b) {
  return (a->time < b->time);
}

constexpr bool TimeCompareSliders(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SliderData* const& a,
                                  BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SliderData* const& b) {
  return (a->_headTime < b->_headTime);
}

constexpr BeatmapSaveData::NoteColorType
GetNoteColorType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteType noteType) {
  return noteType == BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteType::NoteB
             ? BeatmapSaveData::NoteColorType::ColorB
             : BeatmapSaveData::NoteColorType::ColorA;
}

constexpr BeatmapSaveData::NoteColorType
GetNoteColorType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ColorType colorType) {
  return colorType == BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ColorType::ColorB
             ? BeatmapSaveData::NoteColorType::ColorB
             : BeatmapSaveData::NoteColorType::ColorA;
}

constexpr int
GetHeightForObstacleType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType obstacleType) {
  return obstacleType != BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType::Top ? 5 : 3;
}

constexpr int
GetLayerForObstacleType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType obstacleType) {
  return obstacleType != BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType::Top ? 0 : 2;
}

constexpr float SpawnRotationForEventValue(int index) {
  if (index >= 0 && index < spawnRotations.size()) {
    return spawnRotations[index];
  }

  return 0.0f;
}

CustomBeatmapSaveData* CustomBeatmapSaveData::Convert2_6_0(CustomJSONData::v2::CustomBeatmapSaveData* beatmap) {
  auto startTime = std::chrono::high_resolution_clock::now();
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Initiating converting 2.0.0 to 3.0.0 map");

  SAFEPTR_VLIST_ARG(BeatmapSaveData::ColorNoteData*, colorNotes, beatmap->notes->_size);
  SAFEPTR_VLIST_ARG(BeatmapSaveData::BombNoteData*, bombNotes, beatmap->notes->_size);
  SAFEPTR_VLIST_ARG(BeatmapSaveData::SliderData*, sliders, beatmap->sliders->_size);
  SAFEPTR_VLIST_ARG(BeatmapSaveData::ObstacleData*, obstacles, beatmap->obstacles->_size);
  SAFEPTR_VLIST_ARG(BeatmapSaveData::WaypointData*, waypoints, beatmap->waypoints->_size);

  SAFEPTR_VLIST(BeatmapSaveData::ColorBoostEventData*, colorBoosts);
  SAFEPTR_VLIST(BeatmapSaveData::RotationEventData*, rotationEvents);
  SAFEPTR_VLIST(BeatmapSaveData::BpmChangeEventData*, bpmChanges);
  SAFEPTR_VLIST(BeatmapSaveData::BasicEventData*, basicEvents);

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Sorting");

#define SORT_BEATMAP(Type, list)                                                                                       \
  std::stable_sort(ListW<Type>(list).begin(), ListW<Type>(list).end(), TimeCompare<Type>);

  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteData*, beatmap->notes)
  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleData*, beatmap->obstacles)
  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::WaypointData*, beatmap->waypoints)
  SORT_BEATMAP(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::EventData*, beatmap->events)

  std::stable_sort(beatmap->sliders->_items.ref_to().begin(),
                   beatmap->sliders->_items.ref_to().begin() + beatmap->sliders->_size, TimeCompareSliders);

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting notes");
  for (auto const& n : VList <BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteData*>(beatmap->notes)) {
    auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_NoteData*>(n);

    if (n->type == BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteType::Bomb) {
      auto newNote = CustomBeatmapSaveData_BombNoteData::New_ctor(n->time, n->lineIndex, n->lineLayer.value__);
      newNote->customData = CustomJSONData::JSONWrapperOrNull(customN->customData);

      bombNotes.push_back(newNote);
    } else {
      auto newNote = CustomBeatmapSaveData_ColorNoteData::New_ctor(n->time, n->lineIndex, n->lineLayer.value__,
                                                                   GetNoteColorType(n->type), n->cutDirection, 0);
      newNote->customData = CustomJSONData::JSONWrapperOrNull(customN->customData);
      colorNotes.push_back(newNote);
    }
  }
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converted {} notes and {} bombs", colorNotes.size(), bombNotes.size());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting obstacles");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleData * >(beatmap->obstacles)) {
    auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData*>(n);

    auto obstacle =
        CustomBeatmapSaveData_ObstacleData::New_ctor(n->time, n->lineIndex, GetLayerForObstacleType(n->type),
                                                     n->duration, n->width, GetHeightForObstacleType(n->type));

    obstacle->customData = CustomJSONData::JSONWrapperOrNull(customN->customData);

    obstacles.push_back(obstacle);
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting Sliders");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SliderData*>(beatmap->sliders)) {
    auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_SliderData*>(n);

    auto slider = CustomBeatmapSaveData_SliderData::New_ctor(
        GetNoteColorType(n->colorType), n->_headTime, n->headLineIndex, (int)n->_headLineLayer.value__,
        n->headControlPointLengthMultiplier, n->headCutDirection, n->tailTime, n->tailLineIndex, (int)n->tailLineLayer.value__,
        n->tailControlPointLengthMultiplier, n->tailCutDirection, n->sliderMidAnchorMode);

    slider->customData = CustomJSONData::JSONWrapperOrNull(customN->customData);

    sliders.push_back(slider);
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting waypoints");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::WaypointData*>(beatmap->waypoints)) {
    auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData*>(n);

    auto waypoint =
        CustomJSONData::NewFast<WaypointData*>(n->time, n->lineIndex, n->lineLayer.value__, n->offsetDirection);

    waypoints.push_back(waypoint);
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting events");
  for (auto const& n :
       VList < BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::EventData * >(beatmap->events)) {
    auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_EventData*>(n);

    switch (customN->type) {
    case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event5: {
      auto boost = CustomJSONData::NewFast<ColorBoostEventData*>(n->time, n->value == 1);
      // TODO: Custom data
      colorBoosts.push_back(boost);
      continue;
    }
    case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event14:
    case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event15: {
      auto rotation = CustomJSONData::NewFast<RotationEventData*>(
          n->time,
          n->type == BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event14
              ? BeatmapSaveData::ExecutionTime::Early.value__
              : BeatmapSaveData::ExecutionTime::Late.value__,
          SpawnRotationForEventValue(n->value));
      // TODO: Custom data
      rotationEvents.push_back(rotation);
      continue;
    }
    case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::BpmChange:
      // TODO: Custom data!
      bpmChanges.push_back(CustomJSONData::NewFast<BpmChangeEventData*>(n->time, n->floatValue));
      continue;
    default:
      auto event = v3::CustomBeatmapSaveData_BasicEventData::New_ctor(n->time, n->type, n->value, n->floatValue);
      event->customData = customN->customData;
      basicEvents.push_back(event);
      continue;
    }
  }

  auto keywordsInnerList =
      System::Collections::Generic::List_1<BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>::
          New_ctor(beatmap->specialEventsKeywordFilters->keywords->get_Count());
  VList<BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*> keywords(
      keywordsInnerList);
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Converting specialEventsKeywordFilters {} with size {}",
                                          fmt::ptr(keywordsInnerList), keywords.size());

  for (auto const& n : VList < BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SpecialEventsForKeyword *
                                   >(beatmap->specialEventsKeywordFilters->keywords)) {
    keywords.push_back(
        CustomJSONData::NewFast<BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>(
            n->keyword, n->specialEvents));
  }

  auto basicEventTypesWithKeywords =
      CustomJSONData::NewFast<BeatmapSaveData::BasicEventTypesWithKeywords*>(keywordsInnerList);

  colorNotes->TrimExcess();
  bombNotes->TrimExcess();

  auto v3beatmap =
      CustomBeatmapSaveData::New_ctor(bpmChanges, rotationEvents, colorNotes, bombNotes, obstacles, sliders,
                                      SList<BeatmapSaveData::BurstSliderData*>::New_ctor(), waypoints, basicEvents,
                                      colorBoosts, SList<BeatmapSaveData::LightColorEventBoxGroup*>::New_ctor(),
                                      SList<BeatmapSaveData::LightRotationEventBoxGroup*>::New_ctor(),
                                      SList<BeatmapSaveData::LightTranslationEventBoxGroup*>::New_ctor(),
                                      SList<BeatmapSaveData::FxEventBoxGroup*>::New_ctor(),
                                      BeatmapSaveData::FxEventsCollection::New_ctor(),
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
      "This took {}ms", (int)std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

  return v3beatmap;
}
