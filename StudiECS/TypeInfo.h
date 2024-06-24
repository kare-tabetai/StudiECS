#pragma once
#include "Type.h"
#include <any>
#include <cstddef>
#include <functional>
#include <type_traits>

class TypeInfo {
public:
    using ConstructorFunc = std::function<void(void*)>;
    using DestructorFunc = std::function<void(void*)>;
    using CopyConstructorFunc = std::function<void(const void*, void*)>;
    using MoveConstructorFunc = std::function<void(void*, void*)>;
    using CopyAsignFunc = std::function<void(const void*, void*)>;
    using MoveAsignFunc = std::function<void(void*, void*)>;

    template<class T>
    static TypeInfo Make()
    {
        TypeInfo ret;
        ret.is_trivial_copyable = std::is_trivially_copyable_v<T>;
        ret.is_trivial_destructible = std::is_trivially_destructible_v<T>;
        ret.type_size = sizeof(T);
        ret.align_size = alignof(T);

        if constexpr (std::is_default_constructible_v<T>) {
            ret.default_constructor = [](void* ptr) {
                T* casted_ptr = static_cast<T*>(ptr);
                new (casted_ptr) T();
            };
        }
        if constexpr (std::is_destructible_v<T>) {
            ret.destructor = [](void* ptr) {
                T* casted_ptr = static_cast<T*>(ptr);
                casted_ptr->~T();
            };
        }
        if constexpr (std::is_copy_constructible_v<T>) {
            ret.copy_constructor = [](const void* source, void* dest) {
                const T* casted_source = static_cast<const T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                new (casted_dest) T(*casted_source);
            };
        }
        if constexpr (std::is_move_constructible_v<T>) {
            ret.move_constructor = [](void* source, void* dest) {
                T* casted_source = static_cast<T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                new (casted_dest) T(std::move(*casted_source));
            };
        }
        if constexpr (std::is_copy_assignable_v<T>) {
            ret.copy_asign = [](const void* source, void* dest) {
                const T* casted_source = static_cast<const T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                *casted_dest = *casted_source;
            };
        }
        if constexpr (std::is_move_assignable_v<T>) {
            ret.move_asign = [](void* source, void* dest) {
                T* casted_source = static_cast<T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                *casted_dest = std::move(*casted_source);
            };
        }

        return ret;
    }

    inline bool Construct(void* strage) const
    {
        if (!default_constructor) {
            return false;
        }
        default_constructor(strage);
        return true;
    }

    inline void Destruct(void* strage) const
    {
        destructor(strage);
    }

    inline bool CopyConstruct(const void* source, void* dest) const
    {
        if (!copy_constructor) {
            return false;
        }
        copy_constructor(source, dest);
        return true;
    }

    inline bool MoveConstruct(void* source, void* dest) const
    {
        if (!move_constructor) {
            return false;
        }
        move_constructor(source, dest);
        return true;
    }

    inline bool CopyAsign(const void* source, void* dest) const
    {
        if (!copy_asign) {
            return false;
        }
        copy_asign(source, dest);
        return true;
    }

    inline bool MoveAsign(void* source, void* dest) const
    {
        if (!move_asign) {
            return false;
        }
        move_asign(source, dest);
        return true;
    }

    bool CanTrivialCopy() const { return is_trivial_copyable; }
    bool CanTrivialDestruct() const { return is_trivial_destructible; }
    bool GetTypeSize() const { return type_size; }
    bool GetAlignSize() const { return align_size; }

private:
    bool is_trivial_copyable = false;
    bool is_trivial_destructible = false;
    std::size_t type_size = -1;
    std::size_t align_size = -1;
    TypeDataID id ;

    ConstructorFunc default_constructor = nullptr;
    DestructorFunc destructor = nullptr;
    CopyConstructorFunc copy_constructor = nullptr;
    MoveConstructorFunc move_constructor = nullptr;
    CopyAsignFunc copy_asign = nullptr;
    MoveAsignFunc move_asign = nullptr;
};
