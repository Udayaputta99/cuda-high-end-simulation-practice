#pragma once

#include "ParticleIO.hpp"

#include <vector>
#include <unordered_map>
#include <algorithm>

class CellList {
    public:
    CellList(size_t numOfParticles, size_t numOfCells, double cellSize) : cells(numOfCells*numOfCells*numOfCells, -1), 
                                                         particlesIndex(numOfParticles, -1), numberOfParticles(numOfParticles),
                                                         numberOfCells(numOfCells), cellSize(cellSize) {}

    void build(const std::vector<Particle>& particles) {

        std::fill(particlesIndex.begin(), particlesIndex.end(), -1);
        std::fill(cells.begin(), cells.end(), -1);
        particleToCellMap.clear();
        for (auto i = 0; i < particles.size(); ++i) {
        
            auto cellCoords = particles[i].position / cellSize;
            auto index = from3DIndex(cellCoords, numberOfCells);
            if (cells[index] == -1) {
                cells[index] = i;
            } else {
                size_t new_index = cells[index];
                while (particlesIndex[new_index] != -1) {
                    new_index = particlesIndex[new_index];
                }
                particlesIndex[new_index] = i;
            }
            particleToCellMap.insert({i, index});
        }
    }

    auto getNeighboringCells(size_t index) {
        std::array<size_t, 27> neighbors;
        auto index_3d = to3DIndex(index, numberOfCells);
        size_t i = 0;
        for (int dz = -1; dz <= 1; ++dz)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    size_t nx = (index_3d[0] + dx + numberOfCells)%numberOfCells;
                    size_t ny = (index_3d[1] + dy + numberOfCells)%numberOfCells;
                    size_t nz = (index_3d[2] + dz + numberOfCells)%numberOfCells;

                    size_t idx = from3DIndex({nx, ny, nz}, numberOfCells);
                    neighbors[i++] = idx;
                }
            }
        }
        return neighbors;
    }

    size_t getCellIndex(size_t i) {
        return particleToCellMap[i];
    }

    std::vector<size_t> getAllParticles(size_t cell_index) {
        std::vector<size_t> particles;
        if (cells[cell_index] != -1) {
            particles.push_back(cells[cell_index]);
            while (particlesIndex[particles.back()] != -1) {
                particles.push_back(particlesIndex[particles.back()]);
            }
        }
        return particles;
    }

    size_t from3DIndex(Vec<size_t, 3> position, size_t dim) {
        return dim*dim*position[2] + dim*position[1] + position[0];
    }

    Vec<size_t, 3> to3DIndex(size_t index, size_t dim) {
        size_t z = index / (dim * dim);
        size_t y = (index / dim) % dim;
        size_t x = index % dim;
        return {x, y, z};
    }

    private:
    std::vector<int> cells;
    std::vector<int> particlesIndex;
    std::unordered_map<size_t, size_t> particleToCellMap;
    const size_t numberOfParticles, numberOfCells;
    const double cellSize;

};