#pragma once

typedef uint32_t Str;


inline constexpr Str hash(const char* str) {
    uint32_t hash = 2166136261u; 
    while (*str) {
        hash ^= (uint32_t)(*str++);
        hash *= 16777619u; 
    }
    return hash;
}