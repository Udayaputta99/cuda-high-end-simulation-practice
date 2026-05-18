#include <vector>
#include <random>
#include <iostream>
#include <cmath>
#include "particle.h"
#include "vtuWriter.h"
#include "domain.h"
#include "force.h"
#include "util.h"

std::vector<Particle> generateParticles(const Domain& domain){
    const double velMin = -0.5;
    double velMax = 0.5;
    const double mass = 2.0;
    const double radius = 0.5;
    
    std::vector<Particle> particle_arr;

    for(int pk=0; pk<domain.n_particles_z; ++pk){
        for(int pj=0; pj<domain.n_particles_y; ++pj){
            for(int pi=0; pi<domain.n_particles_x; ++pi){
                // std::vector<double> vel (3,0);
                std::vector<double> vel = randVec(velMin, velMax);
                std::vector<double> acc (3,0);
                std::vector<double> pos{((double)pi*domain.delta_x),
                                         ((double)pj*domain.delta_y),
                                         ((double)pk*domain.delta_z)};
                Particle p {mass,radius,pos,vel,acc};
                particle_arr.push_back(p);
            }
        }
    }
    return particle_arr;
}


int main(){
    //initialize the pvd writer
    std::ofstream pvd("particles.pvd");
    pvd << "<?xml version=\"1.0\"?>\n";
    pvd << "<VTKFile type=\"Collection\" version=\"0.1\">\n";
    pvd << "  <Collection>\n";
    //
    Domain domain;

    double box_len = (double)domain.n_particles_x*domain.delta_x; //assuming equidistant distribution in all directions
    std::cout<<"Box len: "<<box_len<<"\n";
    
    std::vector<Particle> particles = generateParticles(domain);  
    double startTime = 0.0;
    double endTime = 2.0;
    double deltaTime = 0.01;
    double radius = particles[0].p_radius;
    domain.num_cells_x = (int)std::ceil(box_len/domain.rad_cutoff);
    domain.num_cells_total = pow(domain.num_cells_x,3);

    std::cout<<"Num cells in x: "<<domain.num_cells_x<<"\n";
    std::vector<int> cells_arr(pow(domain.num_cells_x,3),-1);
    std::vector<int> neigh_particles_arr(domain.n_particles_total,-1);

    std::string fname = "particle_"+std::to_string(0)+".vtu";
    writeVTU(fname,particles,domain.n_particles_total,radius);
    int count = 0;
    for (double it=deltaTime; it<=endTime; it += deltaTime){
        std::fill(cells_arr.begin(),cells_arr.end(),-1);
        std::fill(neigh_particles_arr.begin(),neigh_particles_arr.end(),-1);

        updatePositionVelocityFirstHalf(particles,deltaTime);
        LennardJones(domain,cells_arr,neigh_particles_arr,particles);
        
        updateVelocitySecondHalf(particles,deltaTime);
        fname = "particle_"+std::to_string(it)+".vtu";
        writeVTU(fname,particles,domain.n_particles_total,radius);
        std::cout<<"Count: "<<count<<"\n";
        // if (count==2) break;
        count += 1;
        // std::cout<<"check"<<count;
        pvd<<"    <DataSet timestep=\"" << it << "\" file=\"" << fname << "\"/>\n";
    }

    pvd<<"  </Collection>\n";
    pvd<<"</VTKFile>\n";
}