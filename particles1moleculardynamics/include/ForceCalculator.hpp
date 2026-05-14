#pragma once

#include "CellList.hpp"
#include "PeriodicBoundary.hpp"
#include "ParticleIO.hpp"
#include "ConfigReader.hpp"

#include <vector>
#include <cmath>

class ForceCalculator {
    public:
    ForceCalculator(CellList* cl, PeriodicBoundary* pb, SimulationConfig config) : cellList(cl), 
                                                        boundary(pb), simConfig(config) {}


    void computeAccelerations(std::vector<Particle>& particles) {
        for (auto i = 0; i < particles.size(); ++i) {
            particles[i].acceleration[0] = 0;
            particles[i].acceleration[1] = 0;
            particles[i].acceleration[2] = 0;
            size_t index = cellList->getCellIndex(i);
            auto neighbors = cellList->getNeighboringCells(index);
            for (auto n : neighbors) {
                auto particleIndexes = cellList->getAllParticles(n);
                for (auto j : particleIndexes) {
                    if (i != j && squaredDistance(particles[i].position, particles[j].position) < simConfig.cutoffRadius*simConfig.cutoffRadius) {
                        auto force = computeForce(particles[i].position, particles[j].position);
                        particles[i].acceleration[0] += force[0];
                        particles[i].acceleration[1] += force[1];
                        particles[i].acceleration[2] += force[2];
                    }

                }
            }
            particles[i].acceleration /= particles[i].mass;
        }
    }

    double computeTotalEnergy(std::vector<Particle>& particles) {
        double totalEnergy = 0;
        for (auto i = 0; i < particles.size(); ++i) {
            totalEnergy += computeKineticEnergy(particles[i]);
            size_t index = cellList->getCellIndex(i);
            auto neighbors = cellList->getNeighboringCells(index);
            for (auto n : neighbors) {
                auto particleIndexes = cellList->getAllParticles(n);
                for (auto j : particleIndexes) {
                    if (i < j && squaredDistance(particles[i].position, particles[j].position) < simConfig.cutoffRadius*simConfig.cutoffRadius) {
                        totalEnergy += computeLennardJonesPotential(particles[i].position, particles[j].position);
                    }
                }
            }
        }
        return totalEnergy;
    }

    private:
    CellList* cellList;
    PeriodicBoundary* boundary;
    SimulationConfig simConfig;

    Vec3D computeForce(const Vec3D &i, const Vec3D &j) {
        auto disp = boundary->displacement(i, j);
        auto squared_distance = squaredDistance(i, j);
        auto distance = std::sqrt(squared_distance);
        auto term = std::pow(simConfig.sigma/distance, 6);
        auto force_magnitude = ((24*simConfig.epsilon)*term*(2*term - 1)) / squared_distance;
        return {force_magnitude*disp[0], force_magnitude*disp[1], force_magnitude*disp[2]};
    }

    double squaredDistance(const Vec3D &i, const Vec3D &j) {
        auto disp = boundary->displacement(i, j);
        return disp[0]*disp[0] + disp[1]*disp[1] + disp[2]*disp[2];
    }

    double computeLennardJonesPotential(const Vec3D& i, const Vec3D& j) {
        auto squared_distance = squaredDistance(i, j);
        auto distance = std::sqrt(squared_distance);
        auto term = std::pow(simConfig.sigma/distance, 6);
        return 4*simConfig.epsilon*(term*term - term);
    }

    double computeKineticEnergy(Particle i) {
        return (i.mass*(i.velocity[0]*i.velocity[0] + i.velocity[1]*i.velocity[1] + i.velocity[2]*i.velocity[2])/2);
    }
};