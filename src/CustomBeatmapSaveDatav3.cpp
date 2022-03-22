#include "CustomBeatmapSaveDatav3.h"
#include "CustomBeatmapSaveDatav2.h"

#include "VList.h"

#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"
#include "GlobalNamespace/BeatmapDataSortedListForTypes_1.hpp"

#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveData_SpecialEventsForKeyword.hpp"

#include "cpp-semver/shared/cpp-semver.hpp"
#include "GlobalNamespace/NoteLineLayer.hpp"

#include <numeric>

using namespace GlobalNamespace;
using namespace BeatmapSaveDataVersion3;
using namespace CustomJSONData::v3;

DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_ColorNoteData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_BombNoteData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_ObstacleData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_BurstSliderData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_SliderData);
DEFINE_TYPE(CustomJSONData::v3, CustomBeatmapSaveData_BasicEventData);



void CustomBeatmapSaveData::ctor(
        System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BpmChangeEventData *> *bpmEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::RotationEventData *> *rotationEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorNoteData *> *colorNotes,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BombNoteData *> *bombNotes,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ObstacleData *> *obstacles,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::SliderData *> *sliders,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BurstSliderData *> *burstSliders,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::WaypointData *> *waypoints,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventData *> *basicBeatmapEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::ColorBoostEventData *> *colorBoostBeatmapEvents,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightColorEventBoxGroup *> *lightColorEventBoxGroups,
        ::System::Collections::Generic::List_1<::BeatmapSaveDataVersion3::BeatmapSaveData::LightRotationEventBoxGroup *> *lightRotationEventBoxGroups,
        ::BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords *basicEventTypesWithKeywords,
        bool useNormalEventsAsCompatibleEvents) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData*), ".ctor", 14);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, bpmEvents,
                                         rotationEvents,
                                         colorNotes,
                                         bombNotes,
                                         obstacles,
                                         sliders,
                                         burstSliders,
                                         waypoints,
                                         basicBeatmapEvents,
                                         colorBoostBeatmapEvents,
                                         lightColorEventBoxGroups,
                                         lightRotationEventBoxGroups,
                                         basicEventTypesWithKeywords,
                                         useNormalEventsAsCompatibleEvents));
    INVOKE_CTOR();
}

void CustomBeatmapSaveData_ColorNoteData::ctor(float beat, int line, int layer,
                                               ::BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType color,
                                               ::GlobalNamespace::NoteCutDirection cutDirection, int angleOffset) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::ColorNoteData*), ".ctor", 6);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, beat, line, layer, color, cutDirection, angleOffset));
    INVOKE_CTOR();
}

void CustomBeatmapSaveData_BombNoteData::ctor(float beat, int line, int layer) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::BombNoteData*), ".ctor", 3);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, beat, line, layer));
    INVOKE_CTOR();
}

void CustomBeatmapSaveData_SliderData::ctor(BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType,
                                            float headBeat, int headLine, int headLayer,
                                            float headControlPointLengthMultiplier,
                                            ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat,
                                            int tailLine, int tailLayer, float tailControlPointLengthMultiplier,
                                            ::GlobalNamespace::NoteCutDirection tailCutDirection,
                                            ::GlobalNamespace::SliderMidAnchorMode sliderMidAnchorMode) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::SliderData*), ".ctor", 12);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, colorType, headBeat, headLine, headLayer, headControlPointLengthMultiplier, headCutDirection, tailBeat, tailLine, tailLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode));
    INVOKE_CTOR();
}

void CustomBeatmapSaveData_BurstSliderData::ctor(BeatmapSaveDataVersion3::BeatmapSaveData::NoteColorType colorType,
                                                 float headBeat, int headLine, int headLayer,
                                                 ::GlobalNamespace::NoteCutDirection headCutDirection, float tailBeat,
                                                 int tailLine, int tailLayer, int sliceCount, float squishAmount) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::BurstSliderData*), ".ctor", 10);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, colorType, headBeat, headLine, headLayer, headCutDirection, tailBeat, tailLine, tailLayer, sliceCount, squishAmount));
    INVOKE_CTOR();
}

void CustomBeatmapSaveData_ObstacleData::ctor(float beat, int line, int layer, float duration, int width, int height) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::ObstacleData*), ".ctor", 6);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, beat, line, layer, duration, width, height));
    INVOKE_CTOR();
}




void CustomJSONData::v3::CustomBeatmapSaveData_BasicEventData::ctor(float time, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType type, int value, float floatValue) {
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::BasicEventData*), ".ctor", 4);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, time, type, value, floatValue));
    INVOKE_CTOR();
}

