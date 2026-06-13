#include <particle_system.h>
#include "cudaError.h"
__global__ void energyKernel(const double mass, 
                             const double* __restrict__ vel, 
                             double* __restrict__ total_ke, 
                             const int n_particles_total){

    int idx = blockDim.x*blockIdx.x + threadIdx.x;
    if (idx >= n_particles_total) return;
    // Extract velocity components
    double vx = vel[3 * idx + 0];
    double vy = vel[3 * idx + 1];
    double vz = vel[3 * idx + 2];

    // Calculate individual kinetic energy: 0.5 * m * v^2
    double v_sqrd = vx*vx + vy*vy + vz*vz;
    double particle_ke = 0.5 * mass * v_sqrd;

    // Atomically add this particle's KE to the total system KE accumulator
    atomicAdd(total_ke, particle_ke);

}


void launchEnergyKernel(ParticleSystem& dev_ps, int blocks, int threads, Domain& domain){
    cudaMemset(domain.total_ke, 0, sizeof(double));
    energyKernel<<<blocks,threads>>>(dev_ps.mass, dev_ps.vel, domain.total_ke, domain.n_particles_total);
    checkCudaError(cudaGetLastError());
    checkCudaError(cudaDeviceSynchronize());
}