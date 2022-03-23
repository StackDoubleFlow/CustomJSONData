#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "CJDLogger.h"

namespace CustomJSONData {

void InstallHooks();

}

// Implements a try-catch handler which will first attempt to run the provided body.
// If there is an uncaught RunMethodException, it will first attempt to log the backtrace.
// If it holds a valid C# exception, it will attempt to raise it, such that it is caught in the il2cpp domain.
// If an exception is thrown that is otherwise what-able is caught, it will attempt to call the what() method
// and then rethrow the exception to the il2cpp domain.
// If an unknown exception is caught, it will terminate explicitly, as opposed to letting it be thrown across the il2cpp domain.
// All logs that occur as a result of this function will be under the core beatsaber-hook global logger.
#define PAPER_IL2CPP_CATCH_HANDLER(...) try { \
    __VA_ARGS__ \
} catch (::il2cpp_utils::RunMethodException const& exc) { \
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("Uncaught RunMethodException! what(): {}", exc.what()); \
    Paper::Logger::WaitForFlush();                                    \
    ::Logger::get().error("Uncaught RunMethodException! what(): %s", exc.what()); \
    exc.log_backtrace(); \
    if (exc.ex) { \
        exc.rethrow(); \
    } \
    SAFE_ABORT(); \
} catch (::std::exception const& exc) { \
    ::Logger::get().error("Uncaught C++ exception! type name: %s, what(): %s", typeid(exc).name(), exc.what()); \
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("Uncaught C++ exception! type name: {}, what(): {}", typeid(exc).name(), exc.what()); \
    Paper::Logger::WaitForFlush();                                    \
    ::il2cpp_utils::raise(exc); \
} catch (...) {                         \
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("Uncaught, unknown C++ exception (not std::exception) with no known what() method!"); \
    Paper::Logger::WaitForFlush();                                    \
    ::Logger::get().error("Uncaught, unknown C++ exception (not std::exception) with no known what() method!"); \
    SAFE_ABORT(); \
}

template<auto Func, class T>
/// @brief Exposes a static wrapper method that forwards to the provided function pointer, wrapping it in IL2CPP_CATCH_HANDLER.
struct PaperHookCatchWrapper;

template<auto Func, class R, class... TArgs>
struct PaperHookCatchWrapper<Func, R (*)(TArgs...)> {
    static R wrapper(TArgs... args) {
        PAPER_IL2CPP_CATCH_HANDLER(
                return Func(args...);
        )
    }
};

// Make a hook that uses the provided method pointer in a match an ensures the signature matches.
// This should be your go-to hook macro when hooking anything that has a codegen equivalent.
// Also includes a catch handler.
#define MAKE_PAPER_HOOK_MATCH(name_, mPtr, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get(); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::PaperHookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)