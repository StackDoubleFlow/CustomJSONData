#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/logging.hpp"

#include "CustomJSONDataHooks.h"
#include "CJSLogger.h"


extern "C" void setup(ModInfo &info) {
    info.id = "CustomJSONData";
    info.version = VERSION;
    CJSLogger::modInfo = info;
}

extern "C" void load() {
    CJSLogger::GetLogger().info("Installing CustomJSONData Hooks!");
    // This prevents any and all Utils logging
    Logger::get().options.silent = true;

    // Install hooks

    CustomJSONData::InstallHooks();

    CJSLogger::GetLogger().info("Installed CustomJSONData Hooks!");
}