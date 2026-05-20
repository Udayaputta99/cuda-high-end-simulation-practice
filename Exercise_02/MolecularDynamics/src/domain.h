#pragma once
struct Domain{
    const int n_particles_x=10;
    const int n_particles_y=n_particles_x;
    const int n_particles_z=n_particles_y;
    const int n_particles_total=n_particles_x*n_particles_y*n_particles_z;
    double delta_x;
    // double delta_y;
    // double delta_z;
    const double sigma=1.0; //reduced units
    const double epsilon=1.0; //reduced units
    const double rad_cutoff=2.5*sigma; //2.5*sigma
    int num_cells_x; //same across all axes
    int num_cells_total;
};
