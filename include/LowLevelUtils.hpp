#pragma once

#include <optional>
#include "beatsaber-hook/shared/utils/il2cpp-utils-methods.hpp"

/// Collection of methods that are intended to be low level performance optimizations for
/// niche use cases
/// Might PR to bs-hooks sometime later
namespace CustomJSONData {



    /// Based on il2cpp_utils::New
    template<typename TOut = Il2CppObject*, il2cpp_utils::CreationType creationType = il2cpp_utils::CreationType::Temporary, bool Unsafe = false, typename... TArgs>
    // Creates a new object of the given class using the given constructor parameters
    // Will only run a .ctor whose parameter types match the given arguments.
    ::std::optional<TOut> New(Il2CppClass* klass, TArgs&& ...args) {
        static auto& logger = il2cpp_utils::getLogger();
        il2cpp_functions::Init();


        /// Cacheing here is fine, template methods don't share static local variables
        static MethodInfo const* CONSTRUCTOR_METHOD;



        Il2CppObject* obj;
        if constexpr (creationType == il2cpp_utils::CreationType::Temporary) {
            // object_new call
            obj = RET_NULLOPT_UNLESS(logger, il2cpp_functions::object_new(klass));
        } else {
            obj = RET_NULLOPT_UNLESS(logger, il2cpp_utils::createManual(klass));
        }

        if constexpr (Unsafe) {
            CONSTRUCTOR_METHOD = RET_NULLOPT_UNLESS(logger, il2cpp_utils::FindMethodUnsafe(obj, ".ctor", sizeof...(args)));
        } else {
            CONSTRUCTOR_METHOD = RET_NULLOPT_UNLESS(logger, il2cpp_utils::FindMethod(obj, ".ctor", std::forward<TArgs>(args)...));
        }

        // runtime_invoke constructor with right type(s) of arguments, return null if constructor errors
        RET_NULLOPT_UNLESS(logger, RunMethod(obj, CONSTRUCTOR_METHOD, args...));
        return FromIl2CppObject<TOut>(obj);
    }

    // Declare a method with name that will be called on construction.
#define DECLARE_FASTER_CTOR(name, ...) \
public: \
void name(__VA_ARGS__); \
template<::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary, class... TArgs> \
static ___TargetType* New_ctor(TArgs&&... args) { \
    static_assert(::custom_types::Decomposer<decltype(&___TargetType::name)>::convertible<TArgs...>(), "Arguments provided to New_ctor must be convertible to the constructor!"); \
    return THROW_UNLESS(CustomJSONData::CacheNew<___TargetType*, creationType>(___TypeRegistration::klass_ptr, std::forward<TArgs>(args)...)); \
} \
___CREATE_INSTANCE_METHOD(name, ".ctor", METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME | METHOD_ATTRIBUTE_RT_SPECIAL_NAME, nullptr)


}