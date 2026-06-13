#pragma once
#include "domain.h"

struct ParticleSystem{
    double radius;
    double mass;
    double* pos;
    double* vel;
    double* acc;
};

void initializeParticleSystem(const Domain& domain, ParticleSystem& ps);