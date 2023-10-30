#pragma once

#include "custom-types/shared/macros.hpp"

#include "System/Func_2.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/ObstacleData.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/WaypointData.hpp"
#include "GlobalNamespace/BasicBeatmapEventData.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypeAndIds_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "System/Object.hpp"
#include "System/Collections/IEnumerable.hpp"
#include "System/Collections/IEnumerator.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "System/Collections/Generic/IEnumerable_1.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"

#include "LowLevelUtils.hpp"
#include "CustomEventData.h"
#include "JSONWrapper.h"
#include "CJDLogger.h"

DECLARE_CLASS_CODEGEN(
    CustomJSONData, CustomBeatmapData, GlobalNamespace::BeatmapData, DECLARE_FASTER_CTOR(ctor, int numberOfLines);
    DECLARE_SIMPLE_DTOR(); DECLARE_INSTANCE_METHOD(CustomBeatmapData*, BaseCopy);

    public
    : void AddBeatmapObjectDataOverride(GlobalNamespace::BeatmapObjectData* beatmapObjectData);
    void AddBeatmapObjectDataInOrderOverride(GlobalNamespace::BeatmapObjectData* beatmapObjectData);
    void InsertBeatmapEventDataOverride(GlobalNamespace::BeatmapEventData* beatmapObjectData);
    void InsertBeatmapEventDataInOrderOverride(GlobalNamespace::BeatmapEventData* beatmapObjectData);

    inline CustomBeatmapData *
    GetCopyOverride() { return GetFilteredCopyOverride([](auto i) constexpr { return i; }); }

    template <typename F>
    CustomBeatmapData *
    GetFilteredCopyOverride(F && filter) {
      isCreatingFilteredCopy = true;

      CustomBeatmapData* copy = BaseCopy();

      auto linkedList = allBeatmapData->get_items();

      for (auto node = linkedList->get_First(); node != nullptr; node = LinkedListNode_1_get_Next(node)) {
        auto beatmapDataItem = node->item;

        if (!beatmapDataItem) continue;

        beatmapDataItem = filter(beatmapDataItem->GetCopy());

        if (!beatmapDataItem) continue;

        if (auto event = il2cpp_utils::try_cast<GlobalNamespace::BeatmapEventData>(beatmapDataItem)) {
          copy->InsertBeatmapEventDataInOrderOverride(*event);
        }

        if (auto object = il2cpp_utils::try_cast<GlobalNamespace::BeatmapObjectData>(beatmapDataItem)) {
          copy->AddBeatmapObjectDataInOrderOverride(*object);
        }

        if (auto customEvent = il2cpp_utils::try_cast<CustomEventData>(beatmapDataItem)) {
          copy->InsertCustomEventDataInOrder(*customEvent);
        }
      }

      isCreatingFilteredCopy = false;

      return copy;
    }

    static System::Type *
    GetCustomType(Il2CppObject * obj);
    static System::Type * GetCustomType(Il2CppClass * obj);

    void InsertCustomEventData(CustomEventData* customEventData);
    void InsertCustomEventDataInOrder(CustomEventData* customEventData);

    template <typename T> static constexpr System::Collections::Generic::LinkedListNode_1<T> *
    LinkedListNode_1_get_Next(System::Collections::Generic::LinkedListNode_1<T> * self) {
      if (self->next != nullptr && self->next != self->list->head) {
        return self->next;
      }
      return nullptr;
    }

    template <class T>
    std::vector<T>
        GetBeatmapItemsCpp(GlobalNamespace::BeatmapDataItem::BeatmapDataItemType type) {
          auto* list = reinterpret_cast<GlobalNamespace::ISortedList_1<T>*>(
              beatmapDataItemsPerTypeAndId->GetList(GetCustomType(classof(T)), type));

          if (!list) return {};

          auto linkedItems = list->get_items();

          std::vector<T> items;
          items.reserve(linkedItems->get_Count());

          for (auto node = linkedItems->get_First(); node != nullptr; node = LinkedListNode_1_get_Next(node)) {
            auto val = node->item;
            if (!val) continue;

            items.template emplace_back(reinterpret_cast<T>(val));
          }

          return items;
        }

    std::vector<GlobalNamespace::BeatmapDataItem*>
        GetAllBeatmapItemsCpp() {
          if (!allBeatmapData) return {};

          auto linkedItems = allBeatmapData->get_items();

          std::vector<GlobalNamespace::BeatmapDataItem*> items;
          items.reserve(linkedItems->get_Count());

          for (auto node = linkedItems->get_First(); node != nullptr; node = LinkedListNode_1_get_Next(node)) {
            auto val = node->item;
            if (!val) continue;

            items.template emplace_back(val);
          }

          return items;
        }

    std::vector<GlobalNamespace::BeatmapObjectData*>
        beatmapObjectDatas;
    std::vector<GlobalNamespace::BeatmapEventData*> beatmapEventDatas; std::vector<CustomEventData*> customEventDatas;

    DECLARE_INSTANCE_FIELD(bool, v2orEarlier);
    // optional
    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper*, customData);
    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapperUTF16*, beatmapCustomData);
    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapperUTF16*, levelCustomData);
    DECLARE_INSTANCE_FIELD(CustomJSONData::DocumentWrapper*, doc);)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomBeatmapEventData, GlobalNamespace::BasicBeatmapEventData,
                      DECLARE_FASTER_CTOR(ctor, float time,
                                          ::GlobalNamespace::BasicBeatmapEventType basicBeatmapEventType, int value,
                                          float floatValue);

                      DECLARE_OVERRIDE_METHOD(CustomBeatmapEventData*, GetCopy,
                                              il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

                      // optional
                      DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper*, customData);)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomObstacleData, GlobalNamespace::ObstacleData,
                      DECLARE_FASTER_CTOR(ctor, float time, int lineIndex, ::GlobalNamespace::NoteLineLayer lineLayer,
                                          float duration, int width, int height);

                      DECLARE_OVERRIDE_METHOD(CustomObstacleData*, GetCopy,
                                              il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

                      // optional
                      DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper*, customData);
                      // Used for Noodle Extensions
                      DECLARE_INSTANCE_FIELD(float, bpm);
                      DECLARE_INSTANCE_FIELD(float, aheadTimeNoodle);
)

