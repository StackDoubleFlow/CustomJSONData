#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "songloader/shared/API.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"

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

#include "System/Comparison_1.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
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

v3::CustomBeatmapSaveData* cachedSaveData;

// This is to prevent issues with string limits
std::string to_utf8(std::u16string_view view) {
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(view.data());
}

#define FindMethodGetter(qualifiedTypeName, methodName) \
::il2cpp_utils::il2cpp_type_check::MetadataGetter<#methodName, qualifiedTypeName, decltype(&qualifiedTypeName::methodName)>::get();

static std::string_view GetVersionFromPath(std::string_view path)
{
    // SongCore has a fallback so i guess i do too
    static const std::string_view fallback = "2.0.0";

    auto versionIt = path.find("_version");
    auto size = 9;

    if (versionIt == std::string::npos) {
        versionIt = path.find("version");
        size = 8;
    }

    if (versionIt != std::string::npos) {
        return path.substr(versionIt, size);
    }

    return fallback;
}

// This hook loads the json data (with custom data) into a BeatmapSaveData 
MAKE_HOOK_MATCH(BeatmapSaveData_DeserializeFromJSONString, &BeatmapSaveDataVersion3::BeatmapSaveData::DeserializeFromJSONString, BeatmapSaveData*, StringW stringData) {
    CJDLogger::GetLogger().debug("Parsing json");
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!stringData) {
        CJDLogger::GetLogger().error("No string data");
        return nullptr;
    }

    try {
        auto sharedDoc = std::make_shared<rapidjson::Document>();
        rapidjson::Document &doc = *sharedDoc;
        std::string contents(stringData);
        rapidjson::ParseResult result = doc.Parse(contents);

        if (!result || doc.IsNull() || doc.HasParseError()) {
            std::string errorCodeStr(rapidjson::GetParseError_En(result.Code()));
            CJDLogger::GetLogger().debug("Unable to parse json due to %s", errorCodeStr.c_str());
            return nullptr;
        }
        CJDLogger::GetLogger().debug("Parsing json success");

        auto version = GetVersionFromPath(contents);

        v3::CustomBeatmapSaveData* saveData;

        if (semver::lte(std::string(version), "2.6.0")) {
             saveData = v3::CustomBeatmapSaveData::Convert2_6_0(v2::CustomBeatmapSaveData::Deserialize(sharedDoc));
        } else {
            saveData = v3::CustomBeatmapSaveData::Deserialize(sharedDoc);
        }

        cachedSaveData = saveData;

        CJDLogger::GetLogger().debug("Finished reading beatmap data");
        auto stopTime = std::chrono::high_resolution_clock::now();
        CJDLogger::GetLogger().debug("This took %ims", (int) std::chrono::duration_cast<std::chrono::milliseconds>(
                stopTime - startTime).count());

        return saveData;
    } catch (std::exception const& e) {
        CJDLogger::GetLogger().debug("There was an error loading the beatmap through CJD. Cause of error: %s", e.what());
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
                                      noteLineLayer,
                                      noteLineLayer,
                                      NoteData::GameplayType::Normal,
                                      NoteData::ScoringType::Normal,
                                      colorType,
                                      cutDirection,
                                      0,
                                      0,
                                      lineIndex,
                                      0,
                                      0,
                                      1);

    b->customData = JSONWrapper::New_ctor();
    b->customData->value = customData;

    return b;
}

