#pragma once
#include "Constant.h"
#include "Type.h"

/// \brief ChunkのindexとChunk内のEntityやCDがあるindexを示す構造体
class EntityIndex {
public:
    EntityIndex() = default;
    constexpr EntityIndex(ChunkIndex _chunk_index, LocalIndex _local_index)
        : chunk_index(_chunk_index)
        , local_index(_local_index)
    {
    }

    constexpr bool IsLastChunk(size_t chunk_size) const
    {
        return chunk_index + 1 == chunk_size;
    }

    constexpr bool IsLastLocalIndex(uint32 local_index_size) const
    {
        return local_index + 1 == local_index_size;
    }

    constexpr bool IsLast(size_t chunk_size, uint32 local_index_size) const
    {
        return IsLastChunk(chunk_size) && IsLastLocalIndex(local_index_size);
    }

    constexpr bool InRange(size_t chunk_size, uint32 local_index_size, uint32 last_local_index_size) const
    {
        if (!IsLastChunk(chunk_size)) {
            return local_index < local_index_size;
        } else {
            return local_index < last_local_index_size;
        }
    }

    /// \brief indexを加算
    /// \ret_val chunkのサイズを超えた場合はtrue
    bool Increment(size_t chunk_size, uint32 local_index_size)
    {
        if (!IsLastLocalIndex(local_index_size)) {
            local_index++;
            return false;
        } else {
            if (!IsLastChunk(chunk_size)) {
                chunk_index++;
                local_index = 0;
                return false;
            } else {
                chunk_index++;
                local_index = 0;
                return true;
            }
        }
    }

    EntityIndex GetIncremented(size_t chunk_size, uint32 local_index_size) const
    {
        EntityIndex tmp = *this;
        tmp.Increment(chunk_size, local_index_size);
        return tmp;
    }

    ChunkIndex GetChunkIndex() const { return chunk_index; }
    LocalIndex GetLocalIndex() const { return local_index; }

private:
    ChunkIndex chunk_index = kUint8Max;
    LocalIndex local_index = kUint32Max;
};

constexpr EntityIndex kBeginEntityIndex = EntityIndex(0, 0);
