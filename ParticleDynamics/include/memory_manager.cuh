#pragma once
#include "domain.h"
#include "particle_system.h"
#include "neigh_list.h"

void allocateHostMemory(const Domain& domain, ParticleSystem& ps);

void allocateDeviceMemory(const Domain& domain, ParticleSystem& ps);
void allocateDeviceMemory(const Domain& domain, NeighbourList& nl);

void freeHostMemory(ParticleSystem& ps);
void freeDeviceMemory(ParticleSystem& ps);
void freeDeviceMemory(NeighbourList& nl);

void copyHostToDevice( ParticleSystem& dev_ps, ParticleSystem& host_ps, const Domain& domain);
void copyDeviceToHost( ParticleSystem& host_ps, ParticleSystem& dev_ps, const Domain& domain);

void synchronizeKernels();