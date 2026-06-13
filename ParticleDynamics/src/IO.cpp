#include <fstream>
#include <string>

void writeVTU(const std::string& filename, const double* pos, int N, double radius)
{
    std::ofstream f(filename);
    f << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\">\n";
    f << "  <UnstructuredGrid>\n";
    f << "  <Piece NumberOfPoints=\"" << N << "\" NumberOfCells=\"0\">\n";
    f << "    <PointData Scalars=\"Radius\">\n";
    f << "      <DataArray type=\"Float64\" Name=\"Radius\" format=\"ascii\">\n";
    for (int i = 0; i < N; i++)
        f << "        " << radius << "\n";
    f << "      </DataArray>\n";
    f << "    </PointData>\n";
    f << "    <Points>\n";
    f << "      <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (int i = 0; i < N; i++)
        f << "        " << pos[3*i+0] << " " << pos[3*i+1] << " " << pos[3*i+2] << "\n";
        // f << "        " << pos[3*i+0] << " " << pos[3*i+1] << " " << 0 << "\n";
    f << "      </DataArray>\n";
    f << "    </Points>\n";
    f << "    <Cells>\n";
    f << "      <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\"/>\n";
    f << "      <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\"/>\n";
    f << "      <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\"/>\n";
    f << "    </Cells>\n";
    f << "  </Piece>\n";
    f << "  </UnstructuredGrid>\n";
    f << "</VTKFile>\n";
}

#include <iostream>
#include <iomanip>
#include "domain.h"

void printSolverDetails(const Domain& domain) {
    // Define a consistent width for column alignment
    const int w = 24;

    std::cout << "\n==================================================\n";
    std::cout << "              SIMULATION CONFIGURATION         \n";
    std::cout << "==================================================\n";

    // 1. Core Dimensions & System Specs
    std::cout << std::left << std::setw(w) << "Simulation Dimension:" << domain.dimension << "D\n";
    std::cout << std::left << std::setw(w) << "Total Particles:"     << domain.n_particles_total << " (";
    
    if (domain.dimension == 1) {
        std::cout << domain.n_particles_x << ")\n";
    } else if (domain.dimension == 2) {
        std::cout << domain.n_particles_x << " x " << domain.n_particles_y << ")\n";
    } else {
        std::cout << domain.n_particles_x << " x " << domain.n_particles_y << " x " << domain.n_particles_z << ")\n";
    }

    // 2. Boundary Condition Translation
    std::cout << std::left << std::setw(w) << "Boundary Condition:";
    if (domain.bc == PERIODIC)        std::cout << "PERIODIC\n";
    else if (domain.bc == REFLECTIVE) std::cout << "REFLECTIVE\n";
    else                                 std::cout << "OPEN / NONE\n";

    std::cout << "--------------------------------------------------\n";

    // 3. Physical Space Dimensions
    std::cout << "Simulation Box Size:\n";
    std::cout << "  " << std::left << std::setw(w - 2) << "X-Length:" << domain.box_len_x << "\n";
    if (domain.dimension >= 2) {
        std::cout << "  " << std::left << std::setw(w - 2) << "Y-Length:" << domain.box_len_y << "\n";
    }
    if (domain.dimension == 3) {
        std::cout << "  " << std::left << std::setw(w - 2) << "Z-Length:" << domain.box_len_z << "\n";
    }

    std::cout << "--------------------------------------------------\n";

    // 4. Spatial Sorting Grid (Linked Cells)
    std::cout << std::left << std::setw(w) << "Total Grid Cells:" << domain.n_cells_total << " (";
    if (domain.dimension == 1) {
        std::cout << domain.n_cells_x << ")\n";
    } else if (domain.dimension == 2) {
        std::cout << domain.n_cells_x << " x " << domain.n_cells_y << ")\n";
    } else {
        std::cout << domain.n_cells_x << " x " << domain.n_cells_y << " x " << domain.n_cells_z << ")\n";
    }

    std::cout << "--------------------------------------------------\n";

    // 5. Physics Constants & Time Parameters
    std::cout << "Thermodynamics & Time Parameters:\n";
    std::cout << "  " << std::left << std::setw(w - 2) << "Temperature (T):"    << domain.T << "\n";
    std::cout << "  " << std::left << std::setw(w - 2) << "Cutoff Radius:"     << domain.rad_cutoff << "\n";
    std::cout << "  " << std::left << std::setw(w - 2) << "Time Step (dt):"     << domain.deltaTime << "\n";
    std::cout << "  " << std::left << std::setw(w - 2) << "Lennard-Jones Sigma:" << domain.sigma << "\n";
    std::cout << "  " << std::left << std::setw(w - 2) << "Lennard-Jones Eps:"   << domain.epsilon << "\n";
    
    std::cout << "==================================================\n\n";
}
