#include "CustomBeatmapSaveDatav2.h"
#include "BeatmapFieldUtils.hpp"

#include "JSONWrapper.h"
#include "VList.h"
#include "JsonUtils.h"

#include "cpp-semver/shared/cpp-semver.hpp"

using namespace GlobalNamespace;
using namespace BeatmapSaveDataVersion2_6_0AndEarlier;

DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_NoteData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_SliderData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_ObstacleData);
DEFINE_TYPE(CustomJSONData::v2, CustomBeatmapSaveData_EventData);

void CustomJSONData::v2::CustomBeatmapSaveData::ctor(
    System::Collections::Generic::List_1<EventData*>* events,
    System::Collections::Generic::List_1<NoteData*>* notes,
    System::Collections::Generic::List_1<SliderData*>* sliders,
    System::Collections::Generic::List_1<WaypointData*>* waypoints,
    System::Collections::Generic::List_1<ObstacleData*>* obstacles,
    SpecialEventKeywordFiltersData* specialEventsKeywordFilters) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(BeatmapSaveData*), ".ctor", 6);

  il2cpp_utils::RunMethodRethrow(this, ctor, events, notes, sliders, waypoints, obstacles, specialEventsKeywordFilters);
  //    this->events = events;
  //    this->notes = notes;
  //    this->waypoints = waypoints;
  //    this->obstacles = obstacles;
  //    this->specialEventsKeywordFilters = specialEventsKeywordFilters;
}

void CustomJSONData::v2::CustomBeatmapSaveData_NoteData::ctor(float time, int lineIndex, BeatmapSaveDataCommon::NoteLineLayer lineLayer,
                                                              NoteType type,
                                                              BeatmapSaveDataCommon::NoteCutDirection cutDirection) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(NoteData*), ".ctor", 5);
  il2cpp_utils::RunMethodRethrow(this, ctor, time, lineIndex, lineLayer, type, cutDirection);
  //    this->time = time;
  //    this->lineIndex = lineIndex;
  //    this->lineLayer = lineLayer;
  //    this->type = type;
  //    this->cutDirection = cutDirection;
}

void CustomJSONData::v2::CustomBeatmapSaveData_ObstacleData::ctor(float time, int lineIndex,
                                                                  ObstacleType type, float duration,
                                                                  int width) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(ObstacleData*), ".ctor", 5);
  il2cpp_utils::RunMethodRethrow(this, ctor, time, lineIndex, type, duration, width);
  this->_time = time;
  this->_lineIndex = lineIndex;
  this->_type = type;
  this->_duration = duration;
  this->_width = width;
}

void CustomJSONData::v2::CustomBeatmapSaveData_EventData::ctor(float time, BeatmapSaveDataCommon::BeatmapEventType type,
                                                               int value, float floatValue) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(EventData*), ".ctor", 4);
  il2cpp_utils::RunMethodRethrow(this, ctor, time, type, value, floatValue);

  this->_time = time;
  this->_type = type;
  this->_value = value;
  this->_floatValue = floatValue;
}

void CustomJSONData::v2::CustomBeatmapSaveData_SliderData::ctor(
    ColorType colorType, float headTime, int headLineIndex,
    BeatmapSaveDataCommon::NoteLineLayer headLineLayer, float headControlPointLengthMultiplier,
    BeatmapSaveDataCommon::NoteCutDirection headCutDirection, float tailTime, int tailLineIndex,
    BeatmapSaveDataCommon::NoteLineLayer tailLineLayer, float tailControlPointLengthMultiplier,
    BeatmapSaveDataCommon::NoteCutDirection tailCutDirection, BeatmapSaveDataCommon::SliderMidAnchorMode sliderMidAnchorMode) {
  INVOKE_CTOR();
  static auto const* ctor = il2cpp_utils::FindMethodUnsafe(classof(SliderData*), ".ctor", 12);
  il2cpp_utils::RunMethodRethrow(
      this, ctor, colorType, headTime, headLineIndex, headLineLayer, headControlPointLengthMultiplier, headCutDirection,
      tailTime, tailLineIndex, tailLineLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode);
}

