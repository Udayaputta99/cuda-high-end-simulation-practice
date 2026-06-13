#include<random>
#include<cmath>
#include "particle_system.h"

void allocateHostMemory(const Domain& domain, ParticleSystem& ps){
    ps.pos = new double [3*domain.n_particles_total];
    ps.vel = new double [3*domain.n_particles_total];
    ps.acc = new double [3*domain.n_particles_total];
}

void freeHostMemory(ParticleSystem& host_ps){
    delete[] host_ps.pos;
    delete[] host_ps.vel;
    delete[] host_ps.acc;
}

void initializeParticleSystem(const Domain& domain, ParticleSystem& host_ps){
    std::mt19937 generator(21);
    const double stddev = std::sqrt(domain.T);
    std::normal_distribution<double> guassianDistr(0.0,stddev);
    
    host_ps.radius = 0.1;
    host_ps.mass = 1.0;
    
    int idx = 0.0;
    for (int k=0; k<domain.n_particles_z; ++k){
        for (int j=0; j<domain.n_particles_y; ++j){
            for (int i=0; i<domain.n_particles_x; ++i){
                host_ps.pos[3*idx+0] = i*domain.delta_x;
                host_ps.pos[3*idx+1] = j*domain.delta_y;
                host_ps.pos[3*idx+2] = k*domain.delta_z;
                
                host_ps.vel[3*idx+0] = guassianDistr(generator);
                host_ps.vel[3*idx+1] = guassianDistr(generator);
                host_ps.vel[3*idx+2] = guassianDistr(generator);

                if (domain.gravityEnabled){
                    host_ps.acc[3*idx+0] = 0.0;
                    host_ps.acc[3*idx+1] = -0.5; //acc in -ve y-axis in reduced units
                    host_ps.acc[3*idx+0] = 0.0;
                }
                else{
                    host_ps.acc[3*idx+0] = 0.0;
                    host_ps.acc[3*idx+1] = 0.0;
                    host_ps.acc[3*idx+0] = 0.0;
                }
                ++idx;
            }
        }
    }
}