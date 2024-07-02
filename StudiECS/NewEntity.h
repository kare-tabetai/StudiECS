#pragma once
#include "Type.h"
#include "Constant.h"
#include "EnumUtil.h"
#include <cassert>



class NewEntity {
public:
    enum class Flag : uint8 {
        None = 0,
        Invalid = 1 << 0,
        IsInChunk = 1 << 1,
    };

    struct Impl {
        uint32 index = kUint32Max;
        uint16 generation = kUint16Max;
        Flag flag = Flag::None;
        uint8 world_number = kUint8Max;
    };

    NewEntity() = default;

    constexpr NewEntity(uint32 index,uint8 world_number, Flag flag)
    {
        impl.index = index;
        impl.generation = 0;
        impl.flag = flag;
        impl.world_number = world_number;
    }

    explicit operator uint64() const
    {
        return id;
    }

    static constexpr NewEntity Invalid()
    {
        return NewEntity(0, 0, Flag::Invalid);
    }

    void Release()
    {
        AddFlag(Flag::Invalid);
        impl.generation++;
    }

    /// \brief –³Œø‚Èentity‚©‚Ç‚¤‚©
    bool IsInvalid() const
    {
        return ToBool(impl.flag | Flag::Invalid);
    }

    NewEntity& SetFlag(Flag flag)
    {
        impl.flag = flag;
        return *this;
    }
    NewEntity& AddFlag(Flag flag)
    {
        impl.flag |= flag;
        return *this;
    }
    NewEntity& RemoveFlag(Flag flag)
    {
        impl.flag &= ~flag;
        return *this;
    }


    bool IncrementGeneration()
    {
        if (impl.generation == kUint16Max) [[unlikely]] {
            assert(false);
            impl.generation = 0;
            return false;
        }

        impl.generation++;
        return true;
    }

    auto operator<=>(const NewEntity&) const = default;

    private:
    union   {
        Impl impl = {};
        uint64 id ;
    };
};

namespace std {
template<>
struct hash<NewEntity> {
    std::size_t operator()(const NewEntity& entity) const
    {
        return std::hash<uint64>()(static_cast<uint64>(entity));
    }
};
}