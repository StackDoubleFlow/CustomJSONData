#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "CJDLogger.h"

namespace CustomJSONData {

void InstallHooks();

namespace v2 {
void InstallHooks();
}

namespace v3 {
void InstallHooks();
}

}
