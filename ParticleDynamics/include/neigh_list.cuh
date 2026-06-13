#pragma once
#include "domain.h"
struct NeighbourList{
    int* cells_arr;
    int* particles_arr;
}

void initializeCellsParticlesArray(NeighbourList& nl, Domain& domain);