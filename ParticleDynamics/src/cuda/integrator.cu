#include <cuda_runtime.h>
#include "domain.h"
#include "particle_system.h"
#include "neigh_list.h"
#include "cudaError.h"

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
        if (pos[3*idx+0]<0.0) pos[3*idx+0] += domain.dom_len_x; 
        else if (pos[3*idx+0]>=domain.dom_len_x) pos[3*idx+0] -= domain.dom_len_x; 

        if (pos[3*idx+1]<0.0) pos[3*idx+1] += domain.dom_len_y; 
        else if (pos[3*idx+1]>=domain.dom_len_y) pos[3*idx+1] -= domain.dom_len_y;

        if (pos[3*idx+2]<0.0) pos[3*idx+2] += domain.dom_len_z; 
        else if (pos[3*idx+2]>=domain.dom_len_z) pos[3*idx+2] -= domain.dom_len_z;
    }
    else if (domain.bc == REFLECTIVE) {

        if (pos[3*idx+0] < 0.0) { pos[3*idx+0] = -pos[3*idx+0]; vel[3*idx+0] = -vel[3*idx+0]; }
        else if (pos[3*idx+0] >= domain.dom_len_x) { pos[3*idx+0] = 2.0 * domain.dom_len_x - pos[3*idx+0]; vel[3*idx+0] = -vel[3*idx+0]; }

        if (pos[3*idx+1] < 0.0) { pos[3*idx+1] = -pos[3*idx+1]; vel[3*idx+1] = -vel[3*idx+1]; } 
        else if (pos[3*idx+1] >= domain.dom_len_y) { pos[3*idx+1] = 2.0 * domain.dom_len_y - pos[3*idx+1]; vel[3*idx+1] = -vel[3*idx+1]; }

        if (pos[3*idx+2] < 0.0) { pos[3*idx+2] = -pos[3*idx+2]; vel[3*idx+2] = -vel[3*idx+2]; } 
        else if (pos[3*idx+2] >= domain.dom_len_z) { pos[3*idx+2] = 2.0 * domain.dom_len_z - pos[3*idx+2]; vel[3*idx+2] = -vel[3*idx+2]; }
    }
}

