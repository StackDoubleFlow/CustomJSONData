#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "scotland2/shared/loader.hpp"

#include "paper2_scotland2/shared/logger.hpp"

using LogLevel = Paper::LogLevel;

namespace CustomJSONData {
static modloader::ModInfo const modInfo{"CustomJSONData", VERSION, 1};
}

class CJDLogger {
public:
  // Register file log in main.cpp
  static constexpr auto Logger =
      Paper::ConstLoggerContext("CustomJSONData"); // Paper::Logger::WithContext<"CustomJSONData", false>();
};

// Implements a try-catch handler which will first attempt to run the provided body.
// If there is an uncaught RunMethodException, it will first attempt to log the backtrace.
// If it holds a valid C# exception, it will attempt to raise it, such that it is caught in the il2cpp domain.
// If an exception is thrown that is otherwise what-able is caught, it will attempt to call the what() method
// and then rethrow the exception to the il2cpp domain.
// If an unknown exception is caught, it will terminate explicitly, as opposed to letting it be thrown across the il2cpp
// domain. All logs that occur as a result of this function will be under the core beatsaber-hook global logger.
#define PAPER_IL2CPP_CATCH_HANDLER(...)                                                                                \
  try {                                                                                                                \
    __VA_ARGS__                                                                                                        \
  } catch (::il2cpp_utils::RunMethodException const& exc) {                                                            \
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("Uncaught RunMethodException! what(): {}", exc.what());                    \
    Paper::Logger::WaitForFlush();                                                                                     \
    CJDLogger::Logger.error("Uncaught RunMethodException! what(): {}", exc.what());                                    \
    exc.log_backtrace();                                                                                               \
    CJDLogger::Logger.Backtrace(100);                                                                                  \
    if (exc.ex) {                                                                                                      \
      exc.rethrow();                                                                                                   \
    }                                                                                                                  \
    SAFE_ABORT();                                                                                                      \
  } catch (::il2cpp_utils::exceptions::StackTraceException const& exc) {                                               \
    CJDLogger::Logger.error("Uncaught StackTraceException! what(): {}", exc.what());                                   \
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("Uncaught StackTraceException! what(): {}", exc.what());                   \
    exc.log_backtrace();                                                                                               \
    CJDLogger::Logger.Backtrace(100);                                                                                  \
    SAFE_ABORT();                                                                                                      \
  } catch (::std::exception const& exc) {                                                                              \
    CJDLogger::Logger.error("Uncaught C++ exception! type name: {}, what(): {}", typeid(exc).name(), exc.what());      \
    CJDLogger::Logger.fmtLog<LogLevel::ERR>("Uncaught C++ exception! type name: {}, what(): {}", typeid(exc).name(),   \
                                            exc.what());                                                               \
    Paper::Logger::WaitForFlush();                                                                                     \
    CJDLogger::Logger.Backtrace(100);                                                                                  \
    ::il2cpp_utils::raise(exc);                                                                                        \
  } catch (...) {                                                                                                      \
    CJDLogger::Logger.fmtLog<LogLevel::ERR>(                                                                           \
        "Uncaught, unknown C++ exception (not std::exception) with no known what() method!");                          \
    Paper::Logger::WaitForFlush();                                                                                     \
    CJDLogger::Logger.error("Uncaught, unknown C++ exception (not std::exception) with no known what() method!");      \
    CJDLogger::Logger.Backtrace(100);                                                                                  \
    SAFE_ABORT();                                                                                                      \
  }

template <auto Func, class T>
/// @brief Exposes a static wrapper method that forwards to the provided function pointer, wrapping it in
/// IL2CPP_CATCH_HANDLER.
struct PaperHookCatchWrapper;

template <auto Func, class R, class... TArgs> struct PaperHookCatchWrapper<Func, R (*)(TArgs...)> {
  static R wrapper(TArgs... args) {
    PAPER_IL2CPP_CATCH_HANDLER(return Func(args...);)
  }
};

// Make a hook that uses the provided method pointer in a match an ensures the signature matches.
// This should be your go-to hook macro when hooking anything that has a codegen equivalent.
// Also includes a catch handler.
#define MAKE_PAPER_HOOK_MATCH(name_, mPtr, retval, ...)                                                                \
  struct Hook_##name_ {                                                                                                \
    using funcType = retval (*)(__VA_ARGS__);                                                                          \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>,                  \
                  "Hook method signature does not match!");                                                            \
    constexpr static char const* name() {                                                                              \
      return #name_;                                                                                                   \
    }                                                                                                                  \
    static MethodInfo const* getInfo() {                                                                               \
      return ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo();                                    \
    }                                                                                                                  \
    static funcType* trampoline() {                                                                                    \
      return &name_;                                                                                                   \
    }                                                                                                                  \
    static inline retval (*name_)(__VA_ARGS__) = nullptr;                                                              \
    static funcType hook() {                                                                                           \
      return &::PaperHookCatchWrapper<&hook_##name_, funcType>::wrapper;                                               \
    }                                                                                                                  \
    static retval hook_##name_(__VA_ARGS__);                                                                           \
  };                                                                                                                   \
  retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Same as MAKE_PAPER_HOOK_MATCH, but for manual MethodInfo* usage.
#define MAKE_PAPER_HOOK_FIND(name_, infoGet, retval, ...)                                                              \
  struct Hook_##name_ {                                                                                                \
    using funcType = retval (*)(__VA_ARGS__);                                                                          \
    constexpr static char const* name() {                                                                              \
      return #name_;                                                                                                   \
    }                                                                                                                  \
    static MethodInfo const* getInfo() {                                                                               \
      return infoGet;                                                                                                  \
    }                                                                                                                  \
    static funcType* trampoline() {                                                                                    \
      return &name_;                                                                                                   \
    }                                                                                                                  \
    static inline retval (*name_)(__VA_ARGS__) = nullptr;                                                              \
    static funcType hook() {                                                                                           \
      return &::PaperHookCatchWrapper<&hook_##name_, funcType>::wrapper;                                               \
    }                                                                                                                  \
    static retval hook_##name_(__VA_ARGS__);                                                                           \
  };                                                                                                                   \
  retval Hook_##name_::hook_##name_(__VA_ARGS__)