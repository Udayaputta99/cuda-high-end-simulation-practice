#pragma once
struct Domain{
    const int n_particles_x=20;
    const int n_particles_y=n_particles_x;
    const int n_particles_z=n_particles_y;
    const int n_particles_total=n_particles_x*n_particles_y*n_particles_z;
    double delta_x;
    // const double delta_y=delta_x;
    // const double delta_z=delta_x;
    const double sigma=1.0; //reduced units
    const double epsilon=1.0; //reduced units
    const double rad_cutoff=2.5*sigma; //2.5*sigma
    const float T = 2.68; //reduced temp 
    const double startTime = 0.0;
    const double endTime = 1.0;
    const double deltaTime = 0.005;
    
    int num_cells_x; //same across all axes
    int num_cells_total;
};