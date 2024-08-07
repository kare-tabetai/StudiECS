#pragma once
#include "IdGenerator.h"
#include <any>
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
        constexpr bool kTriviallyConstructible = std::is_trivially_constructible_v<T>;
        constexpr bool kTriviallyDestructible = std::is_trivially_destructible_v<T>;

        TypeInfo ret;

#if _DEBUG
        ret.m_type_name = TypeIDGenerator<T>::type_name();
#endif // DEBUG

        ret.m_is_trivial_copyable = std::is_trivially_copyable_v<T>;
        ret.m_is_trivial_destructible = kTriviallyDestructible;
        ret.m_type_size = sizeof(T);
        ret.m_align_size = alignof(T);
        ret.m_is_empty_type = std::is_empty_v<T>;
        ret.m_id = TypeIDGenerator<T>::id();

        if constexpr (std::is_default_constructible_v<T> && !kTriviallyConstructible) {
            ret.m_default_constructor = [](void* ptr) {
                T* casted_ptr = static_cast<T*>(ptr);
                new (casted_ptr) T();
            };
        }
        if constexpr (std::is_destructible_v<T> && !kTriviallyDestructible) {
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

#if _DEBUG
    std::string GetTypeName() const { return m_type_name; }
#endif // DEBUG

    bool Construct(void* strage) const
    {
        if (m_default_constructor) {
            m_default_constructor(strage);
            return true;
        } else {
            return false;
        }

    }

    bool Destruct(void* strage) const
    {
        if (m_destructor) {
            m_destructor(strage);
            return true;
        } else {
            return false;
        }
    }

    bool CopyConstruct(const void* source, void* dest) const
    {
        if (m_copy_constructor) {
            m_copy_constructor(source, dest);
            return true;
        } else {
            return false;
        }
    }

    bool MoveConstruct(void* source, void* dest) const
    {
        if (m_move_constructor) {
            m_move_constructor(source, dest);
            return true;
        } else {
            return false;
        }

    }

    bool CopyAsign(const void* source, void* dest) const
    {
        if (m_copy_asign) {
            m_copy_asign(source, dest);
            return true;
        } else {
            return false;
        }
    }

    bool MoveAsign(void* source, void* dest) const
    {
        if (m_move_asign) {
            m_move_asign(source, dest);
            return true;
        } else {
            return false;
        }
    }

    bool CanTrivialCopy() const { return m_is_trivial_copyable; }
    bool CanTrivialDestruct() const { return m_is_trivial_destructible; }
    bool IsEmptyType() const { return m_is_empty_type; }
    std::size_t GetTypeSize() const { return m_type_size; }
    std::size_t GetAlignSize() const { return m_align_size; }
    TypeDataID GetID() const { return m_id; }

private:
#if _DEBUG
    std::string m_type_name = "";
#endif // DEBUG

    bool m_is_trivial_copyable = false;
    bool m_is_trivial_destructible = false;
    bool m_is_empty_type = false;
    std::size_t m_type_size = std::numeric_limits<std::size_t>::max();
    std::size_t m_align_size = std::numeric_limits<std::size_t>::max();
    TypeDataID m_id = kUint32Max;

    ConstructorFunc m_default_constructor = nullptr;
    DestructorFunc m_destructor = nullptr;
    CopyConstructorFunc m_copy_constructor = nullptr;
    MoveConstructorFunc m_move_constructor = nullptr;
    CopyAsignFunc m_copy_asign = nullptr;
    MoveAsignFunc m_move_asign = nullptr;
};

class TypeInfoHelper {
public:
    /// \brief moveÇ™Ç≈Ç´ÇΩÇÁmove,Ç≈Ç´Ç»Ç©Ç¡ÇΩÇÁcopy,copyÇ‡ñ≥óùÇ»ÇÁfalseÇ≈ï‘Ç∑
    static bool MoveOrCopy(const TypeInfo& info, void* source, void* dest) {
        assert(source != dest);

        if (info.CanTrivialCopy()) {
            // TODO: std::memmoveÇ≈àÍäáÇ≈ìÆÇ©Ç∑
        }
        if (info.MoveAsign(source, dest)) {
            // moveÇ™Ç≈Ç´ÇΩÇÁtrue
            return true;
        }
        if (info.CopyAsign(source, dest)) {
            // copyÇ™Ç≈Ç´ÇΩÇÁtrue
            return true;
        }
        return false;
    }
};