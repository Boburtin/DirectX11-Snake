#pragma once

inline constexpr auto ROWS = 32ull;

inline constexpr auto COLS = 32ull;

inline constexpr auto TILES = ROWS * COLS;

inline constexpr auto PX_PER_TILE = 20ull;

inline constexpr auto WINDOW_WIDTH = COLS * PX_PER_TILE;

inline constexpr auto WINDOW_HEIGHT = ROWS * PX_PER_TILE;

inline constexpr auto HALF_PX = PX_PER_TILE / 2;