CustomNoteData* CreateCustomBombNoteData(float time, int lineIndex, NoteLineLayer noteLineLayer, v3::CustomDataOpt const& customData) {
    auto b = CustomNoteData::New_ctor(time,
                                      lineIndex,
                                      noteLineLayer,
                                      noteLineLayer,
                                      NoteData::GameplayType::Bomb,
                                      NoteData::ScoringType::NoScore,
                                      ColorType::None,
                                      NoteCutDirection::None,
                                      0,
                                      0,
                                      lineIndex,
                                      0,
                                      0,
                                      1);

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
                                      NoteData::GameplayType::BurstSliderElement,
                                      NoteData::ScoringType::BurstSliderElement,
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
            SliderData::Type::Burst,
            colorType,
            false,
            headTime,
            headLineIndex,
            headLineLayer,
            headBeforeJumpLineLayer,
            0,
            headCutDirection,
            0,
            false,
            tailTime,
            tailLineIndex,
            tailLineLayer,
            tailBeforeJumpLineLayer,
            0,
            tailCutDirection,
            0,
            SliderMidAnchorMode::Straight,
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
            SliderData::Type::Normal,
            colorType,
            false,
            headTime,
            headLineIndex,
            headLineLayer,
            headBeforeJumpLineLayer,
            headControlPointLengthMultiplier,
            headCutDirection,
            0,
            false,
            tailTime,
            tailLineIndex,
            tailLineLayer,
            tailBeforeJumpLineLayer,
            tailControlPointLengthMultiplier,
            tailCutDirection,
            0,
            midAnchorMode,
            0,
            1);
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

    template <typename U, typename F>
    requires(std::is_pointer_v<U>)
    void AddConverter(F&& o) {
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

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_InsertItem, classof(BeatmapDataSortedListForTypes_1<BeatmapDataItem*>*), "InsertItem", void, BeatmapDataSortedListForTypes_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
    self->GetList(CustomBeatmapData::GetCustomType(item))->Insert(item);
}

MAKE_HOOK_FIND_INSTANCE(CustomBeatmapDataSortedListForTypes_RemoveItem, classof(BeatmapDataSortedListForTypes_1<BeatmapDataItem*>*), "RemoveItem", void, BeatmapDataSortedListForTypes_1<BeatmapDataItem*>* self, BeatmapDataItem* item) {
    auto list = self->GetList(CustomBeatmapData::GetCustomType(item));
    auto node = list->NodeForItem(item);
    list->Remove(node);
}

