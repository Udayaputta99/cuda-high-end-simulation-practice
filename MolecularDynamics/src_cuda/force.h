#include <cuda_rutime.h>


__device__ void createNeighbourList(const Domain domain, 
    double* __restrict__ cells_arr, double* __restrict__ neigh_particle_arr,                                
    const double* __restrict__ position, 
    const int ti, const int tj, const int tk, const int idx
                                    ){
    //make sure to reset the cells and particle arrays
    //if (position[3*ti+0]<0 || position[3*tj+1]<0 || position[3*tk+2]<0) continue;
    int cell_x = (int)(position[3*ti+0]/domain.rad_cutoff);
    int cell_y = (int)(position[3*tj+1]/domain.rad_cutoff);
    int cell_z = (int)(position[3*tk+2]/domain.rad_cutoff);

    if (cell_x >= domain.num_cells_x || cell_x >= domain.num_cells_x || cell_x >= domain.num_cells_x) continue;
    int c = cell_x + domain.num_cells_x*cell_y +
                    domain.n_particles_x*domain.n_particles_x*cell_z;
    if (c < idx)
    neigh_particle_arr[idx] = cells_arr[c];
    cells_arr[c] = idx;
}