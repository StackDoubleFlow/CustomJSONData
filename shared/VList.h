#pragma once
#include "System/Collections/Generic/List_1.hpp"
#include "CJDLogger.h"

// template<typename ListTy>
// class ListIterator {
// public:
//     using iterator_category = random_access_iterator_tag;
//     using difference_type   = std::ptrdiff_t;
//     using value_type        = ListTy::value_type;
//     using pointer           = ListTy::pointer;
//     using reference         = ListTy::reference;

//     ListIterator(pointer ptr_) : ptr(ptr_) {}

//     reference operator*() const {
//         return *ptr;
//     }

//     pointer operator->() {
//         return ptr;
//     }

//     ListIterator& operator++() {
//         ptr++;
//         return *this;
//     }  

//     ListIterator operator++(int) {
//         ListIterator tmp = *this;
//         (*this)++;
//         return tmp;
//     }

//     bool operator==(const Iterator& other) const {
//         return ptr == other.ptr;
//     };

//     bool operator!=(const Iterator& other) const {
//         return ptr != other.ptr;
//     };  
// private:
//     pointer ptr;
// }

template<typename Ty>
class VList {
private:
    using InnerTy = System::Collections::Generic::List_1<Ty>;
    InnerTy *inner;

public:
    using value_type = Ty;
    using pointer = Ty*;
    using const_pointer = const Ty*;
    using reference = Ty&;

    // Maybe I'll use my own iterator type if needed
    using iterator = pointer;
    using const_iterator = const_pointer;

public:
    VList() 
        : inner(InnerTy::New_ctor(0)) {};
    
    VList(int size) 
        : inner(InnerTy::New_ctor(size)) {
    };

    VList(InnerTy *list)
        : inner(list) {};

    InnerTy* operator*() const {
        return inner;
    }

    operator InnerTy*() const {
        return inner;
    }

    Ty& operator[](const size_t pos) const {
        return inner->items.get(pos);
    }

    [[nodiscard]] int size() const {
        return inner->size;
    }

    auto resize(const size_t cap) {
        return inner->EnsureCapacity(cap);
    }

    void trim() const {
        return inner->TrimExcess();
    }

    void push_back(const Ty& val) {
        // TODO: C++ impl
        return inner->Add(val);
    }

    iterator begin() {
        return inner->items.begin();
    }

    iterator end() {
        return inner->items.begin() + size();
    }

    [[nodiscard]] InnerTy * getInner() const {
        return inner;
    }

    [[nodiscard]] constexpr void* convert() const noexcept {
        return inner;
    }
};

// If it's not just a pointer then bad things will happen
static_assert(sizeof(VList<int>) == 0x8);

template<typename Ty>
struct il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<VList<Ty>> {
    static inline Il2CppClass* get() {
        return classof(System::Collections::Generic::List_1<Ty>*);
    }
};

static_assert(il2cpp_utils::has_il2cpp_conversion<VList<int>>);

template<class T>
struct ::il2cpp_utils::il2cpp_type_check::need_box<VList<T>> {
    constexpr static bool value = false;
};