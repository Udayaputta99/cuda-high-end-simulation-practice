#include "domain.h"
#include "particle_system.h"
#include "utils.h"
#include "neigh_list.cuh"
#include "memory_manager.cuh"


int main(int argc, char* argv[]){
    Domain domain;
    domainParamInitialization(domain);
    domainCellsInitialization(domain);

    ParticleSystem host_ps;
    ParticleSystem dev_ps;
    NeighbourList dev_nl;//only device arrays are needed

    allocateHostMemory(domain, host_ps);
    allocateDeviceMemory(domain, dev_ps);
    allocateDeviceMemory(domain, dev_nl);

    initializeParticleSystem(domain,host_ps);

    printSolverDetails(domain);

    // int count = 0;
    // outputFile(count,host_ps.pos,domain.n_particles_total,host_ps.radius);
    for (double it=0.0; it<=domain.endTime; it += domain.deltaTime){
        resetCellsParticleArray(dev_nl,domain);
        launchUpdatePosVelFirstHalfKernel(domain, dev_ps, blocks, threads);
        launchCreateNeighbourListKernel(domain, dev_nl, dev_ps, blocks, threads);
    }
    freeHostMemory(host_ps);
    freeDeviceMemory(dev_ps);

}