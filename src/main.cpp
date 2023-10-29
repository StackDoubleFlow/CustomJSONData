#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"

extern "C" void setup(ModInfo& info) {
  info.id = "CustomJSONData";
  info.version = VERSION;
  CJDLogger::modInfo = info;
}

extern "C" void load() {
  Paper::Logger::RegisterFileContextId(CJDLogger::Logger.tag, "CustomJSONDataPaper");

  CJDLogger::Logger.fmtLog<LogLevel::INF>("Installing CustomJSONData Hooks!");

  CustomJSONData::InstallHooks();

  CJDLogger::Logger.fmtLog<LogLevel::INF>("Installed CustomJSONData Hooks!");
}