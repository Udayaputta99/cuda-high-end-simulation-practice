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
    //const double T = 300; //Temp in Kelvin
    //const double mass_SI = 6.63e-26; //mass of a single Argon atom in kg
    const double T = 2.68; //reduced unit
    const double mass = 1.0; //reduced unit
    std::vector<Particle> particle_arr;
    std::mt19937 generator(42); //generator for vel distri
    for(int pk=0; pk<domain.n_particles_z; ++pk){
        for(int pj=0; pj<domain.n_particles_y; ++pj){
            for(int pi=0; pi<domain.n_particles_x; ++pi){
                // std::vector<double> vel (3,0);
                std::vector<double> vel (3,0);
                gaussianVelocityDistribution(vel,T,generator);
                std::vector<double> acc (3,0);
                std::vector<double> pos{((double)pi*domain.delta_x),
                                         ((double)pj*domain.delta_x),
                                         ((double)pk*domain.delta_x)};
                Particle p {mass,pos,vel,acc};
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
    
    Domain domain;
    //epsilon, sigma, mass, Kb are 1 in reduced units
    const double epsilon_SI = 1.54412e-21; //(https://onlinelibrary.wiley.com/doi/pdf/10.1002/9783527676750.app5)
    const double sigma_SI = 3.62e-10; //vander waals radius of single Argon atom * 2 (https://chem.libretexts.org/Bookshelves/Physical_and_Theoretical_Chemistry_Textbook_Maps/Supplemental_Modules_(Physical_and_Theoretical_Chemistry)/Physical_Properties_of_Matter/Atomic_and_Molecular_Properties/Intermolecular_Forces/Specific_Interactions/Lennard-Jones_Potential)
    domain.delta_x = domain.rad_cutoff; //2.5*sigma
    double box_len = (double)(domain.n_particles_x)*domain.delta_x; //assuming equidistant distribution in all directions
    std::cout<<"Box len: "<<box_len<<"\n";
    
    std::vector<Particle> particles = generateParticles(domain);  
    double startTime = 0.0;
    
    double deltaTime = 0.005;
    double endTime = deltaTime*100.0;
    double radius = 0.5;
    domain.num_cells_x = (int)std::ceil(box_len/domain.rad_cutoff);
    domain.num_cells_total = domain.num_cells_x*domain.num_cells_x*domain.num_cells_x;

    std::cout<<"Num cells in x: "<<domain.num_cells_x<<"\n";
    std::vector<int> cells_arr(domain.num_cells_total,-1);
    std::vector<int> neigh_particles_arr(domain.n_particles_total,-1);
    
    int count=0;
    std::string rootFolder = "particle_"; 
    std::string fname = rootFolder+std::to_string(count)+".vtu";
    writeVTU(fname,particles,domain.n_particles_total,radius);
    
    for (double it=deltaTime; it<=endTime; it += deltaTime){
        count++;
        std::fill(cells_arr.begin(),cells_arr.end(),-1);
        std::fill(neigh_particles_arr.begin(),neigh_particles_arr.end(),-1);

        updatePositionVelocityFirstHalf(particles,deltaTime);
        LennardJones(domain,cells_arr,neigh_particles_arr,particles);
        
        updateVelocitySecondHalf(particles,deltaTime);
        fname = rootFolder+std::to_string(count)+".vtu";
        writeVTU(fname,particles,domain.n_particles_total,radius);
        pvd<<"    <DataSet timestep=\"" << count << "\" file=\"" << fname << "\"/>\n";
        // std::cout<<count<<"\n";
    }

    pvd<<"  </Collection>\n";
    pvd<<"</VTKFile>\n";
}