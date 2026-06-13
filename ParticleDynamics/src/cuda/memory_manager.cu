#include <cuda_runtime.h>
#include "domain.h"
#include "particle_system.h"
#include "kernels.cuh"

void allocateDeviceMemory(const Domain& domain, ParticleSystem& dev_ps){
    // ps.n_particles = domain.n_particles_total;
    cudaMalloc(&dev_ps.pos, 3*domain.n_particles_total*sizeof(double));
    cudaMalloc(&dev_ps.vel, 3*domain.n_particles_total*sizeof(double));
    cudaMalloc(&dev_ps.acc, 3*domain.n_particles_total*sizeof(double));
    checkCudaError(cudaGetLastError());
}

//overloaded function for allocating device memory for cells and particles arr 
void allocateDeviceMemory(const Domain& domain, NeighbourList& nl){
    cudaMalloc(&nl.cells_arr, domain.n_cells_total*sizeof(int));
    cudaMalloc(&nl.particles_arr, domain.n_particles_total*sizeof(int));
    checkCudaError(cudaGetLastError());
}   

void freeDeviceMemory(ParticleSystem& dev_ps){
    cudaFree(dev_ps.pos);
    cudaFree(dev_ps.vel);
    cudaFree(dev_ps.acc);
    checkCudaError(cudaGetLastError());
    dev_ps.pos = nullptr;
    dev_ps.vel = nullptr;
    dev_ps.acc = nullptr;    
}

void freeDeviceMemory(NeighbourList& nl){
    cudaFree(nl.cells_arr);
    cudaFree(nl.particles_arr);
    checkCudaError(cudaGetLastError());
    nl.cells_arr = nullptr;
    nl.particles_arr = nullptr;
}

void copyHostToDevice(const ParticleSystem& host_ps, ParticleSystem& dev_ps, const Domain& domain){
    cudaMemcpy(dev_ps.pos,host_ps.pos,3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice);
    checkCudaError(cudaGetLastError());
}

void copyDeviceToHost(const ParticleSystem& host_ps, ParticleSystem& dev_ps, const Domain& domain){
    cudaMemcpy(host_ps.pos,dev_ps.pos,3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost);
    checkCudaError(cudaGetLastError());
}