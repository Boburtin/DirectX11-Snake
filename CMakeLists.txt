cmake_minimum_required(VERSION 3.20)

project(SnakeD3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(SnakeD3D WIN32)

target_compile_definitions(SnakeD3D PRIVATE
    UNICODE
    _UNICODE
)

if(MSVC)
    target_link_options(SnakeD3D PRIVATE "/ENTRY:wWinMainCRTStartup")
elseif(MINGW)
    target_compile_options(SnakeD3D PRIVATE "-municode")
    target_link_options(SnakeD3D PRIVATE "-municode")
endif()

target_sources(SnakeD3D PRIVATE
    src/main.cpp
)

target_link_libraries(SnakeD3D PRIVATE
    d3d11
    d3dcompiler
)

