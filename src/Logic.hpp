#pragma once
// my headers
#include "ring_buf.hpp"
// std library
#include <bitset>
#include <cstddef>

template <typename T, std::size_t N> struct Snake
{
    ring_buf<T, N> board{};
    Snake() = default;
};

typedef std::bitset<256> KeySet, *PKeySet;

template <typename T, std::size_t N> struct GameContext
{
    Snake<T, N> snake{};
    KeySet keys{};
    GameContext() = default;
};
