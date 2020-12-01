
#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"

class CJSLogger {
public:
    static inline ModInfo modInfo = ModInfo();
    static const Logger& GetLogger()
    {
        static const Logger& log(modInfo);
        return log;
    }
};