static void ConvertBeatmapSaveDataPreV2_5_0(CustomJSONData::v2::CustomBeatmapSaveData* beatmapSaveData) {
  CJDLogger::Logger.fmtLog<LogLevel::INF>("Converting pre v2.5.0");

  using namespace CustomJSONData::v2;

  auto size = beatmapSaveData->events ? beatmapSaveData->events->_size : 0;
  auto list = VList<EventData*>::New();
  list->EnsureCapacity(size);

  for (auto* originalEventData : ListW<EventData*>(beatmapSaveData->events)) {
    auto* eventData = il2cpp_utils::cast<CustomBeatmapSaveData_EventData>(originalEventData);
    CustomBeatmapSaveData_EventData* newData = nullptr;
    if (eventData->type == BeatmapSaveDataCommon::BeatmapEventType::Event10) {
      newData = CRASH_UNLESS(CustomBeatmapSaveData_EventData::New_ctor(
          eventData->time, BeatmapSaveDataCommon::BeatmapEventType::BpmChange, eventData->value, eventData->floatValue));
    }

    if (eventData->type == BeatmapSaveDataCommon::BeatmapEventType::BpmChange) {
      if (eventData->value != 0) {
        newData = CRASH_UNLESS(CustomBeatmapSaveData_EventData::New_ctor(eventData->time, eventData->type,
                                                                         eventData->value, (float)eventData->value));
      }
    } else {
      newData = CRASH_UNLESS(
          CustomBeatmapSaveData_EventData::New_ctor(eventData->time, eventData->type, eventData->value, 1.0F));
    }

    if (newData) {
      newData->customData = eventData->customData;
    }

    list.push_back(newData ? newData : eventData);
  }

  beatmapSaveData->_events = list;
}


inline decltype(CustomJSONData::JSONWrapper::value) GetCustomData(rapidjson::Value const& doc) {
  auto customDataIt = doc.FindMember("_customData");
  if (customDataIt != doc.MemberEnd() && customDataIt->value.IsObject()) {
    return customDataIt->value;
  }

  return std::nullopt;
}

