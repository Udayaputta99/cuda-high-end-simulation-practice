#pragma once
#include "domain.h"

struct ParticleSystem{
    double radius;
    double mass;
    double* pos;
    double* vel;
    double* acc;
};

struct NeighbourList{
    int* cells_arr;
    int* particles_arr;
}

void allocateHostMemory(const Domain& domain, ParticleSystem& ps);

void allocateDeviceMemory(const Domain& domain, ParticleSystem& ps);
void allocateDeviceMemory(const Domain& domain, NeighbourList& nl);

void freeHostMemory(ParticleSystem& ps);
void freeDeviceMemory(ParticleSystem& ps);

void copyHostToDevice(const ParticleSystem& host_ps, ParticleSystem& dev_ps);
void copyDeviceToHost(const ParticleSystem& host_ps, ParticleSystem& dev_ps);

void initializeParticleSystem(const Domain& domain, ParticleSystem& ps);