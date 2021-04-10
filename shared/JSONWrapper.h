#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "System/Object.hpp"

#include "CJDLogger.h"

#include <map>

using ADMAP = std::map<char, void*>;

DECLARE_CLASS_CODEGEN(CustomJSONData, JSONWrapper, Il2CppObject,
    DECLARE_OVERRIDE_METHOD(void, Finalize, il2cpp_utils::FindMethod("System", "Object", "Finalize"));

    REGISTER_FUNCTION(
        CJDLogger::GetLogger().debug("Registering JSONWrapper!");
    )
public:
    rapidjson::Value *value;
    ADMAP associatedData;
)