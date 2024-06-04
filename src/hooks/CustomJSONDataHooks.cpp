#include "BeatmapSaveDataVersion2_6_0AndEarlier/zzzz__EventData_def.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveDataItem.hpp"
#include "BeatmapDataLoaderVersion2_6_0AndEarlier/BeatmapDataLoader.hpp"

#include "BeatmapDataLoaderVersion3/BeatmapDataLoader.hpp"
#include "BeatmapSaveDataVersion3/BeatmapSaveDataItem.hpp"

#include "BeatmapDataLoaderVersion4/BeatmapDataLoader.hpp"
#include "BeatmapSaveDataVersion4/LightshowSaveData.hpp"

#include "GlobalNamespace/BpmTimeProcessor.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/IEnvironmentInfo.hpp"
#include "GlobalNamespace/IEnvironmentLightGroups.hpp"
#include "GlobalNamespace/EnvironmentLightGroups.hpp"
#include "GlobalNamespace/DefaultEnvironmentEvents.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/BasicBeatmapEventDataProcessor.hpp"
#include "GlobalNamespace/BeatmapDataStrobeFilterTransform.hpp"
#include "GlobalNamespace/LightColorBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentIntensityReductionOptions.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/BeatmapEventDataLightsExtensions.hpp"

#include "System/Action.hpp"

#include "UnityEngine/JsonUtility.hpp"

#include "System/Reflection/MemberInfo.hpp"
#include "System/Collections/Generic/InsertionBehavior.hpp"

#include "beatsaber-hook/shared/utils/typedefs-list.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"

#include "paper/shared/Profiler.hpp"

#include "sombrero/shared/linq_functional.hpp"
#include "sombrero/shared/Vector2Utils.hpp"
#include "sombrero/shared/Vector3Utils.hpp"

#include "custom-types/shared/register.hpp"

#include "songcore/shared/CustomJSONData.hpp"

#include "JSONWrapper.h"
#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "BeatmapFieldUtils.hpp"
#include "BeatmapDataLoaderUtils.hpp"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

#include <regex>
#include <chrono>
#include <codecvt>
#include <locale>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;
using namespace BeatmapSaveDataVersion4;



constexpr bool Approximately(float a, float b) {
  return std::abs(b - a) < std::max(1E-06F * std::max(std::abs(a), std::abs(b)), 1E-45F * 8.0F);
}

