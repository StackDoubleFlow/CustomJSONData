#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "songloader/shared/API.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"

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

#include "GlobalNamespace/GameSongController.hpp"
#include "UnityEngine/Resources.hpp"

#include "CustomBeatmapSaveDatav2.h"
#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapData.h"
#include "CustomEventData.h"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

#include <regex>

// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"
#include "GlobalNamespace/BeatmapEventDataBoxGroupLists.hpp"
#include "GlobalNamespace/BeatmapDataLoader_EventBoxGroupConvertor.hpp"
#include "GlobalNamespace/SpawnRotationBeatmapEventData.hpp"
#include "GlobalNamespace/DefaultEnvironmentEventsFactory.hpp"

#include <chrono>
#include <codecvt>
#include <locale>

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapSaveDataVersion3;

SafePtr<v3::CustomBeatmapSaveData> cachedSaveData;
SafePtr<System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*>> eventList;
SafePtr<BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords> eventList2;


// This is to prevent issues with string limits
std::string to_utf8(std::u16string_view view) {
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(view.data());
}

#define FindMethodGetter(qualifiedTypeName, methodName) \
::il2cpp_utils::il2cpp_type_check::MetadataGetter<#methodName, qualifiedTypeName, decltype(&qualifiedTypeName::methodName)>::get();

static std::string GetVersionFromPath(std::string_view path)
{
    // SongCore has a fallback so i guess i do too
    static const std::string_view fallback = "2.0.0";

    auto truncatedText = path.substr(0, 50);
    static const std::regex versionRegex (R"("_?version"\s*:\s*"[0-9]+\.[0-9]+\.?[0-9]?")");
    std::match_results<std::string_view::const_iterator> matches;
    if(std::regex_search(truncatedText.begin(), truncatedText.end(), matches, versionRegex)) {
        if (!matches.empty()) {
            auto version = matches[0].str();
            version = version.substr(0, version.length() - 1);
            version = version.substr(version.find_last_of('\"') + 1, version.length());

            return version;
        }
    }

    return std::string(fallback);
}


BeatmapData * CustomBeatmapData_GetFilteredCopy(CustomBeatmapData* self, System::Func_2<::GlobalNamespace::BeatmapDataItem*, ::GlobalNamespace::BeatmapDataItem*>* processDataItem) {
    self->isCreatingFilteredCopy = true;
    auto beatmapData = self->BaseCopy();
    auto enumerator = self->allBeatmapData->get_items()->GetEnumerator();

    while (true)
    {
        if (!enumerator.MoveNext()) {
            break;
        }

        auto const& beatmapDataItem = enumerator.get_Current();

        BeatmapDataItem* beatmapDataItem2 = processDataItem->Invoke(beatmapDataItem->GetCopy());
        if (beatmapDataItem2)
        {
            if (auto event = il2cpp_utils::try_cast<BeatmapEventData>(beatmapDataItem2)) {
                beatmapData->InsertBeatmapEventData(*event);
            }

            if (auto object = il2cpp_utils::try_cast<BeatmapObjectData>(beatmapDataItem2)) {
                beatmapData->AddBeatmapObjectData(*object);
            }

            if (auto customEvent = il2cpp_utils::try_cast<CustomEventData>(beatmapDataItem2)) {
                beatmapData->InsertCustomEventData(*customEvent);
            }
        }
    }

    self->isCreatingFilteredCopy = false;
    return beatmapData;
}

BeatmapData * CustomBeatmapData_GetCopy(CustomBeatmapData* self) {
    auto copy = self->BaseCopy();

    auto enumerator = self->allBeatmapData->get_items()->GetEnumerator();

    while (true)
    {
        if (!enumerator.MoveNext()) {
            break;
        }

        auto const& beatmapDataItem = enumerator.get_Current();

        if (beatmapDataItem)
        {
            if (auto event = il2cpp_utils::try_cast<BeatmapEventData>(beatmapDataItem)) {
                copy->InsertBeatmapEventData(*event);
            }

            if (auto object = il2cpp_utils::try_cast<BeatmapObjectData>(beatmapDataItem)) {
                copy->AddBeatmapObjectData(*object);
            }

            if (auto customEvent = il2cpp_utils::try_cast<CustomEventData>(beatmapDataItem)) {
                copy->InsertCustomEventData(*customEvent);
            }
        }
    }

    return copy;
}

MAKE_PAPER_HOOK_MATCH(BeatmapData_GetCopy, &CustomBeatmapData::GetCopy, BeatmapData *, BeatmapData* self) {
    static auto CustomBeatmapDataKlass = classof(CustomBeatmapData*);

    if (self->klass == CustomBeatmapDataKlass) {
        return CustomBeatmapData_GetCopy(reinterpret_cast<CustomBeatmapData*>(self));
    }

    return BeatmapData_GetCopy(self);
}

