#include <cmath>
#include <iostream>
#include "domain.h"

void domainParamInitialization(Domain& domain){
    domain.dimension = 3;
    domain.n_particles_x = 10;
    domain.n_particles_y = domain.n_particles_x;
    domain.n_particles_z = domain.n_particles_x;
    domain.n_particles_total = domain.n_particles_x*domain.n_particles_y*domain.n_particles_z;
    domain.delta_x = 1.0;
    domain.delta_y = domain.delta_x;
    domain.delta_z = domain.delta_x;

    domain.sigma = 1.0; //reduced units
    domain.epsilon = 1.0;
    domain.rad_cutoff = 2.5*domain.sigma;
    domain.deltaTime = 0.005;
    domain.endTime = 1.0;
    domain.T = 2.68; //reduced units
    domain.gravityEnabled = true;
    domain.bc = PERIODIC; //can be PERIODIC, REFLECTIVE

}

void domainBoxInitialization(Domain& domain){

    domain.n_cells_x = (int)std::ceil(((double)domain.n_particles_x*domain.delta_x)/domain.rad_cutoff);
    domain.n_cells_y = (int)std::ceil(((double)domain.n_particles_y*domain.delta_y)/domain.rad_cutoff);
    domain.n_cells_z = (int)std::ceil(((double)domain.n_particles_z*domain.delta_z)/domain.rad_cutoff);

    if (domain.n_cells_x<1) domain.n_cells_x = 1;
    if (domain.n_cells_y<1) domain.n_cells_y = 1;
    if (domain.n_cells_z<1) domain.n_cells_z = 1;

    domain.n_cells_total = domain.n_cells_x*domain.n_cells_y*domain.n_cells_z;
    domain.cell_width_x = domain.rad_cutoff;
    domain.cell_width_y = domain.rad_cutoff;
    domain.cell_width_z = domain.rad_cutoff;

}