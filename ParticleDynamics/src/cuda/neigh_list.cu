#include <cuda_runtime.h>
#include "neigh_list.cuh"
#include "domain.h"

__global__ void createNeighbourList(const Domain domain, 
                                    int* __restrict__ cells_arr,
                                    int* __restrict__ particles_arr,
                                    const double* __restrict__ pos){
    const int idx = blockDim.x*blockIdx.x + threadIdx.x;
    if (idx>=domain.n_particles_total) return;
    //following is to find the cell each particle belongs to, works since 1 thread --> 1 particle
    int cell_x = (int)(pos[3*idx+0]/domain.rad_cutoff);
    int cell_y = (int)(pos[3*idx+1]/domain.rad_cutoff);
    int cell_z = (int)(pos[3*idx+2]/domain.rad_cutoff);
    if (cell_x < domain.n_cells_x && cell_y < domain.n_cells_y && cell_z < domain.n_cells_z
        && cell_x >=0 && cell_y >=0 && cell_z >=0){
        int cell_idx = cell_x + domain.n_cells_x*cell_y + domain.n_cells_x*domain.n_cells_y*cell_z;
        int old_head_idx = atomicExch(&cells_arr[cell_idx],idx);
        particles_arr[idx] = old_head_idx;
    }
}
