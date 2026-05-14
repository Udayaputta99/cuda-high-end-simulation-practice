#pragma once

#include "../include/ParticleIO.hpp"
#include "../include/PeriodicBoundary.hpp"

#include <vector>

class Integrator{
    public:
    Integrator(PeriodicBoundary* boundary) : pb(boundary) {}
    void updatePositionsAndHalfVelocities(std::vector<Particle>& particles, double timeStep) {
        for (auto j = 0; j < particles.size(); ++j) {
            
            particles[j].position = particles[j].position + particles[j].velocity*timeStep + (particles[j].acceleration*timeStep*timeStep)/2;

            particles[j].position = pb->wrapPosition(particles[j].position);

            particles[j].velocity = particles[j].velocity + (particles[j].acceleration*timeStep)/2;
        }
    }

    void completeVelocityUpdate(std::vector<Particle>& particles, double timeStep) {
        for (auto j = 0; j < particles.size(); ++j) {
            particles[j].velocity = particles[j].velocity + (particles[j].acceleration*timeStep)/2;
        }
    }

    private:
    PeriodicBoundary* pb;
};