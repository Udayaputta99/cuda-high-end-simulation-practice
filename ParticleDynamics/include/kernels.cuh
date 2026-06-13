#pragma once
#include <iostream>
#include "domain.h"
#include "particle_system.h"
#include "neigh_list.h"

void launchUpdatePosVelFirstHalfKernel(const Domain& domain, ParticleSystem& device_ps,int blocks, int threads);
void launchLJPotentialVelocitySeconfHalfKernel(const Domain& domain,NeighbourList& dev_nl, ParticleSystem& dev_ps, const int blocks, const int threads);

