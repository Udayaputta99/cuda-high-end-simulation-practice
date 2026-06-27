cmake_minimum_required(VERSION 3.18)
project(ParticleSolver VERSION 1.0 LANGUAGES CXX CUDA) # Enable CUDA as a first-class language

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CUDA_STANDARD 17)

# Set the CUDA architecture to sm_86 (A40, Ampere cards)
set(CMAKE_CUDA_ARCHITECTURES 86) 

# Add all files to the executable target
add_executable(lbm
    src/lbm.cpp
)

# Tell the host compiler (g++) to use -march=native
target_compile_options(lbm PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-march=native>
)

# Tell CMake where to look for your .h and .cuh files
target_include_directories(lbm PRIVATE include)