CustomJSONData::v2::CustomBeatmapSaveData*
CustomJSONData::v2::CustomBeatmapSaveData::Deserialize(std::shared_ptr<rapidjson::Document> const& sharedDoc) {
  auto const& doc = *sharedDoc;

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse notes");

  auto notes = VList<NoteData*>::New();
  auto notesArrIt = doc.FindMember("_notes");

  if (notesArrIt != doc.MemberEnd() && notesArrIt->value.IsArray()) {
    auto const& notesArr = notesArrIt->value;
    notes->EnsureCapacity(notesArr.Size());

    for (rapidjson::SizeType i = 0; i < notesArr.Size(); i++) {
      rapidjson::Value const& note_json = notesArr[i];

      float time = NEJSON::ReadOptionalFloat(note_json, "_time").value_or(0);
      int lineIndex = NEJSON::ReadOptionalFloat(note_json, "_lineIndex").value_or(0);
      auto lineLayer = BeatmapSaveDataCommon::NoteLineLayer(NEJSON::ReadOptionalFloat(note_json, "_lineLayer").value_or(0));
      auto type = BeatmapSaveDataVersion2_6_0AndEarlier::NoteType(NEJSON::ReadOptionalFloat(note_json, "_type").value_or(0));
      auto cutDirection = BeatmapSaveDataCommon::NoteCutDirection(NEJSON::ReadOptionalFloat(note_json, "_cutDirection").value_or(0));
      auto* note =
          CRASH_UNLESS(CustomBeatmapSaveData_NoteData::New_ctor(time, lineIndex, lineLayer, type, cutDirection));

      auto customDataIt = note_json.FindMember("_customData");
      if (customDataIt != note_json.MemberEnd() && customDataIt->value.IsObject()) {
        note->customData = customDataIt->value;
      }
      notes.push_back(note);
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} notes", notes.size());

  auto sliders = VList<SliderData*>::New();
  auto slidersArrIt = doc.FindMember("_sliders");

  if (slidersArrIt != doc.MemberEnd() && slidersArrIt->value.IsArray()) {
    auto const& slidersArr = slidersArrIt->value;
    sliders->EnsureCapacity(slidersArr.Size());

    for (rapidjson::SizeType i = 0; i < slidersArr.Size(); i++) {
      rapidjson::Value const& slider_json = slidersArr[i];

      int lineIndex = slider_json["_lineIndex"].GetInt();
      auto lineLayer = BeatmapSaveDataCommon::NoteLineLayer(slider_json["_lineLayer"].GetInt());
      auto type = BeatmapSaveDataVersion2_6_0AndEarlier::NoteType(slider_json["_type"].GetInt());
      auto cutDirection = BeatmapSaveDataCommon::NoteCutDirection(slider_json["_cutDirection"].GetInt());

      float time = slider_json["_time"].GetFloat();
      auto colorType = ColorType(slider_json["_colorType"].GetInt());
      int headLineIndex = slider_json["_headLineIndex"].GetInt();
      BeatmapSaveDataCommon::NoteLineLayer noteLineLayer = slider_json["_noteLineLayer"].GetInt();
      float headControlPointLengthMultiplier = slider_json["_headControlPointLengthMultiplier"].GetFloat();
      BeatmapSaveDataCommon::NoteCutDirection noteCutDirection = slider_json["_noteCutDirection"].GetInt();
      float tailTime = slider_json["_tailTime"].GetFloat();
      int tailLineIndex = slider_json["_tailLineIndex"].GetInt();
      BeatmapSaveDataCommon::NoteLineLayer tailLineLayer = slider_json["_tailLineLayer"].GetInt();
      float tailControlPointLengthMultiplier = slider_json["_tailControlPointLengthMultiplier"].GetFloat();
      BeatmapSaveDataCommon::NoteCutDirection tailCutDirection = slider_json["_tailCutDirection"].GetInt();
      BeatmapSaveDataCommon::SliderMidAnchorMode sliderMidAnchorMode = slider_json["_sliderMidAnchorMode"].GetInt();

      auto* slider = CRASH_UNLESS(CustomBeatmapSaveData_SliderData::New_ctor(
          colorType, time, headLineIndex, noteLineLayer, headControlPointLengthMultiplier, noteCutDirection, tailTime,
          tailLineIndex, tailLineLayer, tailControlPointLengthMultiplier, tailCutDirection, sliderMidAnchorMode));
      slider->customData = GetCustomData(slider_json);

      sliders.push_back(slider);
    }
  }
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} sliders", sliders.size());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse obstacles");
  auto obstaclesArrIt = doc.FindMember("_obstacles");

  auto obstacles = VList<ObstacleData*>::New();

  if (obstaclesArrIt->value.IsArray()) {
    auto const& obstaclesArr = obstaclesArrIt->value;

    obstacles->EnsureCapacity(obstaclesArr.Size());

    for (rapidjson::SizeType i = 0; i < obstaclesArr.Size(); i++) {
      rapidjson::Value const& obstacle_json = obstaclesArr[i];

      float time = obstacle_json["_time"].GetFloat();
      int lineIndex = obstacle_json["_lineIndex"].GetInt();
      auto type = ObstacleType(obstacle_json["_type"].GetInt());
      float duration = obstacle_json["_duration"].GetFloat();
      int width = obstacle_json["_width"].GetInt();
      auto* obstacle =
          CRASH_UNLESS(CustomBeatmapSaveData_ObstacleData::New_ctor(time, lineIndex, type, duration, width));

      obstacle->customData = GetCustomData(obstacle_json);

      obstacles.push_back(obstacle);
    }
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} obstacles", obstacles.size());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse events");

  auto eventsArrIt = doc.FindMember("_events");
  auto events = VList<EventData*>::New();

  if (eventsArrIt != doc.MemberEnd() && eventsArrIt->value.IsArray()) {
    // Parse events
    rapidjson::Value const& eventsArr = eventsArrIt->value;
    events->EnsureCapacity(eventsArr.Size());

    for (rapidjson::SizeType i = 0; i < eventsArr.Size(); i++) {
      rapidjson::Value const& event_json = eventsArr[i];

      float time = event_json["_time"].GetFloat();
      auto type = BeatmapSaveDataCommon::BeatmapEventType(event_json["_type"].GetInt());
      int value = event_json["_value"].GetInt();
      float floatValue = 0;

      auto floatValueIt = event_json.FindMember("_floatValue");
      if (floatValueIt != event_json.MemberEnd()) {
        floatValue = floatValueIt->value.GetFloat();
      }

      auto* event = CRASH_UNLESS(CustomBeatmapSaveData_EventData::New_ctor(time, type, value, floatValue));
      event->customData = GetCustomData(event_json);

      events.push_back(event);
    }
  }
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} events", events.size());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse waypoints");
  auto waypoints_arrIt = doc.FindMember("_waypoints");

  VList<WaypointData*> waypoints = VList<WaypointData*>::New();

  if (waypoints_arrIt != doc.MemberEnd() && waypoints_arrIt->value.IsArray()) {
    rapidjson::Value const& waypoints_arr = doc["_waypoints"];

    waypoints->EnsureCapacity(waypoints_arr.Size());

    for (rapidjson::SizeType i = 0; i < waypoints_arr.Size(); i++) {
      rapidjson::Value const& waypoint_json = waypoints_arr[i];

      float time = waypoint_json["_time"].GetFloat();
      int lineIndex = waypoint_json["_lineIndex"].GetInt();
      auto lineLayer = BeatmapSaveDataCommon::NoteLineLayer(waypoint_json["_lineLayer"].GetInt());
      auto offsetDirection = BeatmapSaveDataCommon::OffsetDirection(waypoint_json["_offsetDirection"].GetInt());
      auto* waypoint =
          CustomJSONData::NewFast<WaypointData*>(time, lineIndex, lineLayer, offsetDirection);
      waypoints.push_back(waypoint);
    }
  }
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsed {} waypoints", waypoints.size());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse specialEventsKeywordFilters");
  auto specialEventsKeywordFiltersJsonObjIt = doc.FindMember("_specialEventsKeywordFilters");
  VList<SpecialEventsForKeyword*> specialEventsKeywordFiltersList =
      VList<SpecialEventsForKeyword*>::New();

  if (specialEventsKeywordFiltersJsonObjIt != doc.MemberEnd()) {
    rapidjson::Value const& specialEventsKeywordFiltersJsonObj = specialEventsKeywordFiltersJsonObjIt->value;

    auto _keywords = specialEventsKeywordFiltersJsonObj.FindMember("_keywords");

    if (_keywords != specialEventsKeywordFiltersJsonObj.MemberEnd()) {
      specialEventsKeywordFiltersList->EnsureCapacity(_keywords->value.Size());

      for (auto const& keyword_json : _keywords->value.GetArray()) {
        std::string keyword = keyword_json["_keyword"].GetString();
        Il2CppString* keyword_il2cpp = il2cpp_utils::newcsstr(keyword);

        auto specialEventsArray = keyword_json["_specialEvents"].GetArray();
        auto specialEvents = VList<BeatmapSaveDataCommon::BeatmapEventType>::New(specialEventsArray.Size());

        for (auto const& specialEvent : specialEventsArray) {
          // safety, why not?
          if (!specialEvent.IsNumber()) {
            continue;
          }

          specialEvents.push_back(specialEvent.GetInt());
        }

        specialEventsKeywordFiltersList.push_back(
            CustomJSONData::NewFast<SpecialEventsForKeyword*>(keyword_il2cpp, specialEvents.getPtr()));
      }
    }
  }
  auto* specialEventsKeywordFilters = CustomJSONData::NewFast<SpecialEventKeywordFiltersData*>(
      specialEventsKeywordFiltersList.getPtr());

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse root");
  auto* saveData = CRASH_UNLESS(
      CustomBeatmapSaveData::New_ctor(events, notes, sliders, waypoints, obstacles, specialEventsKeywordFilters));
  saveData->doc = sharedDoc;
  saveData->customEventsData = std::make_shared<std::vector<CustomJSONData::CustomEventSaveData>>();
  auto customDataIt = doc.FindMember("_customData");
  if (customDataIt->value.IsObject()) {
    rapidjson::Value const& customData = customDataIt->value;
    saveData->customData = customData;

    auto customEventsIt = customData.FindMember("_customEvents");
    if (customEventsIt != customData.MemberEnd() && customEventsIt->value.IsArray()) {
      CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parse custom events");

      rapidjson::Value const& customEventsArr = customEventsIt->value;
      for (rapidjson::SizeType i = 0; i < customEventsArr.Size(); i++) {
        rapidjson::Value const& eventValue = customEventsArr[i];

        // Any consequences? Nah never
        auto typeIt = eventValue.FindMember("_type");
        if (typeIt == eventValue.MemberEnd() || typeIt->value.IsNull()) {
          continue;
        }

        float time = 0;
        // Dammit Reaxt
        auto timeIt = eventValue.FindMember("_time");
        if (timeIt != eventValue.MemberEnd()) {
          rapidjson::Value const& timeValue = timeIt->value;
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
    saveData->_version = versionIt->value.GetString();
  } else {
    saveData->_version = nullptr;
  }

  // Below taken straight from BeatmapSaveData.DeserializeFromJSONString
  CJDLogger::Logger.fmtLog<LogLevel::INF>("v2 Version {}", static_cast<std::string>(saveData->version ?: "null"));
  if (saveData->version && !static_cast<std::string>(saveData->version).empty()) {
    if (semver::lt(static_cast<std::string>(saveData->version), "2.5.0")) {
      ::ConvertBeatmapSaveDataPreV2_5_0(saveData);
    }
  } else {
    ::ConvertBeatmapSaveDataPreV2_5_0(saveData);
  }

  return saveData;
}
