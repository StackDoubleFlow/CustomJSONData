#include "CustomBeatmapSaveDatav2.h"
#include "BeatmapFieldUtils.hpp"

#include "VList.h"

#include "GlobalNamespace/BeatmapEventTypeExtensions.hpp"

#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveData_SpecialEventKeywordFiltersData.hpp"
#include "BeatmapSaveDataVersion2_6_0AndEarlier/BeatmapSaveData_SpecialEventsForKeyword.hpp"

#include "cpp-semver/shared/cpp-semver.hpp"

using namespace GlobalNamespace;
using namespace BeatmapSaveDataVersion2_6_0AndEarlier;

DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_NoteData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_SliderData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_ObstacleData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_EventData);

void CustomJSONData::v2::CustomBeatmapSaveData::ctor(List<BeatmapSaveData::EventData*>* events,
                                                       List<BeatmapSaveData::NoteData*>* notes,
                                                       List<BeatmapSaveData::SliderData*>* sliders,
                                                       List<BeatmapSaveData::WaypointData*>* waypoints,
                                                       List<BeatmapSaveData::ObstacleData*>* obstacles,
                                                       BeatmapSaveData::SpecialEventKeywordFiltersData *specialEventsKeywordFilters) {
    INVOKE_CTOR();
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData*), ".ctor", 6);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, events, notes, sliders, waypoints, obstacles, specialEventsKeywordFilters));
//    this->events = events;
//    this->notes = notes;
//    this->waypoints = waypoints;
//    this->obstacles = obstacles;
//    this->specialEventsKeywordFilters = specialEventsKeywordFilters;
}



void CustomJSONData::v2::CustomBeatmapSaveData_NoteData::ctor(float time, int lineIndex, NoteLineLayer lineLayer, BeatmapSaveData::NoteType type, NoteCutDirection cutDirection) {
    INVOKE_CTOR();
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::NoteData*), ".ctor", 5);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, time, lineIndex, lineLayer, type, cutDirection));
//    this->time = time;
//    this->lineIndex = lineIndex;
//    this->lineLayer = lineLayer;
//    this->type = type;
//    this->cutDirection = cutDirection;
}



void CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData::ctor(float time, int lineIndex, BeatmapSaveData::ObstacleType type, float duration, int width) {
    INVOKE_CTOR();
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::ObstacleData*), ".ctor", 5);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, time, lineIndex, type, duration, width));
    this->time = time;
    this->lineIndex = lineIndex;
    this->type = type;
    this->duration = duration;
    this->width = width;
}



void CustomJSONData::v2::CustomBeatmapSaveData_EventData::ctor(float time, BeatmapSaveData::BeatmapEventType type, int value, float floatValue) {
    INVOKE_CTOR();
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::EventData*), ".ctor", 4);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, time, type, value, floatValue));

    this->time = time;
    this->type = type;
    this->value = value;
    this->floatValue = floatValue;
}

void CustomJSONData::v2::CustomBeatmapSaveData_SliderData::ctor(
        BeatmapSaveDataVersion2_6_0AndEarlier::BeatmapSaveData::ColorType colorType, float headTime, int headLineIndex,
        ::GlobalNamespace::NoteLineLayer headLineLayer, float headControlPointLengthMultiplier,
        ::GlobalNamespace::NoteCutDirection headCutDirection, float tailTime, int tailLineIndex,
        ::GlobalNamespace::NoteLineLayer tailLineLayer, float tailControlPointLengthMultiplier,
        ::GlobalNamespace::NoteCutDirection tailCutDirection,
        ::GlobalNamespace::SliderMidAnchorMode sliderMidAnchorMode) {
    INVOKE_CTOR();
    static auto* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData::SliderData*), ".ctor", 12);
    CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, colorType, headTime, headLineIndex, headLineLayer, headControlPointLengthMultiplier, headCutDirection, tailTime, tailLineIndex, tailLineLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode));
}


