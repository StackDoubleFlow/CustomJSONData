#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "System/Object.hpp"

#include "CJDLogger.h"

#include <unordered_map>
#include <any>

#include "songcore/shared/CustomJSONData.hpp"
#include "LowLevelUtils.hpp"

DECLARE_CLASS_CODEGEN(CustomJSONData, DocumentWrapper, Il2CppObject) {

  DECLARE_DEFAULT_CTOR();
  DECLARE_SIMPLE_DTOR();

public:
  std::shared_ptr<rapidjson::Document> doc;
};

DECLARE_CLASS_CODEGEN(CustomJSONData, JSONWrapper, Il2CppObject) {
  DECLARE_FASTER_CTOR(ctor);
  DECLARE_SIMPLE_DTOR();

  DECLARE_INSTANCE_METHOD(JSONWrapper*, GetCopy);

public:
  std::optional<std::reference_wrapper<rapidjson::Value const>> value;
  std::unordered_map<char, std::any> associatedData;
};

DECLARE_CLASS_CODEGEN(CustomJSONData, JSONWrapperUTF16, Il2CppObject) {

  DECLARE_FASTER_CTOR(ctor);
  DECLARE_SIMPLE_DTOR();

public:
  std::optional<std::reference_wrapper<SongCore::CustomJSONData::ValueUTF16 const>> value;
  std::unordered_map<char, std::any> associatedData;

  DECLARE_INSTANCE_METHOD(JSONWrapperUTF16*, GetCopy);
};
