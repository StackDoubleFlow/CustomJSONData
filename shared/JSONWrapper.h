#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "System/Object.hpp"

#include "CJSLogger.h"

DECLARE_CLASS_CODEGEN(CustomJSONData, JSONWrapper, Il2CppObject,
    REGISTER_FUNCTION(JSONWrapper,
        CJSLogger::GetLogger().debug("Registering JSONWrapper!");
    )
public:
    rapidjson::Value *value;
)