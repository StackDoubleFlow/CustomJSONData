#pragma once

#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"
#include "GlobalNamespace/BPMChangeBeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapDataLoader_SpecialEventsFilter.hpp"
#include "GlobalNamespace/BeatmapDataLoader_BpmTimeProcessor.hpp"
#include "GlobalNamespace/BeatmapEventTransitionType.hpp"
#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "GlobalNamespace/DefaultEnvironmentEvents.hpp"
#include "GlobalNamespace/EnvironmentColorType.hpp"
#include "GlobalNamespace/BeatmapEventDataBoxGroup.hpp"
#include "GlobalNamespace/IIndexFilter.hpp"
#include "GlobalNamespace/RangeIndexFilter.hpp"
#include "GlobalNamespace/EnvironmentLightGroups_LightGroupData.hpp"
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

    inline JSONWrapper *ToJsonWrapper(v3::CustomDataOpt const &val) {
        auto wrapper = JSONWrapper::New_ctor();
        wrapper->value = val;

        return wrapper;
    }

    inline JSONWrapperUTF16 *ToJsonWrapper(v3::CustomDataOptUTF16 const &val) {
        auto wrapper = JSONWrapperUTF16::New_ctor();
        wrapper->value = val;

        return wrapper;
    }

    constexpr CustomNoteData *
    CreateCustomBasicNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer, ColorType colorType,
                              NoteCutDirection cutDirection, v3::CustomDataOpt const &customData) {
        auto b = CustomNoteData::New_ctor(time,
                                          lineIndex,
                                          (NoteLineLayer) noteLineLayer,
                                          (NoteLineLayer) noteLineLayer,
                                          (NoteData::GameplayType) NoteData::GameplayType::Normal,
                                          (NoteData::ScoringType) NoteData::ScoringType::Normal,
                                          (ColorType) colorType,
                                          (NoteCutDirection) cutDirection,
                                          0.0f,
                                          0.0f,
                                          lineIndex,
                                          0.0f,
                                          0.0f,
                                          1.0f);

        b->customData = ToJsonWrapper(customData);

        return b;
    }

    constexpr CustomNoteData *CreateCustomBombNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
                                             v3::CustomDataOpt const &customData) {
        auto b = CustomNoteData::New_ctor(time,
                                          lineIndex,
                                          noteLineLayer,
                                          noteLineLayer,
                                          (NoteData::GameplayType) NoteData::GameplayType::Bomb,
                                          (NoteData::ScoringType) NoteData::ScoringType::NoScore,
                                          (ColorType) ColorType::None,
                                          (NoteCutDirection) NoteCutDirection::None,
                                          0.0f,
                                          0.0f,
                                          lineIndex,
                                          0.0f,
                                          0.0f,
                                          1.0f);

        b->customData = ToJsonWrapper(customData);

        return b;
    }

    constexpr CustomNoteData *CreateCustomBurstNoteData(float time,
                                              int lineIndex,
                                              NoteLineLayer noteLineLayer,
                                              NoteLineLayer beforeJumpNoteLineLayer,
                                              ColorType colorType,
                                              NoteCutDirection cutDirection,
                                              float cutSfxVolumeMultiplier, v3::CustomDataOpt const &customData) {
        auto b = CustomNoteData::New_ctor(time,
                                          lineIndex,
                                          noteLineLayer,
                                          beforeJumpNoteLineLayer,
                                          (NoteData::GameplayType) NoteData::GameplayType::BurstSliderElement,
                                          (NoteData::ScoringType) NoteData::ScoringType::BurstSliderElement,
                                          colorType,
                                          cutDirection,
                                          0,
                                          0,
                                          lineIndex,
                                          0,
                                          0,
                                          cutSfxVolumeMultiplier);

        b->customData = ToJsonWrapper(customData);

        return b;
    }

    constexpr auto CustomSliderData_CreateCustomBurstSliderData(
            ColorType colorType,
            float headTime,
            int headLineIndex,
            NoteLineLayer headLineLayer,
            NoteLineLayer headBeforeJumpLineLayer,
            NoteCutDirection headCutDirection,
            float tailTime,
            int tailLineIndex,
            NoteLineLayer tailLineLayer,
            NoteLineLayer tailBeforeJumpLineLayer,
            NoteCutDirection tailCutDirection,
            int sliceCount,
            float squishAmount,
            v3::CustomDataOpt const &customData) {
        auto slider = CustomSliderData::New_ctor(
                (SliderData::Type) SliderData::Type::Burst,
                colorType,
                false,
                headTime,
                headLineIndex,
                headLineLayer,
                headBeforeJumpLineLayer,
                0.0f,
                headCutDirection,
                0.0f,
                false,
                tailTime,
                tailLineIndex,
                tailLineLayer,
                tailBeforeJumpLineLayer,
                0.0f,
                tailCutDirection,
                0.0f,
                (SliderMidAnchorMode) SliderMidAnchorMode::Straight,
                sliceCount,
                squishAmount);
        slider->customData = ToJsonWrapper(customData);

        return slider;
    }

    constexpr auto CustomSliderData_CreateCustomSliderData(
            ColorType colorType,
            float headTime,
            int headLineIndex,
            NoteLineLayer headLineLayer,
            NoteLineLayer headBeforeJumpLineLayer,
            float headControlPointLengthMultiplier,
            NoteCutDirection headCutDirection,
            float tailTime,
            int tailLineIndex,
            NoteLineLayer tailLineLayer,
            NoteLineLayer tailBeforeJumpLineLayer,
            float tailControlPointLengthMultiplier,
            NoteCutDirection tailCutDirection,
            SliderMidAnchorMode midAnchorMode,
            v3::CustomDataOpt const &customData) {
        auto slider = CustomSliderData::New_ctor(
                (SliderData::Type) SliderData::Type::Normal,
                colorType,
                false,
                headTime,
                headLineIndex,
                headLineLayer,
                headBeforeJumpLineLayer,
                headControlPointLengthMultiplier,
                headCutDirection,
                0.0f,
                false,
                tailTime,
                tailLineIndex,
                tailLineLayer,
                tailBeforeJumpLineLayer,
                tailControlPointLengthMultiplier,
                tailCutDirection,
                0.0f,
                midAnchorMode,
                0,
                1.0f);
        slider->customData = ToJsonWrapper(customData);

        return slider;
    }

    template<typename T>
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
        if (environmentColorType == BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color0) {
            return EnvironmentColorType::Color0;
        }
        if (environmentColorType != BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color1) {
            return EnvironmentColorType::Color0;
        }
        return EnvironmentColorType::Color1;
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

    constexpr LightRotationBeatmapEventData::Axis ConvertAxis(BeatmapSaveDataVersion3::BeatmapSaveData::Axis axis) {
        if (axis == BeatmapSaveDataVersion3::BeatmapSaveData::Axis::X) {
            return LightRotationBeatmapEventData::Axis::X;
        }
        if (axis != BeatmapSaveDataVersion3::BeatmapSaveData::Axis::Y) {
            return LightRotationBeatmapEventData::Axis::X;
        }
        return LightRotationBeatmapEventData::Axis::Y;
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
            default:
                return EaseType::None;
        }
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


    template<typename To, typename From, typename... TArgs>
    requires(std::is_pointer_v<To> && std::is_pointer_v<From>)
    struct CppConverter {
        std::unordered_map<Il2CppClass *, std::function<To(From, TArgs...)>> converters;

        template<typename U, typename F>
        requires(std::is_pointer_v<U> &&
        std::is_convertible_v<U, From> &&
        std::is_convertible_v<F, std::function<To(U, TArgs...)>>)
        constexpr void AddConverter(F&& o) {
            converters[classof(U)] = [o](From const &t, TArgs const&... args) constexpr { return o(static_cast<U>(t), args...); };
        }


        constexpr To ProcessItem(From o, TArgs&... args) const {
            auto uKlass = o ? o->klass : classof(From);
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
        const float bpmChangeStartTime;
        const float bpmChangeStartBpmTime;
        const float bpm;

        constexpr BpmChangeData(float const bpmChangeStartTime, float const bpmChangeStartBpmTime, float const bpm)
                : bpmChangeStartTime(bpmChangeStartTime), bpmChangeStartBpmTime(bpmChangeStartBpmTime), bpm(bpm) {}
    };

    struct BpmTimeProcessor {
        std::vector<BpmChangeData> bpmChangeDataList;

        BpmTimeProcessor(float startBpm,
                         VList<BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData *> bpmEventsSaveData) {
            bpmChangeDataList.reserve(bpmEventsSaveData.size() + 1);
            bpmChangeDataList.emplace_back(0, 0, startBpm);

            for (auto const &v: bpmEventsSaveData) {
                if (!v) continue;

                auto const &bpmChangeData = bpmChangeDataList.back();
                float beat = BeatmapSaveDataItem_GetBeat(v);
                float bpm = v->m;
                float bpmChangeStartTime = bpmChangeData.bpmChangeStartTime +
                                           (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
                bpmChangeDataList.emplace_back(bpmChangeStartTime, beat, bpm);
            }
        }

        constexpr float ConvertBeatToTime(float beat) {
            int num = 0;
            while (num < bpmChangeDataList.size() - 1 && bpmChangeDataList[num + 1].bpmChangeStartBpmTime < beat) {
                num++;
            }
            auto const &bpmChangeData = bpmChangeDataList[num];
            return bpmChangeData.bpmChangeStartTime +
                   (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
        }
    };

    constexpr IIndexFilter *IndexFilterConvertor_Convert(BeatmapSaveData::IndexFilter *indexFilter, int groupSize) {
        if (IndexFilter_GetFilterType(indexFilter) == BeatmapSaveData::IndexFilter::IndexFilterType::Division) {
            int param = IndexFilter_GetParam0(indexFilter);
            int param2 = IndexFilter_GetParam1(indexFilter);
            auto num = (int) std::ceil((float) groupSize / (float) param);
            if (IndexFilter_GetReversed(indexFilter)) {
                int num2 = groupSize - num * param2 - 1;
                return reinterpret_cast<IIndexFilter *>(RangeIndexFilter::New_ctor(num2, std::max(0, num2 - num + 1)));
            }
            int num3 = num * param2;
            return reinterpret_cast<IIndexFilter *>(RangeIndexFilter::New_ctor(num3,
                                                                               std::min(groupSize - 1,
                                                                                        num3 + num - 1)));
        } else {
            if (IndexFilter_GetFilterType(indexFilter) != BeatmapSaveData::IndexFilter::IndexFilterType::StepAndOffset) {
                return nullptr;
            }
            int param3 = IndexFilter_GetParam0(indexFilter);
            int param4 = IndexFilter_GetParam1(indexFilter);
            int num4 = groupSize - param3;
            if (num4 <= 0) {
                throw std::runtime_error("Argument out of range for index converter!");
            }
            int count = (param4 == 0) ? 1 : (int) std::ceil((float) num4 / (float) param4);
            if (IndexFilter_GetReversed(indexFilter)) {
                return reinterpret_cast<IIndexFilter *>(BaseIndexFilter::New_ctor(groupSize - 1 - param3, -param4,
                                                                                  count));
            }
            return reinterpret_cast<IIndexFilter *>(BaseIndexFilter::New_ctor(param3, param4, count));
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

    constexpr LightColorBaseData *LightColorBaseData_Convert(BeatmapSaveData::LightColorBaseData *saveData) {
        return LightColorBaseData::New_ctor(BeatmapSaveDataItem_GetBeat(saveData),
                                            ConvertBeatmapEventTransitionType(LightColorBaseData_GetTransitionType(saveData)),
                                            ConvertColorType(LightColorBaseData_GetColorType(saveData)),
                                            LightColorBaseData_GetBrightness(saveData),
                                            LightColorBaseData_GetStrobeFrequency(saveData));
    }

    constexpr LightRotationBaseData *LightRotationBaseData_Convert(BeatmapSaveData::LightRotationBaseData *saveData) {
        return LightRotationBaseData::New_ctor(saveData->b, saveData->get_usePreviousEventRotationValue(),
                                               ConvertEaseType(saveData->e), LightRotationBaseData_GetRotation(saveData),
                                               LightRotationBaseData_GetLoopsCount(saveData),
                                               ConvertRotationOrientation(saveData->o));
    }

    struct EventBoxGroupConvertor {
        EventBoxGroupConvertor(EnvironmentLightGroups *lightGroups) {
            this->lightGroups = lightGroups;
            dataConvertor.AddConverter<BeatmapSaveData::LightColorEventBox *>(
                    [](BeatmapSaveData::LightColorEventBox *saveData,
                       EnvironmentLightGroups::LightGroupData *lightGroupData) {
                        auto indexFilter = IndexFilterConvertor_Convert(saveData->f, lightGroupData->numberOfElements);
                        auto saveDataList = reinterpret_cast<System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData *> *>(saveData->e);
                        VList<LightColorBaseData *> list(saveDataList->get_Count());

                        for (auto saveData2: VList(saveDataList)) {
                            list.push_back(LightColorBaseData_Convert(saveData2));
                        }

                        auto beatDistributionParamType = DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
                        auto brightnessDistributionParamType = DistributionParamType_Convert(
                                saveData->get_brightnessDistributionParamType());
                        return LightColorBeatmapEventDataBox::New_ctor(indexFilter,
                                                                       EventBox_GetBeatDistributionParam(saveData),
                                                                       beatDistributionParamType,
                                                                       saveData->get_brightnessDistributionParam(),
                                                                       brightnessDistributionParamType,
                                                                       saveData->get_brightnessDistributionShouldAffectFirstBaseEvent(),
                                                                       reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightColorBaseData *> *>(list.getInner()));
                    });

            dataConvertor.AddConverter<BeatmapSaveData::LightRotationEventBox *>(
                    [](BeatmapSaveData::LightRotationEventBox *saveData,
                       EnvironmentLightGroups::LightGroupData *lightGroupData) {
                        auto indexFilter = IndexFilterConvertor_Convert(saveData->f,
                                                                        lightGroupData->numberOfElements);
                        auto collection = reinterpret_cast<System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData *> *>(saveData->get_lightRotationBaseDataList());

                        VList<LightRotationBaseData *> list(collection->get_Count());

                        for (auto saveData2: VList(collection)) {
                            list.push_back(LightRotationBaseData_Convert(saveData2));
                        }

                        auto beatDistributionParamType = DistributionParamType_Convert(EventBox_GetBeatDistributionParamType(saveData));
                        auto rotationDistributionParamType = DistributionParamType_Convert(
                                saveData->get_rotationDistributionParamType());
                        return LightRotationBeatmapEventDataBox::New_ctor(indexFilter,
                                                                          EventBox_GetBeatDistributionParam(saveData),
                                                                          beatDistributionParamType,
                                                                          ConvertAxis(saveData->a),
                                                                          saveData->get_flipRotation(),
                                                                          saveData->get_rotationDistributionParam(),
                                                                          rotationDistributionParamType,
                                                                          saveData->get_rotationDistributionShouldAffectFirstBaseEvent(),
                                                                          reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightRotationBaseData *> *>(list.getInner()));
                    });
        }


        BeatmapEventDataBoxGroup * Convert(BeatmapSaveDataVersion3::BeatmapSaveData::EventBoxGroup *eventBoxGroupSaveData) const {
            auto dataForGroup = this->lightGroups->GetDataForGroup(eventBoxGroupSaveData->get_groupId());
            if (dataForGroup == nullptr) {
                return nullptr;
            }

            auto collection = reinterpret_cast<System::Collections::Generic::List_1<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox *> *>(eventBoxGroupSaveData->get_baseEventBoxes());
            VList<BeatmapEventDataBox *> list(collection->get_Count());

            for (auto item: VList<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox *>(collection)) {
                auto beatmapEventDataBox = dataConvertor.ProcessItem(item, dataForGroup);
                if (beatmapEventDataBox != nullptr) {
                    list.push_back(beatmapEventDataBox);
                }
            }

            return BeatmapEventDataBoxGroup::New_ctor(eventBoxGroupSaveData->b,
                                                      reinterpret_cast<IReadOnlyCollection_1<::GlobalNamespace::BeatmapEventDataBox *> *>(list.getInner()));
        }

        CppConverter<BeatmapEventDataBox *, BeatmapSaveDataVersion3::BeatmapSaveData::EventBox *, EnvironmentLightGroups::LightGroupData *> dataConvertor;

        EnvironmentLightGroups *lightGroups;
    };

    struct BeatmapEventDataBoxGroupListsCpp {
        std::unordered_map<int, BeatmapEventDataBoxGroupList*> _beatmapEventDataBoxGroupListDict;

        BeatmapData *_beatmapData;
        GlobalNamespace::IBeatToTimeConvertor* _beatToTimeConvertor;
        bool _updateBeatmapDataOnInsert;

        BeatmapEventDataBoxGroupListsCpp(BeatmapData *beatmapData, GlobalNamespace::IBeatToTimeConvertor* const &beatToTimeConvertor,
                                         bool updateBeatmapDataOnInsert) : _beatmapData(beatmapData),
                                                                           _beatToTimeConvertor(beatToTimeConvertor),
                                                                           _updateBeatmapDataOnInsert(
                                                                                   updateBeatmapDataOnInsert) {}

        auto Insert(int groupId, BeatmapEventDataBoxGroup* beatmapEventDataBoxGroup) {
            auto& beatmapEventDataBoxGroupList = _beatmapEventDataBoxGroupListDict[groupId];

            if (!beatmapEventDataBoxGroupList) {
                beatmapEventDataBoxGroupList = BeatmapEventDataBoxGroupList::New_ctor(groupId, this->_beatmapData,
                                                                                this->_beatToTimeConvertor);
                beatmapEventDataBoxGroupList->updateBeatmapDataOnInsert = _updateBeatmapDataOnInsert;
            }

            return beatmapEventDataBoxGroupList->Insert(beatmapEventDataBoxGroup);
        }

        auto SyncWithBeatmapData() const {
            for (auto const& [groupid, list] : _beatmapEventDataBoxGroupListDict) {

            }
        }

        auto BeatmapEventDataBoxGroupList_SyncWithBeatmapData() const {

        }
    };

    void DefaultEnvironmentEventsFactory_InsertDefaultEnvironmentEventsLightGroups(DefaultEnvironmentEvents * defaultEnvironmentEvents, EnvironmentLightGroups* environmentLightGroups, BeatmapEventDataBoxGroupListsCpp& beatmapEventDataBoxGroupLists) {
        if (defaultEnvironmentEvents && !defaultEnvironmentEvents->get_isEmpty() && defaultEnvironmentEvents->lightGroupEvents.convert() != nullptr)
        {
            for (auto lightGroupEvent : defaultEnvironmentEvents->lightGroupEvents)
            {
                auto dataForGroup = environmentLightGroups->GetDataForGroup(lightGroupEvent->groupId);
                if (dataForGroup != nullptr)
                {
                    BeatmapEventDataBoxGroup* beatmapEventDataBoxGroup = BeatmapEventDataBoxGroupFactory::CreateSingleLightBeatmapEventDataBoxGroup(0.0f, dataForGroup->numberOfElements, lightGroupEvent->environmentColorType, lightGroupEvent->brightness, lightGroupEvent->rotationX, lightGroupEvent->rotationY);
                    beatmapEventDataBoxGroupLists.Insert(dataForGroup->groupId, beatmapEventDataBoxGroup);
                }
            }
        }
    }
}