DECLARE_CLASS_CODEGEN(
    CustomJSONData, CustomNoteData, GlobalNamespace::NoteData,
    DECLARE_FASTER_CTOR(ctor, float time, int lineIndex, ::GlobalNamespace::NoteLineLayer noteLineLayer,
                        ::GlobalNamespace::NoteLineLayer beforeJumpNoteLineLayer,
                        ::GlobalNamespace::NoteData::GameplayType gameplayType,
                        ::GlobalNamespace::NoteData::ScoringType scoringType, ::GlobalNamespace::ColorType colorType,
                        ::GlobalNamespace::NoteCutDirection cutDirection, float timeToNextColorNote,
                        float timeToPrevColorNote, int flipLineIndex, float flipYSide, float cutDirectionAngleOffset,
                        float cutSfxVolumeMultiplier);

    DECLARE_OVERRIDE_METHOD(CustomNoteData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

    // optional
    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper*, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm); DECLARE_INSTANCE_FIELD(float, aheadTimeNoodle);)

DECLARE_CLASS_CODEGEN(
    CustomJSONData, CustomSliderData, GlobalNamespace::SliderData,
    DECLARE_FASTER_CTOR(
        ctor, GlobalNamespace::SliderData::Type sliderType, ::GlobalNamespace::ColorType colorType, bool hasHeadNote,
        float headTime, int headLineIndex, ::GlobalNamespace::NoteLineLayer headLineLayer,
        ::GlobalNamespace::NoteLineLayer headBeforeJumpLineLayer, float headControlPointLengthMultiplier,
        ::GlobalNamespace::NoteCutDirection headCutDirection, float headCutDirectionAngleOffset, bool hasTailNote,
        float tailTime, int tailLineIndex, ::GlobalNamespace::NoteLineLayer tailLineLayer,
        ::GlobalNamespace::NoteLineLayer tailBeforeJumpLineLayer, float tailControlPointLengthMultiplier,
        ::GlobalNamespace::NoteCutDirection tailCutDirection, float tailCutDirectionAngleOffset,
        ::GlobalNamespace::SliderMidAnchorMode midAnchorMode, int sliceCount, float squishAmount);

    DECLARE_OVERRIDE_METHOD(CustomSliderData*, GetCopy, il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

    // optional
    DECLARE_INSTANCE_FIELD(CustomJSONData::JSONWrapper*, customData);
    // Used for Noodle Extensions
    DECLARE_INSTANCE_FIELD(float, bpm);)

DECLARE_CLASS_CODEGEN(CustomJSONData, CustomWaypointData, GlobalNamespace::WaypointData,
                      DECLARE_FASTER_CTOR(ctor, float time, int lineIndex, ::GlobalNamespace::NoteLineLayer lineLayer,
                                          ::GlobalNamespace::OffsetDirection offsetDirection);

                      DECLARE_OVERRIDE_METHOD(CustomWaypointData*, GetCopy,
                                              il2cpp_utils::FindMethod("", "BeatmapDataItem", "GetCopy"));

                      // Used for Noodle Extensions
                      DECLARE_INSTANCE_FIELD(float, bpm);
)
