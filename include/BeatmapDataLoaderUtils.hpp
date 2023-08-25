#pragma once

#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "GlobalNamespace/BPMChangeBeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapDataLoader_SpecialEventsFilter.hpp"
#include "GlobalNamespace/BeatmapDataLoader_BpmTimeProcessor.hpp"
#include "GlobalNamespace/BeatmapEventTransitionType.hpp"
#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "GlobalNamespace/DefaultEnvironmentEvents.hpp"
#include "GlobalNamespace/EnvironmentColorType.hpp"
#include "GlobalNamespace/BeatmapEventDataBoxGroup.hpp"
#include "GlobalNamespace/LightTranslationBeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightTranslationBaseData.hpp"
#include "GlobalNamespace/LightColorBaseData.hpp"
#include "GlobalNamespace/BeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightColorBeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightRotationBeatmapEventDataBox.hpp"
#include "GlobalNamespace/LightRotationBaseData.hpp"
#include "GlobalNamespace/BeatmapEventDataBoxGroupLists.hpp"
#include "GlobalNamespace/SpawnRotationBeatmapEventData.hpp"
#include "GlobalNamespace/DefaultEnvironmentEventsFactory.hpp"
#include "GlobalNamespace/DefaultEnvironmentEvents_LightGroupEvent.hpp"
#include "GlobalNamespace/BeatmapEventDataBoxGroupFactory.hpp"
#include "GlobalNamespace/BeatmapEventDataBoxGroupList.hpp"
#include "GlobalNamespace/IndexFilter.hpp"
#include "GlobalNamespace/LightGroupSO.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_IndexFilter.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveData_IndexFilterRandomType.hpp"

#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"

#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "CustomEventData.h"
#include "CJDLogger.h"
#include "VList.h"
#include "BeatmapFieldUtils.hpp"

// this header exists purely for organizing

namespace CustomJSONData {
using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;

inline JSONWrapper* ToJsonWrapper(v3::CustomDataOpt const& val) {
  auto wrapper = JSONWrapper::New_ctor();
  wrapper->value = val;

  return wrapper;
}

inline JSONWrapperUTF16* ToJsonWrapper(v3::CustomDataOptUTF16 const& val) {
  auto wrapper = JSONWrapperUTF16::New_ctor();
  wrapper->value = val;

  return wrapper;
}

constexpr CustomNoteData* CreateCustomBasicNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
                                                    ColorType colorType, NoteCutDirection cutDirection,
                                                    JSONWrapper* customData) {
  auto b = CustomNoteData::New_ctor(time, lineIndex, (NoteLineLayer)noteLineLayer, (NoteLineLayer)noteLineLayer,
                                    (NoteData::GameplayType)NoteData::GameplayType::Normal,
                                    (NoteData::ScoringType)NoteData::ScoringType::Normal, (ColorType)colorType,
                                    (NoteCutDirection)cutDirection, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f, 1.0f);

  b->customData = customData->GetCopy();

  return b;
}

constexpr CustomNoteData* CreateCustomBombNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
                                                   JSONWrapper* customData) {
  auto b = CustomNoteData::New_ctor(time, lineIndex, noteLineLayer, noteLineLayer,
                                    (NoteData::GameplayType)NoteData::GameplayType::Bomb,
                                    (NoteData::ScoringType)NoteData::ScoringType::NoScore, (ColorType)ColorType::None,
                                    (NoteCutDirection)NoteCutDirection::None, 0.0f, 0.0f, lineIndex, 0.0f, 0.0f, 1.0f);

  b->customData = customData->GetCopy();

  return b;
}

constexpr CustomNoteData* CreateCustomBurstNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
                                                    NoteLineLayer beforeJumpNoteLineLayer, ColorType colorType,
                                                    NoteCutDirection cutDirection, float cutSfxVolumeMultiplier,
                                                    JSONWrapper* customData) {
  auto b = CustomNoteData::New_ctor(time, lineIndex, noteLineLayer, beforeJumpNoteLineLayer,
                                    (NoteData::GameplayType)NoteData::GameplayType::BurstSliderElement,
                                    (NoteData::ScoringType)NoteData::ScoringType::BurstSliderElement, colorType,
                                    cutDirection, 0, 0, lineIndex, 0, 0, cutSfxVolumeMultiplier);

  b->customData = customData->GetCopy();

  return b;
}

