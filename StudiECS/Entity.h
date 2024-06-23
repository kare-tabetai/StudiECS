#pragma once
#include "Type.h"
#include <bitset>

class Entity {
public:
    static constexpr Entity Invalid()
    {
        Entity entity;
        //entity.value.index_generation.flags
        //TODO:
        return entity;
    }

    uint64 Get() const{ return value.entity; }
    uint32 Index() const { return value.index_generation.index; }

    /// \brief 無効なentityかどうか
    bool IsInvalid() const {

    }

    bool operator==(const Entity& other) const
    {
        return value.entity == other.value.entity;
    }

private:
    enum class Flag : uint16 {
        None = 0,
        Invalid = 1 << 0,
    };

    struct IndexGeneration {
        Flag flags = Flag::None;
        uint16 generation = 0;
        ChunkContainerIndex index = 0;
    };
    /// \brief entityが削除されていた場合はChunkContainer内で削除済みリストの要素として扱う
    /// \note https://skypjack.github.io/2019-05-06-ecs-baf-part-3/
    struct DeletedElement {
        Flag flags = Flag::None;
        uint16 generation = 0;
        ChunkContainerIndex next_deleted_index = 0;
    };
    union Impl {
        uint64 entity = 0;
        IndexGeneration index_generation;
        DeletedElement deleted_element;
    };

    Impl value = {};
};
static_assert(sizeof(Entity) == 8);

namespace std {
template<>
struct hash<Entity> {
    std::size_t operator()(const Entity& entity) const
    {
        // シンプルなハッシュ関数
        return std::hash<uint64>()(entity.Get());
    }
};
}