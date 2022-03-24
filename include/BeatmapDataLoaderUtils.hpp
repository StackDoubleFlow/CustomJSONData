#pragma once

#include "GlobalNamespace/ColorBoostBeatmapEventData.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"
#include "GlobalNamespace/ISortedList_1.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/MultipleSortedListsEnumerator_1.hpp"
#include "GlobalNamespace/BPMChangeBeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapDataLoader_SpecialEventsFilter.hpp"
#include "GlobalNamespace/BeatmapDataLoader_BpmTimeProcessor.hpp"
#include "GlobalNamespace/BeatmapEventTransitionType.hpp"
#include "GlobalNamespace/BeatmapDataLoader.hpp"
#include "GlobalNamespace/BeatmapDataMirrorTransform.hpp"
#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "GlobalNamespace/DataConvertor_1.hpp"
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
#include "GlobalNamespace/BeatmapDataLoader_EventBoxGroupConvertor.hpp"
#include "GlobalNamespace/SpawnRotationBeatmapEventData.hpp"
#include "GlobalNamespace/DefaultEnvironmentEventsFactory.hpp"
#include "GlobalNamespace/GameSongController.hpp"

#include "System/Comparison_1.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "System/Linq/Enumerable.hpp"
#include "System/Version.hpp"

#include "UnityEngine/Resources.hpp"

#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "CustomEventData.h"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"


#include <chrono>
#include <codecvt>
#include <locale>

// this header exists purely for organizing

namespace CustomJSONData {
    using namespace System;
    using namespace System::Collections::Generic;
    using namespace GlobalNamespace;
    using namespace CustomJSONData;
    using namespace BeatmapSaveDataVersion3;

    JSONWrapper *ToJsonWrapper(v3::CustomDataOpt const &val) {
        auto wrapper = JSONWrapper::New_ctor();
        wrapper->value = val;

        return wrapper;
    }

    JSONWrapperUTF16 *ToJsonWrapper(v3::CustomDataOptUTF16 const &val) {
        auto wrapper = JSONWrapperUTF16::New_ctor();
        wrapper->value = val;

        return wrapper;
    }

    CustomNoteData *
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

        b->customData = JSONWrapper::New_ctor();
        b->customData->value = customData;