constexpr auto CustomSliderData_CreateCustomBurstSliderData(
    ColorType colorType, float headTime, int headLineIndex, NoteLineLayer headLineLayer,
    NoteLineLayer headBeforeJumpLineLayer, NoteCutDirection headCutDirection, float tailTime, int tailLineIndex,
    NoteLineLayer tailLineLayer, NoteLineLayer tailBeforeJumpLineLayer, NoteCutDirection tailCutDirection,
    int sliceCount, float squishAmount, JSONWrapper* customData) {
  auto slider =
      CustomSliderData::New_ctor((SliderData::Type)SliderData::Type::Burst, colorType, false, headTime, headLineIndex,
                                 headLineLayer, headBeforeJumpLineLayer, 0.0f, headCutDirection, 0.0f, false, tailTime,
                                 tailLineIndex, tailLineLayer, tailBeforeJumpLineLayer, 0.0f, tailCutDirection, 0.0f,
                                 (SliderMidAnchorMode)SliderMidAnchorMode::Straight, sliceCount, squishAmount);
  slider->customData = customData->GetCopy();

  return slider;
}

constexpr auto CustomSliderData_CreateCustomSliderData(
    ColorType colorType, float headTime, int headLineIndex, NoteLineLayer headLineLayer,
    NoteLineLayer headBeforeJumpLineLayer, float headControlPointLengthMultiplier, NoteCutDirection headCutDirection,
    float tailTime, int tailLineIndex, NoteLineLayer tailLineLayer, NoteLineLayer tailBeforeJumpLineLayer,
    float tailControlPointLengthMultiplier, NoteCutDirection tailCutDirection, SliderMidAnchorMode midAnchorMode,
    JSONWrapper* customData) {
  auto slider = CustomSliderData::New_ctor(
      (SliderData::Type)SliderData::Type::Normal, colorType, false, headTime, headLineIndex, headLineLayer,
      headBeforeJumpLineLayer, headControlPointLengthMultiplier, headCutDirection, 0.0f, false, tailTime, tailLineIndex,
      tailLineLayer, tailBeforeJumpLineLayer, tailControlPointLengthMultiplier, tailCutDirection, 0.0f, midAnchorMode,
      0, 1.0f);
  slider->customData = customData->GetCopy();

  return slider;
}

template <typename T> constexpr bool TimeCompare(T const a, T const b) {
  return (BeatmapSaveDataItem_GetBeat(a) < BeatmapSaveDataItem_GetBeat(b));
}

constexpr bool TimeCompare(CustomJSONData::CustomEventSaveData const& a, CustomJSONData::CustomEventSaveData const& b) {
  return (a.time < b.time);
}

constexpr NoteLineLayer GetNoteLineLayer(int lineLayer) {
  switch (lineLayer) {
  case 0:
    return NoteLineLayer::Base;
  case 1:
    return NoteLineLayer::Upper;
  case 2:
    return NoteLineLayer::Top;
  default:
    return NoteLineLayer::Base;
  }
}

constexpr ColorType ConvertColorType(BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType noteType) {
  if (noteType == BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType::ColorA) {
    return ColorType::ColorA;
  }
  if (noteType != BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType::ColorB) {
    return ColorType::None;
  }
  return ColorType::ColorB;
}

constexpr EnvironmentColorType
ConvertColorType(BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType environmentColorType) {
  switch (environmentColorType) {
  case BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color0:
    return EnvironmentColorType::Color0;
  case BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color1:
    return EnvironmentColorType::Color1;
  case BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::ColorWhite:
    return EnvironmentColorType::ColorW;
  default:
    return EnvironmentColorType::Color0;
  }
}

constexpr BeatmapEventTransitionType
ConvertBeatmapEventTransitionType(BeatmapSaveDataVersion3::BeatmapSaveData::TransitionType transitionType) {
  switch (transitionType) {
  case BeatmapSaveDataVersion3::BeatmapSaveData::TransitionType::Instant:
    return BeatmapEventTransitionType::Instant;
  case BeatmapSaveDataVersion3::BeatmapSaveData::TransitionType::Interpolate:
    return BeatmapEventTransitionType::Interpolate;
  case BeatmapSaveDataVersion3::BeatmapSaveData::TransitionType::Extend:
    return BeatmapEventTransitionType::Extend;
  default:
    return BeatmapEventTransitionType::Instant;
  }
}

