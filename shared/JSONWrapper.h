#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "System/Object.hpp"

#include "CJDLogger.h"

#include <unordered_map>

DECLARE_CLASS_CODEGEN(CustomJSONData, JSONWrapper, Il2CppObject,
    DECLARE_DEFAULT_CTOR();
    DECLARE_SIMPLE_DTOR();

    REGISTER_FUNCTION(
        CJDLogger::GetLogger().debug("Registering JSONWrapper!");
        REGISTER_DEFAULT_CTOR();
        REGISTER_SIMPLE_DTOR();
    )
public:
    ~JSONWrapper();
    rapidjson::Value *value;
    std::unordered_map<char, void*> associatedData;
)