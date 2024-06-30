#pragma once
#include "Type.h"

constexpr uint8 kUint8Max = std::numeric_limits<uint8>::max();
constexpr uint16 kUint16Max = std::numeric_limits<uint16>::max();
constexpr uint32 kUint32Max = std::numeric_limits<uint32>::max();
constexpr uint64 kUint64Max = std::numeric_limits<uint64>::max();

constexpr std::byte kByteMax = static_cast<std::byte>(std::numeric_limits<ToUnderlying<std::byte>>::max());

constexpr size_t kChunkSize = 64 * 1024; // 64KB
