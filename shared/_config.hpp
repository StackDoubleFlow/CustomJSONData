#pragma once

#define CJD_MOD_EXPORT __attribute__((visibility("default")))
#define CJD_MOD_EXTERN_FUNC extern "C" CJD_MOD_EXPORT