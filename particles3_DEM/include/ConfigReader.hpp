#pragma once

#include <fstream>

#include "json.hpp"

struct SimulationConfig {
    size_t numberOfParticles;
    size_t numberOfSteps;
    size_t numberOfCells;

    double timeStep;
    double cellSize;
    double cubeLength;

    double springConstant;
    double dissipationConstant;
    double gravity;

    int writeInterval;
};

class ConfigReader {
    public:
        static SimulationConfig readConfig() {

            std::ifstream file("config.json");
            nlohmann::json config;
            file >> config;

            SimulationConfig simConfig;
            simConfig.numberOfParticles = config["number_of_particles"];
            simConfig.numberOfCells = config["no_of_cells"];
            simConfig.numberOfSteps = config["number_of_steps"];
            simConfig.timeStep = config["time_step"];
            simConfig.springConstant = config["spring_constant"];
            simConfig.dissipationConstant = config["dissipation_constant"];
            simConfig.cellSize = config["cell_size"];
            simConfig.writeInterval = config["write_interval"];
            simConfig.gravity = config["gravity"];

            simConfig.cubeLength = simConfig.numberOfCells*simConfig.cellSize;

            return simConfig;
        }

};