        return b;
    }

    CustomNoteData *CreateCustomBombNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer,
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

        b->customData = JSONWrapper::New_ctor();
        b->customData->value = customData;

        return b;
    }

    CustomNoteData *CreateCustomBurstNoteData(float time,
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

        b->customData = JSONWrapper::New_ctor();
        b->customData->value = customData;

        return b;
    }


    static auto CreateCustomBurstSliderData(
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

    static auto CreateCustomSliderData(
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
        return (a->b < b->b);
    }


    static NoteLineLayer GetNoteLineLayer(int lineLayer) {
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

    static ColorType ConvertColorType(BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType noteType) {
        if (noteType == BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType::ColorA) {
            return ColorType::ColorA;
        }
        if (noteType != BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType::ColorB) {
            return ColorType::None;
        }
        return ColorType::ColorB;
    }

    static EnvironmentColorType
    ConvertColorType(BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType environmentColorType) {
        if (environmentColorType == BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color0) {
            return EnvironmentColorType::Color0;
        }
        if (environmentColorType != BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color1) {
            return EnvironmentColorType::Color0;
        }
        return EnvironmentColorType::Color1;
    }

    static BeatmapEventTransitionType
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

    static LightRotationBeatmapEventData::Axis ConvertAxis(BeatmapSaveDataVersion3::BeatmapSaveData::Axis axis) {
        if (axis == BeatmapSaveDataVersion3::BeatmapSaveData::Axis::X) {
            return LightRotationBeatmapEventData::Axis::X;
        }
        if (axis != BeatmapSaveDataVersion3::BeatmapSaveData::Axis::Y) {
            return LightRotationBeatmapEventData::Axis::X;
        }
        return LightRotationBeatmapEventData::Axis::Y;
    }

    static EaseType ConvertEaseType(BeatmapSaveDataVersion3::BeatmapSaveData::EaseType easeType) {
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

    static NoteLineLayer ConvertNoteLineLayer(int layer) {
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

    static SliderData::Type ConvertSliderType(BeatmapSaveDataVersion3::BeatmapSaveData::SliderType sliderType) {
        if (sliderType == BeatmapSaveDataVersion3::BeatmapSaveData::SliderType::Normal) {
            return SliderData::Type::Normal;
        }
        if (sliderType != BeatmapSaveDataVersion3::BeatmapSaveData::SliderType::Burst) {
            return SliderData::Type::Normal;
        }
        return SliderData::Type::Burst;
    }

    static LightRotationDirection ConvertRotationOrientation(
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


    template<typename T> requires(std::is_pointer_v<T>)
    struct CppConverter {
        std::unordered_map<Il2CppClass *, std::function<T(T)>> converters;

        template<typename U> requires(std::is_pointer_v<U>)
        void AddConverter(std::function<T(U)> const& o) {
            converters[classof(U)] = [o](T const &t) { return reinterpret_cast<T>(o(reinterpret_cast<U>(t))); };
        }

        template<typename U> requires(std::is_pointer_v<U>)
        T ProcessItem(U
        o) const {
            auto uKlass = o ? o->klass : classof(U);
            auto it = converters.find(uKlass);
            if (it == converters.end()) {
//                for (auto const& [klass, func] : converters) {
//                    if (il2cpp_functions::class_is_assignable_from(klass, uKlass)) {
//                        return func(reinterpret_cast<T>(o));
//                    }
//                }
                return {};
            }

            return (it->second)(reinterpret_cast<T>(o));
        }
    };

    template<typename T, typename ExtraParam> requires(std::is_pointer_v<T>)
    struct CppConverter2Param {
        std::unordered_map<Il2CppClass *, std::function<T(T, ExtraParam)>> converters;

        template<typename U> requires(std::is_pointer_v<U>)
        void AddConverter(std::function<T(U, ExtraParam)> const& o) {
            converters[classof(U)] = [o](T const &t, ExtraParam const &extraParam) {
                return reinterpret_cast<T>(o(reinterpret_cast<U>(t), extraParam));
            };
        }

        template<typename U> requires(std::is_pointer_v<U>)
        T ProcessItem(U
        o, ExtraParam extraParam) const {
            auto uKlass = o ? o->klass : classof(U);
            auto it = converters.find(uKlass);
            if (it == converters.end()) {
//                for (auto const& [klass, func] : converters) {
//                    if (il2cpp_functions::class_is_assignable_from(klass, uKlass)) {
//                        return func(reinterpret_cast<T>(o), extraParam);
//                    }
//                }
                return {};
            }

            return (it->second)(reinterpret_cast<T>(o), extraParam);
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
            bpmChangeDataList.reserve(bpmEventsSaveData.size());
            bpmChangeDataList.emplace_back(0, 0, startBpm);

            for (auto const &v: bpmEventsSaveData) {
                if (!v) continue;

                auto const &bpmChangeData = bpmChangeDataList.back();
                float beat = v->get_beat();
                float bpm = v->get_bpm();
                float bpmChangeStartTime = bpmChangeData.bpmChangeStartTime +
                                           (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
                bpmChangeDataList.emplace_back(bpmChangeStartTime, beat, bpm);
            }
        }

        float ConvertBeatToTime(float beat) {
            int num = 0;
            while (num < bpmChangeDataList.size() - 1 && bpmChangeDataList[num + 1].bpmChangeStartBpmTime < beat) {
                num++;
            }
            auto const &bpmChangeData = bpmChangeDataList[num];
            return bpmChangeData.bpmChangeStartTime +
                   (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
        }
    };

    static IIndexFilter *IndexFilterConvertor_Convert(BeatmapSaveData::IndexFilter *indexFilter, int groupSize) {
        CRASH_UNLESS(indexFilter);
        if (indexFilter->get_type() == BeatmapSaveData::IndexFilter::IndexFilterType::Division) {
            int param = indexFilter->get_param0();
            int param2 = indexFilter->get_param1();
            auto num = (int) std::ceil((float) groupSize / (float) param);
            if (indexFilter->get_reversed()) {
                int num2 = groupSize - num * param2 - 1;
                return reinterpret_cast<IIndexFilter *>(RangeIndexFilter::New_ctor(num2, std::max(0, num2 - num + 1)));
            }
            int num3 = num * param2;
            return reinterpret_cast<IIndexFilter *>(RangeIndexFilter::New_ctor(num3,
                                                                               std::min(groupSize - 1,
                                                                                        num3 + num - 1)));
        } else {
            if (indexFilter->get_type() != BeatmapSaveData::IndexFilter::IndexFilterType::StepAndOffset) {
                return nullptr;
            }
            int param3 = indexFilter->get_param0();
            int param4 = indexFilter->get_param1();
            int num4 = groupSize - param3;
            if (num4 <= 0) {
                throw std::runtime_error("Argument out of range for index converter!");
            }
            int count = (param4 == 0) ? 1 : (int) std::ceil((float) num4 / (float) param4);
            if (indexFilter->get_reversed()) {
                return reinterpret_cast<IIndexFilter *>(BaseIndexFilter::New_ctor(groupSize - 1 - param3, -param4,
                                                                                  count));
            }
            return reinterpret_cast<IIndexFilter *>(BaseIndexFilter::New_ctor(param3, param4, count));
        }
    }

    static BeatmapEventDataBox::DistributionParamType
    DistributionParamType_Convert(BeatmapSaveData::EventBox::DistributionParamType distributionParamType) {
        if (distributionParamType == BeatmapSaveDataVersion3::BeatmapSaveData::EventBox::DistributionParamType::Wave) {
            return BeatmapEventDataBox::DistributionParamType::Wave;
        }
        if (distributionParamType != BeatmapSaveDataVersion3::BeatmapSaveData::EventBox::DistributionParamType::Step) {
            throw std::runtime_error("distributionParamType error!");
        }
        return BeatmapEventDataBox::DistributionParamType::Step;
    }

    static LightColorBaseData *LightColorBaseData_Convert(BeatmapSaveData::LightColorBaseData *saveData) {
        return LightColorBaseData::New_ctor(saveData->get_beat(),
                                            ConvertBeatmapEventTransitionType(saveData->get_transitionType()),
                                            ConvertColorType(saveData->get_colorType()), saveData->get_brightness(),
                                            saveData->get_strobeBeatFrequency());
    }

    static LightRotationBaseData *LightRotationBaseData_Convert(BeatmapSaveData::LightRotationBaseData *saveData) {
        return LightRotationBaseData::New_ctor(saveData->get_beat(), saveData->get_usePreviousEventRotationValue(),
                                               ConvertEaseType(saveData->get_easeType()), saveData->get_rotation(),
                                               saveData->get_loopsCount(),
                                               ConvertRotationOrientation(saveData->get_rotationDirection()));
    }

    struct EventBoxGroupConvertor {
        EventBoxGroupConvertor(EnvironmentLightGroups *lightGroups) {
            this->lightGroups = lightGroups;
            dataConvertor.AddConverter<BeatmapSaveData::LightColorEventBox *>(
                    [](BeatmapSaveData::LightColorEventBox *saveData,
                       EnvironmentLightGroups::LightGroupData *lightGroupData) {
                        auto indexFilter = IndexFilterConvertor_Convert(saveData->get_indexFilter(), lightGroupData->numberOfElements);
                        auto saveDataList = reinterpret_cast<System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData *> *>(saveData->get_lightColorBaseDataList());
                        VList<LightColorBaseData *> list(saveDataList->get_Count());

                        for (auto saveData2: VList(saveDataList)) {
                            list.push_back(LightColorBaseData_Convert(saveData2));
                        }

                        auto beatDistributionParamType = DistributionParamType_Convert(
                                saveData->get_beatDistributionParamType());
                        auto brightnessDistributionParamType = DistributionParamType_Convert(
                                saveData->get_brightnessDistributionParamType());
                        return LightColorBeatmapEventDataBox::New_ctor(indexFilter,
                                                                       saveData->get_beatDistributionParam(),
                                                                       beatDistributionParamType,
                                                                       saveData->get_brightnessDistributionParam(),
                                                                       brightnessDistributionParamType,
                                                                       saveData->get_brightnessDistributionShouldAffectFirstBaseEvent(),
                                                                       reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightColorBaseData *> *>(list.getInner()));
                    });

            dataConvertor.AddConverter<BeatmapSaveData::LightRotationEventBox *>(
                    [](BeatmapSaveData::LightRotationEventBox *saveData,
                       EnvironmentLightGroups::LightGroupData *lightGroupData) {
                        auto indexFilter = IndexFilterConvertor_Convert(saveData->get_indexFilter(),
                                                                        lightGroupData->numberOfElements);
                        VList<LightRotationBaseData *> list(reinterpret_cast<IReadOnlyCollection_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData *> *>(saveData->get_lightRotationBaseDataList())->get_Count());

                        for (auto saveData2: VList(
                                reinterpret_cast<System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData *> *>(saveData->get_lightRotationBaseDataList()))) {
                            list.push_back(LightRotationBaseData_Convert(saveData2));
                        }

                        auto beatDistributionParamType = DistributionParamType_Convert(
                                saveData->get_rotationDistributionParamType());
                        auto rotationDistributionParamType = DistributionParamType_Convert(
                                saveData->get_rotationDistributionParamType());
                        return LightRotationBeatmapEventDataBox::New_ctor(indexFilter,
                                                                          saveData->get_beatDistributionParam(),
                                                                          beatDistributionParamType,
                                                                          ConvertAxis(saveData->get_axis()),
                                                                          saveData->get_flipRotation(),
                                                                          saveData->get_rotationDistributionParam(),
                                                                          rotationDistributionParamType,
                                                                          saveData->get_rotationDistributionShouldAffectFirstBaseEvent(),
                                                                          reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightRotationBaseData *> *>(list.getInner()));
                    });
        }


        BeatmapEventDataBoxGroup *
        Convert(BeatmapSaveDataVersion3::BeatmapSaveData::EventBoxGroup *eventBoxGroupSaveData) {
            auto collection = reinterpret_cast<IReadOnlyCollection_1<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox *> *>(eventBoxGroupSaveData->get_baseEventBoxes());
            VList<BeatmapEventDataBox *> list;


            auto dataForGroup = this->lightGroups->GetDataForGroup(eventBoxGroupSaveData->get_groupId());
            if (dataForGroup == nullptr) {
                return nullptr;
            }

            for (auto item: VList<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox *>(
                    reinterpret_cast<System::Collections::Generic::List_1<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox *> *>(collection))) {
                auto beatmapEventDataBox = dataConvertor.ProcessItem(item, dataForGroup);
                if (beatmapEventDataBox != nullptr) {
                    list.push_back(beatmapEventDataBox);
                }
            }

            list.trim();
            return BeatmapEventDataBoxGroup::New_ctor(eventBoxGroupSaveData->get_beat(),
                                                      reinterpret_cast<IReadOnlyCollection_1<::GlobalNamespace::BeatmapEventDataBox *> *>(list.getInner()));
        }

        CppConverter2Param<BeatmapEventDataBox *, EnvironmentLightGroups::LightGroupData *> dataConvertor;

        EnvironmentLightGroups *lightGroups;
    };

}