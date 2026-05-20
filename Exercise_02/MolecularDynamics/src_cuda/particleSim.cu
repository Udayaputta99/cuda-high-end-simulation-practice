#include <cstdio>
#include <iostream>
#include <fstream>
#include <cmath>
#include "domain.h"
#include "cudautils.h"
#include "vtuWriter.h"
#include "particle.h"
#include "force.h"

#include <cuda_runtime.h>

#include <cuda_runtime.h>

struct GpuLaunchConfig {
    dim3 blocks;
    dim3 threads;
};

// 3D Configuration Function
GpuLaunchConfig get3DLaunchConfig(const Domain& domain) {
    GpuLaunchConfig config;

    // 8 threads per axis = 512 threads per block. Highly efficient for warps.
    const int threadsPerAxis = 8; 

    // Compute independent ceiling division blocks for every single spatial axis
    int bx = (domain.n_particles_x + threadsPerAxis - 1) / threadsPerAxis;
    int by = (domain.n_particles_y + threadsPerAxis - 1) / threadsPerAxis;
    int bz = (domain.n_particles_z + threadsPerAxis - 1) / threadsPerAxis;

    config.threads = dim3(threadsPerAxis, threadsPerAxis, threadsPerAxis);
    config.blocks  = dim3(bx, by, bz);

    return config;
}

int main(){
    std::ofstream pvd("particles.pvd");
    pvd << "<?xml version=\"1.0\"?>\n";
    pvd << "<VTKFile type=\"Collection\" version=\"0.1\">\n";
    pvd << "  <Collection>\n";

    Domain domain;
    domain.delta_x = domain.rad_cutoff;
    double box_len = (double)(domain.n_particles_x)*domain.delta_x; //assuming equidistant distribution in all directions
    std::cout<<"Box len: "<<box_len<<"\n";

    //host arrays
    double* h_position = new double[3*domain.n_particles_total];
    double* h_velocity = new double[3*domain.n_particles_total];   
    double* h_acceleration = new double[3*domain.n_particles_total];   
    
    //device arrays
    double* d_position = nullptr;
    double* d_velocity = nullptr;   
    double* d_acceleration = nullptr;   
    GpuLaunchConfig config = get3DLaunchConfig(domain);
    
    // cudaDeviceProp prop;
    // cudaGetDeviceProperties(&prop, 0);

    // std::cout << "Max threads per block: " << prop.maxThreadsPerBlock << "\n";
    // std::cout << "Max threads per SM: " << prop.maxThreadsPerMultiProcessor << "\n";
    // std::cout << "Max grid size X: " << prop.maxGridSize[0] << "\n";

    //allocate device memory and copy to device
    checkCudaError(cudaMalloc(&d_position, 3*domain.n_particles_total*sizeof(double)));
    checkCudaError(cudaMalloc(&d_velocity, 3*domain.n_particles_total*sizeof(double)));
    checkCudaError(cudaMalloc(&d_acceleration, 3*domain.n_particles_total*sizeof(double)));
    checkCudaError(cudaMemcpy(d_position,h_position,3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(d_velocity,h_velocity,3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(d_acceleration,h_acceleration,3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice));    
    generateParticles<<<config.blocks,config.threads>>>(domain,d_position,d_velocity,d_acceleration);
    cudaDeviceSynchronize();
    //checkCudaError(cudaMemcpy(h_position,d_position,3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost));
    
    double p_mass = 1.0;
    double p_radius = 0.5;
    domain.num_cells_x = (int)std::ceil(box_len/domain.rad_cutoff);
    domain.num_cells_total = pow(domain.num_cells_x,3);
    // printf("Num cells total: %d\n",domain.num_cells_total);
    // printf("Num cells x: %d\n",domain.num_cells_x);
    // printf("Num of particles in x: %d\n", domain.n_particles_x);
    // printf("Num of threads in x: %d, y: %d, z: %d\n",config.threads.x,config.threads.y,config.threads.z);
    // printf("Num of blocks in x: %d, y: %d, z: %d\n",config.blocks.x,config.blocks.y,config.blocks.z);

    //initializing host cells and particles array for neighbourlist
    int* h_cells_arr = new int[domain.num_cells_total];
    int* h_particles_arr = new int[domain.n_particles_total];

    //initializing device cells and particles array for neighbourlist
    int* d_cells_arr = nullptr;
    int* d_particles_arr = nullptr;
    checkCudaError(cudaMalloc(&d_cells_arr,sizeof(int)*domain.num_cells_total));
    checkCudaError(cudaMalloc(&d_particles_arr,sizeof(int)*domain.n_particles_total));
    checkCudaError(cudaMemcpy(d_cells_arr,h_cells_arr,sizeof(int)*domain.num_cells_total,cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(d_particles_arr,h_particles_arr,sizeof(int)*domain.n_particles_total,cudaMemcpyHostToDevice));

    std::string fname = "particle_"+std::to_string(0)+".vtu";
    writeVTU(fname,h_position,domain.n_particles_total,p_radius);
    int count = 0;
    //solver loop starts
    for (double it=domain.startTime; it<=domain.endTime; it += domain.deltaTime){
        cudaMemset(d_cells_arr, -1, domain.num_cells_total * sizeof(int));
        cudaMemset(d_particles_arr, -1, domain.n_particles_total * sizeof(int));
        updatePositionVelocityFirstHalf<<<config.blocks,config.threads>>>(domain,d_position,d_velocity,d_acceleration);
        cudaDeviceSynchronize();
        createNeighbourList<<<config.blocks,config.threads>>>(domain,d_cells_arr,d_particles_arr,d_position);
        cudaDeviceSynchronize();
        LennardJones_VelocitySecondHalf<<<config.blocks,config.threads>>>(domain,d_cells_arr,d_particles_arr,d_position,d_velocity,d_acceleration,p_mass);
        cudaDeviceSynchronize();
        checkCudaError(cudaMemcpy(h_position,d_position,3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost));
        fname = "particle_"+std::to_string(count)+".vtu";
        writeVTU(fname,h_position,domain.n_particles_total,p_radius);
        pvd<<"    <DataSet timestep=\"" << count << "\" file=\"" << fname << "\"/>\n";
        count++;
    }
    // checkCudaError(cudaMemcpy(h_position,d_position,3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost));
    // checkCudaError(cudaMemcpy(h_velocity,d_velocity,3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost));
    // checkCudaError(cudaMemcpy(h_acceleration,d_acceleration,3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost));


    checkCudaError(cudaFree(d_position));
    checkCudaError(cudaFree(d_velocity));
    checkCudaError(cudaFree(d_acceleration));
    checkCudaError(cudaFree(d_cells_arr));
    checkCudaError(cudaFree(d_particles_arr));
    delete[] h_position;
    delete[] h_velocity;
    delete[] h_acceleration;
    delete[] h_cells_arr;
    delete[] h_particles_arr;
    printf("Program exited without errors\n");
    pvd<<"  </Collection>\n";
    pvd<<"</VTKFile>\n";
}