MAKE_PAPER_HOOK_MATCH(BeatmapData_GetFilteredCopy, &CustomBeatmapData::GetFilteredCopy, BeatmapData *, BeatmapData* self, System::Func_2<::GlobalNamespace::BeatmapDataItem*, ::GlobalNamespace::BeatmapDataItem*>* processDataItem) {
    static auto CustomBeatmapDataKlass = classof(CustomBeatmapData*);

    if (self->klass == CustomBeatmapDataKlass) {
        return CustomBeatmapData_GetFilteredCopy(reinterpret_cast<CustomBeatmapData*>(self), processDataItem);
    }

    return BeatmapData_GetFilteredCopy(self, processDataItem);
}

// This hook loads the json data (with custom data) into a BeatmapSaveData 
MAKE_PAPER_HOOK_MATCH(BeatmapSaveData_DeserializeFromJSONString, &BeatmapSaveDataVersion3::BeatmapSaveData::DeserializeFromJSONString, BeatmapSaveData*, StringW stringData) {
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json");
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!stringData) {
        CJDLogger::Logger.fmtLog<LogLevel::ERR>("No string data");
        return nullptr;
    }

    try {
        auto sharedDoc = std::make_shared<rapidjson::Document>();
        rapidjson::Document &doc = *sharedDoc;
        std::string contents(stringData);
        rapidjson::ParseResult result = doc.Parse(contents);

        if (!result || doc.IsNull() || doc.HasParseError()) {
            std::string errorCodeStr(rapidjson::GetParseError_En(result.Code()));
            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Unable to parse json due to {}", errorCodeStr);
            return nullptr;
        }
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json success");

        auto version = GetVersionFromPath(contents);

        v3::CustomBeatmapSaveData* saveData;

        if (semver::lte(std::string(version), "2.6.0")) {
            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 2.0.0 beatmap");
            SafePtr<v2::CustomBeatmapSaveData> beatmap(v2::CustomBeatmapSaveData::Deserialize(sharedDoc));

            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Got {} notes in beatmap v2", beatmap->notes->size);

            saveData = v3::CustomBeatmapSaveData::Convert2_6_0((v2::CustomBeatmapSaveData*) beatmap);

          } else {
            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing 3.0.0 beatmap");
            saveData = v3::CustomBeatmapSaveData::Deserialize(sharedDoc);
        }

        eventList2 = saveData->basicEventTypesWithKeywords;
        eventList = saveData->basicEventTypesWithKeywords->d;

        cachedSaveData = saveData;

        CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished reading beatmap data {}", fmt::ptr(saveData));
        auto stopTime = std::chrono::high_resolution_clock::now();
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("This took {}ms", (int) std::chrono::duration_cast<std::chrono::milliseconds>(
                stopTime - startTime).count());

        CJDLogger::Logger.fmtLog<LogLevel::DBG>("Basic event keywords {} ->d {} notes in beatmap v2 {} {}", fmt::ptr(saveData->basicEventTypesWithKeywords), fmt::ptr(saveData->basicEventTypesWithKeywords->d), saveData->basicEventTypesWithKeywords->d->get_Count(), fmt::ptr(saveData->basicEventTypesWithKeywords->d->items.convert()));
        return saveData;
    } catch (std::exception const& e) {
        CJDLogger::Logger.fmtLog<LogLevel::DBG>("There was an error loading the beatmap through CJD. Cause of error: {}", e.what());
        return nullptr;
    }
}


JSONWrapper* ToJsonWrapper(v3::CustomDataOpt const& val) {
    auto wrapper = JSONWrapper::New_ctor();
    wrapper->value = val;

    return wrapper;
}

