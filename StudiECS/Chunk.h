#pragma once
#include <array>
#include <cstddef>

class Chunk {
  static constexpr size_t kChunkSize = 64 * 1024; // 64KB
  std::array<std::byte, kChunkSize> chunk;
};