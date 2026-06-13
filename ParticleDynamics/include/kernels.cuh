#pragma once
#include <iostream>
#include "particle_system.h"
#include "domain.h"

#define checkCudaError(ans) check((ans),__FILE__,__LINE__)
inline void check(cudaError_t err, const char *file, int line){
    if (err != cudaSuccess){
        std::cerr<<"CUDA Runtime error at: "<<file<<":"<<line<<std::endl;
        std::cerr<<cudaGetErrorString(err)<<" "<<file<<std::endl;
    }
}

void launchUpdatePosVelFirstHalfKernel(const Domain& domain, ParticleSystem& device_ps,int blocks, int threads);