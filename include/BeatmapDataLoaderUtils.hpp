#pragma once

#include "BeatmapSaveDataVersion2_6_0AndEarlier/EventData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/ColorType.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/ObstacleType.hpp"
#include "BeatmapDataLoaderVersion2_6_0AndEarlier/BeatmapDataLoader.hpp"

#include "BeatmapSaveDataCommon/NoteLineLayer.hpp"
#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "GlobalNamespace/BPMChangeBeatmapEventData.hpp"
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
#include "GlobalNamespace/BeatmapEventDataBoxGroupList.hpp"
#include "GlobalNamespace/EnvironmentLightGroups.hpp"
#include "GlobalNamespace/IndexFilter.hpp"
#include "GlobalNamespace/LightGroupSO.hpp"
#include "GlobalNamespace/ILightGroup.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"

#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "GlobalNamespace/IEnvironmentLightGroups.hpp"

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
  auto* wrapper = JSONWrapper::New_ctor();
  wrapper->value = val;

  return wrapper;
}

inline JSONWrapperUTF16* ToJsonWrapper(v3::CustomDataOptUTF16 const& val) {
  auto* wrapper = JSONWrapperUTF16::New_ctor();
  wrapper->value = val;

  return wrapper;
}

static CustomNoteData* CreateCustomBasicNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
                                                    ColorType colorType, NoteCutDirection cutDirection,
                                                    JSONWrapper* customData) {
  auto* b = CustomNoteData::New_ctor(time, lineIndex, NoteLineLayer(noteLineLayer), NoteLineLayer(noteLineLayer),
                                     NoteData::GameplayType(NoteData::GameplayType::Normal),
                                     NoteData::ScoringType(NoteData::ScoringType::Normal), ColorType(colorType),
                                     NoteCutDirection(cutDirection), 0.0F, 0.0F, lineIndex, 0.0F, 0.0F, 1.0F);

  b->customData = customData->GetCopy();

  return b;
}

static CustomNoteData* CreateCustomBombNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
                                                   JSONWrapper* customData) {
  auto* b = CustomNoteData::New_ctor(time, lineIndex, noteLineLayer, noteLineLayer,
                                     NoteData::GameplayType(NoteData::GameplayType::Bomb),
                                     NoteData::ScoringType(NoteData::ScoringType::NoScore), ColorType(ColorType::None),
                                     NoteCutDirection(NoteCutDirection::None), 0.0F, 0.0F, lineIndex, 0.0F, 0.0F, 1.0F);

  b->customData = customData->GetCopy();

  return b;
}

static CustomNoteData* CreateCustomBurstNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
                                                    NoteLineLayer beforeJumpNoteLineLayer, ColorType colorType,
                                                    NoteCutDirection cutDirection, float cutSfxVolumeMultiplier,
                                                    JSONWrapper* customData) {
  auto* b = CustomNoteData::New_ctor(time, lineIndex, noteLineLayer, beforeJumpNoteLineLayer,
                                     NoteData::GameplayType(NoteData::GameplayType::BurstSliderElement),
                                     NoteData::ScoringType(NoteData::ScoringType::BurstSliderElement), colorType,
                                     cutDirection, 0, 0, lineIndex, 0, 0, cutSfxVolumeMultiplier);

  b->customData = customData->GetCopy();

  return b;
}

static auto CustomSliderData_CreateCustomBurstSliderData(
    ColorType colorType, float headTime, int headLineIndex, NoteLineLayer headLineLayer,
    NoteLineLayer headBeforeJumpLineLayer, NoteCutDirection headCutDirection, float tailTime, int tailLineIndex,
    NoteLineLayer tailLineLayer, NoteLineLayer tailBeforeJumpLineLayer, NoteCutDirection tailCutDirection,
    int sliceCount, float squishAmount, JSONWrapper* customData) {
  auto* slider =
      CustomSliderData::New_ctor(GlobalNamespace::SliderData::Type::Burst, colorType, false, headTime, headLineIndex,
                                 headLineLayer, headBeforeJumpLineLayer, 0.0F, headCutDirection, 0.0F, false, tailTime,
                                 tailLineIndex, tailLineLayer, tailBeforeJumpLineLayer, 0.0F, tailCutDirection, 0.0F,
                                 SliderMidAnchorMode(SliderMidAnchorMode::Straight), sliceCount, squishAmount);
  slider->customData = customData->GetCopy();

  return slider;
}

