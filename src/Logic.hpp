#pragma once
// my headers
#include "ring_buf.hpp"
// std library
#include <bitset>
#include <cstddef>
#include <cstdint>

typedef std::bitset<256> KeySet, *PKeySet;

template <typename T, std::size_t N> struct Snake
{
    ring_buf<T, N> board{};

    Snake() = default;
};

template <typename T, std::size_t N> struct GameContext
{
    Snake<T, N> snake{};
    KeySet keys{};

    GameContext() = default;
};

struct XorShift64
{
    std::uint64_t seed;

    // simple 2 shift has a period of 2^64 - 1, very nice
    void shift()
    {
        seed ^= seed << 7;
        seed ^= seed >> 9;
    }

    std::uint64_t next()
    {
        shift();
        return seed;
    }

    XorShift64() : seed(__rdtsc() | 1)
    {
        shift();
    }
};