constexpr LightAxis ConvertAxis(BeatmapSaveDataVersion3::BeatmapSaveData::Axis axis) {
  switch (axis) {
  case BeatmapSaveDataVersion3::BeatmapSaveData::Axis::X:
    return LightAxis::X;
  case BeatmapSaveDataVersion3::BeatmapSaveData::Axis::Y:
    return LightAxis::Y;
  case BeatmapSaveDataVersion3::BeatmapSaveData::Axis::Z:
    return LightAxis::Z;
  default:
    return LightAxis::Z;
  }
}

constexpr EaseType ConvertEaseType(BeatmapSaveDataVersion3::BeatmapSaveData::EaseType easeType) {
  switch (easeType) {
  case BeatmapSaveDataVersion3::BeatmapSaveData::EaseType::None:
    return EaseType::None;
  case BeatmapSaveDataVersion3::BeatmapSaveData::EaseType::Linear:
    return EaseType::Linear;
  case BeatmapSaveDataVersion3::BeatmapSaveData::EaseType::InQuad:
    return EaseType::InQuad;
  case BeatmapSaveDataVersion3::BeatmapSaveData::EaseType::OutQuad:
    return EaseType::OutQuad;
  case BeatmapSaveDataVersion3::BeatmapSaveData::EaseType::InOutQuad:
    return EaseType::InOutQuad;
  }

  SAFE_ABORT_MSG("Message");
}

constexpr NoteLineLayer ConvertNoteLineLayer(int layer) {
  switch (layer) {
  case 0:
    return NoteLineLayer::Base;
  case 1:
    return NoteLineLayer::Upper;
  case 2:
    return NoteLineLayer::Top;
  default:
    return NoteLineLayer::Base;
  }
}

constexpr SliderData::Type ConvertSliderType(BeatmapSaveDataVersion3::BeatmapSaveData::SliderType sliderType) {
  if (sliderType == BeatmapSaveDataVersion3::BeatmapSaveData::SliderType::Normal) {
    return SliderData::Type::Normal;
  }
  if (sliderType != BeatmapSaveDataVersion3::BeatmapSaveData::SliderType::Burst) {
    return SliderData::Type::Normal;
  }
  return SliderData::Type::Burst;
}

constexpr LightRotationDirection ConvertRotationOrientation(
    BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData::RotationDirection rotationDirection) {
  switch (rotationDirection) {
  case BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData::RotationDirection::Automatic:
    return LightRotationDirection::Automatic;
  case BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData::RotationDirection::Clockwise:
    return LightRotationDirection::Clockwise;
  case BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData::RotationDirection::Counterclockwise:
    return LightRotationDirection::Counterclockwise;
  default:
    return LightRotationDirection::Automatic;
  }
}

template <typename To, typename From, typename... TArgs>
requires(std::is_pointer_v<To>&& std::is_pointer_v<From>) struct CppConverter {
  std::unordered_map<Il2CppClass*, std::function<To(From, TArgs...)>> converters;

  template <typename U, typename F>
  requires(std::is_pointer_v<U>&& std::is_convertible_v<U, From>&&
               std::is_convertible_v<F, std::function<To(U, TArgs...)>>) 
               constexpr void AddConverter(F&& o) {
    converters[classof(U)] = [o](From const& t, TArgs const&... args) constexpr {
      return o(static_cast<U>(t), args...);
    };
  }

  constexpr To ProcessItem(From o, TArgs&... args) const {
    if (!o) {
      return {};
    }
    auto uKlass = o->klass;
    auto it = converters.find(uKlass);
    if (it == converters.end()) {
      //                for (auto const& [klass, func] : converters) {
      //                    if (il2cpp_functions::class_is_assignable_from(klass, uKlass)) {
      //                        return func(reinterpret_cast<T>(o));
      //                    }
      //                }
      return {};
    }

    return (it->second)(o, args...);
  }
};

struct BpmChangeData {
  float const bpmChangeStartTime;
  float const bpmChangeStartBpmTime;
  float const bpm;

  constexpr BpmChangeData(float const bpmChangeStartTime, float const bpmChangeStartBpmTime, float const bpm)
      : bpmChangeStartTime(bpmChangeStartTime), bpmChangeStartBpmTime(bpmChangeStartBpmTime), bpm(bpm) {}
};

struct BpmTimeProcessor {
  std::vector<BpmChangeData> bpmChangeDataList;
  int currentBpmChangesDataIdx;

