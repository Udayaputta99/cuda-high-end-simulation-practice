#pragma once
#include "domain.h"
#include "particle_system.h"
struct NeighbourList{
    int* cells_arr;
    int* particles_arr;
};

void resetCellsParticlesArray(NeighbourList& nl, Domain& domain);
void launchCreateNeighbourListKernel(const Domain domain, NeighbourList& dev_nl,
                                    ParticleSystem& dev_ps, const int blocks, const int threads);