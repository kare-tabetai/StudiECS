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
    };

    struct Impl {
        constexpr Impl(uint32 _record_index, uint16 _generation, Flag _flag, WorldNumber _world_number)
            : record_index(_record_index)
            , generation(_generation)
            , flag(_flag)
            , world_number(_world_number)
        {
        }
        uint32 record_index;
        uint16 generation;
        Flag flag;
        WorldNumber world_number;
    };

    struct ReleasedData {
        constexpr ReleasedData(uint32 _next_released_index, uint16 _current_generation, Flag _flag, ChunkIndex _chunk_index)
            : next_released_index(_next_released_index)
            , current_generation(_current_generation)
            , flag(_flag)
            , next_released_chunk_index(_chunk_index)
        {
        }
        uint32 next_released_index;
        uint16 current_generation;
        Flag flag;
        ChunkIndex next_released_chunk_index;
    };

    NewEntity() = default;

    constexpr NewEntity(WorldNumber world_number, Flag flag)
        : impl(0,0,flag,world_number)
    {
    }

    constexpr NewEntity(uint32 next_released_index, ChunkIndex chunk_index, Flag flag)
        : relesed_data(next_released_index,0,flag,chunk_index)
    {
    }

    explicit operator uint64() const
    {
        return id;
    }

    static constexpr NewEntity Invalid()
    {
        return NewEntity(0, Flag::Invalid);
    }

    void ReUse(uint32 record_index,WorldNumber world_number)
    {
        impl = Impl(record_index, impl.generation, Flag::None, world_number);
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

    uint32 GetRecordIndex() const
    {
        return impl.record_index;
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

    union {
        Impl impl;
        ReleasedData relesed_data;
        uint64 id;
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