static auto CustomSliderData_CreateCustomSliderData(
    ColorType colorType, float headTime, int headLineIndex, NoteLineLayer headLineLayer,
    NoteLineLayer headBeforeJumpLineLayer, float headControlPointLengthMultiplier, NoteCutDirection headCutDirection,
    float tailTime, int tailLineIndex, NoteLineLayer tailLineLayer, NoteLineLayer tailBeforeJumpLineLayer,
    float tailControlPointLengthMultiplier, NoteCutDirection tailCutDirection, SliderMidAnchorMode midAnchorMode,
    JSONWrapper* customData) {
  auto* slider = CustomSliderData::New_ctor(
      GlobalNamespace::SliderData::Type::Normal, colorType, false, headTime, headLineIndex, headLineLayer,
      headBeforeJumpLineLayer, headControlPointLengthMultiplier, headCutDirection, 0.0F, false, tailTime, tailLineIndex,
      tailLineLayer, tailBeforeJumpLineLayer, tailControlPointLengthMultiplier, tailCutDirection, 0.0F, midAnchorMode,
      0, 1.0F);
  slider->customData = customData->GetCopy();

  return slider;
}

template <typename T>
constexpr bool TimeCompare(T const a, T const b) {
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

constexpr ColorType ConvertColorType(BeatmapSaveDataVersion2_6_0AndEarlier::ColorType colorType) {
  if (colorType == BeatmapSaveDataVersion2_6_0AndEarlier::ColorType::ColorA) {
    return ColorType::ColorA;
  }
  if (colorType != BeatmapSaveDataVersion2_6_0AndEarlier::ColorType::ColorB) {
    return ColorType::None;
  }
  return ColorType::ColorB;
}

constexpr ColorType ConvertColorType(BeatmapSaveDataCommon::NoteColorType noteType) {
  if (noteType == BeatmapSaveDataCommon::NoteColorType::ColorA) {
    return ColorType::ColorA;
  }
  if (noteType != BeatmapSaveDataCommon::NoteColorType::ColorB) {
    return ColorType::None;
  }
  return ColorType::ColorB;
}

constexpr BasicBeatmapEventType ConvertBasicBeatmapEventType(BeatmapSaveDataCommon::BeatmapEventType beatmapEventType) {
  return beatmapEventType.value__;
}

constexpr int GetHeightForObstacleType(BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleType obstacleType) {
  if (obstacleType != BeatmapSaveDataVersion2_6_0AndEarlier::ObstacleType::Top) {
    return 5;
  }
  return 3;
}

constexpr int SpawnRotationForEventValue(int index) {
  if (index >= 0 &&
      index <
          BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::BasicEventConverter::getStaticF__spawnRotations()
              .size()) {
    return BeatmapDataLoaderVersion2_6_0AndEarlier::BeatmapDataLoader::BasicEventConverter::getStaticF__spawnRotations()
        [index];
  }
  return 0.0f;
}

constexpr GlobalNamespace::NoteCutDirection
ConvertNoteCutDirection(BeatmapSaveDataCommon::NoteCutDirection noteCutDirection) {
  GlobalNamespace::NoteCutDirection noteCutDirection2;
	switch (noteCutDirection)
	{
	case BeatmapSaveDataCommon::NoteCutDirection::Up:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::Up;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::Down:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::Down;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::Left:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::Left;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::Right:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::Right;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::UpLeft:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::UpLeft;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::UpRight:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::UpRight;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::DownLeft:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::DownLeft;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::DownRight:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::DownRight;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::Any:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::Any;
		break;
	case BeatmapSaveDataCommon::NoteCutDirection::None:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::None;
		break;
	default:
		noteCutDirection2 = GlobalNamespace::NoteCutDirection::Any;
		break;
	}
	return noteCutDirection2;
}

constexpr EnvironmentColorType ConvertColorType(BeatmapSaveDataCommon::EnvironmentColorType environmentColorType) {
  switch (environmentColorType) {
  case BeatmapSaveDataCommon::EnvironmentColorType::Color0:
    return EnvironmentColorType::Color0;
  case BeatmapSaveDataCommon::EnvironmentColorType::Color1:
    return EnvironmentColorType::Color1;
  case BeatmapSaveDataCommon::EnvironmentColorType::ColorWhite:
    return EnvironmentColorType::ColorW;
  default:
    return EnvironmentColorType::Color0;
  }
}

constexpr BeatmapEventTransitionType ConvertBeatmapEventTransitionType(BeatmapSaveDataVersion3::TransitionType transitionType) {
  switch (transitionType) {
  case BeatmapSaveDataVersion3::TransitionType::Instant:
    return BeatmapEventTransitionType::Instant;
  case BeatmapSaveDataVersion3::TransitionType::Interpolate:
    return BeatmapEventTransitionType::Interpolate;
  case BeatmapSaveDataVersion3::TransitionType::Extend:
    return BeatmapEventTransitionType::Extend;
  default:
    return BeatmapEventTransitionType::Instant;
  }
}

constexpr EaseType ConvertBeatmapEventTransitionTypeToEaseType(BeatmapSaveDataVersion3::TransitionType transitionType) {
  if (transitionType != BeatmapSaveDataVersion3::TransitionType::Interpolate) {
    return EaseType::None;
	}
  return EaseType::Linear;
}


constexpr LightAxis ConvertAxis(BeatmapSaveDataCommon::Axis axis) {
  switch (axis) {
  case BeatmapSaveDataCommon::Axis::X:
    return LightAxis::X;
  case BeatmapSaveDataCommon::Axis::Y:
    return LightAxis::Y;
  case BeatmapSaveDataCommon::Axis::Z:
    return LightAxis::Z;
  default:
    return LightAxis::Z;
  }
}

// BeatmapDataLoader.ConverEaseType
constexpr EaseType ConvertEaseType(BeatmapSaveDataCommon::EaseType easeType) {
  switch (easeType) {
  case BeatmapSaveDataCommon::EaseType::None:
    return EaseType::None;
  case BeatmapSaveDataCommon::EaseType::Linear:
    return EaseType::Linear;
  case BeatmapSaveDataCommon::EaseType::InQuad:
    return EaseType::InQuad;
  case BeatmapSaveDataCommon::EaseType::OutQuad:
    return EaseType::OutQuad;
  case BeatmapSaveDataCommon::EaseType::InOutQuad:
    return EaseType::InOutQuad;
  case BeatmapSaveDataCommon::EaseType::InSine:
  case BeatmapSaveDataCommon::EaseType::OutSine:
  case BeatmapSaveDataCommon::EaseType::InOutSine:
  case BeatmapSaveDataCommon::EaseType::InCubic:
  case BeatmapSaveDataCommon::EaseType::OutCubic:
  case BeatmapSaveDataCommon::EaseType::InOutCubic:
  case BeatmapSaveDataCommon::EaseType::InQuart:
  case BeatmapSaveDataCommon::EaseType::OutQuart:
  case BeatmapSaveDataCommon::EaseType::InOutQuart:
  case BeatmapSaveDataCommon::EaseType::InQuint:
  case BeatmapSaveDataCommon::EaseType::OutQuint:
  case BeatmapSaveDataCommon::EaseType::InOutQuint:
  case BeatmapSaveDataCommon::EaseType::InExpo:
  case BeatmapSaveDataCommon::EaseType::OutExpo:
  case BeatmapSaveDataCommon::EaseType::InOutExpo:
    break;
  case BeatmapSaveDataCommon::EaseType::InCirc:
    return EaseType::InCirc;
  case BeatmapSaveDataCommon::EaseType::OutCirc:
    return EaseType::OutCirc;
  case BeatmapSaveDataCommon::EaseType::InOutCirc:
    return EaseType::InOutCirc;
  case BeatmapSaveDataCommon::EaseType::InBack:
    return EaseType::InBack;
  case BeatmapSaveDataCommon::EaseType::OutBack:
    return EaseType::OutBack;
  case BeatmapSaveDataCommon::EaseType::InOutBack:
    return EaseType::InOutBack;
  case BeatmapSaveDataCommon::EaseType::InElastic:
    return EaseType::InElastic;
  case BeatmapSaveDataCommon::EaseType::OutElastic:
    return EaseType::OutElastic;
  case BeatmapSaveDataCommon::EaseType::InOutElastic:
    return EaseType::InOutElastic;
  case BeatmapSaveDataCommon::EaseType::InBounce:
    return EaseType::InBounce;
  case BeatmapSaveDataCommon::EaseType::OutBounce:
    return EaseType::OutBounce;
  case BeatmapSaveDataCommon::EaseType::InOutBounce:
    return EaseType::InOutBounce;
  case BeatmapSaveDataCommon::EaseType::BeatSaberInOutBack:
    return EaseType::BeatSaberInOutBack;
  case BeatmapSaveDataCommon::EaseType::BeatSaberInOutElastic:
    return EaseType::BeatSaberInOutElastic;
  case BeatmapSaveDataCommon::EaseType::BeatSaberInOutBounce:
    return EaseType::BeatSaberInOutBounce;
  }

  return EaseType::None;
}

// BeatmapDataLoader.ConvertNoteLineLayer
constexpr NoteLineLayer ConvertNoteLineLayer(BeatmapSaveDataCommon::NoteLineLayer layer) {
  switch (layer) {
  case BeatmapSaveDataCommon::NoteLineLayer::Base:
    return NoteLineLayer::Base;
  case BeatmapSaveDataCommon::NoteLineLayer::Upper:
    return NoteLineLayer::Upper;
  case BeatmapSaveDataCommon::NoteLineLayer::Top:
    return NoteLineLayer::Top;
  default:
    return NoteLineLayer::Base;
  }
}

constexpr OffsetDirection ConvertOffsetDirection(BeatmapSaveDataCommon::OffsetDirection offsetDirection) {
  switch (offsetDirection) {
  case BeatmapSaveDataCommon::OffsetDirection::Up:
    return OffsetDirection::Up;
  case BeatmapSaveDataCommon::OffsetDirection::Down:
    return OffsetDirection::Down;
  case BeatmapSaveDataCommon::OffsetDirection::Left:
    return OffsetDirection::Left;
  case BeatmapSaveDataCommon::OffsetDirection::Right:
    return OffsetDirection::Right;
  case BeatmapSaveDataCommon::OffsetDirection::UpLeft:
    return OffsetDirection::UpLeft;
  case BeatmapSaveDataCommon::OffsetDirection::UpRight:
    return OffsetDirection::UpRight;
  case BeatmapSaveDataCommon::OffsetDirection::DownLeft:
    return OffsetDirection::DownLeft;
  case BeatmapSaveDataCommon::OffsetDirection::DownRight:
    return OffsetDirection::DownRight;
  case BeatmapSaveDataCommon::OffsetDirection::None:
    return OffsetDirection::None;
  }
  return OffsetDirection::None;
}

// BeatmapDataLoader.ConvertSliderType
constexpr GlobalNamespace::SliderData::Type ConvertSliderType(BeatmapSaveDataVersion3::SliderType sliderType) {
  if (sliderType == BeatmapSaveDataVersion3::SliderType::Normal) {
    return GlobalNamespace::SliderData::Type::Normal;
  }
  if (sliderType != BeatmapSaveDataVersion3::SliderType::Burst) {
    return GlobalNamespace::SliderData::Type::Normal;
  }
  return GlobalNamespace::SliderData::Type::Burst;
}

constexpr SliderMidAnchorMode ConvertSliderMidAnchorMode(BeatmapSaveDataCommon::SliderMidAnchorMode mode) {
  switch (mode) {
  case BeatmapSaveDataCommon::SliderMidAnchorMode::Straight:
    return SliderMidAnchorMode::Straight;
  case BeatmapSaveDataCommon::SliderMidAnchorMode::Clockwise:
    return SliderMidAnchorMode::Clockwise;
  case BeatmapSaveDataCommon::SliderMidAnchorMode::CounterClockwise:
    return SliderMidAnchorMode::CounterClockwise;
  default:
    return SliderMidAnchorMode::Straight;
  }
}

constexpr LightRotationDirection ConvertRotationOrientation(
    BeatmapSaveDataCommon::RotationDirection rotationDirection) {
  switch (rotationDirection) {
  case BeatmapSaveDataCommon::RotationDirection::Automatic:
    return LightRotationDirection::Automatic;
  case BeatmapSaveDataCommon::RotationDirection::Clockwise:
    return LightRotationDirection::Clockwise;
  case BeatmapSaveDataCommon::RotationDirection::Counterclockwise:
    return LightRotationDirection::Counterclockwise;
  default:
    return LightRotationDirection::Automatic;
  }
}

template <typename To, typename From, typename... TArgs>
  requires(std::is_pointer_v<To> && std::is_pointer_v<From>)
struct CppConverter {
  std::unordered_map<Il2CppClass*, std::function<To(From, TArgs...)>> converters;

  template <typename U, typename F>
    requires(std::is_pointer_v<U> && std::is_convertible_v<U, From> &&
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
  float bpmChangeStartTime;
  float bpmChangeStartBpmTime;
  float bpm;

  constexpr BpmChangeData(float const bpmChangeStartTime, float const bpmChangeStartBpmTime, float const bpm)
      : bpmChangeStartTime(bpmChangeStartTime), bpmChangeStartBpmTime(bpmChangeStartBpmTime), bpm(bpm) {}
};

struct BpmTimeProcessor {
  std::vector<BpmChangeData> bpmChangeDataList{};
  int currentBpmChangesDataIdx = 0;

  // clang-format off
/* 
		bool flag = bpmEventsSaveData.Count > 0 && bpmEventsSaveData[0].beat == 0f && bpmEventsSaveData[0].bpm > 0f;
		if (flag)
		{
			startBpm = bpmEventsSaveData[0].bpm;
		}
		this._bpmChangeDataList = new List<BpmTimeProcessor.BpmChangeData>
		{
			new BpmTimeProcessor.BpmChangeData(0f, 0f, startBpm)
		};
		for (int i = flag ? 1 : 0; i < bpmEventsSaveData.Count; i++)
		{
			List<BpmTimeProcessor.BpmChangeData> bpmChangeDataList = this._bpmChangeDataList;
			int index = bpmChangeDataList.Count - 1;
			BpmTimeProcessor.BpmChangeData prevBpmChangeData = bpmChangeDataList[index];
			float beat = bpmEventsSaveData[i].beat;
			float bpm = bpmEventsSaveData[i].bpm;
			float bpmChangeStartTime = BpmTimeProcessor.CalculateTime(prevBpmChangeData, beat);
			this._bpmChangeDataList.Add(new BpmTimeProcessor.BpmChangeData(bpmChangeStartTime, beat, bpm));
		}
*/
  // clang-format on
  BpmTimeProcessor(float startBpm,
                   VList<BeatmapSaveDataVersion3::BpmChangeEventData*> bpmEventsSaveData) {
    bool hasBpm = !bpmEventsSaveData.empty() && bpmEventsSaveData[0]->beat == 0.0F;
    if (hasBpm) {
      startBpm = bpmEventsSaveData[0]->bpm;
    }
    this->bpmChangeDataList = { BpmChangeData(0.0F, 0.0F, startBpm) };
    bpmChangeDataList.reserve(bpmEventsSaveData.size());

    for (int i = hasBpm ? 1 : 0; i < bpmEventsSaveData.size(); i++) {
      auto bpmChangeDataList = this->bpmChangeDataList;
      auto const& prevBpmChangeData = bpmChangeDataList.back();
      float beat = bpmEventsSaveData[i]->beat;
      float bpm = bpmEventsSaveData[i]->bpm;
      float bpmChangeStartTime = CalculateTime(prevBpmChangeData, beat);
      bpmChangeDataList.emplace_back(bpmChangeStartTime, beat, bpm);
    }
  }

// clang-format off
/* 
		EventData[] array = (from e in events
		where e.type == BeatmapEventType.BpmChange
		select e).ToArray<EventData>();
		bool flag = array.Length != 0 && array[0].time == 0f && array[0].floatValue > 0f;
		if (flag)
		{
			startBpm = array[0].floatValue;
		}
		this._bpmChangeDataList = new List<BpmTimeProcessor.BpmChangeData>
		{
			new BpmTimeProcessor.BpmChangeData(0f, 0f, startBpm)
		};
		for (int i = flag ? 1 : 0; i < array.Length; i++)
		{
			List<BpmTimeProcessor.BpmChangeData> bpmChangeDataList = this._bpmChangeDataList;
			int index = bpmChangeDataList.Count - 1;
			BpmTimeProcessor.BpmChangeData prevBpmChangeData = bpmChangeDataList[index];
			float time = array[i].time;
			float floatValue = array[i].floatValue;
			float bpmChangeStartTime = BpmTimeProcessor.CalculateTime(prevBpmChangeData, time);
			this._bpmChangeDataList.Add(new BpmTimeProcessor.BpmChangeData(bpmChangeStartTime, time, floatValue));
		}
*/
  // clang-format on
  BpmTimeProcessor(float startBpm, VList<BeatmapSaveDataVersion2_6_0AndEarlier::EventData*> events) {
    std::vector<BeatmapSaveDataVersion2_6_0AndEarlier::EventData*> array;
    array.reserve(events.size());
    
    for (auto&& e : events) {
      if (e->type != BeatmapSaveDataCommon::BeatmapEventType::BpmChange) continue;
      array.emplace_back(e);
    }
    bool flag = array.size() != 0 && array[0]->time == 0.0f && array[0]->floatValue > 0.0f;
    if (flag) {
      startBpm = array[0]->floatValue;
    }
    this->bpmChangeDataList = { BpmChangeData(0.0F, 0.0F, startBpm) };

    for (int i = flag ? 1 : 0; i < array.size(); i++) {
      auto bpmChangeDataList = this->bpmChangeDataList;
      auto const& prevBpmChangeData = bpmChangeDataList.back();
      float time = array[i]->time;
      float floatValue = array[i]->floatValue;
      float bpmChangeStartTime = CalculateTime(prevBpmChangeData, time);
      this->bpmChangeDataList.emplace_back(BpmChangeData(bpmChangeStartTime, time, floatValue));
    }
  }

  float CalculateTime(BpmChangeData prevBpmChangeData, float beat) {
    return prevBpmChangeData.bpmChangeStartTime +
           (beat - prevBpmChangeData.bpmChangeStartBpmTime) / prevBpmChangeData.bpm * 60.0f;
  }

  constexpr void Reset() {
    currentBpmChangesDataIdx = 0;
  }

  // clang-format off
/* 
		while (this.currentBpmChangesDataIdx > 0)
		{
			if (this._bpmChangeDataList[this.currentBpmChangesDataIdx].bpmChangeStartBpmTime < beat)
			{
				break;
			}
			this.currentBpmChangesDataIdx--;
		}
		while (this.currentBpmChangesDataIdx < this._bpmChangeDataList.Count - 1 && this._bpmChangeDataList[this.currentBpmChangesDataIdx + 1].bpmChangeStartBpmTime < beat)
		{
			this.currentBpmChangesDataIdx++;
		}
		return BpmTimeProcessor.CalculateTime(this._bpmChangeDataList[this.currentBpmChangesDataIdx], beat);
*/
  // clang-format on
  [[nodiscard]] constexpr float ConvertBeatToTime(float beat) {

    while (this->currentBpmChangesDataIdx > 0) {
      if (bpmChangeDataList[this->currentBpmChangesDataIdx].bpmChangeStartBpmTime < beat) {
        break;
      }
      currentBpmChangesDataIdx--;
    }
    while (currentBpmChangesDataIdx < bpmChangeDataList.size() - 1 &&
           bpmChangeDataList[currentBpmChangesDataIdx + 1].bpmChangeStartBpmTime < beat) {
      currentBpmChangesDataIdx++;
    }
    auto const& bpmChangeData = bpmChangeDataList[currentBpmChangesDataIdx];
    return CalculateTime(bpmChangeData, beat);
  }
};

constexpr GlobalNamespace::IndexFilter* IndexFilterConvertor_Convert(BeatmapSaveDataVersion3::IndexFilter* indexFilter, int groupSize) {
  int num =
      (indexFilter->get_chunks() == 0)
          ? 1
          : static_cast<int>(std::ceil(static_cast<float>(groupSize) / static_cast<float>(indexFilter->get_chunks())));
  int num2 = std::ceil(static_cast<float>(groupSize) / static_cast<float>(num));
  auto type = indexFilter->get_type();
  if (type != BeatmapSaveDataCommon::IndexFilterType::Division) {
    if (type != BeatmapSaveDataCommon::IndexFilterType::StepAndOffset) {
      return nullptr;
    }
    int param = indexFilter->get_param0();
    int param2 = indexFilter->get_param1();
    int num3 = num2 - param;
    if (num3 <= 0) {
      CJDLogger::Logger.fmtLog<LogLevel::ERR>("Error {}!", num3);
      throw il2cpp_utils::exceptions::StackTraceException("ArgumentOutOfRangeException");
    }
    int count = (param2 == 0) ? 1 : static_cast<int>(std::ceil(static_cast<float>(num3) / static_cast<float>(param2)));
    if (indexFilter->get_reversed()) {
      return CustomJSONData::NewFast<GlobalNamespace::IndexFilter*>(
          num2 - 1 - param, -param2, count, groupSize,
          GlobalNamespace::IndexFilter::IndexFilterRandomType(indexFilter->get_random().value__), indexFilter->get_seed(), num,
          indexFilter->get_limit(),
          GlobalNamespace::IndexFilter::IndexFilterLimitAlsoAffectType(indexFilter->get_limitAlsoAffectsType().value__));
    }
    return CustomJSONData::NewFast<GlobalNamespace::IndexFilter*>(
        param, param2, count, groupSize, GlobalNamespace::IndexFilter::IndexFilterRandomType(indexFilter->get_random().value__),
        indexFilter->get_seed(), num, indexFilter->get_limit(),
        GlobalNamespace::IndexFilter::IndexFilterLimitAlsoAffectType(indexFilter->get_limitAlsoAffectsType().value__));
  }

  int param3 = indexFilter->get_param0();
  int param4 = indexFilter->get_param1();
  int num4 = std::ceil(static_cast<float>(num2) / static_cast<float>(param3));
  if (indexFilter->get_reversed()) {
    int num5 = num2 - num4 * param4 - 1;
    return CustomJSONData::NewFast<GlobalNamespace::IndexFilter*>(
        num5, std::max(0, num5 - num4 + 1), groupSize,
        GlobalNamespace::IndexFilter::IndexFilterRandomType(indexFilter->get_random().value__), indexFilter->get_seed(), num,
        indexFilter->get_limit(),
        GlobalNamespace::IndexFilter::IndexFilterLimitAlsoAffectType(indexFilter->get_limitAlsoAffectsType().value__));
  }
  int num6 = num4 * param4;
  return CustomJSONData::NewFast<GlobalNamespace::IndexFilter*>(
      num6, std::min(num2 - 1, num6 + num4 - 1), groupSize,
      GlobalNamespace::IndexFilter::IndexFilterRandomType(indexFilter->get_random().value__), indexFilter->get_seed(), num,
      indexFilter->get_limit(),
      GlobalNamespace::IndexFilter::IndexFilterLimitAlsoAffectType(indexFilter->get_limitAlsoAffectsType().value__));
}

constexpr GlobalNamespace::BeatmapEventDataBox::DistributionParamType
DistributionParamType_Convert(BeatmapSaveDataCommon::DistributionParamType distributionParamType) {
  if (distributionParamType == BeatmapSaveDataCommon::DistributionParamType::Wave) {
    return GlobalNamespace::BeatmapEventDataBox::DistributionParamType::Wave;
  }
  if (distributionParamType != BeatmapSaveDataCommon::DistributionParamType::Step) {
    throw std::runtime_error("distributionParamType error!");
  }
  return GlobalNamespace::BeatmapEventDataBox::DistributionParamType::Step;
}

constexpr GlobalNamespace::LightColorBaseData* LightColorBaseData_Convert(BeatmapSaveDataVersion3::LightColorBaseData* saveData) {
  return CustomJSONData::NewFast<GlobalNamespace::LightColorBaseData*>(
      BeatmapSaveDataItem_GetBeat(saveData),
      (LightColorBaseData_GetTransitionType(saveData) == BeatmapSaveDataVersion3::TransitionType::Extend),
      ConvertBeatmapEventTransitionTypeToEaseType(LightColorBaseData_GetTransitionType(saveData)),
      ConvertColorType(LightColorBaseData_GetColorType(saveData)), LightColorBaseData_GetBrightness(saveData),
      LightColorBaseData_GetStrobeFrequency(saveData),
      saveData->strobeBrightness,
      saveData->strobeFade);
}

constexpr GlobalNamespace::LightRotationBaseData* LightRotationBaseData_Convert(BeatmapSaveDataVersion3::LightRotationBaseData* saveData) {
  return CustomJSONData::NewFast<GlobalNamespace::LightRotationBaseData*>(
      saveData->b, saveData->get_usePreviousEventRotationValue(), ConvertEaseType(saveData->e),
      LightRotationBaseData_GetRotation(saveData), LightRotationBaseData_GetLoopsCount(saveData),
      ConvertRotationOrientation(saveData->o));
}

constexpr GlobalNamespace::LightTranslationBaseData*
LightTranslationBaseData_Convert(BeatmapSaveDataVersion3::LightTranslationBaseData* saveData) {
  return CustomJSONData::NewFast<GlobalNamespace::LightTranslationBaseData*>(
      saveData->b, saveData->get_usePreviousEventTranslationValue(), ConvertEaseType(saveData->e), saveData->t);
}

struct EventBoxGroupConvertor {
  explicit EventBoxGroupConvertor(IEnvironmentLightGroups* lightGroups) : lightGroups(lightGroups) {

    dataConvertor.AddConverter<BeatmapSaveDataVersion3::LightColorEventBox*>([](BeatmapSaveDataVersion3::LightColorEventBox* saveData,
                                                                        GlobalNamespace::ILightGroup* lightGroupData) {
      auto* indexFilter = IndexFilterConvertor_Convert(saveData->f, lightGroupData->numberOfElements);
      auto* saveDataList = reinterpret_cast<
          System::Collections::Generic::List_1<BeatmapSaveDataVersion3::LightColorBaseData*>*>(
          saveData->e);
      auto list = VList<GlobalNamespace::LightColorBaseData*>::New();
      list->EnsureCapacity(saveDataList->get_Count());

      for (auto* saveData2 : VList<BeatmapSaveDataVersion3::LightColorBaseData*>(saveDataList)) {
        list.push_back(LightColorBaseData_Convert(saveData2));
      }

      auto beatDistributionParamType = DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
      auto brightnessDistributionParamType =
          DistributionParamType_Convert(saveData->get_brightnessDistributionParamType());
      return CustomJSONData::NewFast<GlobalNamespace::LightColorBeatmapEventDataBox*>(
          indexFilter, EventBox_GetBeatDistributionParam(saveData), beatDistributionParamType,
          saveData->get_brightnessDistributionParam(), brightnessDistributionParamType,
          saveData->get_brightnessDistributionShouldAffectFirstBaseEvent(),
          saveData->get_brightnessDistributionEaseType().value__,
          reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightColorBaseData*>*>(list.convert()));
    });

    dataConvertor.AddConverter<BeatmapSaveDataVersion3::LightRotationEventBox*>(
        [](BeatmapSaveDataVersion3::LightRotationEventBox* saveData, GlobalNamespace::ILightGroup* lightGroupData) {
          auto* indexFilter = IndexFilterConvertor_Convert(saveData->f, lightGroupData->numberOfElements);
          auto* collection = reinterpret_cast<System::Collections::Generic::List_1<
              BeatmapSaveDataVersion3::LightRotationBaseData*>*>(
              saveData->get_lightRotationBaseDataList());

          auto list = VList<GlobalNamespace::LightRotationBaseData*>::New();
          list->EnsureCapacity(collection->get_Count());

          for (auto* saveData2 :
               VList<BeatmapSaveDataVersion3::LightRotationBaseData*>(collection)) {
            list.push_back(LightRotationBaseData_Convert(saveData2));
          }

          auto beatDistributionParamType =
              DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
          auto rotationDistributionParamType =
              DistributionParamType_Convert(saveData->get_rotationDistributionParamType());
          return CustomJSONData::NewFast<GlobalNamespace::LightRotationBeatmapEventDataBox*>(
              indexFilter, EventBox_GetBeatDistributionParam(saveData), beatDistributionParamType,
              ConvertAxis(saveData->a), saveData->get_flipRotation(), saveData->get_rotationDistributionParam(),
              rotationDistributionParamType, saveData->get_rotationDistributionShouldAffectFirstBaseEvent(),
              saveData->get_rotationDistributionEaseType().value__,
              reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightRotationBaseData*>*>(list.convert()));
        });
    dataConvertor.AddConverter<BeatmapSaveDataVersion3::LightTranslationEventBox*>(
        [](BeatmapSaveDataVersion3::LightTranslationEventBox* saveData, GlobalNamespace::ILightGroup* lightGroupData) {
          auto* indexFilter = IndexFilterConvertor_Convert(saveData->f, lightGroupData->numberOfElements);

          auto* collection = reinterpret_cast<System::Collections::Generic::List_1<
              BeatmapSaveDataVersion3::LightTranslationBaseData*>*>(
              saveData->get_lightTranslationBaseDataList());

          auto list = VList<GlobalNamespace::LightTranslationBaseData*>::New();
          list->EnsureCapacity(collection->get_Count());

          for (auto* saveData2 :
               VList<BeatmapSaveDataVersion3::LightTranslationBaseData*>(collection)) {
            list.push_back(LightTranslationBaseData_Convert(saveData2));
          }

          auto beatDistributionParamType =
              DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
          auto gapDistributionParamType = DistributionParamType_Convert(saveData->get_gapDistributionParamType());

          return CustomJSONData::NewFast<GlobalNamespace::LightTranslationBeatmapEventDataBox*>(
              indexFilter, EventBox_GetBeatDistributionParam(saveData), beatDistributionParamType,
              ConvertAxis(saveData->a), saveData->get_flipTranslation(), saveData->get_gapDistributionParam(),
              gapDistributionParamType, saveData->get_gapDistributionShouldAffectFirstBaseEvent(),
              ConvertEaseType(saveData->get_gapDistributionEaseType()),
              reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightTranslationBaseData*>*>(list.convert()));
        });
  }

  BeatmapEventDataBoxGroup*
  Convert(BeatmapSaveDataVersion3::EventBoxGroup* eventBoxGroupSaveData) const {
    auto* dataForGroup = this->lightGroups->GetDataForGroup(eventBoxGroupSaveData->get_groupId());
    if (dataForGroup == nullptr) {
      return nullptr;
    }

    if (auto dataForGroupUnity = il2cpp_utils::try_cast<UnityEngine::Object>(dataForGroup);
        !dataForGroupUnity || (dataForGroupUnity.value()->m_CachedPtr == nullptr)) {
      return nullptr;
    }

    auto* collection =
        reinterpret_cast<System::Collections::Generic::List_1<BeatmapSaveDataVersion3::EventBox*>*>(
            eventBoxGroupSaveData->get_baseEventBoxes());
    auto list = VList<GlobalNamespace::BeatmapEventDataBox*>::New();
    list->EnsureCapacity(collection->get_Count());

    for (auto* item : VList<BeatmapSaveDataVersion3::EventBox*>(collection)) {
      auto* beatmapEventDataBox = dataConvertor.ProcessItem(item, dataForGroup);
      if (beatmapEventDataBox != nullptr) {
        list.push_back(beatmapEventDataBox);
      }
    }

    return CustomJSONData::NewFast<GlobalNamespace::BeatmapEventDataBoxGroup*>(
        eventBoxGroupSaveData->b,
        reinterpret_cast<IReadOnlyCollection_1<GlobalNamespace::BeatmapEventDataBox*>*>(list.convert()));
  }

  CppConverter<BeatmapEventDataBox*, BeatmapSaveDataVersion3::EventBox*, GlobalNamespace::ILightGroup*>
      dataConvertor;

  IEnvironmentLightGroups* lightGroups;
};
} // namespace CustomJSONData