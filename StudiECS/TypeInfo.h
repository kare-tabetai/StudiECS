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
        ret.m_is_trivial_copyable = std::is_trivially_copyable_v<T>;
        ret.m_is_trivial_destructible = std::is_trivially_destructible_v<T>;
        ret.m_type_size = sizeof(T);
        ret.m_align_size = alignof(T);
        ret.m_is_empty_type = std::is_empty_v<T>;
        ret.m_id = TypeIDGenerator<T>::id();

        if constexpr (std::is_default_constructible_v<T>) {
            ret.m_default_constructor = [](void* ptr) {
                T* casted_ptr = static_cast<T*>(ptr);
                new (casted_ptr) T();
            };
        }
        if constexpr (std::is_destructible_v<T>) {
            ret.m_destructor = [](void* ptr) {
                T* casted_ptr = static_cast<T*>(ptr);
                casted_ptr->~T();
            };
        }
        if constexpr (std::is_copy_constructible_v<T>) {
            ret.m_copy_constructor = [](const void* source, void* dest) {
                const T* casted_source = static_cast<const T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                new (casted_dest) T(*casted_source);
            };
        }
        if constexpr (std::is_move_constructible_v<T>) {
            ret.m_move_constructor = [](void* source, void* dest) {
                T* casted_source = static_cast<T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                new (casted_dest) T(std::move(*casted_source));
            };
        }
        if constexpr (std::is_copy_assignable_v<T>) {
            ret.m_copy_asign = [](const void* source, void* dest) {
                const T* casted_source = static_cast<const T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                *casted_dest = *casted_source;
            };
        }
        if constexpr (std::is_move_assignable_v<T>) {
            ret.m_move_asign = [](void* source, void* dest) {
                T* casted_source = static_cast<T*>(source);
                T* casted_dest = static_cast<T*>(dest);
                *casted_dest = std::move(*casted_source);
            };
        }

        return ret;
    }

    inline bool Construct(void* strage) const
    {
        if (!m_default_constructor) {
            return false;
        }
        m_default_constructor(strage);
        return true;
    }

    inline void Destruct(void* strage) const
    {
        m_destructor(strage);
    }

    inline bool CopyConstruct(const void* source, void* dest) const
    {
        if (!m_copy_constructor) {
            return false;
        }
        m_copy_constructor(source, dest);
        return true;
    }

    inline bool MoveConstruct(void* source, void* dest) const
    {
        if (!m_move_constructor) {
            return false;
        }
        m_move_constructor(source, dest);
        return true;
    }

    inline bool CopyAsign(const void* source, void* dest) const
    {
        if (!m_copy_asign) {
            return false;
        }
        m_copy_asign(source, dest);
        return true;
    }

    inline bool MoveAsign(void* source, void* dest) const
    {
        if (!m_move_asign) {
            return false;
        }
        m_move_asign(source, dest);
        return true;
    }

    bool CanTrivialCopy() const { return m_is_trivial_copyable; }
    bool CanTrivialDestruct() const { return m_is_trivial_destructible; }
    bool IsEmptyType() const { return m_is_empty_type; }
    bool GetTypeSize() const { return m_type_size; }
    bool GetAlignSize() const { return m_align_size; }
    TypeDataID GetID() const { return m_id; }

private:
    bool m_is_trivial_copyable = false;
    bool m_is_trivial_destructible = false;
    bool m_is_empty_type = false;
    std::size_t m_type_size = -1;
    std::size_t m_align_size = -1;
    TypeDataID m_id ;

    ConstructorFunc m_default_constructor = nullptr;
    DestructorFunc m_destructor = nullptr;
    CopyConstructorFunc m_copy_constructor = nullptr;
    MoveConstructorFunc m_move_constructor = nullptr;
    CopyAsignFunc m_copy_asign = nullptr;
    MoveAsignFunc m_move_asign = nullptr;
};
