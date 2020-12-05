#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/logging.hpp"

#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"


extern "C" void setup(ModInfo &info) {
    info.id = "CustomJSONData";
    info.version = VERSION;
    CJDLogger::modInfo = info;
}

extern "C" void load() {
    CJDLogger::GetLogger().info("Installing CustomJSONData Hooks!");
    // This prevents any and all Utils logging
    Logger::get().options.silent = true;

    // Install hooks

    CustomJSONData::InstallHooks();

    CJDLogger::GetLogger().info("Installed CustomJSONData Hooks!");
}