// clang-format off
/**
		BeatmapData beatmapData2 = new BeatmapData(beatmapData.numberOfLines);
		bool flag = environmentIntensityReductionOptions.compressExpand == EnvironmentIntensityReductionOptions.CompressExpandReductionType.RemoveWithStrobeFilter;
		bool flag2 = environmentIntensityReductionOptions.rotateRings == EnvironmentIntensityReductionOptions.RotateRingsReductionType.RemoveWithStrobeFilter;
		Dictionary<BasicBeatmapEventType, BeatmapDataStrobeFilterTransform.StrobeStreakData> dictionary = new Dictionary<BasicBeatmapEventType, BeatmapDataStrobeFilterTransform.StrobeStreakData>
		{
			{
				BasicBeatmapEventType.Event0,
				new BeatmapDataStrobeFilterTransform.StrobeStreakData()
			},
			{
				BasicBeatmapEventType.Event1,
				new BeatmapDataStrobeFilterTransform.StrobeStreakData()
			},
			{
				BasicBeatmapEventType.Event2,
				new BeatmapDataStrobeFilterTransform.StrobeStreakData()
			},
			{
				BasicBeatmapEventType.Event3,
				new BeatmapDataStrobeFilterTransform.StrobeStreakData()
			},
			{
				BasicBeatmapEventType.Event4,
				new BeatmapDataStrobeFilterTransform.StrobeStreakData()
			}
		};
		foreach (BeatmapDataItem beatmapDataItem in beatmapData.allBeatmapDataItems)
		{
			LightColorBeatmapEventData lightColorBeatmapEventData = beatmapDataItem as LightColorBeatmapEventData;
			if (lightColorBeatmapEventData != null)
			{
				lightColorBeatmapEventData.DisableStrobe();
				beatmapData2.InsertBeatmapEventDataInOrder(lightColorBeatmapEventData);
			}
			else
			{
				BasicBeatmapEventData basicBeatmapEventData = beatmapDataItem as BasicBeatmapEventData;
				if (basicBeatmapEventData == null)
				{
					BeatmapEventData beatmapEventData = beatmapDataItem as BeatmapEventData;
					if (beatmapEventData != null)
					{
						beatmapData2.InsertBeatmapEventDataInOrder(beatmapEventData);
					}
					else
					{
						BeatmapObjectData beatmapObjectData = beatmapDataItem as BeatmapObjectData;
						if (beatmapObjectData != null)
						{
							beatmapData2.AddBeatmapObjectDataInOrder(beatmapObjectData);
						}
					}
				}
				else if ((!flag || basicBeatmapEventData.basicBeatmapEventType != BasicBeatmapEventType.Event9) && (!flag2 || basicBeatmapEventData.basicBeatmapEventType != BasicBeatmapEventType.Event8))
				{
					if (!basicBeatmapEventData.basicBeatmapEventType.IsCoreLightIntensityChangeEvent())
					{
						beatmapData2.InsertBeatmapEventDataInOrder(basicBeatmapEventData);
					}
					else if (basicBeatmapEventData.basicBeatmapEventType.IsCoreLightIntensityChangeEvent() && basicBeatmapEventData.HasLightFadeEventDataValue())
					{
						beatmapData2.InsertBeatmapEventDataInOrder(basicBeatmapEventData);
					}
					else
					{
						BeatmapDataStrobeFilterTransform.StrobeStreakData strobeStreakData = dictionary[basicBeatmapEventData.basicBeatmapEventType];
						if (strobeStreakData.isActive)
						{
							if (basicBeatmapEventData.time - strobeStreakData.lastSwitchTime < 0.1f)
							{
								strobeStreakData.AddStrobeData(basicBeatmapEventData);
							}
							else
							{
								if (!Mathf.Approximately(strobeStreakData.strobeStartTime, strobeStreakData.lastSwitchTime))
								{
									int onEventDataValue = BeatmapDataStrobeFilterTransform.GetOnEventDataValue(strobeStreakData.startColorType);
									BasicBeatmapEventData beatmapEventData2 = new BasicBeatmapEventData(strobeStreakData.strobeStartTime, basicBeatmapEventData.basicBeatmapEventType, onEventDataValue, basicBeatmapEventData.floatValue);
									beatmapData2.InsertBeatmapEventDataInOrder(beatmapEventData2);
									int value;
									if (strobeStreakData.lastIsOn)
									{
										value = BeatmapDataStrobeFilterTransform.GetOnEventDataValue(strobeStreakData.lastColorType);
									}
									else
									{
										value = BeatmapDataStrobeFilterTransform.GetFlashAndFadeToBlackEventDataValue(strobeStreakData.lastColorType);
									}
									BasicBeatmapEventData beatmapEventData3 = new BasicBeatmapEventData(strobeStreakData.lastSwitchTime, basicBeatmapEventData.basicBeatmapEventType, value, basicBeatmapEventData.floatValue);
									beatmapData2.InsertBeatmapEventDataInOrder(beatmapEventData3);
								}
								else
								{
									beatmapData2.InsertBeatmapEventDataInOrder(strobeStreakData.originalBasicBeatmapEventData);
								}
								strobeStreakData.StartPotentialStrobe(basicBeatmapEventData);
							}
						}
						else
						{
							strobeStreakData.StartPotentialStrobe(basicBeatmapEventData);
						}
					}
				}
			}
		}
		foreach (KeyValuePair<BasicBeatmapEventType, BeatmapDataStrobeFilterTransform.StrobeStreakData> keyValuePair in dictionary)
		{
			if (keyValuePair.Value.isActive)
			{
				beatmapData2.InsertBeatmapEventDataInOrder(keyValuePair.Value.originalBasicBeatmapEventData);
			}
		}
		foreach (string specialBasicBeatmapEventKeyword in beatmapData.specialBasicBeatmapEventKeywords)
		{
			beatmapData2.AddSpecialBasicBeatmapEventKeyword(specialBasicBeatmapEventKeyword);
		}
		return beatmapData2;
*/
// clang-format on
MAKE_PAPER_HOOK_MATCH(BeatmapDataStrobeFilterTransform_CreateTransformedData,
                      &BeatmapDataStrobeFilterTransform::CreateTransformedData, IReadonlyBeatmapData*,
                      IReadonlyBeatmapData* beatmapData,
                      EnvironmentIntensityReductionOptions* environmentIntensityReductionOptions) {
  if (!beatmapData || reinterpret_cast<Il2CppObject*>(beatmapData)->klass != classof(CustomBeatmapData*)) {
    return BeatmapDataStrobeFilterTransform_CreateTransformedData(beatmapData, environmentIntensityReductionOptions);
  }
  // Won't work since the constructors are base game
  //
  //    auto fixedBeatmap = BeatmapDataStrobeFilterTransform_CreateTransformedData(beatmapData,
  //                                                                               environmentIntensityReductionOptions);
  //
  //    auto customBeatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmapData);
  //    CustomBeatmapData *newBeatmap;
  //
  ////    if (auto customBeatmapData = il2cpp_utils::try_cast<CustomBeatmapData>(beatmapData)) {
  ////        for (auto const &c: customBeatmapData.value()->customEventDatas) {
  ////            beatmapData2->InsertCustomEventDataInOrder(c);
  ////        }
  //        newBeatmap = customBeatmapData->GetFilteredCopyOverride(
  //                [&](BeatmapDataItem *const &i) -> BeatmapDataItem * {
  //
  //                    if (i->type == BeatmapDataItem::BeatmapDataItemType::BeatmapEvent) {
  //                        return nullptr;
  //                    }
  //
  //                    return i;
  //                });
  ////    } else {
  ////        newBeatmap = CustomBeatmapData::New_ctor(beatmapData->i_IBeatmapDataBasicInfo()->get_numberOfLines());
  ////
  ////    }
  //
  //
  //    auto const &linkedItems = fixedBeatmap->get_allBeatmapDataItems();
  //    for (auto node = linkedItems->get_First();
  //         node != nullptr; node = CustomBeatmapData::LinkedListNode_1_get_Next(node)) {
  //        if (!node->item || node->item->type != BeatmapDataItem::BeatmapDataItemType::BeatmapEvent) {
  //            continue;
  //        }
  //
  //        newBeatmap->InsertBeatmapEventDataInOrderOverride(il2cpp_utils::cast<BeatmapEventData>(node->item));
  //    }

  auto* customBeatmapData = il2cpp_utils::cast<CustomBeatmapData>(beatmapData);
  bool flag = environmentIntensityReductionOptions->compressExpand ==
              EnvironmentIntensityReductionOptions::CompressExpandReductionType::RemoveWithStrobeFilter;
  bool flag2 = environmentIntensityReductionOptions->rotateRings ==
               EnvironmentIntensityReductionOptions::RotateRingsReductionType::RemoveWithStrobeFilter;
  std::unordered_map<int, BeatmapDataStrobeFilterTransform::StrobeStreakData*> dictionary(
      { { BasicBeatmapEventType::Event0.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
        { BasicBeatmapEventType::Event1.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
        { BasicBeatmapEventType::Event2.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
        { BasicBeatmapEventType::Event3.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() },
        { BasicBeatmapEventType::Event4.value__, BeatmapDataStrobeFilterTransform::StrobeStreakData::New_ctor() } });

  auto* newBeatmap = customBeatmapData->BaseCopy();

  for (auto const& o : customBeatmapData->beatmapObjectDatas) {
    if (!o) {
      continue;
    }
    newBeatmap->AddBeatmapObjectDataInOrderOverride(o);
  }

  for (auto const& beatmapDataItem : customBeatmapData->beatmapEventDatas) {
    if (!beatmapDataItem) {
      continue;
    }

    BasicBeatmapEventData* basicBeatmapEventData =
        il2cpp_utils::try_cast<BasicBeatmapEventData>(beatmapDataItem).value_or(nullptr);
    LightColorBeatmapEventData* lightColorBeatmapEventData =
        il2cpp_utils::try_cast<LightColorBeatmapEventData>(beatmapDataItem).value_or(nullptr);

    if (lightColorBeatmapEventData) {
      lightColorBeatmapEventData->DisableStrobe();
      newBeatmap->InsertBeatmapEventDataInOrderOverride(lightColorBeatmapEventData);
      continue;
    }
    if (!basicBeatmapEventData) {
      newBeatmap->InsertBeatmapEventDataInOrder(beatmapDataItem);
      continue;
    }
    if ((!flag || basicBeatmapEventData->basicBeatmapEventType != BasicBeatmapEventType::Event9) &&
        (!flag2 || basicBeatmapEventData->basicBeatmapEventType != BasicBeatmapEventType::Event8)) {
      if (!BeatmapEventTypeExtensions::IsCoreLightIntensityChangeEvent(basicBeatmapEventData->basicBeatmapEventType)) {
        newBeatmap->InsertBeatmapEventDataInOrderOverride(basicBeatmapEventData);
        continue;
      }

      if (BeatmapEventDataLightsExtensions::HasLightFadeEventDataValue(basicBeatmapEventData)) {
        newBeatmap->InsertBeatmapEventDataInOrderOverride(basicBeatmapEventData);
        continue;
      }

      BeatmapDataStrobeFilterTransform::StrobeStreakData* strobeStreakData =
          dictionary[basicBeatmapEventData->basicBeatmapEventType.value__];
      CRASH_UNLESS(strobeStreakData);
      if (strobeStreakData->isActive) {
        if (basicBeatmapEventData->time - strobeStreakData->lastSwitchTime < 0.1F) {
          strobeStreakData->AddStrobeData(basicBeatmapEventData);
        } else {
          if (!Approximately(strobeStreakData->strobeStartTime, strobeStreakData->lastSwitchTime)) {
            int onEventDataValue =
                BeatmapDataStrobeFilterTransform::GetOnEventDataValue(strobeStreakData->startColorType);
            auto* beatmapEventData2 = static_cast<BasicBeatmapEventData*>(basicBeatmapEventData->GetCopy());
            beatmapEventData2->_time_k__BackingField = strobeStreakData->strobeStartTime;
            beatmapEventData2->value = onEventDataValue;
            newBeatmap->InsertBeatmapEventDataInOrderOverride(beatmapEventData2);
            int value = 0;
            if (strobeStreakData->lastIsOn) {
              value = BeatmapDataStrobeFilterTransform::GetOnEventDataValue(strobeStreakData->lastColorType);
            } else {
              value = BeatmapDataStrobeFilterTransform::GetFlashAndFadeToBlackEventDataValue(
                  strobeStreakData->lastColorType);
            }
            auto* beatmapEventData3 = static_cast<BasicBeatmapEventData*>(basicBeatmapEventData->GetCopy());
            beatmapEventData3->_time_k__BackingField = strobeStreakData->lastSwitchTime;
            beatmapEventData3->value = value;
            newBeatmap->InsertBeatmapEventDataInOrderOverride(beatmapEventData3);
          } else {
            newBeatmap->InsertBeatmapEventDataInOrderOverride(strobeStreakData->originalBasicBeatmapEventData);
          }
          strobeStreakData->StartPotentialStrobe(basicBeatmapEventData);
        }
      } else {
        strobeStreakData->StartPotentialStrobe(basicBeatmapEventData);
      }
    }
  }

  for (auto const& keyValuePair : dictionary) {
    if (!keyValuePair.second) {
      continue;
    }
    if (keyValuePair.second->isActive) {
      newBeatmap->InsertBeatmapEventDataInOrderOverride(keyValuePair.second->originalBasicBeatmapEventData);
    }
  }
  return newBeatmap->i___GlobalNamespace__IReadonlyBeatmapData();
}



BeatmapCallbacksController* beatmapCallbacksController;

MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_ManualUpdate, &BeatmapCallbacksController::ManualUpdate, void,
                      BeatmapCallbacksController* self, float songTime) {
  if (songTime == self->_prevSongTime) {
    return BeatmapCallbacksController_ManualUpdate(self, songTime);
  }

  // TRANSPILE HERE
  if (self != beatmapCallbacksController) {
    CustomEventCallbacks::RegisterCallbacks(self);
    beatmapCallbacksController = self;
  }
  //

  return BeatmapCallbacksController_ManualUpdate(self, songTime);
}

MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_Dispose, &BeatmapCallbacksController::Dispose, void,
                      BeatmapCallbacksController* self) {
  CustomEventCallbacks::firstNode.emplace(nullptr);
  return BeatmapCallbacksController_Dispose(self);
}

static float GetAheadTime(Il2CppObject const* obj) {

  static auto const* CustomNoteKlass = classof(CustomJSONData::CustomNoteData*);
  static auto const* CustomObstacleKlass = classof(CustomJSONData::CustomObstacleData*);

  if (obj->klass == CustomNoteKlass) {
    return static_cast<CustomJSONData::CustomNoteData const*>(obj)->aheadTimeNoodle;
  }

  if (obj->klass == CustomObstacleKlass) {
    return static_cast<CustomJSONData::CustomObstacleData const*>(obj)->aheadTimeNoodle;
  }

  return 0;
}

// clang-format off
/*
	if (songTime == this._prevSongTime)
		{
			return;
		}
		this._songTime = songTime;
		this._processingCallbacks = true;
		if (songTime > this._prevSongTime)
		{
			using (Dictionary<float, CallbacksInTime>.Enumerator enumerator = this._callbacksInTimes.GetEnumerator())
			{
				while (enumerator.MoveNext())
				{
					KeyValuePair<float, CallbacksInTime> keyValuePair = enumerator.Current;
					CallbacksInTime value = keyValuePair.Value;
					for (LinkedListNode<BeatmapDataItem> linkedListNode = (value.lastProcessedNode != null) ? value.lastProcessedNode.Next : this._beatmapData.allBeatmapDataItems.First; linkedListNode != null; linkedListNode = linkedListNode.Next)
					{
						BeatmapDataItem value2 = linkedListNode.Value;
						if (value2.time - value.aheadTime > songTime)
						{
							break;
						}
						if (value2.type == BeatmapDataItem.BeatmapDataItemType.BeatmapEvent || (value2.type == BeatmapDataItem.BeatmapDataItemType.BeatmapObject && value2.time >= this._startFilterTime))
						{
							this._callCallbacksBehavior.CallCallbacks(value, value2);
						}
						value.lastProcessedNode = linkedListNode;
					}
				}
				goto IL_1B4;
			}
		}
		foreach (KeyValuePair<float, CallbacksInTime> keyValuePair2 in this._callbacksInTimes)
		{
			CallbacksInTime value3 = keyValuePair2.Value;
			LinkedListNode<BeatmapDataItem> linkedListNode2 = value3.lastProcessedNode;
			while (linkedListNode2 != null)
			{
				BeatmapDataItem value4 = linkedListNode2.Value;
				if (value4.time - value3.aheadTime <= songTime)
				{
					break;
				}
				if (value4.type == BeatmapDataItem.BeatmapDataItemType.BeatmapEvent)
				{
					BeatmapEventData beatmapEventData = (BeatmapEventData)value4;
					if (beatmapEventData.previousSameTypeEventData != null)
					{
						this._callCallbacksBehavior.CallCallbacks(value3, beatmapEventData.previousSameTypeEventData);
					}
					else
					{
						BeatmapEventData @default = beatmapEventData.GetDefault(beatmapEventData);
						if (@default != null)
						{
							this._callCallbacksBehavior.CallCallbacks(value3, @default);
						}
					}
				}
				linkedListNode2 = linkedListNode2.Previous;
				value3.lastProcessedNode = linkedListNode2;
			}
		}
		IL_1B4:
		this._prevSongTime = songTime;
		this._processingCallbacks = false;
		Action action = this.didProcessAllCallbacksThisFrameEvent;
		if (action == null)
		{
			return;
		}
		action();
*/
// clang-format on
MAKE_PAPER_HOOK_MATCH(BeatmapCallbacksController_ManualUpdateTranspile, &BeatmapCallbacksController::ManualUpdate, void,
                      BeatmapCallbacksController* self, float songTime) {
  // TRANSPILE HERE
  if (self != beatmapCallbacksController) {
    CustomEventCallbacks::RegisterCallbacks(self);
    beatmapCallbacksController = self;
  }
  //

  if (songTime == self->_prevSongTime) {
    return;
  }

  self->_songTime = songTime;
  self->_processingCallbacks = true;
  if (songTime > self->_prevSongTime) {
    auto enumerator = self->_callbacksInTimes->GetEnumerator();

    while (enumerator.MoveNext()) {
      auto keyValuePair = enumerator.get_Current();
      auto* value = keyValuePair.get_Value();

      using NodePtr = System::Collections::Generic::LinkedListNode_1<GlobalNamespace::BeatmapDataItem*>*;

      auto* firstNode = CustomEventCallbacks::firstNode ? (NodePtr)CustomEventCallbacks::firstNode : nullptr;

      for (auto* linkedListNode =
               (value->lastProcessedNode != nullptr)
                   ? CustomJSONData::LinkedListNode_1_get_Next(value->lastProcessedNode)
                   : (firstNode ? firstNode : self->_beatmapData->get_allBeatmapDataItems()->get_First());
           linkedListNode != nullptr; linkedListNode = CustomJSONData::LinkedListNode_1_get_Next(linkedListNode)) {
        auto* value2 = linkedListNode->get_Value();
        // transpile here NE
        if (value2->time - value->aheadTime - GetAheadTime(value2) > songTime) {
          break;
        }
        //
        if (value2->type == BeatmapDataItem::BeatmapDataItemType::BeatmapEvent ||
            /// TRANSPILE HERE
            value2->type.value__ == 2 ||
            /// TRANSPILE HERE
            (value2->type == BeatmapDataItem::BeatmapDataItemType::BeatmapObject &&
             value2->time >= self->_startFilterTime)) {
          value->CallCallbacks(value2);
        }
        value->lastProcessedNode = linkedListNode;
      }
    }
    enumerator.Dispose();
  } else {
    auto callbacksInTimesEnumerator = self->_callbacksInTimes->GetEnumerator();

    while (callbacksInTimesEnumerator.MoveNext()) {
      auto keyValuePair2 = callbacksInTimesEnumerator.get_Current();
      auto* value3 = keyValuePair2.get_Value();
      auto* linkedListNode2 = value3->lastProcessedNode;
      while (linkedListNode2 != nullptr) {
        auto* value4 = linkedListNode2->get_Value();
        if (value4->time - value3->aheadTime <= songTime) {
          break;
        }

        /// TRANSPILE HERE
        /// STOPS INFINITE LOOP BY RUNNING THIS REGARDLESS IF THE CONDITION ABOVE IS MET
        /// WHILE THIS SHOULD BE FIXED IN PINKCORE, WE KEEP IT AS A SAFEGUARD
        if (value4->type != BeatmapDataItem::BeatmapDataItemType::BeatmapEvent) {
          break;
        }
        ///

        auto* beatmapEventData = static_cast<BeatmapEventData*>(value4);
        if (beatmapEventData->previousSameTypeEventData != nullptr) {
          value3->CallCallbacks(beatmapEventData->previousSameTypeEventData);
        } else {
          auto* def = beatmapEventData->GetDefault(beatmapEventData);
          if (def != nullptr) {
            value3->CallCallbacks(def);
          }
        }

        value3->lastProcessedNode = linkedListNode2 = linkedListNode2->get_Previous();
      }
    }
    callbacksInTimesEnumerator.Dispose();
  }

  self->_prevSongTime = songTime;
  self->_processingCallbacks = false;

  if (self->didProcessAllCallbacksThisFrameEvent) {
    self->didProcessAllCallbacksThisFrameEvent->Invoke();
  }
}

// clang-format off
/*
		beatmapData.InsertBeatmapEventData(new BasicBeatmapEventData(0f, BasicBeatmapEventType.Event0, 1, 1f));
		beatmapData.InsertBeatmapEventData(new BasicBeatmapEventData(0f, BasicBeatmapEventType.Event4, 1, 1f));
*/
// clang-format on
MAKE_PAPER_HOOK_MATCH(InsertDefaultEvents, &DefaultEnvironmentEventsFactory::InsertDefaultEvents, void,
                      BeatmapData* beatmapData) {
  // TRANSPILE HERE
  beatmapData->InsertBeatmapEventData(CustomBeatmapEventData::New_ctor(0.0F, BasicBeatmapEventType::Event0, 1, 1.0F));
  beatmapData->InsertBeatmapEventData(CustomBeatmapEventData::New_ctor(0.0F, BasicBeatmapEventType::Event4, 1, 1.0F));
  // END TRANSPILE HERE
}

void CustomJSONData::InstallHooks() {

  il2cpp_functions::Class_Init(classof(BeatmapData*));
  custom_types::Register::AutoRegister();

  // Stupid workaround because stupid NE
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapCallbacksController_ManualUpdateTranspile);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataStrobeFilterTransform_CreateTransformedData);
  INSTALL_HOOK_ORIG(CJDLogger::Logger, InsertDefaultEvents);
  INSTALL_HOOK(CJDLogger::Logger, BeatmapCallbacksController_Dispose);

  v2::InstallHooks();
  v3::InstallHooks();
  InstallBeatmapHooks();
}
