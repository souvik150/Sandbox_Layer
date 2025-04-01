cmake_minimum_required(VERSION 3.10)
project(WASI_Sandbox_Sample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(wasi_sandbox
    src/main.cpp
    src/sandbox.cpp
    src/wasi_fs_layer.cpp
)
