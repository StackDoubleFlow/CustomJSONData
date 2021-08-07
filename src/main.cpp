#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"


extern "C" void setup(ModInfo &info) {
    info.id = "CustomJSONData";
    info.version = VERSION;
    CJDLogger::modInfo = info;
}

extern "C" void load() {
    CJDLogger::GetLogger().info("Installing CustomJSONData Hooks!");

    CustomJSONData::InstallHooks();

    CJDLogger::GetLogger().info("Installed CustomJSONData Hooks!");
}