MAKE_HOOK_MATCH(GetBeatmapDataFromBeatmapSaveData, &BeatmapDataLoader::GetBeatmapDataFromBeatmapSaveData, BeatmapData *,
                BeatmapSaveDataVersion3::BeatmapSaveData* beatmapSaveData,
                float startBpm, bool loadingForDesignatedEnvironment,
                ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                ::GlobalNamespace::EnvironmentLightGroups* environmentLightGroups,
                ::GlobalNamespace::DefaultEnvironmentEvents* defaultEnvironmentEvents) {

    CJDLogger::GetLogger().debug("Parsing save data");
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
    auto bpmEvents = VList(beatmapSaveData->bpmEvents);
    for (auto basicEventTypesForKeyword : VList(beatmapSaveData->basicEventTypesWithKeywords->get_data()))
    {
        beatmapData->AddSpecialBasicBeatmapEventKeyword(basicEventTypesForKeyword->k);
    }
    auto bpmTimeProcessor = BeatmapDataLoader::BpmTimeProcessor::New_ctor(startBpm, bpmEvents);
    auto specialEventsFilter = BeatmapDataLoader::SpecialEventsFilter::New_ctor(beatmapSaveData->basicEventTypesWithKeywords, environmentKeywords);

    auto const BeatToTime = [&bpmTimeProcessor](float beat) constexpr {
        return bpmTimeProcessor->ConvertBeatToTime(beat);
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
        std::sort(VList(vlist).begin(), VList(vlist).end(), TimeCompare<typename decltype(VList(vlist))::value_type>);
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
            return BeatmapSaveData::ColorBoostEventData::New_ctor(
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
            return BeatmapSaveData::ColorBoostEventData::New_ctor(
                    BeatToTime(data->get_beat()),
                    data->get_boost());
        });
    }

    auto addEvent = [&beatmapData](auto o) { beatmapData->InsertBeatmapEventData(o); };
    dataConvertProcess(eventConverter, bpmEvents, addEvent);
    dataConvertProcess(eventConverter, beatmapSaveData->basicBeatmapEvents, addEvent);
    dataConvertProcess(eventConverter, beatmapSaveData->colorBoostBeatmapEvents, addEvent);
    dataConvertProcess(eventConverter, beatmapSaveData->rotationEvents, addEvent);


    auto beatmapEventDataBoxGroupLists = BeatmapEventDataBoxGroupLists::New_ctor(beatmapData, reinterpret_cast<IBeatToTimeConvertor *>(bpmTimeProcessor), false);
    auto eventBoxGroupConvertor = BeatmapDataLoader::EventBoxGroupConvertor::New_ctor(environmentLightGroups);


    auto handleEventBoxGroup = [&eventBoxGroupConvertor, &beatmapEventDataBoxGroupLists](auto const& eventGroup) {
        std::sort(VList(eventGroup).begin(), VList(eventGroup).end(), TimeCompare<typename decltype(VList(eventGroup))::value_type>);
        for (auto const& o : VList(eventGroup)) {
            auto beatmapEventDataBoxGroup = eventBoxGroupConvertor->Convert(o);
            if (beatmapEventDataBoxGroup != nullptr)
            {
                beatmapEventDataBoxGroupLists->Insert(o->get_groupId(), beatmapEventDataBoxGroup);
            }
        }
    };

    handleEventBoxGroup(beatmapSaveData->lightColorEventBoxGroups);
    handleEventBoxGroup(beatmapSaveData->lightRotationEventBoxGroups);



    if (!flag)
    {
        DefaultEnvironmentEventsFactory::InsertDefaultEnvironmentEvents(beatmapData, beatmapEventDataBoxGroupLists, defaultEnvironmentEvents, environmentLightGroups);
    }

    if (auto customBeatmapSaveData = il2cpp_utils::try_cast<v3::CustomBeatmapSaveData>(beatmapSaveData))
    {
        for (auto& customEventSaveData : *customBeatmapSaveData.value()->customEventsData) {
            beatmapData->InsertCustomEventData(CustomEventData::New_ctor(bpmTimeProcessor->ConvertBeatToTime(customEventSaveData.time),
                                                                         (void*) &customEventSaveData.type,
                                                                         customEventSaveData.typeHash,
                                                                         (void*) const_cast<rapidjson::Value*>(customEventSaveData.data)));
        }
    }


    beatmapEventDataBoxGroupLists->SyncWithBeatmapData();
    beatmapData->ProcessRemainingData();



    CJDLogger::GetLogger().debug("Finished processing beatmap data");
    auto stopTime = std::chrono::high_resolution_clock::now();
    CJDLogger::GetLogger().debug("This took %ims", (int) std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return beatmapData;
}


// CustomJSONData::CustomLevelInfoSaveData*, std::string const&, BeatmapSaveDataVersion3::BeatmapSaveData*, GlobalNamespace::BeatmapDataBasicInfo*
void BeatmapDataLoadedEvent(StandardLevelInfoSaveData *standardLevelInfoSaveData, const std::string &filename, BeatmapSaveDataVersion3::BeatmapSaveData *beatmapData, GlobalNamespace::BeatmapDataBasicInfo*) {
    if (!beatmapData) {
        CJDLogger::GetLogger().warning("Beatmap is null, no custom level data");
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
    auto logger = CJDLogger::GetLogger().WithContext("InstallHooks");

    // Install hooks
//    INSTALL_HOOK(logger, BeatmapData_AddBeatmapObjectData)
    INSTALL_HOOK_ORIG(logger, BeatmapSaveData_DeserializeFromJSONString)
    INSTALL_HOOK_ORIG(logger, GetBeatmapDataFromBeatmapSaveData)
    INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_InsertItem);
    INSTALL_HOOK_ORIG(logger, CustomBeatmapDataSortedListForTypes_RemoveItem);

    RuntimeSongLoader::API::AddBeatmapDataBasicInfoLoadedEvent(BeatmapDataLoadedEvent);

    custom_types::Register::AutoRegister();
}
