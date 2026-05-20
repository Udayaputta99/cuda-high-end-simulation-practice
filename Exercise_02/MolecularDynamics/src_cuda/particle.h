#pragma once 
#include <curand.h>
#include <curand_kernel.h>
#include <cuda_runtime.h>


__device__ void gaussianVelocityDistribution(const Domain domain, double* __restrict__ velocity){
    const double mass = 1.0;
    const int ti = blockDim.x*blockIdx.x+threadIdx.x;
    const int tj = blockDim.y*blockIdx.y+threadIdx.y;
    const int tk = blockDim.z*blockIdx.z+threadIdx.z;
    const int idx = ti + domain.n_particles_x*tj + domain.n_particles_x*domain.n_particles_x*tk;
    if (idx < domain.n_particles_total){
        const double stddev = (double)sqrt(domain.T);
        curandState state;
        curand_init(12345ULL,idx,0,&state);
        velocity[3*idx+0] = curand_normal_double(&state)*stddev;
        velocity[3*idx+1] = curand_normal_double(&state)*stddev;
        velocity[3*idx+2] = curand_normal_double(&state)*stddev;
    }

}

__global__ void generateParticles(const Domain domain, double* __restrict__ position, 
                                                 double* __restrict__ velocity,
                                                 double* __restrict__ acceleration){
    
    const int ti = blockDim.x*blockIdx.x+threadIdx.x;
    const int tj = blockDim.y*blockIdx.y+threadIdx.y;
    const int tk = blockDim.z*blockIdx.z+threadIdx.z;
    const int idx = ti + domain.n_particles_x*tj + domain.n_particles_x*domain.n_particles_x*tk;
    if (ti<domain.n_particles_x && tj<domain.n_particles_x && tk<domain.n_particles_x ){
        // printf("%d,%d,%d\n",ti,tj,tk);
        position[3*idx+0] = ti*domain.delta_x;
        position[3*idx+1] = tj*domain.delta_x; 
        position[3*idx+2] = tk*domain.delta_x; 
        // velocity[3*idx+0] = 1.0;
        // velocity[3*idx+1] = 1.0; 
        // velocity[3*idx+2] = 1.0; 
        gaussianVelocityDistribution(domain,velocity);
        // velocity[3*idx+0] = (idx==0)?1:0;
        // velocity[3*idx+1] = (idx==0)?1:0; 
        // velocity[3*idx+2] = (idx==0)?1:0; 

        acceleration[3*idx+0] = 0.0;
        acceleration[3*idx+1] = 0.0; 
        acceleration[3*idx+2] = 0.0; 
    }
}

__global__ void updatePositionVelocityFirstHalf(const Domain domain, double* __restrict__ position,
                                                                    double* __restrict__ velocity,
                                                                    double* __restrict__ acceleration
                                                                    ){
    const int ti = blockDim.x*blockIdx.x+threadIdx.x;
    const int tj = blockDim.y*blockIdx.y+threadIdx.y;
    const int tk = blockDim.z*blockIdx.z+threadIdx.z; 

    double deltaTime = domain.deltaTime;
    if (ti<domain.n_particles_x && tj<domain.n_particles_x && tk<domain.n_particles_x ){
        int idx = ti + tj*domain.n_particles_x + tk*domain.n_particles_x*domain.n_particles_x;

        position[3*idx+0] += velocity[3*idx+0]*deltaTime+
                            acceleration[3*idx+0]*deltaTime*deltaTime*0.5;
        position[3*idx+1] += velocity[3*idx+1]*deltaTime+
                            acceleration[3*idx+1]*deltaTime*deltaTime*0.5;
        position[3*idx+2] += velocity[3*idx+2]*deltaTime+
                            acceleration[3*idx+2]*deltaTime*deltaTime*0.5;
        velocity[3*idx+0] += acceleration[3*idx+0]*deltaTime*0.5;
        velocity[3*idx+1] += acceleration[3*idx+1]*deltaTime*0.5;
        velocity[3*idx+2] += acceleration[3*idx+2]*deltaTime*0.5;
    }
}
