#include <cuda_runtime.h>
#include "kernels.cuh"
#include "domain.h"

__global__ void updatePosVelFirstHalfKernel(const Domain domain, double* pos,
                                                                    double* vel,
                                                                    double* acc){
    int idx = blockDim.x*blockIdx.x + threadIdx.x;
    if (idx >= domain.n_particles_total) return;

    double dt = domain.deltaTime;
    pos[3*idx+0] += vel[3*idx+0]*dt + acc[3*idx+0]*dt*dt*0.5;
    pos[3*idx+1] += vel[3*idx+1]*dt + acc[3*idx+1]*dt*dt*0.5;
    pos[3*idx+2] += vel[3*idx+2]*dt + acc[3*idx+2]*dt*dt*0.5;
    vel[3*idx+0] += acc[3*idx+0]*dt*0.5;
    vel[3*idx+1] += acc[3*idx+1]*dt*0.5;
    vel[3*idx+2] += acc[3*idx+2]*dt*0.5;  

    if (domain.bc == PERIODIC){
            if (pos[3*idx+0]<0.0) pos[3*idx+0] += domain.box_len_x; 
            if (pos[3*idx+0]>=domain.box_len_x) pos[3*idx+0] -= domain.box_len_x; 
            if (pos[3*idx+1]<0.0) pos[3*idx+1] += domain.box_len_y; 
            if (pos[3*idx+1]>=domain.box_len_y) pos[3*idx+1] -= domain.box_len_y;
            if (pos[3*idx+2]<0.0) pos[3*idx+2] += domain.box_len_z; 
            if (pos[3*idx+2]>=domain.box_len_z) pos[3*idx+2] -= domain.box_len_z;
    }
}

__global__ void LennardJonesPotentialVelocitySeconfHalfKernel(const Domain domain,
                                            const int* __restrict__ cells_arr,
                                            const int* __restrict__ neigh_particles_arr,
                                            double* __restrict__ pos,
                                            double* __restrict__ vel,
                                            double* __restrict__ acc,
                                            const double mass){
    const int idx = blockDim.x*blockIdx.x+threadIdx.x;
    if (idx >= domain.n_particles_total) return;
    acc[3*idx+0] = 0.0;
    acc[3*idx+1] = 0.0;
    acc[3*idx+2] = 0.0;
    double Fx = 0.0;
    double Fy = 0.0;
    double Fz = 0.0;
    double sigma_pow_six = domain.sigma*domain.sigma*domain.sigma*domain.sigma*domain.sigma*domain.sigma;

    double px = pos[3*idx+0];
    double py = pos[3*idx+1];
    double pz = pos[3*idx+2];
    int i = (int)(px/domain.rad_cutoff);
    int j = (domain.dimension==2)?(int)(py/domain.rad_cutoff):0;
    int k = (domain.dimension==3)?(int)(pz/domain.rad_cutoff):0;

    int min_dx = -1, max_dx = 1;
    int min_dy = (domain.dimension==2)?-1:0;
    int max_dy = (domain.dimension==2)? 1:0;
    int min_dz = (domain.dimension==3)?-1:0;
    int max_dz = (domain.dimension==3)? 1:0;

    for (int dz=min_dz; dz<max_dz; ++dz){
        for (int dy=min_dy; dy<max_dy; ++dy){
            for (int dx=min_dx; dx<max_dx; ++dx){
                int neigh_i = i+dx;
                int neigh_j = j+dy;
                int neigh_k = k+dz;
                
            }
        }
    }
}


void launchUpdatePosVelFirstHalfKernel(const Domain& domain, ParticleSystem& device_ps,int blocks, int threads){
    updatePosVelFirstHalfKernel<<<blocks,threads>>>(domain,device_ps.pos,device_ps.vel,device_ps.acc);
    checkCudaError(cudaGetLastError());
    checkCudaError(cudaDeviceSynchronize());
}