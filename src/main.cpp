#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"



extern "C" void setup(CModInfo& info) {
  info = CustomJSONData::modInfo.to_c();
}

extern "C" void late_load() {
  Paper::Logger::RegisterFileContextId(CJDLogger::Logger.tag, "CustomJSONDataPaper");

  CJDLogger::Logger.fmtLog<LogLevel::INF>("Installing CustomJSONData Hooks!");

  CustomJSONData::InstallHooks();

  CJDLogger::Logger.fmtLog<LogLevel::INF>("Installed CustomJSONData Hooks!");
}