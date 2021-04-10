#pragma once

#include "custom-types/shared/macros.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "System/Object.hpp"

#include "CJDLogger.h"

#include <map>

using ADMAP = std::map<char, void*>;

DECLARE_CLASS_CODEGEN(CustomJSONData, JSONWrapper, Il2CppObject,
    DECLARE_OVERRIDE_METHOD(void, Finalize, il2cpp_utils::FindMethod("System", "Object", "Finalize"));
    DECLARE_DEFAULT_CTOR();
    DECLARE_SIMPLE_DTOR();

    REGISTER_FUNCTION(
        CJDLogger::GetLogger().debug("Registering JSONWrapper!");
        REGISTER_DEFAULT_CTOR();
        REGISTER_SIMPLE_DTOR();
    )
public:
    rapidjson::Value *value;
    ADMAP associatedData;
)