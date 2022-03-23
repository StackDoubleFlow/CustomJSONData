#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"

#include "paper/shared/logger.hpp"

using LogLevel = Paper::LogLevel;

class CJDLogger {
public:
    static inline ModInfo modInfo = ModInfo();
    static Logger& GetLoggerOld() {
        static auto logger = new class Logger(modInfo, LoggerOptions(false, true));
        return *logger;
    }

    static inline auto Logger = Paper::Logger::WithContext<"CustomJSONData">("CustomJSONDataPaper");
};
