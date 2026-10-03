cmake_minimum_required(VERSION 3.20)

project(SnakeD3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(SnakeD3D WIN32)

target_compile_definitions(SnakeD3D PRIVATE
    UNICODE
    _UNICODE
)

if(MINGW)
    target_link_options(SnakeD3D PRIVATE "-municode")
    find_package(directxmath CONFIG REQUIRED)
    target_link_libraries(SnakeD3D PRIVATE Microsoft::DirectXMath)
endif()

target_sources(SnakeD3D PRIVATE
    src/winmain.cpp
)

target_link_libraries(SnakeD3D PRIVATE
    d3d11
    d3dcompiler
)

