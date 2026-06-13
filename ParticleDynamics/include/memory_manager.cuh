#pragma once
#include "domain.h"
#include "particle_system.h"
#include "neigh_list.cuh"

void allocateHostMemory(const Domain& domain, ParticleSystem& ps);

void allocateDeviceMemory(const Domain& domain, ParticleSystem& ps);
void allocateDeviceMemory(const Domain& domain, NeighbourList& nl);

void freeHostMemory(ParticleSystem& ps);
void freeDeviceMemory(ParticleSystem& ps);

void copyHostToDevice(const ParticleSystem& host_ps, ParticleSystem& dev_ps);
void copyDeviceToHost(const ParticleSystem& host_ps, ParticleSystem& dev_ps);