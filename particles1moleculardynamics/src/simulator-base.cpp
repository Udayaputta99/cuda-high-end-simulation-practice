#include <vector>

#include "../include/ConfigReader.hpp"
#include "../include/ParticleIO.hpp"
#include "../include/Vec.hpp"
#include "../include/PeriodicBoundary.hpp"
#include "../include/CellList.hpp"
#include "../include/ForceCalculator.hpp"
#include "../include/Integrator.hpp"


int main(int argc, char* argv[]) {

    SimulationConfig config = ConfigReader::readConfig();
    
    auto particles = ParticleIO::readParticlesFromCSV();

    CellList cl(config.numberOfParticles, config.numberOfCells, config.cellSize);
    cl.build(particles);

    PeriodicBoundary pb(config.cubeLength);

    ForceCalculator fc(&cl, &pb, config);
    fc.computeAccelerations(particles);
    Integrator integrator(&pb);

    ParticleIO::writeParticlesVTK(config.outputFolder, 0, particles);

    for (auto i = 0; i < config.numberOfSteps; ++i) {
        integrator.updatePositionsAndHalfVelocities(particles, config.timeStep);
        cl.build(particles);
        fc.computeAccelerations(particles); 
        integrator.completeVelocityUpdate(particles, config.timeStep);

        if ((i + 1) % config.writeInterval == 0) {
            ParticleIO::writeParticlesVTK(config.outputFolder, i + 1, particles);
        }

        auto energy = fc.computeTotalEnergy(particles);

        std::cout << "Step: " << i << "   Total Energy: " << energy << "\n";   
    }

    return 0;
}