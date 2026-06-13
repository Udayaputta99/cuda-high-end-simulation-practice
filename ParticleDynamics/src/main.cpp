#include "domain.h"
#include "particle_system.h"
#include "neigh_list.h"
#include "kernels.cuh"
#include "utils.h"
#include "memory_manager.cuh"
#include <fstream>

int main(int argc, char* argv[]){
    Domain domain;
    domainParamInitialization(domain); //initialising the parameters of sim domain
    domainCellsInitialization(domain); //init cell number 

    ParticleSystem host_ps;
    ParticleSystem dev_ps;
    NeighbourList dev_nl; //only device arrays are needed

    allocateHostMemory(domain, host_ps);
    allocateDeviceMemory(domain, dev_ps);
    allocateDeviceMemory(domain, dev_nl); //allocate memory for cells_arr, and particles_arr

    initializeParticleSystem(domain,host_ps); //giving values to (host) pos, vel, arr 
    copyHostToDevice(dev_ps,host_ps,domain); //copying pos,vel,arr arrays from host --> device
    printSolverDetails(domain);
    int blocks = 10;
    int threads = 256;
    
    double p_radius = host_ps.radius;

    std::string fname = "particle_"+std::to_string(0)+".vtu";
    static std::ofstream pvd("particles.pvd");
    pvdInit(pvd);
    outputFile(0,host_ps.pos,domain.n_particles_total,p_radius);
    int count = 1;
    synchronizeKernels();
    for (double it=0.0; it<=domain.endTime; it += domain.deltaTime){
        resetCellsParticlesArrays(dev_nl,domain); //resets value of cells_arr , particles_arr to -1
        synchronizeKernels();
        launchUpdatePosVelFirstHalfKernel(domain, dev_ps, blocks, threads);
        synchronizeKernels();
        launchCreateNeighbourListKernel(domain, dev_nl, dev_ps, blocks, threads);
        synchronizeKernels();
        launchLJPotentialVelocitySeconfHalfKernel(domain, dev_nl, dev_ps, blocks, threads);
        synchronizeKernels();
        copyDeviceToHost(host_ps, dev_ps, domain);
        synchronizeKernels();
        fname = "particle_"+std::to_string(count)+".vtu";
        writeVTU(fname, host_ps.pos, domain.n_particles_total, p_radius);
        pvdWriteVTU(pvd, count, fname);
        count++;  
        std::cout<<"Count: "<<count<<"\n"; 
    }
    pvdFinalize(pvd);
    freeHostMemory(host_ps);
    freeDeviceMemory(dev_ps);
    freeDeviceMemory(dev_nl);
}