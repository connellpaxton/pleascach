#pragma once

#include <stdint.h>

typedef uint8_t  u8;
typedef  int8_t  i8;
typedef uint16_t u16;
typedef  int16_t i16;
typedef uint32_t u32;
typedef  int32_t i32;
typedef uint64_t u64;
typedef  int64_t i64;

static inline u8 operator ""_u8(unsigned long long u) {
    return static_cast<u8>(u);
}

static inline i8 operator ""_i8(unsigned long long u) {
    return static_cast<i8>(u);
}

static inline u16 operator ""_u16(unsigned long long u) {
    return static_cast<u16>(u);
}

static inline i16 operator ""_i16(unsigned long long u) {
    return static_cast<i16>(u);
}

static inline u32 operator ""_u32(unsigned long long u) {
    return static_cast<u32>(u);
}

static inline i32 operator ""_i32(unsigned long long u) {
    return static_cast<i32>(u);
}

static inline u64 operator ""_u64(unsigned long long u) {
    return static_cast<u64>(u);
}

static inline i64 operator ""_i64(unsigned long long u) {
    return static_cast<i64>(u);
}
