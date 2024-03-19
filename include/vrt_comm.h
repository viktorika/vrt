#pragma once

#include <cstddef>

namespace vrt {

#ifdef __GNUC__
#  define likely(x) __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define likely(x) (x)
#  define unlikely(x) (x)
#endif

constexpr size_t kNodeChildMaxCnt = 256;
constexpr size_t kMaxKeySize = 1 << 23;

constexpr uint8_t kFree = 0;
constexpr uint8_t kLocked = 1;

}