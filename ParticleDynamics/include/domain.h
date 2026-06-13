#pragma once
#include <string>

enum BC{
    PERIODIC,
    REFLECTIVE
};

struct Domain{
    int dimension;

    int n_particles_x,n_particles_y,n_particles_z;
    int n_particles_total;

    int n_cells_x, n_cells_y, n_cells_z;
    int n_cells_total;
    double cell_width_x, cell_width_y, cell_width_z;

    double delta_x, delta_y, delta_z;
    double rad_cutoff;
    double sigma;
    double epsilon;
    double deltaTime;
    double endTime; 
    double T; //temperature

    bool gravityEnabled;
    BC bc;
};

void domainParamInitialization(Domain& domain);
void domainBoxInitialization(Domain& domain);