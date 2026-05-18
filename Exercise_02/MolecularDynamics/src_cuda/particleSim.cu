#include <cstdio>
#include <iostream>
#include <fstream>
#include "domain.h"
#include "cudautils.h"
#include "vtuWriter.h"
#include "particle.h"
#include "force.h"
__global__ void solver(const int it,const Domain domain, double* __restrict__ position, 
                                                 double* __restrict__ velocity,
                                                 double* __restrict__ acceleration,
                                                double* __restrict__ cells_arr,
                                                double* __restrict__ particles_arr){
    int ti = blockDim.x*blockIdx.x+threadIdx.x;
    int tj = blockDim.y*blockIdx.y+threadIdx.y;
    int tk = blockDim.z*blockIdx.z+threadIdx.z;
    int idx = ti + domain.n_particles_x*tj + 
                domain.n_particles_x*domain.n_particles_x*tk;
    if (ti<domain.n_particles_x && tj<domain.n_particles_x && tk<domain.n_particles_x ){
        if (it==0){
            //Particle generation only once at t=0

            // printf("%d,%d,%d\n",ti,tj,tk);

            position[3*idx+0] = ti*domain.delta_x;
            position[3*idx+1] = tj*domain.delta_x; 
            position[3*idx+2] = tk*domain.delta_x; 

            velocity[3*idx+0] = (idx==0)?1:0;
            velocity[3*idx+1] = (idx==0)?1:0; 
            velocity[3*idx+2] = (idx==0)?1:0; 

            acceleration[3*idx+0] = 0.0;
            acceleration[3*idx+1] = 0.0; 
            acceleration[3*idx+2] = 0.0; 
            
        }
        else{
            updatePositionVelocityFirstHalf(domain,position,velocity,acceleration,ti,tj,tk);
            // reset cells_arr particles_arr
            updateVelocitySecondHalf(domain,velocity,acceleration,ti,tj,tk);
        }
    }
}


int main(){
    std::ofstream pvd("particles.pvd");
    pvd << "<?xml version=\"1.0\"?>\n";
    pvd << "<VTKFile type=\"Collection\" version=\"0.1\">\n";
    pvd << "  <Collection>\n";

    Domain domain;

    double box_len = (double)domain.n_particles_x*domain.delta_x; //assuming equidistant distribution in all directions
    std::cout<<"Box len: "<<box_len<<"\n";

    //host arrays
    double* h_position = new double[3*domain.n_particles_total];
    double* h_velocity = new double[3*domain.n_particles_total];   
    double* h_acceleration = new double[3*domain.n_particles_total];   
    
    //device arrays
    double* d_position = nullptr;
    double* d_velocity = nullptr;   
    double* d_acceleration = nullptr;   
    int baseDim = 10;
    dim3 threads (baseDim,baseDim,baseDim);
    dim3 blocks (1,1,1);

    //allocate device memory and copy to device
    checkCudaError(cudaMalloc(&d_position, 3*domain.n_particles_total*sizeof(double)));
    checkCudaError(cudaMalloc(&d_velocity, 3*domain.n_particles_total*sizeof(double)));
    checkCudaError(cudaMalloc(&d_acceleration, 3*domain.n_particles_total*sizeof(double)));
    checkCudaError(cudaMemcpy(d_position,h_position,3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(d_velocity,h_velocity,3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(d_acceleration,h_acceleration,3*domain.n_particles_total*sizeof(double),cudaMemcpyHostToDevice));    
    //generateParticles<<<blocks,threads>>>(domain,d_position,d_velocity,d_acceleration);
    //cudaDeviceSynchronize();
    //checkCudaError(cudaMemcpy(h_position,d_position,3*domain.n_particles_total*sizeof(double),cudaMemcpyDeviceToHost));

    double p_radius = 1.0;
    domain.num_cells_x = (int)std::ceil(box_len/domain.rad_cutoff);
    domain.num_cells_total = pow(domain.num_cells_x,3);
    printf("Num cells total: %d\n",domain.num_cells_total);
    printf("Num cells x: %d\n",domain.num_cells_x);

    //initializing host cells and particles array for neighbourlist
    double* h_cells_arr = new double[3*domain.num_cells_x];
    double* h_particles_arr = new double[domain.n_particles_total];

    //initializing device cells and particles array for neighbourlist
    double* d_cells_arr = nullptr;
    double* d_particles_arr = nullptr;
    checkCudaError(cudaMalloc(&d_cells_arr,sizeof(double)*3*domain.num_cells_x));
    checkCudaError(cudaMalloc(&d_particles_arr,sizeof(double)*3*domain.n_particles_total));
    checkCudaError(cudaMemcpy(d_cells_arr,h_cells_arr,sizeof(double)*3*domain.num_cells_x,cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(d_particles_arr,d_particles_arr,sizeof(double)*3*domain.n_particles_total,cudaMemcpyHostToDevice));

    //std::string fname = "particle_"+std::to_string(0)+".vtu";
    //writeVTU(fname,h_position,domain.n_particles_total,p_radius);

    //solver loop starts
    for (double it=domain.startTime; it<=domain.endTime; it += domain.deltaTime){
        solver<<<blocks,threads>>>();
    }


    checkCudaError(cudaFree(d_position));
    checkCudaError(cudaFree(d_velocity));
    checkCudaError(cudaFree(d_acceleration));
    checkCudaError(cudaFree(d_cells_arr));
    checkCudaError(cudaFree(d_particles_arr));
    delete h_position;
    delete h_velocity;
    delete h_acceleration;
    delete h_cells_arr;
    delete h_particles_arr;
    // printf("Program exited without errors\n");
    pvd<<"    <DataSet timestep=\"" << 0 << "\" file=\"" << fname << "\"/>\n";
    pvd<<"  </Collection>\n";
    pvd<<"</VTKFile>\n";
}