inline decltype(CustomJSONData::v3::CustomBeatmapSaveData::customData) GetCustomData(rapidjson::Value const & doc) {
    auto customDataIt = doc.FindMember("_customData");
    if (customDataIt != doc.MemberEnd() && customDataIt->value.IsObject()) {
        return customDataIt->value;
    }

    return std::nullopt;
}

#define IT_HASH auto nameHash = std::hash<std::string_view>()(it.name.GetString());

#define IF_CHECK_HASH_FROM_CONSTANTS(str) \
        static const auto str##_Hash = std::hash<std::string_view>()(Constants::str); \
        if (nameHash == str##_Hash)

#define IF_CHECK_HASH(str) \
        static const auto str##_Hash = std::hash<std::string_view>()(#str); \
        if (nameHash == str##_Hash)

static inline bool ParseBool(rapidjson::Value const &val) {
    if (val.IsNumber()) {
        return val.GetInt() == 1;
    }
    if (val.IsString()) {
        return std::string_view(val.GetString()) == "true";
    }
    if (!val.IsBool()) {
        return false;
    }

    return val.GetBool();
}

static auto DeserializeBpmChangeEventData(rapidjson::Value const &val) {
    float beat;
    float bpm;


    for (auto const& it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH(m) {
            bpm = it.value.GetFloat();
        }
    }


    return BeatmapSaveData::BpmChangeEventData::New_ctor(beat, bpm);
}

static auto DeserializeRotation(rapidjson::Value const &val)
{
    float beat;
    BeatmapSaveData::ExecutionTime executionTime;
    float rotation;

    for (auto const& it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH(e) {
            executionTime = it.value.GetInt();
        }

        IF_CHECK_HASH(r) {
            rotation = it.value.GetFloat();
        }
    }

    return BeatmapSaveData::RotationEventData::New_ctor(beat, executionTime, rotation);
}

static auto DeserializeColorNote(rapidjson::Value const &val) {

    float beat;
    int line;
    int layer;
    BeatmapSaveData::NoteColorType color;
    NoteCutDirection cutDirection;
    int angleOffset;
    CustomDataOpt customData;

    for (auto const& it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(line) {
            line = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(layer) {
            layer = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(colorType) {
            color = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(cutDirection) {
            cutDirection = it.value.GetInt();
        }

        IF_CHECK_HASH(a) {
            angleOffset = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(customData) {
            customData = it.value;
        }
    }

    auto note = CustomBeatmapSaveData_ColorNoteData::New_ctor(beat, line, layer, color, cutDirection, angleOffset);
    note->customData = customData;

    return note;
}

static auto DeserializeBombNote(rapidjson::Value const &val) {
    float beat;
    int line;
    int layer;
    CustomDataOpt data;

    for (auto const& it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(line) {
            line = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(layer) {
            layer = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(customData) {
            data = it.value;
        }
    }

    auto bomb = CustomBeatmapSaveData_BombNoteData::New_ctor(beat, line, layer);
    bomb->customData = data;

    return bomb;
}

static auto DeserializeObstacle(rapidjson::Value const &val) {
    float beat;
    int line;
    int layer;
    float duration;
    int width;
    int height;
    CustomDataOpt data;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(line) {
            line = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(layer) {
            layer = it.value.GetInt();
        }

        IF_CHECK_HASH(d) {
            duration = it.value.GetFloat();
        }

        IF_CHECK_HASH(w) {
            width = it.value.GetInt();
        }

        IF_CHECK_HASH(h) {
            height = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(customData) {
            data = it.value;
        }
    }

    auto obstacle = CustomBeatmapSaveData_ObstacleData::New_ctor(beat, line, layer, duration, width, height);
    obstacle->customData = data;
    return obstacle;
}

static auto DeserializeSlider(rapidjson::Value const &val) {
    BeatmapSaveData::NoteColorType color;
    float headBeat;
    int headLine;
    int headLayer;
    float headControlPointLengthMultiplier;
    NoteCutDirection headCutDirection;
    float tailBeat;
    int tailLine;
    int tailLayer;
    float tailControlPointLengthMultiplier;
    NoteCutDirection tailCutDirection;
    SliderMidAnchorMode sliderMidAnchorMode;
    CustomDataOpt data;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(colorType) {
            color = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            headBeat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(line) {
            headLine = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(layer) {
            headLayer = it.value.GetInt();
        }

        IF_CHECK_HASH(mu) {
          headControlPointLengthMultiplier = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(cutDirection) {
            headCutDirection = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(tailBeat) {
            tailBeat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(tailLine) {
            tailLine = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(tailLayer) {
            tailLayer = it.value.GetInt();
        }

        IF_CHECK_HASH(tmu) {
            tailControlPointLengthMultiplier = it.value.GetFloat();
        }

        IF_CHECK_HASH(tc) {
            tailCutDirection = it.value.GetInt();
        }

        IF_CHECK_HASH(m) {
            sliderMidAnchorMode = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(customData) {
            data = it.value;
        }
    }

    auto slider = CustomBeatmapSaveData_SliderData::New_ctor(
            color,
            headBeat,
            headLine,
            headLayer,
            headControlPointLengthMultiplier,
            headCutDirection,
            tailBeat,
            tailLine,
            tailLayer,
            tailControlPointLengthMultiplier,
            tailCutDirection,
            sliderMidAnchorMode);

    slider->customData = data;

    return slider;
}


static auto DeserializeBurstSlider(rapidjson::Value const &val) {
    BeatmapSaveData::NoteColorType color;
    float headBeat;
    int headLine;
    int headLayer;
    NoteCutDirection headCutDirection;
    float tailBeat;
    int tailLine;
    int tailLayer;
    int sliceCount;
    float squishAmount;
    CustomDataOpt data;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(colorType) {
            color = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            headBeat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(line) {
            headLine = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(layer) {
            headLayer = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(cutDirection) {
            headCutDirection = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(tailBeat) {
            tailBeat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(tailLine) {
            tailLine = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(tailLayer) {
            tailLayer = it.value.GetInt();
        }

        IF_CHECK_HASH(sc) {
            sliceCount = it.value.GetInt();
        }

        IF_CHECK_HASH(s) {
            squishAmount = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(customData) {
            data = it.value;
        }
    }

    auto slider = CustomBeatmapSaveData_BurstSliderData::New_ctor(
            color,
            headBeat,
            headLine,
            headLayer,
            headCutDirection,
            tailBeat,
            tailLine,
            tailLayer,
            sliceCount,
            squishAmount);

    slider->customData = data;

    return slider;
}

static auto DeserializeWaypoint(rapidjson::Value const &val) {
    float beat;
    int line;
    int layer;
    OffsetDirection offsetDirection;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(line) {
            line = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(layer) {
            layer = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(cutDirection) {
            offsetDirection = it.value.GetInt();
        }
    }

    return BeatmapSaveData::WaypointData::New_ctor(beat, line, layer, offsetDirection);
}

static auto DeserializeBasicEvent(rapidjson::Value const &val) {
    float beat;
    BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType eventType;
    int value;
    float floatValue;
    CustomDataOpt data;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH(et) {
            eventType = it.value.GetInt();
        }

        IF_CHECK_HASH(i) {
            value = it.value.GetInt();
        }

        IF_CHECK_HASH(f) {
            floatValue = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(customData) {
            data = it.value;
        }
    }

    auto event = CustomBeatmapSaveData_BasicEventData::New_ctor(beat, eventType, value, floatValue);
    event->customData = data;
    return event;
}

static auto DeserializeColorBoostEventData(rapidjson::Value const &val) {
    float beat;
    bool boost;
    CustomDataOpt data;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH(o) {
            boost = ParseBool(it.value);
        }

        IF_CHECK_HASH_FROM_CONSTANTS(customData) {
            data = it.value;
        }
    }

    auto event = BeatmapSaveData::ColorBoostEventData::New_ctor(beat, boost);
//    event->customData = data;
    return event;
}

static auto DeserializeIndexFilter(rapidjson::Value const &val) {
    BeatmapSaveData::IndexFilter::IndexFilterType type;
    int param0;
    int param1;
    bool reversed;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH(f) {
            type = it.value.GetInt();
        }

        IF_CHECK_HASH(p) {
            param0 = it.value.GetInt();
        }

        IF_CHECK_HASH(t) {
            param1 = it.value.GetInt();
        }

        IF_CHECK_HASH(r) {
            reversed = ParseBool(it.value);
        }
    }

    return BeatmapSaveData::IndexFilter::New_ctor(type, param0, param1, reversed);
}

static auto DeserializeLightColorEventBoxGroup(rapidjson::Value const &val) {
    float beat;
    VList<BeatmapSaveData::LightColorEventBox*> eventBoxes;
    int groupId;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(groupId) {
            groupId = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(eventBoxes) {
            // idc if it shadows
            for (auto const& arrIt : it.value.GetArray()) {

                /* nullable */ BeatmapSaveData::IndexFilter* indexFilter;
                float beatDistributionParam;
                BeatmapSaveData::EventBox::DistributionParamType beatDistributionParamType;
                float brightnessDistributionParam;
                bool brightnessDistributionShouldAffectFirstBaseEvent;
                BeatmapSaveData::EventBox::DistributionParamType brightnessDistributionParamType;
                VList<BeatmapSaveData::LightColorBaseData*> lightColorBaseDataList;

                for (auto const& it : arrIt.GetObject()) {
                    IT_HASH

                    IF_CHECK_HASH_FROM_CONSTANTS(indexFilter) {
                        indexFilter = DeserializeIndexFilter(it.value);
                    }

                    IF_CHECK_HASH_FROM_CONSTANTS(beatDistributionParam) {
                        beatDistributionParam = it.value.GetFloat();
                    }

                    IF_CHECK_HASH_FROM_CONSTANTS(beatDistributionParamType) {
                        beatDistributionParamType = it.value.GetInt();
                    }

                    IF_CHECK_HASH(r) {
                        brightnessDistributionParam = it.value.GetFloat();
                    }

                    IF_CHECK_HASH(b) {
                        brightnessDistributionShouldAffectFirstBaseEvent = ParseBool(it.value);
                    }

                    IF_CHECK_HASH(t) {
                        brightnessDistributionParamType = it.value.GetInt();
                    }

                    IF_CHECK_HASH(e) {
                        for (auto const& arrIt : it.value.GetArray()) {
                            float lightBeat;
                            BeatmapSaveData::TransitionType transitionType;
                            BeatmapSaveData::EnvironmentColorType colorType;
                            float brightness;
                            int strobeFrequency;

                            for (auto const& it : arrIt.GetObject()) {
                                IT_HASH

                                IF_CHECK_HASH_FROM_CONSTANTS(beat) {
                                    lightBeat = it.value.GetFloat();
                                }

                                IF_CHECK_HASH(i) {
                                    transitionType = it.value.GetInt();
                                }

                                IF_CHECK_HASH_FROM_CONSTANTS(colorType) {
                                    colorType = it.value.GetInt();
                                }

                                IF_CHECK_HASH(s) {
                                    brightness = it.value.GetFloat();
                                }

                                IF_CHECK_HASH(f) {
                                    strobeFrequency = it.value.GetInt();
                                }
                            }

                            lightColorBaseDataList.push_back(BeatmapSaveData::LightColorBaseData::New_ctor(lightBeat,
                                                                                          transitionType,
                                                                                          colorType,
                                                                                          brightness,
                                                                                          strobeFrequency));
                        }
                    }
                }

                eventBoxes.push_back(BeatmapSaveData::LightColorEventBox::New_ctor(
                        indexFilter,
                        beatDistributionParam,
                        beatDistributionParamType,
                        brightnessDistributionParam,
                        brightnessDistributionShouldAffectFirstBaseEvent,
                        brightnessDistributionParamType,
                        lightColorBaseDataList));
            }
        }
    }

    return BeatmapSaveData::LightColorEventBoxGroup::New_ctor(beat, groupId, eventBoxes);
}

static auto DeserializeLightRotationEventBoxGroup(rapidjson::Value const &val) {
    float beat;
    VList<BeatmapSaveData::LightRotationEventBox*> eventBoxes;
    int groupId;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            beat = it.value.GetFloat();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(groupId) {
            groupId = it.value.GetInt();
        }

        IF_CHECK_HASH_FROM_CONSTANTS(eventBoxes) {
            // idc if it shadows
            for (auto const& arrIt : it.value.GetArray()) {

                /* nullable */ BeatmapSaveData::IndexFilter* indexFilter = nullptr;
                float beatDistributionParam;
                BeatmapSaveData::EventBox::DistributionParamType beatDistributionParamType;
                float rotationDistributionParam;
                BeatmapSaveData::EventBox::DistributionParamType rotationDistributionParamType;
                bool rotationDistributionShouldAffectFirstBaseEvent;
                BeatmapSaveData::Axis axis;
                bool flipRotation;
                VList<BeatmapSaveData::LightRotationBaseData*> lightRotationBaseDataList;

                for (auto const& it : arrIt.GetObject()) {
                    IT_HASH

                    IF_CHECK_HASH_FROM_CONSTANTS(indexFilter) {
                        indexFilter = DeserializeIndexFilter(it.value);
                    }

                    IF_CHECK_HASH_FROM_CONSTANTS(beatDistributionParam) {
                        beatDistributionParam = it.value.GetFloat();
                    }

                    IF_CHECK_HASH_FROM_CONSTANTS(beatDistributionParamType) {
                        beatDistributionParamType = it.value.GetInt();
                    }

                    IF_CHECK_HASH(s) {
                        rotationDistributionParam = it.value.GetFloat();
                    }

                    IF_CHECK_HASH(t) {
                        rotationDistributionParamType = it.value.GetInt();
                    }

                    IF_CHECK_HASH(b) {
                        rotationDistributionShouldAffectFirstBaseEvent = ParseBool(it.value);
                    }

                    IF_CHECK_HASH(a) {
                        axis = it.value.GetInt();
                    }

                    IF_CHECK_HASH(r) {
                        flipRotation = ParseBool(it.value);
                    }

                    IF_CHECK_HASH(l) {
                        for (auto const& arrIt : it.value.GetArray()) {
                            float lightBeat;
                            bool usePreviousEventRotationValue;
                            BeatmapSaveData::EaseType easeType;
                            int loopsCount;
                            float rotation;
                            BeatmapSaveData::LightRotationBaseData::RotationDirection rotationDirection;

                            for (auto const& it : arrIt.GetObject()) {
                                IT_HASH

                                IF_CHECK_HASH_FROM_CONSTANTS(beat) {
                                    lightBeat = it.value.GetFloat();
                                }

                                IF_CHECK_HASH(p) {
                                    usePreviousEventRotationValue = ParseBool(it.value);
                                }

                                IF_CHECK_HASH(e) {
                                    easeType = it.value.GetInt();
                                }

                                IF_CHECK_HASH(l) {
                                    loopsCount = it.value.GetInt();
                                }

                                IF_CHECK_HASH(r) {
                                    rotation = it.value.GetFloat();
                                }

                                IF_CHECK_HASH(o) {
                                    rotationDirection = it.value.GetInt();
                                }
                            }

                            lightRotationBaseDataList.push_back(BeatmapSaveData::LightRotationBaseData::New_ctor(
                                    lightBeat,
                                    usePreviousEventRotationValue,
                                    easeType,
                                    loopsCount,
                                    rotation,
                                    rotationDirection));
                        }
                    }
                }

                eventBoxes.push_back(BeatmapSaveData::LightRotationEventBox::New_ctor(
                        (BeatmapSaveData::IndexFilter*) indexFilter,
                        beatDistributionParam,
                        beatDistributionParamType,
                        rotationDistributionParam,
                        rotationDistributionParamType,
                        rotationDistributionShouldAffectFirstBaseEvent,
                        axis,
                        flipRotation,
                        lightRotationBaseDataList));
            }
        }
    }

    return BeatmapSaveData::LightRotationEventBoxGroup::New_ctor(beat, groupId, eventBoxes);
}

static auto DeserializeBasicEventTypesForKeyword(rapidjson::Value const &val) {
    std::string_view keyword;
    VList<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType> eventTypes;

    for (auto const & it : val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH(k) {
            keyword = it.value.GetString();
        }

        IF_CHECK_HASH(e) {
            auto const& arr = it.value.GetArray();
            eventTypes.resize(arr.Size());
            for (auto const& arrIt : arr) {
                if (!it.value.GetInt()) continue;

                eventTypes.push_back(it.value.GetInt());
            }
        }
    }

    return BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword::New_ctor(keyword, eventTypes);
}

static auto DeserializeCustomEvent(rapidjson::Value const &val) {
    float beat;
    std::string_view type;
    rapidjson::Value const *data;

    for (auto const &it: val.GetObject()) {
        IT_HASH

        IF_CHECK_HASH_FROM_CONSTANTS(beat) {
            if (it.value.IsString()) {
                beat = std::stof(it.value.GetString());
            } else {
                beat = it.value.GetFloat();
            }
        }

        IF_CHECK_HASH(t) {
            type = it.value.GetString();
        }

        IF_CHECK_HASH(d) {
            data = &it.value;
        }
    }

    return CustomJSONData::CustomEventSaveData(type, beat, data);
}

CustomJSONData::v3::CustomBeatmapSaveData *
CustomJSONData::v3::CustomBeatmapSaveData::Deserialize(std::shared_ptr<rapidjson::Document> sharedDoc) {
    auto const& doc = *sharedDoc;

    CJDLogger::GetLogger().debug("Parse notes");

    VList<BeatmapSaveData::BpmChangeEventData*> bpmEvents;
    VList<BeatmapSaveData::RotationEventData*> rotationEvents;
    VList<BeatmapSaveData::ColorNoteData*> colorNotes;
    VList<BeatmapSaveData::BombNoteData*> bombNotes;
    VList<BeatmapSaveData::ObstacleData*> obstacles;
    VList<BeatmapSaveData::SliderData*> sliders;
    VList<BeatmapSaveData::BurstSliderData*> burstSliders;
    VList<BeatmapSaveData::WaypointData*> waypoints;
    VList<BeatmapSaveData::BasicEventData*> basicBeatmapEvents;
    VList<BeatmapSaveData::ColorBoostEventData*> colorBoostBeatmapEvents;
    VList<BeatmapSaveData::LightColorEventBoxGroup*> lightColorEventBoxGroups;
    VList<BeatmapSaveData::LightRotationEventBoxGroup*> lightRotationEventBoxGroups;
    VList<BasicEventTypesWithKeywords::BasicEventTypesForKeyword*> basicEventTypesForKeyword;
    bool useNormalEventsAsCompatibleEvents;

    for (auto const& it : doc.GetObject()) {
        IT_HASH

        IF_CHECK_HASH(bpmEvents) {
            for (auto const &o: it.value.GetArray()) {
                bpmEvents.push_back(DeserializeBpmChangeEventData(o));
            }
        }

        IF_CHECK_HASH(rotationEvents) {
            for (auto const &o: it.value.GetArray()) {
                rotationEvents.push_back(DeserializeRotation(o));
            }
        }
        IF_CHECK_HASH(colorNotes) {
            for (auto const &o: it.value.GetArray()) {
                colorNotes.push_back(DeserializeColorNote(o));
            }
        }
        IF_CHECK_HASH(bombNotes) {
            for (auto const &o: it.value.GetArray()) {
                bombNotes.push_back(DeserializeBombNote(o));
            }
        }
        IF_CHECK_HASH(obstacles) {
            for (auto const &o: it.value.GetArray()) {
                obstacles.push_back(DeserializeObstacle(o));
            }
        }
        IF_CHECK_HASH(sliders) {
            for (auto const &o: it.value.GetArray()) {
                sliders.push_back(DeserializeSlider(o));
            }
        }
        IF_CHECK_HASH(burstSliders) {
            for (auto const &o: it.value.GetArray()) {
                burstSliders.push_back(DeserializeBurstSlider(o));
            }
        }
        IF_CHECK_HASH(waypoints) {
            for (auto const &o: it.value.GetArray()) {
                waypoints.push_back(DeserializeWaypoint(o));
            }
        }
        IF_CHECK_HASH(basicBeatmapEvents) {
            for (auto const &o: it.value.GetArray()) {
                basicBeatmapEvents.push_back(DeserializeBasicEvent(o));
            }
        }
        IF_CHECK_HASH(colorBoostBeatmapEvents) {
            for (auto const &o: it.value.GetArray()) {
                colorBoostBeatmapEvents.push_back(DeserializeColorBoostEventData(o));
            }
        }
        IF_CHECK_HASH(lightColorEventBoxGroups) {
            for (auto const &o: it.value.GetArray()) {
                lightColorEventBoxGroups.push_back(DeserializeLightColorEventBoxGroup(o));
            }
        }
        IF_CHECK_HASH(lightRotationEventBoxGroups) {
            for (auto const &o: it.value.GetArray()) {
                lightRotationEventBoxGroups.push_back(DeserializeLightRotationEventBoxGroup(o));
            }
        }
        IF_CHECK_HASH(basicEventTypesWithKeywords) {
            for (auto const & it : it.value.GetObject()) {
                if (it.name == "d") {
                    basicEventTypesForKeyword.push_back(DeserializeBasicEventTypesForKeyword(it.value));
                }
            }
        }
        IF_CHECK_HASH(useNormalEventsAsCompatibleEvents) {
            useNormalEventsAsCompatibleEvents = it.value.GetBool();
        }
    }


    auto beatmap = CustomBeatmapSaveData::New_ctor(
            bpmEvents,
            rotationEvents,
            colorNotes,
            bombNotes,
            obstacles,
            sliders,
            burstSliders,
            waypoints,
            basicBeatmapEvents,
            colorBoostBeatmapEvents,
            lightColorEventBoxGroups,
            lightRotationEventBoxGroups,
            BasicEventTypesWithKeywords::New_ctor(basicEventTypesForKeyword),
            useNormalEventsAsCompatibleEvents);

    CustomDataOpt dataOpt = GetCustomData(doc);

    beatmap->customData = dataOpt;

    auto customEvents = std::make_shared<std::vector<CustomJSONData::CustomEventSaveData>>();

    if (dataOpt) {
        auto customEventsIt = dataOpt->get().FindMember("customEvents");
        if (customEventsIt != dataOpt->get().MemberEnd() && !customEventsIt->value.IsNull()) {
            customEvents->reserve(customEventsIt->value.GetArray().Size());
            for (auto const& it : customEventsIt->value.GetArray()) {
                customEvents->emplace_back(DeserializeCustomEvent(it));
            }
        }
    }

    beatmap->customEventsData = customEvents;

    return beatmap;
}

static inline const std::array<float, 8> spawnRotations
        {
                -60.0f,
                -45.0f,
                -30.0f,
                -15.0f,
                15.0f,
                30.0f,
                45.0f,
                60.0f
        };

template <typename T>
constexpr bool TimeCompare(T const& a, T const& b) {
    return (a->time < b->time);
}


constexpr bool TimeCompareSliders(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SliderData* const& a, BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::SliderData* const& b) {
    return (a->headTime < b->headTime);
}

static BeatmapSaveData::NoteColorType GetNoteColorType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteType noteType) {
    return noteType == BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteType::NoteB
           ? BeatmapSaveData::NoteColorType::ColorB
           : BeatmapSaveData::NoteColorType::ColorA;
}

static BeatmapSaveData::NoteColorType GetNoteColorType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ColorType colorType) {
    return colorType == BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ColorType::ColorB
           ? BeatmapSaveData::NoteColorType::ColorB
           : BeatmapSaveData::NoteColorType::ColorA;
}

static int GetHeightForObstacleType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType obstacleType) {
    return obstacleType != BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType::Top ? 5 : 3;
}

static int GetLayerForObstacleType(BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType obstacleType)
{
return obstacleType != BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleType::Top ? 0 : 2;
}

static float SpawnRotationForEventValue(int index) {
    if (index >= 0 && index < spawnRotations.size()) {
        return spawnRotations[index];
    }

    return 0.0f;
}


CustomBeatmapSaveData *CustomBeatmapSaveData::Convert2_6_0(CustomJSONData::v2::CustomBeatmapSaveData const *beatmap) {
    auto startTime = std::chrono::high_resolution_clock::now();
    CJDLogger::GetLogger().debug("Initiating converting 2.0.0 to 3.0.0 map");

    VList<BeatmapSaveData::ColorNoteData*> colorNotes(beatmap->notes->get_Count());
    VList<BeatmapSaveData::BombNoteData*> bombNotes(beatmap->notes->get_Count());
    VList<BeatmapSaveData::SliderData*> sliders(beatmap->sliders->get_Count());
    VList<BeatmapSaveData::ObstacleData*> obstacles(beatmap->obstacles->get_Count());
    VList<BeatmapSaveData::WaypointData*> waypoints(beatmap->waypoints->get_Count());

    VList<BeatmapSaveData::ColorBoostEventData*> colorBoosts;
    VList<BeatmapSaveData::RotationEventData*> rotationEvents;
    VList<BeatmapSaveData::BpmChangeEventData*> bpmChanges;
    VList<BeatmapSaveData::BasicEventData*> basicEvents;


    CJDLogger::GetLogger().debug("Sorting");
    std::stable_sort(beatmap->notes->items.begin(), beatmap->notes->items.begin() + beatmap->notes->get_Count(), TimeCompare<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteData*>);
    std::stable_sort(beatmap->obstacles->items.begin(), beatmap->obstacles->items.begin() + beatmap->obstacles->get_Count(), TimeCompare<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ObstacleData*>);
    std::stable_sort(beatmap->sliders->items.begin(), beatmap->sliders->items.begin() + beatmap->sliders->get_Count(), TimeCompareSliders);
    std::stable_sort(beatmap->waypoints->items.begin(), beatmap->waypoints->items.begin() + beatmap->waypoints->get_Count(), TimeCompare<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::WaypointData*>);
    std::stable_sort(beatmap->events->items.begin(), beatmap->events->items.begin() + beatmap->events->get_Count(), TimeCompare<BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::EventData*>);

    CJDLogger::GetLogger().debug("Converting notes");
    for (auto const& n : VList(beatmap->notes)) {
        auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_NoteData*>(n);

        if (n->type == BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::NoteType::Bomb) {
            auto newNote = CustomBeatmapSaveData_BombNoteData::New_ctor(n->time,
                                                                        n->lineIndex,
                                                                        (int)n->lineLayer);
            newNote->customData = customN->customData;

            bombNotes.push_back(newNote);
        } else {
            auto newNote = CustomBeatmapSaveData_ColorNoteData::New_ctor(n->time,
                                                          n->lineIndex,
                                                          (int)n->lineLayer,
                                                          GetNoteColorType(n->type),
                                                          n->cutDirection,
                                                          0);
            newNote->customData = customN->customData;
            colorNotes.push_back(newNote);
        }
    }

    CJDLogger::GetLogger().debug("Converting obstacles");
    for (auto const& n : VList(beatmap->obstacles)) {
        auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData*>(n);

        auto obstacle = CustomBeatmapSaveData_ObstacleData::New_ctor(
                n->time,
                n->lineIndex,
                GetLayerForObstacleType(n->type),
                n->duration,
                n->width,
                GetHeightForObstacleType(n->type));

        obstacle->customData = customN->customData;

        obstacles.push_back(obstacle);
    }

    CJDLogger::GetLogger().debug("Converting Sliders");
    for (auto const& n : VList(beatmap->sliders)) {
        auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_SliderData*>(n);

        auto slider = CustomBeatmapSaveData_SliderData::New_ctor(
                GetNoteColorType(n->colorType),
                n->headTime,
                n->headLineIndex,
                (int)n->headLineLayer,
                n->headControlPointLengthMultiplier,
                n->headCutDirection,
                n->tailTime,
                n->tailLineIndex,
                (int)n->tailLineLayer,
                n->tailControlPointLengthMultiplier,
                n->tailCutDirection,
                n->sliderMidAnchorMode);

        slider->customData = customN->customData;

        sliders.push_back(slider);
    }

    CJDLogger::GetLogger().debug("Converting waypoints");
    for (auto const& n : VList(beatmap->waypoints)) {
        auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData*>(n);

        auto waypoint = WaypointData::New_ctor(
                n->time,
                n->lineIndex,
                (int)n->lineLayer,
                n->offsetDirection);

        waypoints.push_back(waypoint);
    }

    CJDLogger::GetLogger().debug("Converting events");
    for (auto const& n : VList(beatmap->events)) {
        auto customN = reinterpret_cast<CustomJSONData::v2::CustomBeatmapSaveData_EventData*>(n);

        switch (customN->type) {
            case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event5: {
                auto boost = ColorBoostEventData::New_ctor(n->time, n->value == 1);
                // TODO: Custom data
                colorBoosts.push_back(boost);
                break;
            }
            case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event14:
            case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event15: {
                auto rotation = RotationEventData::New_ctor(n->time,
                                                            n->type ==
                                                            BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::Event14
                                                            ? BeatmapSaveData::ExecutionTime::Early
                                                            : BeatmapSaveData::ExecutionTime::Late,
                                                            SpawnRotationForEventValue(n->value));
                // TODO: Custom data
                rotationEvents.push_back(rotation);
                break;
            }
            case BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::BeatmapEventType::BpmChange:
                // TODO: Custom data!
                bpmChanges.push_back(BpmChangeEventData::New_ctor(n->time, n->floatValue));
                break;
            default:
                break;

        }
    }

    CJDLogger::GetLogger().debug("Converting specialEventsKeywordFilters");
    VList<BeatmapSaveDataVersion3::BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword*> keywords;

    for (auto const& n : VList(beatmap->specialEventsKeywordFilters->keywords)) {
        keywords.push_back(BeatmapSaveData::BasicEventTypesWithKeywords::BasicEventTypesForKeyword::New_ctor(n->keyword, n->specialEvents));
    }

    auto basicEventTypesWithKeywords =
                 BeatmapSaveData::BasicEventTypesWithKeywords::New_ctor(keywords);

    auto v3beatmap = CustomBeatmapSaveData::New_ctor(bpmChanges,
                                                     rotationEvents,
                                                     colorNotes,
                                                     bombNotes,
                                                     obstacles,
                                                     sliders,
                                                     VList<BeatmapSaveData::BurstSliderData*>(),
                                                     waypoints,
                                                     basicEvents,
                                                     colorBoosts,
                                                     VList<BeatmapSaveData::LightColorEventBoxGroup*>(),
                                                     VList<BeatmapSaveData::LightRotationEventBoxGroup*>(),
                                                     basicEventTypesWithKeywords,
                                                     true);

    CJDLogger::GetLogger().debug("Finished converting 2.0.0 to 3.0.0 map");
    auto stopTime = std::chrono::high_resolution_clock::now();

    CJDLogger::GetLogger().debug("This took %ims", (int) std::chrono::duration_cast<std::chrono::milliseconds>(
            stopTime - startTime).count());

    return v3beatmap;
}
