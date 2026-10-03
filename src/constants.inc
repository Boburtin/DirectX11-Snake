#ifndef CONSTANTS_INC
#define CONSTANTS_INC

inline constexpr unsigned long long ROWS = 32ull;

inline constexpr unsigned long long COLS = 32ull;

inline constexpr unsigned long long TILES = ROWS * COLS;

inline constexpr unsigned long long PX_PER_TILE = 20ull;

inline constexpr unsigned long long WINDOW_WIDTH = COLS * PX_PER_TILE;

inline constexpr unsigned long long WINDOW_HEIGHT = ROWS * PX_PER_TILE;

inline constexpr unsigned long long HALF_PX = PX_PER_TILE / 2;

#endif
