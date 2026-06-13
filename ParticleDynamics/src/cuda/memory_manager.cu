#include <cuda_runtime.h>
#include "domain.h"
#include "particle_system.h"
#include "cudaError.h"
#include "neigh_list.h"

void allocateDeviceMemory(const Domain& domain, ParticleSystem& dev_ps){
    // ps.n_particles = domain.n_particles_total;
    cudaMalloc(&dev_ps.pos, 3*domain.n_particles_total*sizeof(double));
    cudaMalloc(&dev_ps.vel, 3*domain.n_particles_total*sizeof(double));
    cudaMalloc(&dev_ps.acc, 3*domain.n_particles_total*sizeof(double));
    checkCudaError(cudaGetLastError());
}

//overloaded function for allocating device memory for cells and particles arr 
void allocateDeviceMemory(const Domain& domain, NeighbourList& dev_nl){
    cudaMalloc(&dev_nl.cells_arr, domain.n_cells_total*sizeof(int));
    cudaMalloc(&dev_nl.particles_arr, domain.n_particles_total*sizeof(int));
    checkCudaError(cudaGetLastError());
}   

//overloaded function for allocating device memory for tot. kinetic energy
void allocateDeviceMemory(Domain& domain){
    cudaMalloc(&domain.total_ke, sizeof(double));
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

void freeDeviceMemory(NeighbourList& dev_nl){
    cudaFree(dev_nl.cells_arr);
    cudaFree(dev_nl.particles_arr);
    checkCudaError(cudaGetLastError());
    dev_nl.cells_arr = nullptr;
    dev_nl.particles_arr = nullptr;
}

void freeDeviceMemory(Domain& domain){
    cudaFree(domain.total_ke);
    checkCudaError(cudaGetLastError());
    domain.total_ke = nullptr;
}

void copyHostToDevice(ParticleSystem& dev_ps, ParticleSystem& host_ps,  const Domain& domain){
    cudaMemcpy(dev_ps.pos, host_ps.pos, 3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice);
    checkCudaError(cudaGetLastError());
}

void copyDeviceToHost(ParticleSystem& host_ps, ParticleSystem& dev_ps, const Domain& domain){
    cudaMemcpy(host_ps.pos, dev_ps.pos, 3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost);
    checkCudaError(cudaGetLastError());
}

// void copyHostToDevice(const Domain& host_domain, Domain& domain){
//     cudaMemcpy(dev_domain.total_ke, host_domain.total_ke, sizeof(double), cudaMemcpyHostToDevice);
//     checkCudaError(cudaGetLastError());
// }

// void copyDeviceToHost(Domain& host_domain, const Domain& dev_domain){
//     cudaMemcpy(host_domain.total_ke, dev_domain.total_ke, sizeof(double), cudaMemcpyDeviceToHost);
//     checkCudaError(cudaGetLastError());
// }

void synchronizeKernels(){
    checkCudaError(cudaDeviceSynchronize());
}