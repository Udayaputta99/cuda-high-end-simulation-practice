#pragma once

#include <fstream>

#include "json.hpp"

struct SimulationConfig {
    size_t numberOfParticles;
    size_t numberOfSteps;
    size_t numberOfCells;

    double sigma;
    double epsilon;
    double timeStep;
    double cutoffRadius;
    double cellSize;
    double cubeLength;

    int writeInterval;
    std::string outputFolder;
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
            simConfig.cutoffRadius = config["cutoff_radius"];
            simConfig.timeStep = config["time_step"];
            simConfig.sigma = config["sigma"];
            simConfig.epsilon = config["epsilon"];
            simConfig.cellSize = config["cell_size"];
            simConfig.writeInterval = config["write_interval"];
            simConfig.outputFolder = config["output_folder"];

            simConfig.cubeLength = simConfig.numberOfCells*simConfig.cellSize;

            return simConfig;
        }

};