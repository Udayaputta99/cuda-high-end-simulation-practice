cmake_minimum_required(VERSION 3.18)
project(ParticleSolver VERSION 1.0 LANGUAGES CXX CUDA) # Enable CUDA as a first-class language

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES 86) 

# Add all files to the executable target
add_executable(particlesim
    src/main.cpp
    src/particle_system.cpp
    src/IO.cpp
    src/domain_init.cpp
    src/cuda/integrator.cu
    src/cuda/particle_system.cu
)
target_compile_options(particlesim PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-march=native>
)
# Tell CMake where to look for your .h and .cuh files
target_include_directories(particlesim PRIVATE include)