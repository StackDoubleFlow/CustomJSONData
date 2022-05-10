#pragma once

#include <optional>
#include "beatsaber-hook/shared/utils/il2cpp-utils-methods.hpp"

/// Collection of methods that are intended to be low level performance optimizations for
/// niche use cases
/// Might PR to bs-hooks sometime later
namespace CustomJSONData {

    // Faster allocation with method cache
    template <class T, class... TArgs>
    // Bonus points for a requires clause here for classof(T)
    T NewFast(TArgs &&...args)
    {
        static auto klass = classof(T);
        static auto ctor = il2cpp_utils::FindMethod(klass, ".ctor", std::forward<TArgs>(args)...);
        auto *obj = il2cpp_functions::object_new(klass);
        il2cpp_utils::RunMethodUnsafe(obj, ctor, std::forward<TArgs>(args)...);
        return reinterpret_cast<T>(obj);
    }

    // Declare a method with name that will be called on construction.
#define DECLARE_FASTER_CTOR(name, ...)                                                                                                                                                \
public:                                                                                                                                                                               \
    void name(__VA_ARGS__);                                                                                                                                                           \
    template <::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary, class... TArgs>                                                                    \
    static ___TargetType *New_ctor(TArgs &&...args)                                                                                                                                   \
    {                                                                                                                                                                                 \
        static_assert(::custom_types::Decomposer<decltype(&___TargetType::name)>::convertible<TArgs...>(), "Arguments provided to New_ctor must be convertible to the constructor!"); \
        return THROW_UNLESS(CustomJSONData::NewFast<___TargetType *, creationType>(___TypeRegistration::klass_ptr, std::forward<TArgs>(args)...));                                    \
    }                                                                                                                                                                                 \
    ___CREATE_INSTANCE_METHOD(name, ".ctor", METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME | METHOD_ATTRIBUTE_RT_SPECIAL_NAME, nullptr)
}