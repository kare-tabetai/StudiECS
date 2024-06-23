#pragma once
#include <array>
#include <cstddef>

class Chunk {

	template<class CD>
	CD* At(uint32 index) {

	}

  static constexpr size_t kChunkSize = 64 * 1024; // 64KB
  std::array<std::byte, kChunkSize> chunk;
};