__global__ void LJPotentialVelocitySeconfHalfKernel(const Domain domain,
                                            const int* __restrict__ cells_arr,
                                            const int* __restrict__ particles_arr,
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
    int j = (domain.dimension>=2)?(int)(py/domain.rad_cutoff):0;
    int k = (domain.dimension>=3)?(int)(pz/domain.rad_cutoff):0;

    int min_dx = -1, max_dx = 1;
    int min_dy = (domain.dimension>=2)?-1:0;
    int max_dy = (domain.dimension>=2)? 1:0;
    int min_dz = (domain.dimension>=3)?-1:0;
    int max_dz = (domain.dimension>=3)? 1:0;

    for (int dz=min_dz; dz<=max_dz; ++dz){
        for (int dy=min_dy; dy<=max_dy; ++dy){
            for (int dx=min_dx; dx<=max_dx; ++dx){
                int neigh_i = i+dx;
                int neigh_j = j+dy;
                int neigh_k = k+dz;

                if (neigh_i < 0 || neigh_i >= domain.n_cells_x ||
                    neigh_j < 0 || neigh_j >= domain.n_cells_y ||
                    neigh_k < 0 || neigh_k >= domain.n_cells_z) continue;

                int neigh_cell_idx = neigh_i + domain.n_cells_x*neigh_j + domain.n_cells_x*domain.n_cells_y*neigh_k;
                int neigh_particle_idx = cells_arr[neigh_cell_idx];
                while (neigh_particle_idx!=-1){
                    if (neigh_particle_idx != idx){
                        double dx_pos = px - pos[3*neigh_particle_idx+0];
                        double dy_pos = py - pos[3*neigh_particle_idx+1];
                        double dz_pos = pz - pos[3*neigh_particle_idx+2];
                        if (domain.bc == PERIODIC){
                            if (dx_pos>domain.dom_len_x*0.5) dx_pos -= domain.dom_len_x;
                            else if (dx_pos < -domain.dom_len_x*0.5) dx_pos += domain.dom_len_x;
                            if (dy_pos>domain.dom_len_y*0.5) dy_pos -= domain.dom_len_y;
                            else if (dy_pos < -domain.dom_len_y*0.5) dy_pos += domain.dom_len_y;
                            if (dz_pos>domain.dom_len_z*0.5) dz_pos -= domain.dom_len_z;
                            else if (dz_pos < -domain.dom_len_z*0.5) dz_pos += domain.dom_len_z;
                        }
                        double xij_sqrd = dx_pos*dx_pos + dy_pos*dy_pos + dz_pos*dz_pos; //distance b/w particles sqrd
                        if (xij_sqrd<domain.rad_cutoff*domain.rad_cutoff){//if the other particle is in sphere of influence proceed
                            double sigma_xij_powsix = sigma_pow_six/(xij_sqrd*xij_sqrd*xij_sqrd); //(sigma/xij)^6
                            double multiplier = 24.0*domain.epsilon*sigma_xij_powsix*(2.0*sigma_xij_powsix-1)*(1.0/xij_sqrd);
                            Fx += multiplier*dx_pos;
                            if (domain.dimension>=2) Fy += multiplier*dy_pos;
                            if (domain.dimension>=3) Fz += multiplier*dz_pos;
                        }
                    }
                    neigh_particle_idx = particles_arr[neigh_particle_idx];
                }
            }
        }
    }
    if (domain.bc == REFLECTIVE) {
        double rc2 = domain.rad_cutoff * domain.rad_cutoff;
        double sigma2 = domain.sigma * domain.sigma;

        // --- X-AXIS WALLS ---
        // Left Wall
        double dist_left_x = px;
        if (dist_left_x < domain.rad_cutoff) {
            double r2 = (2.0 * dist_left_x) * (2.0 * dist_left_x);
            if (r2 < rc2 && r2 > 1e-12) {
                double inv_r2 = 1.0 / r2;
                double s6 = (sigma2 * inv_r2) * (sigma2 * inv_r2) * (sigma2 * inv_r2);
                Fx += (24.0 * domain.epsilon * inv_r2) * s6 * (2.0 * s6 - 1.0) * (2.0 * dist_left_x);
            }
        }
        // Right Wall
        double dist_right_x = domain.dom_len_x - px;
        if (dist_right_x < domain.rad_cutoff) {
            double r2 = (2.0 * dist_right_x) * (2.0 * dist_right_x);
            if (r2 < rc2 && r2 > 1e-12) {
                double inv_r2 = 1.0 / r2;
                double s6 = (sigma2 * inv_r2) * (sigma2 * inv_r2) * (sigma2 * inv_r2);
                Fx -= (24.0 * domain.epsilon * inv_r2) * s6 * (2.0 * s6 - 1.0) * (2.0 * dist_right_x);
            }
        }

        // --- Y-AXIS WALLS ---
        if (domain.dimension >= 2) {
            // Bottom Wall
            double dist_bottom_y = py;
            if (dist_bottom_y < domain.rad_cutoff) {
                double r2 = (2.0 * dist_bottom_y) * (2.0 * dist_bottom_y);
                if (r2 < rc2 && r2 > 1e-12) {
                    double inv_r2 = 1.0 / r2;
                    double s6 = (sigma2 * inv_r2) * (sigma2 * inv_r2) * (sigma2 * inv_r2);
                    Fy += (24.0 * domain.epsilon * inv_r2) * s6 * (2.0 * s6 - 1.0) * (2.0 * dist_bottom_y);
                }
            }
            // Top Wall
            double dist_top_y = domain.dom_len_y - py;
            if (dist_top_y < domain.rad_cutoff) {
                double r2 = (2.0 * dist_top_y) * (2.0 * dist_top_y);
                if (r2 < rc2 && r2 > 1e-12) {
                    double inv_r2 = 1.0 / r2;
                    double s6 = (sigma2 * inv_r2) * (sigma2 * inv_r2) * (sigma2 * inv_r2);
                    Fy -= (24.0 * domain.epsilon * inv_r2) * s6 * (2.0 * s6 - 1.0) * (2.0 * dist_top_y);
                }
            }
        }

        // --- Z-AXIS WALLS ---
        if (domain.dimension == 3) {
            // Back Wall
            double dist_back_z = pz;
            if (dist_back_z < domain.rad_cutoff) {
                double r2 = (2.0 * dist_back_z) * (2.0 * dist_back_z);
                if (r2 < rc2 && r2 > 1e-12) {
                    double inv_r2 = 1.0 / r2;
                    double s6 = (sigma2 * inv_r2) * (sigma2 * inv_r2) * (sigma2 * inv_r2);
                    Fz += (24.0 * domain.epsilon * inv_r2) * s6 * (2.0 * s6 - 1.0) * (2.0 * dist_back_z);
                }
            }
            // Front Wall
            double dist_front_z = domain.dom_len_z - pz;
            if (dist_front_z < domain.rad_cutoff) {
                double r2 = (2.0 * dist_front_z) * (2.0 * dist_front_z);
                if (r2 < rc2 && r2 > 1e-12) {
                    double inv_r2 = 1.0 / r2;
                    double s6 = (sigma2 * inv_r2) * (sigma2 * inv_r2) * (sigma2 * inv_r2);
                    Fz -= (24.0 * domain.epsilon * inv_r2) * s6 * (2.0 * s6 - 1.0) * (2.0 * dist_front_z);
                }
            }
        }
    }

    acc[3*idx+0] = Fx/mass;
    acc[3*idx+1] = Fy/mass;
    acc[3*idx+2] = Fz/mass;
    vel[3*idx+0] += acc[3*idx+0]*domain.deltaTime*0.5;
    vel[3*idx+1] += acc[3*idx+1]*domain.deltaTime*0.5;
    vel[3*idx+2] += acc[3*idx+2]*domain.deltaTime*0.5;
}


void launchUpdatePosVelFirstHalfKernel(const Domain& domain, ParticleSystem& device_ps,int blocks, int threads){
    updatePosVelFirstHalfKernel<<<blocks,threads>>>(domain,device_ps.pos,device_ps.vel,device_ps.acc);
    checkCudaError(cudaGetLastError());
    checkCudaError(cudaDeviceSynchronize());
}

void launchLJPotentialVelocitySeconfHalfKernel(const Domain& domain,
                                            NeighbourList& dev_nl,
                                            ParticleSystem& dev_ps, const int blocks, const int threads){
    double particle_mass = dev_ps.mass;
    LJPotentialVelocitySeconfHalfKernel<<<blocks,threads>>>(domain,dev_nl.cells_arr,dev_nl.particles_arr,
                                                            dev_ps.pos,dev_ps.vel,dev_ps.acc,particle_mass);
    checkCudaError(cudaGetLastError());
    checkCudaError(cudaDeviceSynchronize());
                                            }