#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "System/Object.hpp"

#include "CJDLogger.h"

#include <unordered_map>
#include <any>

DECLARE_CLASS_CODEGEN(CustomJSONData, DocumentWrapper, Il2CppObject,
    DECLARE_DEFAULT_CTOR();
    DECLARE_SIMPLE_DTOR();

public:
    std::shared_ptr<rapidjson::Document> doc;
)

DECLARE_CLASS_CODEGEN(CustomJSONData, JSONWrapper, Il2CppObject,
    DECLARE_DEFAULT_CTOR();
    DECLARE_SIMPLE_DTOR();

public:
    std::optional<std::reference_wrapper<rapidjson::Value>> value;
    std::unordered_map<char, std::any> associatedData;
)