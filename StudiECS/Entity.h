#pragma once
#include "Type.h"
#include "Constant.h"
#include "EnumUtil.h"
#include <cassert>

class Entity {
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
        RecordIndex record_index;
        Generation generation;
        Flag flag;
        WorldNumber world_number;
    };

    Entity() = default;

    constexpr Entity(WorldNumber world_number, Flag flag)
        : impl(0,0,flag,world_number)
    {
    }

    explicit operator uint64() const
    {
        return id;
    }

    static constexpr Entity Invalid()
    {
        return Entity(0, Flag::Invalid);
    }

    void ReSet(RecordIndex record_index,Generation generation, WorldNumber world_number)
    {
        impl = Impl(record_index, generation, Flag::None, world_number);
    }

    void Release()
    {
        AddFlag(Flag::Invalid);
    }

    /// \brief ������entity���ǂ���
    bool IsInvalid() const
    {
        return ToBool(impl.flag | Flag::Invalid);
    }

    Entity& SetFlag(Flag flag)
    {
        impl.flag = flag;
        return *this;
    }
    Entity& AddFlag(Flag flag)
    {
        impl.flag |= flag;
        return *this;
    }
    Entity& RemoveFlag(Flag flag)
    {
        impl.flag &= ~flag;
        return *this;
    }

    RecordIndex GetRecordIndex() const
    {
        return impl.record_index;
    }

    Generation GetGeneration()const
    {
        return impl.generation;
    }

    auto operator<=>(const Entity&) const = default;

    union {
        Impl impl;
        uint64 id;
    };
};

namespace std {
template<>
struct hash<Entity> {
    std::size_t operator()(const Entity& entity) const
    {
        return std::hash<uint64>()(static_cast<uint64>(entity));
    }
};
}