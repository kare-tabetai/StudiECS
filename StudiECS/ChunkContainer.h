#pragma once
#include"Chunk.h"
#include<memory>
#include<vector>

class ChunkContainer {
    std::vector<std::unique_ptr<Chunk>> chunks;
};