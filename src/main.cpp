#include "_config.hpp"

#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"

CJD_MOD_EXTERN_FUNC void setup(CModInfo* info) {
  *info = CustomJSONData::modInfo.to_c();
  Paper::Logger::RegisterFileContextId(CJDLogger::Logger.tag, "CustomJSONDataPaper");
}

CJD_MOD_EXTERN_FUNC void late_load() {
  CJDLogger::Logger.fmtLog<LogLevel::INF>("Installing CustomJSONData Hooks!");

  CustomJSONData::InstallHooks();

  CJDLogger::Logger.fmtLog<LogLevel::INF>("Installed CustomJSONData Hooks!");
}