static void ConvertBeatmapSaveDataPreV2_5_0(CustomJSONData::v2::CustomBeatmapSaveData* beatmapSaveData)
{
    CJDLogger::Logger.fmtLog<LogLevel::INF>("Converting pre v2.5.0");

    using namespace CustomJSONData::v2;

    auto size = beatmapSaveData->events ? beatmapSaveData->events->size : 0;
    VList<BeatmapSaveData::EventData*> list(size);
    for (auto originalEventData : VList(beatmapSaveData->events))
    {
        auto* eventData = reinterpret_cast<CustomBeatmapSaveData_EventData *>(originalEventData);
        CustomBeatmapSaveData_EventData* newData = nullptr;
        if (eventData->type == BeatmapSaveData::BeatmapEventType::Event10)
        {
            newData = CRASH_UNLESS(CustomBeatmapSaveData_EventData::New_ctor(eventData->time, BeatmapSaveData::BeatmapEventType::BpmChange, eventData->value, eventData->floatValue));
        }

        if (static_cast<int>(eventData->type) == BeatmapSaveData::BeatmapEventType::BpmChange)
        {
            if (eventData->value != 0)
            {
                newData = CRASH_UNLESS(CustomBeatmapSaveData_EventData::New_ctor(eventData->time, eventData->type, eventData->value, (float) eventData->value));
            }
        }
        else
        {
            newData = CRASH_UNLESS(CustomBeatmapSaveData_EventData::New_ctor(eventData->time, eventData->type, eventData->value, 1.0f));
        }

        if (newData)
            newData->customData = CustomJSONData::JSONObjectOrNull(eventData->customData);

        list.push_back(newData ? newData : eventData);
    }

    beatmapSaveData->events = list;
}

inline decltype(CustomJSONData::v2::CustomBeatmapSaveData::customData) GetCustomData(rapidjson::Value const & doc) {
    auto customDataIt = doc.FindMember("_customData");
    if (customDataIt != doc.MemberEnd() && customDataIt->value.IsObject()) {
        return customDataIt->value;
    }

    return std::nullopt;
}

