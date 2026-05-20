#pragma once
#include <cmath>
#include <vector>
#include <iostream>
void createNeighbourList(std::vector<int>& cells_arr, std::vector<int>& neigh_particles_arr,
                         std::vector<Particle>& particles, Domain& domain ){
    // for (int i=0; i<neigh_particles_arr.size(); ++i){
    //     //needs gaurd if the particle is exactly at the boundary
    //     int cell_x = std::min((int)(position[i][0]/domain.rad_cutoff), domain.num_cells_x-1);
    //     int cell_y = std::min((int)(position[i][1]/domain.rad_cutoff), domain.num_cells_x-1);
    //     int cell_z = std::min((int)(position[i][2]/domain.rad_cutoff), domain.num_cells_x-1);
    //     int c = cell_x + domain.num_cells_x*cell_y + 
    //                     domain.num_cells_x*domain.num_cells_x*cell_z;
    //     neigh_particles_arr[i] = cells[c];
    //     cells[c] = i;
    // }
    // Inside force.h -> createNeighbourList
    for (int i = 0; i < neigh_particles_arr.size(); ++i) {
        // Check if particle is outside the positive grid
        // the following line is the reason behind 3 plane of particles getting removed in paraview 
        //if (particles[i].p_position[0] < 0 || particles[i].p_position[1] < 0 || particles[i].p_position[2] < 0) continue;

        int cell_x = (int)(particles[i].p_position[0] / domain.rad_cutoff);
        int cell_y = (int)(particles[i].p_position[1] / domain.rad_cutoff);
        int cell_z = (int)(particles[i].p_position[2] / domain.rad_cutoff);
        
        // Guard against indices >= num_cells_x
        if (cell_x >= domain.num_cells_x || cell_y >= domain.num_cells_x || cell_z >= domain.num_cells_x) continue;

        int c = cell_x + domain.num_cells_x * cell_y + 
                domain.num_cells_x * domain.num_cells_x * cell_z;
        
        neigh_particles_arr[i] = cells_arr[c];
        cells_arr[c] = i;
    }
}

void LennardJones(Domain& domain, std::vector<int>& cells_arr, std::vector<int>& neigh_particles_arr, std::vector<Particle>& particles){
    
    for (size_t i = 0; i < particles.size(); ++i) {
        particles[i].p_acceleration[0] = 0.0;
        particles[i].p_acceleration[1] = 0.0;
        particles[i].p_acceleration[2] = 0.0;
    }
    createNeighbourList(cells_arr,neigh_particles_arr,particles,domain);
    for (int l=0; l<cells_arr.size(); ++l){
        int particle_idx = cells_arr[l];
            while (particle_idx!=-1){
                std::vector<double> Force(3,0); //initialized Fx, Fy, Fz with zero
                //following is the loops to find the neighbour cells
                int i = (int) l%domain.num_cells_x;  
                int j = (int) (l/domain.num_cells_x)%domain.num_cells_x;
                int k = (int) (l/(domain.num_cells_x*domain.num_cells_x));
                // std::cout<<"check3\n";
                for (int dz=-1; dz<=1; ++dz){
                    for (int dy=-1; dy<=1; ++dy){
                        for (int dx=-1; dx<=1; ++dx){
                            int neigh_i = i+dx;
                            int neigh_j = j+dy;            
                            int neigh_k = k+dz;            
                            //skip the loop is ouot of bounds
                            if (neigh_i<0 || neigh_i >= domain.num_cells_x) continue;
                            if (neigh_j<0 || neigh_j >= domain.num_cells_x) continue;
                            if (neigh_k<0 || neigh_k >= domain.num_cells_x) continue;
                            int neigh_cell_idx = neigh_i + domain.num_cells_x*neigh_j + domain.num_cells_x*domain.num_cells_x*neigh_k;
                        
                            int neighbhour_cell_particle_idx = cells_arr[neigh_cell_idx];
                            // std::cout<<"check1\n";
                            while (neighbhour_cell_particle_idx != -1){
                                if (neighbhour_cell_particle_idx != particle_idx){
                                    double xij_sqrd = pow(particles[particle_idx].p_position[0]-particles[neighbhour_cell_particle_idx].p_position[0],2)+
                                                        pow(particles[particle_idx].p_position[1]-particles[neighbhour_cell_particle_idx].p_position[1],2)+
                                                        pow(particles[particle_idx].p_position[2]-particles[neighbhour_cell_particle_idx].p_position[2],2);
                                    if (xij_sqrd<domain.rad_cutoff*domain.rad_cutoff){
                                        double sigma_xij_powsix = pow(domain.sigma,6)/ pow(xij_sqrd,3);
                                        double multiplier = 24.0*domain.epsilon*sigma_xij_powsix*(2.0*sigma_xij_powsix-1)*(1.0/xij_sqrd);
                                        for (auto f=0; f<3;++f){
                                            Force[f] += multiplier*(particles[particle_idx].p_position[f]-particles[neighbhour_cell_particle_idx].p_position[f]); 
                                        }
                                    }
                                }
                                neighbhour_cell_particle_idx = neigh_particles_arr[neighbhour_cell_particle_idx];
                                
                            }
                            // std::cout<<"check2\n";
                        }
                    }
                }
                
                for (int ii=0; ii<3; ++ii){
                    particles[particle_idx].p_acceleration[ii] = Force[ii]/particles[particle_idx].p_mass;
                }
                particle_idx = neigh_particles_arr[particle_idx];   
                // std::cout<<"check4\n";
            }        
    }
}