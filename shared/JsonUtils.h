#pragma once

#include <functional>
#include <optional>

#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/FastVector2.hpp"
#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/FastColor.hpp"

#ifndef RAPIDJSON_NEON
#define RAPIDJSON_NEON
#endif

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "CJDLogger.h"

namespace NEJSON {

static std::optional<bool> ReadOptionalBool(rapidjson::Value const& object, std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    if (itr->value.IsString()) {
      std::string boolS = itr->value.GetString();
      CJDLogger::Logger.fmtLog<LogLevel::ERR>("ReadOptionalBool: THE VALUE IS A STRING WHY! value: \"{}\"", boolS);

      if (boolS == "true") {
        return true;
      }

      return false;
    }

    if (itr->value.IsNumber()) {
      return itr->value.GetInt() != 0;
    }

    return itr->value.GetBool();
  }
  return std::nullopt;
}

static std::optional<std::string_view> ReadOptionalString(rapidjson::Value const& object, std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd() && itr->value.IsString()) {
    return itr->value.GetString();
  }
  return std::nullopt;
}

static std::optional<float> ReadOptionalFloat(rapidjson::Value const& object, std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    return itr->value.GetFloat();
  }
  return std::nullopt;
}

static std::optional<int> ReadOptionalInt(rapidjson::Value const& object, std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    return itr->value.GetInt();
  }
  return std::nullopt;
}

static std::optional<Sombrero::FastVector2> ReadOptionalVector2(rapidjson::Value const& object,
                                                                std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd() && itr->value.Size() >= 2) {
    float x = itr->value[0].GetFloat();
    float y = itr->value[1].GetFloat();
    return Sombrero::FastVector2(x, y);
  }
  return std::nullopt;
}

// Used for note flip
static std::optional<Sombrero::FastVector2> ReadOptionalVector2_emptyY(rapidjson::Value const& object,
                                                                       std::string_view const key) {
  auto itr = object.FindMember(key.data());

  if (itr != object.MemberEnd() && itr->value.Size() >= 1) {
    float x = itr->value[0].GetFloat();
    float y = 0;

    if (itr->value.Size() > 1) {
      y = itr->value[1].GetFloat();
    }
    return Sombrero::FastVector2(x, y);
  }
  return std::nullopt;
}

using OptPair = std::pair<std::optional<float>, std::optional<float>>;

static OptPair ReadOptionalPair(rapidjson::Value const& object, std::string_view const key) {
  auto itr = object.FindMember(key.data());

  if (itr != object.MemberEnd() && itr->value.Size() >= 1) {
    float x = itr->value[0].GetFloat();
    float y = 0;

    if (itr->value.Size() >= 2) {
      y = itr->value[1].GetFloat();
      return { std::optional<float>(x), std::optional<float>(y) };
    }
    return { std::optional<float>(x), std::nullopt };
  }
  return { std::nullopt, std::nullopt };
}

static std::optional<Sombrero::FastQuaternion> ReadOptionalRotation(rapidjson::Value const& object,
                                                                    std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    Sombrero::FastVector3 rot;
    if (itr->value.IsArray() && itr->value.Size() >= 3) {
      float x = itr->value[0].GetFloat();
      float y = itr->value[1].GetFloat();
      float z = itr->value[2].GetFloat();
      rot = Sombrero::FastVector3(x, y, z);
    } else if (itr->value.IsNumber()) {
      rot = Sombrero::FastVector3(0, itr->value.GetFloat(), 0);
    }

    return Sombrero::FastQuaternion::Euler(rot);
  }
  return std::nullopt;
}

static std::optional<Sombrero::FastVector3> ReadOptionalVector3(rapidjson::Value const& object,
                                                                std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd() && itr->value.Size() >= 3) {
    float x = itr->value[0].GetFloat();
    float y = itr->value[1].GetFloat();
    float z = itr->value[2].GetFloat();
    return Sombrero::FastVector3(x, y, z);
  }
  return std::nullopt;
}

static std::optional<std::array<std::optional<float>, 3>> ReadOptionalScale(rapidjson::Value const& object,
                                                                            std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd() && itr->value.IsArray()) {
    rapidjson::SizeType size = itr->value.Size();
    std::optional<float> x = size >= 1 ? std::optional{ itr->value[0].GetFloat() } : std::nullopt;
    std::optional<float> y = size >= 2 ? std::optional{ itr->value[1].GetFloat() } : std::nullopt;
    std::optional<float> z = size >= 3 ? std::optional{ itr->value[2].GetFloat() } : std::nullopt;
    return { { x, y, z } };
  }
  return std::nullopt;
}

static std::optional<rapidjson::Value::ConstObject> ReadOptionalObject(rapidjson::Value const& object,
                                                                       std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    return itr->value.GetObject();
  }
  return std::nullopt;
}
static std::optional<rapidjson::Value const*> ReadOptionalValuePtr(rapidjson::Value const& object,
                                                                std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    return &itr->value;
  }
  return std::nullopt;
}
static std::optional<std::reference_wrapper<rapidjson::Value const>> ReadOptionalValue(rapidjson::Value const& object,
                                                                std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    return std::ref(itr->value);
  }
  return std::nullopt;
}

template <typename T, typename F>
static std::optional<std::vector<T>> ReadOptionalArray(rapidjson::Value const& object, std::string_view const key,
                                                       F&& func) {
  auto itr = object.FindMember(key.data());
  if (itr == object.MemberEnd()) {
    return std::nullopt;
  }

  auto jsonArray = itr->value.GetArray();

  std::vector<T> vec;
  vec.reserve(jsonArray.Size());

  for (auto const& v : jsonArray) {
    vec.emplace_back(func(v));
  }

  return vec;
}

template <typename T, typename F>
static inline std::vector<T> ReadArrayOrEmpty(rapidjson::Value const& object, std::string_view const key, F&& func) {
  auto opt = ReadOptionalArray<T, F>(object, key, std::forward<F>(func));

  if (!opt) {
    return {};
  }

  return *opt;
}

template <typename T>
static std::optional<T> ReadOptionalType(rapidjson::Value const& object, std::string_view const key) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    return itr->value.Get<T>();
  }
  return std::nullopt;
}
template <typename T, typename F>
static std::optional<T> ReadOptionalType(rapidjson::Value const& object, std::string_view const key, F&& func) {
  auto itr = object.FindMember(key.data());
  if (itr != object.MemberEnd()) {
    return func(itr->value);
  }
  return std::nullopt;
}

inline std::string ToStringJSONValue(rapidjson::Value const& json) {
#if DEBUGB == 1
  return;
#endif
  using namespace rapidjson;

  StringBuffer sb;
  PrettyWriter<StringBuffer> writer(sb);
  json.Accept(writer);
  return sb.GetString();
}

} // namespace NEJSON