CustomJSONData::v2::CustomBeatmapSaveData *
CustomJSONData::v2::CustomBeatmapSaveData::Deserialize(std::shared_ptr<rapidjson::Document> sharedDoc) {
    auto const& doc = *sharedDoc;

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse notes");

    VList<BeatmapSaveData::NoteData *> notes;
    auto notesArrIt = doc.FindMember("_notes");

    if (notesArrIt != doc.MemberEnd() && notesArrIt->value.IsArray()) {
        auto &notesArr = notesArrIt->value;
        notes = VList<BeatmapSaveData::NoteData *>(notesArr.Size());


        for (rapidjson::SizeType i = 0; i < notesArr.Size(); i++) {
            rapidjson::Value const& note_json = notesArr[i];

            float time = note_json["_time"].GetFloat();
            int lineIndex = note_json["_lineIndex"].GetInt();
            auto lineLayer = NoteLineLayer(note_json["_lineLayer"].GetInt());
            auto type = BeatmapSaveData::NoteType(note_json["_type"].GetInt());
            auto cutDirection = NoteCutDirection(note_json["_cutDirection"].GetInt());
            auto note = CRASH_UNLESS(CustomBeatmapSaveData_NoteData::New_ctor(time, lineIndex, lineLayer, type, cutDirection));

            auto customDataIt = note_json.FindMember("_customData");
            if (customDataIt != note_json.MemberEnd() && customDataIt->value.IsObject()) {
                note->customData = CustomJSONData::JSONObjectOrNull(customDataIt->value);
            }
            notes.push_back(note);
        }
    }

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} notes", notes.size());


    VList<BeatmapSaveData::SliderData *> sliders;
    auto slidersArrIt = doc.FindMember("_sliders");

    if (slidersArrIt != doc.MemberEnd() && slidersArrIt->value.IsArray()) {
        auto &slidersArr = slidersArrIt->value;
        sliders = VList<BeatmapSaveData::SliderData *>(slidersArr.Size());


        for (rapidjson::SizeType i = 0; i < slidersArr.Size(); i++) {
            rapidjson::Value const& slider_json = slidersArr[i];

            int lineIndex = slider_json["_lineIndex"].GetInt();
            auto lineLayer = NoteLineLayer(slider_json["_lineLayer"].GetInt());
            auto type = BeatmapSaveData::NoteType(slider_json["_type"].GetInt());
            auto cutDirection = NoteCutDirection(slider_json["_cutDirection"].GetInt());

            float time = slider_json["_time"].GetFloat();
            BeatmapSaveData::ColorType colorType = ColorType(slider_json["_colorType"].GetInt());
            int headLineIndex = slider_json["_headLineIndex"].GetInt();
            NoteLineLayer noteLineLayer = slider_json["_noteLineLayer"].GetInt();
            float headControlPointLengthMultiplier = slider_json["_headControlPointLengthMultiplier"].GetFloat();
            NoteCutDirection noteCutDirection = slider_json["_noteCutDirection"].GetInt();
            float tailTime = slider_json["_tailTime"].GetFloat();
            int tailLineIndex = slider_json["_tailLineIndex"].GetInt();
            NoteLineLayer tailLineLayer = slider_json["_tailLineLayer"].GetInt();
            float tailControlPointLengthMultiplier = slider_json["_tailControlPointLengthMultiplier"].GetFloat();
            NoteCutDirection tailCutDirection = slider_json["_tailCutDirection"].GetInt();
            SliderMidAnchorMode sliderMidAnchorMode = slider_json["_sliderMidAnchorMode"].GetInt();

            auto slider = CRASH_UNLESS(CustomBeatmapSaveData_SliderData::New_ctor(
                    colorType,
                    time,
                    headLineIndex,
                    noteLineLayer,
                    headControlPointLengthMultiplier,
                    noteCutDirection,
                    tailTime,
                    tailLineIndex,
                    tailLineLayer,
                    tailControlPointLengthMultiplier,
                    tailCutDirection,
                    sliderMidAnchorMode));
            slider->customData = GetCustomData(slider_json);

            sliders.push_back(slider);
        }
    }
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} sliders", sliders.size());

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse obstacles");
    auto obstaclesArrIt = doc.FindMember("_obstacles");

    VList<BeatmapSaveData::ObstacleData *> obstacles;

    if (obstaclesArrIt->value.IsArray()) {
        auto &obstaclesArr = obstaclesArrIt->value;

        obstacles = VList<BeatmapSaveData::ObstacleData *>(obstaclesArr.Size());

        for (rapidjson::SizeType i = 0; i < obstaclesArr.Size(); i++) {
            rapidjson::Value const& obstacle_json = obstaclesArr[i];

            float time = obstacle_json["_time"].GetFloat();
            int lineIndex = obstacle_json["_lineIndex"].GetInt();
            ObstacleType type = ObstacleType(obstacle_json["_type"].GetInt());
            float duration = obstacle_json["_duration"].GetFloat();
            int width = obstacle_json["_width"].GetInt();
            auto obstacle = CRASH_UNLESS(
                    CustomBeatmapSaveData_ObstacleData::New_ctor(time, lineIndex, type, duration,
                                                                            width));

            obstacle->customData = GetCustomData(obstacle_json);

            obstacles.push_back(obstacle);
        }
    }

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} obstacles", obstacles.size());

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse events");

    auto eventsArrIt = doc.FindMember("_events");
    VList<BeatmapSaveData::EventData *> events;

    if (eventsArrIt != doc.MemberEnd() && eventsArrIt->value.IsArray()) {
        // Parse events
        rapidjson::Value const& eventsArr = eventsArrIt->value;
        events = VList<BeatmapSaveData::EventData *>(eventsArr.Size());

        for (rapidjson::SizeType i = 0; i < eventsArr.Size(); i++) {
            rapidjson::Value const& event_json = eventsArr[i];

            float time = event_json["_time"].GetFloat();
            auto type = BeatmapSaveData::BeatmapEventType(event_json["_type"].GetInt());
            int value = event_json["_value"].GetInt();
            float floatValue = 0;

            auto floatValueIt = event_json.FindMember("_floatValue");
            if (floatValueIt != event_json.MemberEnd()) {
                floatValue = floatValueIt->value.GetFloat();
            }

            auto event = CRASH_UNLESS(CustomBeatmapSaveData_EventData::New_ctor(time, type, value, floatValue));
            event->customData = GetCustomData(event_json);

            events.push_back(event);
        }
    }
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} events", events.size());

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse waypoints");
    auto waypoints_arrIt = doc.FindMember("_waypoints");

    VList<BeatmapSaveData::WaypointData *> waypoints;

    if (waypoints_arrIt != doc.MemberEnd() && waypoints_arrIt->value.IsArray()) {
        rapidjson::Value const& waypoints_arr = doc["_waypoints"];

        waypoints = VList<BeatmapSaveData::WaypointData *>(waypoints_arr.Size());

        for (rapidjson::SizeType i = 0; i < waypoints_arr.Size(); i++) {
            rapidjson::Value const& waypoint_json = waypoints_arr[i];

            float time = waypoint_json["_time"].GetFloat();
            int lineIndex = waypoint_json["_lineIndex"].GetInt();
            NoteLineLayer lineLayer = NoteLineLayer(waypoint_json["_lineLayer"].GetInt());
            OffsetDirection offsetDirection = OffsetDirection(waypoint_json["_offsetDirection"].GetInt());
            auto waypoint = CustomJSONData::NewFast<BeatmapSaveData::WaypointData*>(time, lineIndex, lineLayer, offsetDirection);
            waypoints.push_back(waypoint);
        }
    }
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} waypoints", waypoints.size());

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse specialEventsKeywordFilters");
    auto specialEventsKeywordFiltersJsonObjIt = doc.FindMember("_specialEventsKeywordFilters");
    VList<BeatmapSaveData::SpecialEventsForKeyword *> specialEventsKeywordFiltersList;

    if (specialEventsKeywordFiltersJsonObjIt != doc.MemberEnd()) {
        rapidjson::Value const &specialEventsKeywordFiltersJsonObj = specialEventsKeywordFiltersJsonObjIt->value;


        auto _keywords = specialEventsKeywordFiltersJsonObj.FindMember("_keywords");

        if (_keywords != specialEventsKeywordFiltersJsonObj.MemberEnd()) {
            specialEventsKeywordFiltersList = VList<BeatmapSaveData::SpecialEventsForKeyword *>(_keywords->value.Size());

            for (auto const &keyword_json: _keywords->value.GetArray()) {
                std::string keyword = keyword_json["_keyword"].GetString();
                Il2CppString *keyword_il2cpp = il2cpp_utils::newcsstr(keyword);

                auto specialEventsArray = keyword_json["_specialEvents"].GetArray();
                VList<BeatmapSaveData::BeatmapEventType> specialEvents(specialEventsArray.Size());

                for (auto &specialEvent: specialEventsArray) {
                    // safety, why not?
                    if (!specialEvent.IsNumber())
                        continue;

                    specialEvents.push_back(specialEvent.GetInt());
                }

                specialEventsKeywordFiltersList.push_back(
                        CustomJSONData::NewFast<BeatmapSaveData::SpecialEventsForKeyword*>(keyword_il2cpp, *specialEvents));
            }
        }
    }
    auto specialEventsKeywordFilters = CustomJSONData::NewFast<BeatmapSaveData::SpecialEventKeywordFiltersData*>(
            *specialEventsKeywordFiltersList);

    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse root");
    auto saveData = CRASH_UNLESS(CustomBeatmapSaveData::New_ctor(*events, *notes, *sliders, *waypoints, *obstacles,
                                                                            specialEventsKeywordFilters));
    saveData->doc = sharedDoc;
    saveData->customEventsData = std::make_shared<std::vector<CustomJSONData::CustomEventSaveData>>();
    auto customDataIt = doc.FindMember("_customData");
    if (customDataIt->value.IsObject()) {
        saveData->customData = CustomJSONData::JSONObjectOrNull(customDataIt->value);
        rapidjson::Value const& customData = *saveData->customData;

        auto customEventsIt = customData.FindMember("_customEvents");
        if (customEventsIt != customData.MemberEnd() && customEventsIt->value.IsArray()) {
            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse custom events");

            rapidjson::Value const& customEventsArr = customEventsIt->value;
            for (rapidjson::SizeType i = 0; i < customEventsArr.Size(); i++) {
                rapidjson::Value const& eventValue = customEventsArr[i];

                // Any consequences? Nah never
                auto typeIt = eventValue.FindMember("_type");
                if (typeIt == eventValue.MemberEnd() || typeIt->value.IsNull())
                    continue;

                float time = 0;
                // Dammit Reaxt
                auto timeIt = eventValue.FindMember("_time");
                if (timeIt != eventValue.MemberEnd()) {
                    rapidjson::Value const&  timeValue = timeIt->value;
                    if (timeValue.GetType() == rapidjson::Type::kStringType) {
                        // Reaxt why
                        time = std::stof(timeValue.GetString());
                    } else {
                        time = timeValue.GetFloat();
                    }
                }

                std::string_view type = typeIt->value.GetString();

                rapidjson::Value const* data = &eventValue["_data"];
                saveData->customEventsData->emplace_back(type, time, data);
            }

            CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} custom events", saveData->customEventsData->size());
        }
    }

    auto versionIt = doc.FindMember("_version");
    if (versionIt != doc.MemberEnd()) {
        saveData->version = versionIt->value.GetString();
    } else {
        saveData->version = nullptr;
    }

    // Below taken straight from BeatmapSaveData.DeserializeFromJSONString
    CJDLogger::Logger.fmtLog<LogLevel::INF>("v2 Version {}", static_cast<std::string>(saveData->version));
    if (saveData->version && !csstrtostr(saveData->version).empty())
    {
        if (semver::lt(static_cast<std::string>(saveData->version), "2.5.0")) {
            ::ConvertBeatmapSaveDataPreV2_5_0(saveData);
        }
    }
    else
    {
        ::ConvertBeatmapSaveDataPreV2_5_0(saveData);
    }

    return saveData;
}