CustomNoteData* CreateCustomBasicNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer, ColorType colorType, NoteCutDirection cutDirection, v3::CustomDataOpt const& customData) {
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

CustomNoteData* CreateCustomBombNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer, v3::CustomDataOpt const& customData) {
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

CustomNoteData* CreateCustomBurstNoteData(float time,
                                                              int lineIndex,
                                                              NoteLineLayer noteLineLayer,
                                                              NoteLineLayer beforeJumpNoteLineLayer,
                                                              ColorType colorType,
                                                              NoteCutDirection cutDirection,
                                                              float cutSfxVolumeMultiplier, v3::CustomDataOpt const& customData) {
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
        v3::CustomDataOpt const& customData)
{
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
        v3::CustomDataOpt const& customData)
{
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

template <typename T>
constexpr bool TimeCompare(T const a, T const b) {
    return (a->b < b->b);
}


static NoteLineLayer GetNoteLineLayer(int lineLayer)
{
    switch (lineLayer)
    {
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
 
static EnvironmentColorType ConvertColorType(BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType environmentColorType) {
    if (environmentColorType == BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color0) {
        return EnvironmentColorType::Color0;
    }
    if (environmentColorType != BeatmapSaveDataVersion3::BeatmapSaveData::EnvironmentColorType::Color1) {
        return EnvironmentColorType::Color0;
    }
    return EnvironmentColorType::Color1;
}

static BeatmapEventTransitionType ConvertBeatmapEventTransitionType(BeatmapSaveDataVersion3::BeatmapSaveData::TransitionType transitionType) {
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

 static LightRotationDirection ConvertRotationOrientation(BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData::RotationDirection rotationDirection) {
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


template <typename T>
requires(std::is_pointer_v<T>)
struct CppConverter {
    std::unordered_map<Il2CppClass*, std::function<T(T)>> converters;

    template <typename U>
    requires(std::is_pointer_v<U>)
    void AddConverter(std::function<T(U)> const& o) {
        converters[classof(U)] = [o](T const& t){ return reinterpret_cast<T>(o(reinterpret_cast<U>(t))); };
    }

    template <typename U>
    requires(std::is_pointer_v<U>)
    T ProcessItem(U o) const {
        auto it = converters.find(o ? o->klass : classof(U));
        if (it == converters.end()) return {};

        return (it->second)(reinterpret_cast<T>(o));
    }
};

template <typename T, typename ExtraParam>
requires(std::is_pointer_v<T>)
struct CppConverter2Param {
    std::unordered_map<Il2CppClass*, std::function<T(T, ExtraParam)>> converters;

    template <typename U>
    requires(std::is_pointer_v<U>)
    void AddConverter(std::function<T(U, ExtraParam)> const& o) {
        converters[classof(U)] = [o](T const& t, ExtraParam const& extraParam){ return reinterpret_cast<T>(o(reinterpret_cast<U>(t), extraParam)); };
    }

    template <typename U>
    requires(std::is_pointer_v<U>)
    T ProcessItem(U o, ExtraParam extraParam) const {
        auto it = converters.find(o ? o->klass : classof(U));
        if (it == converters.end()) return {};

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

    BpmTimeProcessor(float startBpm, VList<BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData*> bpmEventsSaveData)
    {
        bpmChangeDataList.reserve(bpmEventsSaveData.size());
        bpmChangeDataList.emplace_back(0,0,startBpm);

        for (auto const& v : bpmEventsSaveData) {
            if (!v) continue;

            auto const& bpmChangeData = bpmChangeDataList.back();
            float beat = v->get_beat();
            float bpm = v->get_bpm();
            float bpmChangeStartTime = bpmChangeData.bpmChangeStartTime + (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
            bpmChangeDataList.emplace_back(bpmChangeStartTime, beat, bpm);
        }
    }

    float ConvertBeatToTime(float beat)
    {
        int num = 0;
        while (num < bpmChangeDataList.size() - 1 && bpmChangeDataList[num + 1].bpmChangeStartBpmTime < beat)
        {
            num++;
        }
        auto const& bpmChangeData = bpmChangeDataList[num];
        return bpmChangeData.bpmChangeStartTime + (beat - bpmChangeData.bpmChangeStartBpmTime) / bpmChangeData.bpm * 60.0f;
    }
};

static IIndexFilter* IndexFilterConvertor_Convert(BeatmapSaveData::IndexFilter* indexFilter, int groupSize) {
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
                                                                           std::min(groupSize - 1, num3 + num - 1)));
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
            return reinterpret_cast<IIndexFilter *>(BaseIndexFilter::New_ctor(groupSize - 1 - param3, -param4, count));
        }
        return reinterpret_cast<IIndexFilter *>(BaseIndexFilter::New_ctor(param3, param4, count));
    }
}

static BeatmapEventDataBox::DistributionParamType DistributionParamType_Convert(BeatmapSaveData::EventBox::DistributionParamType distributionParamType) {
    if (distributionParamType == BeatmapSaveDataVersion3::BeatmapSaveData::EventBox::DistributionParamType::Wave) {
        return BeatmapEventDataBox::DistributionParamType::Wave;
    }
    if (distributionParamType != BeatmapSaveDataVersion3::BeatmapSaveData::EventBox::DistributionParamType::Step) {
        throw std::runtime_error("distributionParamType error!");
    }
    return BeatmapEventDataBox::DistributionParamType::Step;
}

static LightColorBaseData* LightColorBaseData_Convert(BeatmapSaveData::LightColorBaseData* saveData) {
    return LightColorBaseData::New_ctor(saveData->get_beat(),
                                        ConvertBeatmapEventTransitionType(saveData->get_transitionType()),
                                        ConvertColorType(saveData->get_colorType()), saveData->get_brightness(),
                                        saveData->get_strobeBeatFrequency());
}

static LightRotationBaseData* LightRotationBaseData_Convert(BeatmapSaveData::LightRotationBaseData* saveData) {
    return LightRotationBaseData::New_ctor(saveData->get_beat(), saveData->get_usePreviousEventRotationValue(),
                                           ConvertEaseType(saveData->get_easeType()), saveData->get_rotation(),
                                           saveData->get_loopsCount(),
                                           ConvertRotationOrientation(saveData->get_rotationDirection()));
}

struct EventBoxGroupConvertor {
    EventBoxGroupConvertor(EnvironmentLightGroups* lightGroups) {
        this->lightGroups = lightGroups;
        dataConvertor.AddConverter<BeatmapSaveData::LightColorEventBox*>([](BeatmapSaveData::LightColorEventBox* saveData, EnvironmentLightGroups::LightGroupData* lightGroupData){
            auto indexFilter = IndexFilterConvertor_Convert(saveData->get_indexFilter(), lightGroupData->numberOfElements);
            auto saveDataList = reinterpret_cast<System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorBaseData*>*>(saveData->get_lightColorBaseDataList());
            VList<LightColorBaseData*> list(saveDataList->get_Count());

//            auto enumerator = reinterpret_cast<IEnumerable_1<BeatmapSaveData::LightColorBaseData*>*>(list.getInner())->GetEnumerator();
//            auto nonGenericEnumerator = reinterpret_cast<Collections::IEnumerator*>(enumerator);
//
//            while (true) {
//                if (!nonGenericEnumerator->MoveNext()) {
//                    break;
//                }
//                auto saveData2 = enumerator->get_Current();

            for (auto saveData2 : VList(saveDataList)) {
                list.push_back(LightColorBaseData_Convert(saveData2));
            }

            auto beatDistributionParamType = DistributionParamType_Convert(saveData->get_beatDistributionParamType());
            auto brightnessDistributionParamType = DistributionParamType_Convert(saveData->get_brightnessDistributionParamType());
            return LightColorBeatmapEventDataBox::New_ctor(indexFilter, saveData->get_beatDistributionParam(), beatDistributionParamType, saveData->get_brightnessDistributionParam(), brightnessDistributionParamType, saveData->get_brightnessDistributionShouldAffectFirstBaseEvent(),
                                                           reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightColorBaseData *> *>(list.getInner()));
        });

        dataConvertor.AddConverter<BeatmapSaveData::LightRotationEventBox*>([](BeatmapSaveData::LightRotationEventBox* saveData, EnvironmentLightGroups::LightGroupData* lightGroupData){
            auto indexFilter = IndexFilterConvertor_Convert(saveData->get_indexFilter(), lightGroupData->numberOfElements);
            VList<LightRotationBaseData*> list(reinterpret_cast<IReadOnlyCollection_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData*>*>(saveData->get_lightRotationBaseDataList())->get_Count());

//            auto enumerator = reinterpret_cast<IEnumerable_1<BeatmapSaveData::LightRotationBaseData*>*>(list.getInner())->GetEnumerator();
//            auto nonGenericEnumerator = reinterpret_cast<Collections::IEnumerator*>(enumerator);
//
//            while (true) {
//                if (!nonGenericEnumerator->MoveNext()) {
//                    break;
//                }
//
//                auto saveData2 = enumerator->get_Current();
            for (auto saveData2 : VList(reinterpret_cast<System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationBaseData*>*>(saveData->get_lightRotationBaseDataList()))) {
                list.push_back(LightRotationBaseData_Convert(saveData2));
            }

            auto beatDistributionParamType = DistributionParamType_Convert(saveData->get_rotationDistributionParamType());
            auto rotationDistributionParamType = DistributionParamType_Convert(saveData->get_rotationDistributionParamType());
            return LightRotationBeatmapEventDataBox::New_ctor(indexFilter, saveData->get_beatDistributionParam(), beatDistributionParamType, ConvertAxis(saveData->get_axis()), saveData->get_flipRotation(), saveData->get_rotationDistributionParam(), rotationDistributionParamType, saveData->get_rotationDistributionShouldAffectFirstBaseEvent(),
                                                              reinterpret_cast<IReadOnlyList_1<::GlobalNamespace::LightRotationBaseData *> *>(list.getInner()));
        });
    }


    BeatmapEventDataBoxGroup* Convert(BeatmapSaveDataVersion3::BeatmapSaveData::EventBoxGroup* eventBoxGroupSaveData)
    {
        auto collection = reinterpret_cast<IReadOnlyCollection_1<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox*>*>(eventBoxGroupSaveData->get_baseEventBoxes());
        VList<BeatmapEventDataBox*> list;


        auto dataForGroup = this->lightGroups->GetDataForGroup(eventBoxGroupSaveData->get_groupId());
        if (dataForGroup == nullptr) {
            return nullptr;
        }

//        auto enumerator = reinterpret_cast<IEnumerable_1<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox*>*>(collection)->GetEnumerator();
//        auto nonGenericEnumerator = reinterpret_cast<Collections::IEnumerator*>(enumerator);
//
//        while(true) {
//            if (!nonGenericEnumerator->MoveNext()) break;
//
//            auto item = enumerator->get_Current();
        for (auto item : VList<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox*>(reinterpret_cast<System::Collections::Generic::List_1<BeatmapSaveDataVersion3::BeatmapSaveData::EventBox*> *>(collection))) {
            auto beatmapEventDataBox = dataConvertor.ProcessItem(item, dataForGroup);
            if (beatmapEventDataBox != nullptr) {
                list.push_back(beatmapEventDataBox);
            }
        }

        list.trim();
        return BeatmapEventDataBoxGroup::New_ctor(eventBoxGroupSaveData->get_beat(),
                                                  reinterpret_cast<IReadOnlyCollection_1<::GlobalNamespace::BeatmapEventDataBox *> *>(list.getInner()));
    }

    CppConverter2Param<BeatmapEventDataBox*, EnvironmentLightGroups::LightGroupData*> dataConvertor;

    EnvironmentLightGroups* lightGroups;
};

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_InsertItem, classof(BeatmapDataSortedListForTypes_1<BeatmapDataItem*>*), "InsertItem", void, BeatmapDataSortedListForTypes_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
    auto list = self->GetList(CustomBeatmapData::GetCustomType(item));

    list->Insert(item);
    // fix method resolution
//
//    auto context = CJDLogger::GetLogger().WithContext("insert log");
//
//    il2cpp_utils::LogMethods(context, reinterpret_cast<Il2CppObject*>(list)->klass, true);
//
//    auto* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod(list, "Insert", std::vector<Il2CppClass*>{}, ::std::vector<const Il2CppType*>{&classof(BeatmapDataItem*)->byval_arg})));
//    ::il2cpp_utils::RunMethodRethrow<::System::Collections::Generic::LinkedListNode_1<BeatmapDataItem>*, false>(list, ___internal__method, item);
}

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_RemoveItem, classof(BeatmapDataSortedListForTypes_1<BeatmapDataItem*>*), "RemoveItem", void, BeatmapDataSortedListForTypes_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
    auto list = self->GetList(CustomBeatmapData::GetCustomType(item));
    auto node = list->NodeForItem(item);
    list->Remove(node);
}

MAKE_PAPER_HOOK_MATCH(GetBeatmapDataFromBeatmapSaveData, &BeatmapDataLoader::GetBeatmapDataFromBeatmapSaveData, BeatmapData *,
                BeatmapSaveDataVersion3::BeatmapSaveData* beatmapSaveData,
                float startBpm, bool loadingForDesignatedEnvironment,
                ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                ::GlobalNamespace::EnvironmentLightGroups* environmentLightGroups,
                ::GlobalNamespace::DefaultEnvironmentEvents* defaultEnvironmentEvents) {

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing save data {} cached {}", fmt::ptr(beatmapSaveData), fmt::ptr(cachedSaveData.operator CustomJSONData::v3::CustomBeatmapSaveData *const()));
    auto startTime = std::chrono::high_resolution_clock::now();

    bool flag = loadingForDesignatedEnvironment || (beatmapSaveData->useNormalEventsAsCompatibleEvents && defaultEnvironmentEvents->get_isEmpty());
    CustomBeatmapData* beatmapData;

    static auto CustomBeatmapSaveDataKlass = classof(v3::CustomBeatmapSaveData*);

    if (beatmapSaveData->klass == CustomBeatmapSaveDataKlass) {
        beatmapData = CustomBeatmapData::New_ctor(4);
        beatmapData->customData = beatmapData->customData;
        beatmapData->levelCustomData = beatmapData->levelCustomData;
        beatmapData->beatmapCustomData = beatmapData->beatmapCustomData;
    } else {
        beatmapData = CustomBeatmapData::New_ctor(4);
    }

    beatmapData->InsertBeatmapEventData(BPMChangeBeatmapEventData::New_ctor(-100.0f, startBpm));
    auto bpmEvents = beatmapSaveData->dyn_bpmEvents();
    // TODO: Fix
    CRASH_UNLESS(beatmapSaveData->basicEventTypesWithKeywords);
    CJDLogger::Logger.fmtLog<LogLevel::INF>("beatmapSaveData->basicEventTypesWithKeywords {} beatmapSaveData->basicEventTypesWithKeywords->d {} beatmapSaveData->basicEventTypesWithKeywords->d->items {}",
                                            fmt::ptr(beatmapSaveData->basicEventTypesWithKeywords),
                                            fmt::ptr(beatmapSaveData->basicEventTypesWithKeywords->d),
                                            fmt::ptr(beatmapSaveData->basicEventTypesWithKeywords->d->items.convert()));
    if (beatmapSaveData->basicEventTypesWithKeywords->d && beatmapSaveData->basicEventTypesWithKeywords->d->get_Count() > 0 && beatmapSaveData->basicEventTypesWithKeywords->d->items) {
        for (auto basicEventTypesForKeyword: VList(beatmapSaveData->basicEventTypesWithKeywords->d).toSpan()) {
            if (!basicEventTypesForKeyword || !basicEventTypesForKeyword->k) continue;

            beatmapData->AddSpecialBasicBeatmapEventKeyword(basicEventTypesForKeyword->k);
        }
    }
    CRASH_UNLESS(beatmapSaveData->dyn_bpmEvents());
    BpmTimeProcessor bpmTimeProcessor(startBpm, bpmEvents);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Special events list {}", fmt::ptr(beatmapSaveData->basicEventTypesWithKeywords->d));

    auto specialEventsFilter = BeatmapDataLoader::SpecialEventsFilter::New_ctor(beatmapSaveData->basicEventTypesWithKeywords, environmentKeywords);

    auto const BeatToTime = [&bpmTimeProcessor](float beat) constexpr {
        return bpmTimeProcessor.ConvertBeatToTime(beat);
    };

    // TODO: Remove converter
    CppConverter<BeatmapObjectData*> objectConverter;
    objectConverter.AddConverter<v3::CustomBeatmapSaveData_ColorNoteData*>([&BeatToTime](v3::CustomBeatmapSaveData_ColorNoteData* data) {
        auto noteData = CreateCustomBasicNoteData(
                BeatToTime(data->b),
                data->get_line(),
                ConvertNoteLineLayer(data->get_layer()),
                ConvertColorType(data->get_color()),
                data->get_cutDirection(),
                data->customData);
        noteData->SetCutDirectionAngleOffset(data->get_angleOffset());
        return noteData;
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_BombNoteData*>([&BeatToTime](v3::CustomBeatmapSaveData_BombNoteData* data) {
        return CreateCustomBombNoteData(
            BeatToTime(data->get_beat()),
            data->get_line(),
            ConvertNoteLineLayer(data->get_layer()),
            data->customData);
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_ObstacleData*>([&BeatToTime](v3::CustomBeatmapSaveData_ObstacleData* data) {
        float beat = BeatToTime(data->get_beat());
        auto obstacle = CustomObstacleData::New_ctor(
                beat,
                data->get_line(),
                GetNoteLineLayer(data->get_layer()),
                BeatToTime(data->get_beat() + data->get_duration()) - beat,
                data->get_width(),
                data->get_height());

        obstacle->customData = ToJsonWrapper(data->customData);

        return obstacle;
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_SliderData*>([&BeatToTime](v3::CustomBeatmapSaveData_SliderData* data) {
        return CreateCustomSliderData(
                ConvertColorType(data->get_colorType()),
                BeatToTime(data->get_beat()),
                data->get_headLine(),
                ConvertNoteLineLayer(data->get_headLayer()),
                ConvertNoteLineLayer(data->get_headLayer()),
                data->get_headControlPointLengthMultiplier(),
                data->get_headCutDirection(),
                BeatToTime(data->get_tailBeat()),
                data->get_headLine(),
                ConvertNoteLineLayer(data->get_tailLayer()),
                ConvertNoteLineLayer(data->get_tailLayer()),
                data->get_tailControlPointLengthMultiplier(),
                data->get_tailCutDirection(),
                data->get_sliderMidAnchorMode(),
                data->customData
                );
    });

    objectConverter.AddConverter<v3::CustomBeatmapSaveData_BurstSliderData*>([&BeatToTime](v3::CustomBeatmapSaveData_BurstSliderData* data) {
        return CreateCustomBurstSliderData(
                ConvertColorType(data->get_colorType()),
                BeatToTime(data->get_beat()),
                data->get_headLine(),
                ConvertNoteLineLayer(data->get_headLayer()),
                ConvertNoteLineLayer(data->get_tailLayer()),
                data->get_headCutDirection(),
                BeatToTime(data->get_tailBeat()),
                data->get_tailLine(),
                ConvertNoteLineLayer(data->get_tailLayer()),
                ConvertNoteLineLayer(data->get_tailLayer()),
                NoteCutDirection::Any,
                data->get_sliceCount(),
                data->get_squishAmount(),
                data->customData
        );
    });

    objectConverter.AddConverter<BeatmapSaveData::WaypointData*>([&BeatToTime](BeatmapSaveData::WaypointData* data) {
        return WaypointData::New_ctor(BeatToTime(data->get_beat()),
                                      data->get_line(),
                                      ConvertNoteLineLayer(data->get_layer()),
                                      data->get_offsetDirection());
    });

    auto dataConvertProcess = [](auto const& converter, auto const& vlist, auto const& addToBeatmap) constexpr {
        std::stable_sort(VList(vlist).begin(), VList(vlist).end(), TimeCompare<typename decltype(VList(vlist))::value_type>);
        for (auto const& o : VList(vlist)) {
            auto* beatmapObjectData = converter.ProcessItem(o);

            if (beatmapObjectData != nullptr) {
                addToBeatmap(beatmapObjectData);
            }
        }
    };

    auto addObj = [&beatmapData](auto o) constexpr {beatmapData->AddBeatmapObjectData(o); };

    dataConvertProcess(objectConverter, beatmapSaveData->colorNotes, addObj);
    dataConvertProcess(objectConverter, beatmapSaveData->bombNotes, addObj);
    dataConvertProcess(objectConverter, beatmapSaveData->obstacles, addObj);
    dataConvertProcess(objectConverter, beatmapSaveData->sliders, addObj);
    dataConvertProcess(objectConverter, beatmapSaveData->burstSliders, addObj);
    dataConvertProcess(objectConverter, beatmapSaveData->waypoints, addObj);

    CppConverter<BeatmapEventData*> eventConverter;
    eventConverter.AddConverter<BeatmapSaveData::BpmChangeEventData*>([&BeatToTime](BeatmapSaveData::BeatmapSaveData::BpmChangeEventData* data) {
        return BPMChangeBeatmapEventData::New_ctor(BeatToTime(data->get_beat()),
                                                   data->get_bpm());
    });

    eventConverter.AddConverter<BeatmapSaveData::RotationEventData*>([&BeatToTime](BeatmapSaveData::BeatmapSaveData::RotationEventData* data) {
        SpawnRotationBeatmapEventData::SpawnRotationEventType executionTime =
                                                                     data->get_executionTime() == BeatmapSaveData::ExecutionTime::Early
                                                                     ? SpawnRotationBeatmapEventData::SpawnRotationEventType::Early
                                                                     : SpawnRotationBeatmapEventData::SpawnRotationEventType::Late;

        return SpawnRotationBeatmapEventData::New_ctor(BeatToTime(data->get_beat()),
                                                        executionTime,
                                                        data->get_rotation());
    });

    if (flag) {
        eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>([&BeatToTime, &specialEventsFilter](v3::CustomBeatmapSaveData_BasicEventData* data) {
            if (!specialEventsFilter->IsEventValid(data->get_eventType()))
            {
                return (CustomBeatmapEventData*) nullptr;
            }

            auto event = CustomBeatmapEventData::New_ctor(
                    BeatToTime(data->get_beat()),
                    (GlobalNamespace::BasicBeatmapEventType) data->get_eventType(),
                    data->get_value(),
                    data->get_floatValue());

            event->customData = ToJsonWrapper(data->customData);

            return event;
        });

        eventConverter.AddConverter<BeatmapSaveData::ColorBoostEventData*>([&BeatToTime](BeatmapSaveData::ColorBoostEventData* data) {
            return ColorBoostBeatmapEventData::New_ctor(
                    BeatToTime(data->get_beat()),
                    data->get_boost());
        });
    } else {
        eventConverter.AddConverter<v3::CustomBeatmapSaveData_BasicEventData*>([&BeatToTime](v3::CustomBeatmapSaveData_BasicEventData* data) {
            auto event = CustomBeatmapEventData::New_ctor(
                    BeatToTime(data->get_beat()),
                    (GlobalNamespace::BasicBeatmapEventType) data->get_eventType(),
                    data->get_value(),
                    data->get_floatValue());

            event->customData = ToJsonWrapper(data->customData);

            return event;
        });

        eventConverter.AddConverter<BeatmapSaveData::ColorBoostEventData*>([&BeatToTime](BeatmapSaveData::ColorBoostEventData* data) {
            return ColorBoostBeatmapEventData::New_ctor(
                    BeatToTime(data->get_beat()),
                    data->get_boost());
        });
    }

    auto addEvent = [&beatmapData](auto o) { beatmapData->InsertBeatmapEventData(o); };
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("bpm events");
    dataConvertProcess(eventConverter, bpmEvents, addEvent);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("basic events");
    dataConvertProcess(eventConverter, beatmapSaveData->basicBeatmapEvents, addEvent);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("color boost events");
    dataConvertProcess(eventConverter, beatmapSaveData->colorBoostBeatmapEvents, addEvent);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("rotation events");
    dataConvertProcess(eventConverter, beatmapSaveData->rotationEvents, addEvent);

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("event groups");
    auto bpmTimeProcessorIl2cpp = BeatmapDataLoader::BpmTimeProcessor::New_ctor(startBpm, bpmEvents);
    auto beatmapEventDataBoxGroupLists = BeatmapEventDataBoxGroupLists::New_ctor(beatmapData, reinterpret_cast<IBeatToTimeConvertor *>(bpmTimeProcessorIl2cpp), false);
    auto eventBoxGroupConvertor = SafePtr(BeatmapDataLoader::EventBoxGroupConvertor::New_ctor(environmentLightGroups));


    EventBoxGroupConvertor cppEventBoxConverter(environmentLightGroups);

    auto handleEventBoxGroup = [&eventBoxGroupConvertor, &beatmapEventDataBoxGroupLists, &environmentLightGroups, &cppEventBoxConverter](auto const& eventGroup) {
        std::stable_sort(VList(eventGroup).begin(), VList(eventGroup).end(), TimeCompare<typename decltype(VList(eventGroup))::value_type>);
        for (auto const& o : VList(eventGroup)) {
            if (!o) continue;

            auto beatmapEventDataBoxGroup = cppEventBoxConverter.Convert(o); // eventBoxGroupConvertor->Convert(o);
            if (beatmapEventDataBoxGroup != nullptr)
            {
                beatmapEventDataBoxGroupLists->Insert(o->get_groupId(), beatmapEventDataBoxGroup);
            }
        }
    };

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightColorEventBoxGroups handling events");
    handleEventBoxGroup(beatmapSaveData->lightColorEventBoxGroups);
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("box group lightRotationEventBoxGroups handling events");
    handleEventBoxGroup(beatmapSaveData->lightRotationEventBoxGroups);



    if (!flag)
    {
        DefaultEnvironmentEventsFactory::InsertDefaultEnvironmentEvents(beatmapData, beatmapEventDataBoxGroupLists, defaultEnvironmentEvents, environmentLightGroups);
    }

    if (auto customBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData))
    {
        for (auto& customEventSaveData : *customBeatmapSaveData.value()->customEventsData) {
            beatmapData->InsertCustomEventData(CustomEventData::New_ctor(bpmTimeProcessor.ConvertBeatToTime(customEventSaveData.time),
                                                                         (void*) &customEventSaveData.type,
                                                                         customEventSaveData.typeHash,
                                                                         (void*) const_cast<rapidjson::Value*>(customEventSaveData.data)));
        }
    }


    beatmapEventDataBoxGroupLists->SyncWithBeatmapData();
    beatmapData->ProcessRemainingData();



    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Finished processing beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("This took {}ms", (int) std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return beatmapData;
}


// CustomJSONData::CustomLevelInfoSaveData*, std::string const&, BeatmapSaveDataVersion3::BeatmapSaveData*, GlobalNamespace::BeatmapDataBasicInfo*
void BeatmapDataLoadedEvent(StandardLevelInfoSaveData *standardLevelInfoSaveData, const std::string &filename, BeatmapSaveDataVersion3::BeatmapSaveData *beatmapData, GlobalNamespace::BeatmapDataBasicInfo*) {
    if (!beatmapData) {
        CJDLogger::Logger.fmtLog<LogLevel::WRN>("Beatmap is null, no custom level data");
        return;
    }

    auto *customSaveData = reinterpret_cast<CustomLevelInfoSaveData *>(standardLevelInfoSaveData);
    auto *customBeatmapData = reinterpret_cast<CustomBeatmapData *>(beatmapData);



    JSONWrapperUTF16 *beatmapCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapperUTF16*>());
    if (customSaveData->customData) {
        beatmapCustomData->value = customSaveData->customData->get();
    }

    customBeatmapData->beatmapCustomData = beatmapCustomData;

    JSONWrapperUTF16 *levelCustomData = CRASH_UNLESS(il2cpp_utils::New<JSONWrapperUTF16*>());
    for (auto* beatmapSet : customSaveData->difficultyBeatmapSets) {

        for (auto* beatmap : beatmapSet->difficultyBeatmaps) {
            if (!beatmap || !beatmap->beatmapFilename)
                continue;

            auto *customBeatmap = reinterpret_cast<CustomDifficultyBeatmap *>(beatmap);

            std::string beatmapFilename = to_utf8(csstrtostr(customBeatmap->beatmapFilename));
            if (beatmapFilename == filename && customBeatmap->customData) {
                levelCustomData->value = customBeatmap->customData->get();
            }
        }
    }
    customBeatmapData->levelCustomData = levelCustomData;
}

void CustomJSONData::InstallHooks() {
    auto logger = CJDLogger::GetLoggerOld().WithContext("InstallHooks");

    // Install hooks
//    INSTALL_HOOK(logger, BeatmapData_AddBeatmapObjectData)
    INSTALL_HOOK_ORIG(logger, BeatmapSaveData_DeserializeFromJSONString)
    INSTALL_HOOK_ORIG(logger, GetBeatmapDataFromBeatmapSaveData)
    INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_InsertItem);
    INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_RemoveItem);
    INSTALL_HOOK_ORIG(logger, BeatmapData_GetFilteredCopy);
    INSTALL_HOOK_ORIG(logger, BeatmapData_GetCopy);

    RuntimeSongLoader::API::AddBeatmapDataBasicInfoLoadedEvent(BeatmapDataLoadedEvent);

    il2cpp_functions::Class_Init(classof(BeatmapData*));

    custom_types::logAll(classof(BeatmapData*));
    custom_types::Register::AutoRegister();
}
