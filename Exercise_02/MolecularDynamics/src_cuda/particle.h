#pragma once 
#include <cuda_runtime.h>

// struct ParticleSystem{

//     double *p_mass; //in grams 
//     double *p_radius; // in mm
//     double *p_position_x,*p_position_y,*p_position_z; // in mm
//     double *p_velocity_x,*p_velocity_y,*p_velocity_z; // in mm/s
//     double *p_acceleration_x,*p_acceleration_y,*p_acceleration_z; // in mm/s^2
        
// };

// Particle* generateParticles(const Domain& domain){
//     const double velMin = -0.001;
//     double velMax = 0.01;
//     const double mass = 2.0;
//     const double radius = 0.5;
    
//     Particle* particle_arr;

//     for(int pk=0; pk<domain.n_particles_z; ++pk){
//         for(int pj=0; pj<domain.n_particles_y; ++pj){
//             for(int pi=0; pi<domain.n_particles_x; ++pi){
//                 // std::vector<double> vel (3,0);
//                 double* vel = [0.0,0.0,0.0];
//                 double* acc = [0.0,0.0,0.0];
//                 double* pos = [((double)pi*domain.delta_x),
//                                          ((double)pj*domain.delta_y),
//                                          ((double)pk*domain.delta_z)];
//                 Particle p {mass,radius,pos,vel,acc};
//                 particle_arr.push_back(p);
//             }
//         }
//     }
//     return particle_arr;
// }


__device__ void updatePositionVelocityFirstHalf(const Domain domain, double* __restrict__ position,
                                                                    double* __restrict__ velocity,
                                                                    double* __restrict__ acceleration,
                                                                const int ti, const int tj, const int tk){
    double deltaTime = domain.deltaTime;
    position[3*ti+0] += velocity[3*ti+0]*deltaTime+
                        acceleration[3*ti+0]*deltaTime*deltaTime*0.5;
    position[3*tj+1] += velocity[3*tj+1]*deltaTime+
                        acceleration[3*tj+1]*deltaTime*deltaTime*0.5;
    position[3*tk+2] += velocity[3*tk+2]*deltaTime+
                        acceleration[3*tk+2]*deltaTime*deltaTime*0.5;
    velocity[3*ti+0] += acceleration[3*ti+0]*deltaTime*0.5;
    velocity[3*tj+1] += acceleration[3*tj+1]*deltaTime*0.5;
    velocity[3*tk+2] += acceleration[3*tk+2]*deltaTime*0.5;

}

__device__ void updateVelocitySecondHalf(const Domain domain,double* __restrict__ velocity,
                                                            double* __restrict__ acceleration,
                                                            const int ti, const int tj, const int tk){
    double deltaTime = domain.deltaTime;
    velocity[3*ti+0] += acceleration[3*ti+0]*deltaTime*0.5;
    velocity[3*tj+1] += acceleration[3*tj+1]*deltaTime*0.5;
    velocity[3*tk+2] += acceleration[3*tk+2]*deltaTime*0.5;

}