  BpmTimeProcessor(float startBpm,
                   VList<BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData*> bpmEventsSaveData) {
    bool hasBpm = bpmEventsSaveData.size() > 0 && bpmEventsSaveData[0]->b == 0;
    if (hasBpm) {
      startBpm = bpmEventsSaveData[0]->m;
    }

    bpmChangeDataList.reserve(bpmEventsSaveData.size() + 1);
    bpmChangeDataList.emplace_back(0, 0, startBpm);

    for (auto const& v : bpmEventsSaveData) {
      if (!v || hasBpm) {
        hasBpm = false; // skip first
        continue;
      }

      auto const& bpmChangeData = bpmChangeDataList.back();
      float beat = BeatmapSaveDataItem_GetBeat(v);
      float bpm = v->m;
      float bpmChangeStartTime =
          bpmChangeData.bpmChangeStartTime + (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
      bpmChangeDataList.emplace_back(bpmChangeStartTime, beat, bpm);
    }
  }

  void Reset() {
    currentBpmChangesDataIdx = 0;
  }

  constexpr float ConvertBeatToTime(float beat) {
    //            while (currentBpmChangesDataIdx > 0)
    //            {
    //                if (currentBpmChangesDataIdx == 0) break;
    //                if (bpmChangeDataList[currentBpmChangesDataIdx].bpmChangeStartBpmTime < beat)
    //                {
    //                    break;
    //                }
    //                currentBpmChangesDataIdx--;
    //            }
    //            while (currentBpmChangesDataIdx < bpmChangeDataList.size() - 1 &&
    //            bpmChangeDataList[currentBpmChangesDataIdx + 1].bpmChangeStartBpmTime < beat)
    //            {
    //                currentBpmChangesDataIdx++;
    //            }
    //            currentBpmChangesDataIdx = std::min((int) bpmChangeDataList.size() - 1, currentBpmChangesDataIdx);
    //            auto const& bpmChangeData = bpmChangeDataList[currentBpmChangesDataIdx];
    //            return bpmChangeData.bpmChangeStartTime + (beat - bpmChangeData.bpmChangeStartBpmTime) /
    //            bpmChangeData.bpm * 60.0f;

    int num = 0;
    while (num < bpmChangeDataList.size() - 1 && bpmChangeDataList[num + 1].bpmChangeStartBpmTime < beat) {
      num++;
    }
    auto const& bpmChangeData = bpmChangeDataList[num];
    return bpmChangeData.bpmChangeStartTime + (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
  }
};

constexpr IndexFilter* IndexFilterConvertor_Convert(BeatmapSaveData::IndexFilter* indexFilter, int groupSize) {
  int num = (indexFilter->get_chunks() == 0) ? 1 : (int)std::ceil((float)groupSize / (float)indexFilter->get_chunks());
  int num2 = std::ceil((float)groupSize / (float)num);
  auto type = indexFilter->get_type();
  if (type != BeatmapSaveDataVersion3::BeatmapSaveData::IndexFilter::IndexFilterType::Division) {
    if (type != BeatmapSaveDataVersion3::BeatmapSaveData::IndexFilter::IndexFilterType::StepAndOffset) {
      return nullptr;
    }
    int param = indexFilter->get_param0();
    int param2 = indexFilter->get_param1();
    int num3 = num2 - param;
    if (num3 <= 0) {
      CJDLogger::Logger.fmtLog<LogLevel::ERR>("Error {}!", num3);
      throw il2cpp_utils::exceptions::StackTraceException("ArgumentOutOfRangeException");
    }
    int count = (param2 == 0) ? 1 : (int)std::ceil((float)num3 / (float)param2);
    if (indexFilter->get_reversed()) {
      return CustomJSONData::NewFast<IndexFilter*>(
          num2 - 1 - param, -param2, count, groupSize, (IndexFilter::IndexFilterRandomType)indexFilter->get_random(),
          indexFilter->get_seed(), num, indexFilter->get_limit(),
          (IndexFilter::IndexFilterLimitAlsoAffectType)indexFilter->get_limitAlsoAffectsType());
    }
    return CustomJSONData::NewFast<IndexFilter*>(
        param, param2, count, groupSize, (IndexFilter::IndexFilterRandomType)indexFilter->get_random(),
        indexFilter->get_seed(), num, indexFilter->get_limit(),
        (IndexFilter::IndexFilterLimitAlsoAffectType)indexFilter->get_limitAlsoAffectsType());
  } else {
    int param3 = indexFilter->get_param0();
    int param4 = indexFilter->get_param1();
    int num4 = std::ceil((float)num2 / (float)param3);
    if (indexFilter->get_reversed()) {
      int num5 = num2 - num4 * param4 - 1;
      return CustomJSONData::NewFast<IndexFilter*>(
          num5, std::max(0, num5 - num4 + 1), groupSize, (IndexFilter::IndexFilterRandomType)indexFilter->get_random(),
          indexFilter->get_seed(), num, indexFilter->get_limit(),
          (IndexFilter::IndexFilterLimitAlsoAffectType)indexFilter->get_limitAlsoAffectsType());
    }
    int num6 = num4 * param4;
    return CustomJSONData::NewFast<IndexFilter*>(
        num6, std::min(num2 - 1, num6 + num4 - 1), groupSize,
        (IndexFilter::IndexFilterRandomType)indexFilter->get_random(), indexFilter->get_seed(), num,
        indexFilter->get_limit(), (IndexFilter::IndexFilterLimitAlsoAffectType)indexFilter->get_limitAlsoAffectsType());
  }
}

constexpr BeatmapEventDataBox::DistributionParamType
DistributionParamType_Convert(BeatmapSaveData::EventBox::DistributionParamType distributionParamType) {
  if (distributionParamType == BeatmapSaveDataVersion3::BeatmapSaveData::EventBox::DistributionParamType::Wave) {
    return BeatmapEventDataBox::DistributionParamType::Wave;
  }
  if (distributionParamType != BeatmapSaveDataVersion3::BeatmapSaveData::EventBox::DistributionParamType::Step) {
    throw std::runtime_error("distributionParamType error!");
  }
  return BeatmapEventDataBox::DistributionParamType::Step;
}

constexpr LightColorBaseData* LightColorBaseData_Convert(BeatmapSaveData::LightColorBaseData* saveData) {
  return CustomJSONData::NewFast<LightColorBaseData*>(
      BeatmapSaveDataItem_GetBeat(saveData),
      ConvertBeatmapEventTransitionType(LightColorBaseData_GetTransitionType(saveData)),
      ConvertColorType(LightColorBaseData_GetColorType(saveData)), LightColorBaseData_GetBrightness(saveData),
      LightColorBaseData_GetStrobeFrequency(saveData));
}

constexpr LightRotationBaseData* LightRotationBaseData_Convert(BeatmapSaveData::LightRotationBaseData* saveData) {
  return CustomJSONData::NewFast<LightRotationBaseData*>(
      saveData->b, saveData->get_usePreviousEventRotationValue(), ConvertEaseType(saveData->e),
      LightRotationBaseData_GetRotation(saveData), LightRotationBaseData_GetLoopsCount(saveData),
      ConvertRotationOrientation(saveData->o));
}

constexpr LightTranslationBaseData*
LightTranslationBaseData_Convert(BeatmapSaveData::LightTranslationBaseData* saveData) {
  return CustomJSONData::NewFast<LightTranslationBaseData*>(
      saveData->b, saveData->get_usePreviousEventTranslationValue(), ConvertEaseType(saveData->e), saveData->t);
}

struct EventBoxGroupConvertor {
  EventBoxGroupConvertor(EnvironmentLightGroups* lightGroups) {
    this->lightGroups = lightGroups;
    dataConvertor.AddConverter<BeatmapSaveData::LightColorEventBox*>([](BeatmapSaveData::LightColorEventBox* saveData,
                                                                        LightGroupSO* lightGroupData) {
      auto indexFilter = IndexFilterConvertor_Convert(saveData->f, lightGroupData->numberOfElements);
      auto saveDataList = reinterpret_cast<
          System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData*>*>(
          saveData->e);
      VList<LightColorBaseData*> list(saveDataList->get_Count());

      for (auto saveData2 : VList(saveDataList)) {
        list.push_back(LightColorBaseData_Convert(saveData2));
      }

      auto beatDistributionParamType = DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
      auto brightnessDistributionParamType =
          DistributionParamType_Convert(saveData->get_brightnessDistributionParamType());
      return CustomJSONData::NewFast<LightColorBeatmapEventDataBox*>(
          indexFilter, EventBox_GetBeatDistributionParam(saveData), beatDistributionParamType,
          saveData->get_brightnessDistributionParam(), brightnessDistributionParamType,
          saveData->get_brightnessDistributionShouldAffectFirstBaseEvent(),
          saveData->get_brightnessDistributionEaseType().value,
          reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightColorBaseData*>*>(list.getInner()));
    });

    dataConvertor.AddConverter<BeatmapSaveData::LightRotationEventBox*>([](BeatmapSaveData::LightRotationEventBox*
                                                                               saveData,
                                                                           LightGroupSO* lightGroupData) {
      auto indexFilter = IndexFilterConvertor_Convert(saveData->f, lightGroupData->numberOfElements);
      auto collection = reinterpret_cast<
          System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData*>*>(
          saveData->get_lightRotationBaseDataList());

      VList<LightRotationBaseData*> list(collection->get_Count());

      for (auto saveData2 : VList(collection)) {
        list.push_back(LightRotationBaseData_Convert(saveData2));
      }

      auto beatDistributionParamType = DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
      auto rotationDistributionParamType = DistributionParamType_Convert(saveData->get_rotationDistributionParamType());
      return CustomJSONData::NewFast<LightRotationBeatmapEventDataBox*>(
          indexFilter, EventBox_GetBeatDistributionParam(saveData), beatDistributionParamType, ConvertAxis(saveData->a),
          saveData->get_flipRotation(), saveData->get_rotationDistributionParam(), rotationDistributionParamType,
          saveData->get_rotationDistributionShouldAffectFirstBaseEvent(),
          saveData->get_rotationDistributionEaseType().value,
          reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightRotationBaseData*>*>(list.getInner()));
    });
    dataConvertor.AddConverter<BeatmapSaveData::LightTranslationEventBox*>([](BeatmapSaveData::LightTranslationEventBox*
                                                                                  saveData,
                                                                              LightGroupSO* lightGroupData) {
      auto indexFilter = IndexFilterConvertor_Convert(saveData->f, lightGroupData->numberOfElements);

      auto collection = reinterpret_cast<
          System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightTranslationBaseData*>*>(
          saveData->get_lightTranslationBaseDataList());

      VList<LightTranslationBaseData*> list(collection->get_Count());

      for (auto saveData2 : VList(collection)) {
        list.push_back(LightTranslationBaseData_Convert(saveData2));
      }

      auto beatDistributionParamType = DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
      auto gapDistributionParamType = DistributionParamType_Convert(saveData->get_gapDistributionParamType());

      return CustomJSONData::NewFast<LightTranslationBeatmapEventDataBox*>(
          indexFilter, EventBox_GetBeatDistributionParam(saveData), beatDistributionParamType, ConvertAxis(saveData->a),
          saveData->get_flipTranslation(), saveData->get_gapDistributionParam(), gapDistributionParamType,
          saveData->get_gapDistributionShouldAffectFirstBaseEvent(),
          ConvertEaseType(saveData->get_gapDistributionEaseType()),
          reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightTranslationBaseData*>*>(list.getInner()));
    });
  }

  BeatmapEventDataBoxGroup*
  Convert(BeatmapSaveDataVersion3::BeatmapSaveData::EventBoxGroup* eventBoxGroupSaveData) const {
    auto dataForGroup = this->lightGroups->GetDataForGroup(eventBoxGroupSaveData->get_groupId());
    if (dataForGroup == nullptr || dataForGroup->m_CachedPtr.m_value == nullptr) {
      return nullptr;
    }

    auto collection =
        reinterpret_cast<System::Collections::Generic::List_1<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox*>*>(
            eventBoxGroupSaveData->get_baseEventBoxes());
    VList<BeatmapEventDataBox*> list(collection->get_Count());

    for (auto item : VList<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox*>(collection)) {
      auto beatmapEventDataBox = dataConvertor.ProcessItem(item, dataForGroup);
      if (beatmapEventDataBox != nullptr) {
        list.push_back(beatmapEventDataBox);
      }
    }

    return CustomJSONData::NewFast<BeatmapEventDataBoxGroup*>(
        eventBoxGroupSaveData->b,
        reinterpret_cast<IReadOnlyCollection_1<::GlobalNamespace::BeatmapEventDataBox*>*>(list.getInner()));
  }

  CppConverter<BeatmapEventDataBox*, BeatmapSaveDataVersion3::BeatmapSaveData::EventBox*, LightGroupSO*> dataConvertor;

  EnvironmentLightGroups* lightGroups;
};
} // namespace CustomJSONData