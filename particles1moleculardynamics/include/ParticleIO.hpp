#pragma once

#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "rapidcsv.h"
#include "Vec.hpp"

struct Particle {
    Vec3D position;
    Vec3D velocity;
    Vec3D acceleration;
    double mass;
};

class ParticleIO {
    public:
    static std::vector<Particle> readParticlesFromCSV() {

        rapidcsv::Document doc("data.csv", rapidcsv::LabelParams(0, -1));
        auto x_col = doc.GetColumn<double>("x");
        auto y_col = doc.GetColumn<double>("y");
        auto z_col = doc.GetColumn<double>("z");
        auto mass_col = doc.GetColumn<double>("mass");
        auto vx_col = doc.GetColumn<double>("vx");
        auto vy_col = doc.GetColumn<double>("vy");
        auto vz_col = doc.GetColumn<double>("vz");

        std::vector<Particle> particles;
        particles.resize(x_col.size());

        for (auto i = 0; i < particles.size(); ++i) {
            Particle p;
            p.position[0] = x_col[i];
            p.position[1] = y_col[i];
            p.position[2] = z_col[i];
            p.velocity[0] = vx_col[i];
            p.velocity[1] = vy_col[i];
            p.velocity[2] = vz_col[i];
            p.mass = mass_col[i];
            particles[i] = p;
        }

        return particles;

    }

    static void writeParticlesVTK(const std::string& output_dir, int step, const std::vector<Particle>& particles) {
    std::filesystem::create_directories(output_dir);

    std::ostringstream filename;
    filename << output_dir << "/particles_"
             << std::setw(6) << std::setfill('0') << step
             << ".vtk";

    std::ofstream out(filename.str());

    if (!out) {
        std::cerr << "Error: Could not open VTK file for writing: "
                  << filename.str() << "\n";
        return;
    }

    const std::size_t N = particles.size();

    out << "# vtk DataFile Version 3.0\n";
    out << "Molecular dynamics particles\n";
    out << "ASCII\n";
    out << "DATASET POLYDATA\n";

    out << "POINTS " << N << " double\n";
    for (const auto& p : particles) {
        out << p.position[0] << " " << p.position[1] << " " << p.position[2] << "\n";
    }

    out << "\nVERTICES " << N << " " << 2 * N << "\n";
    for (std::size_t i = 0; i < N; ++i) {
        out << "1 " << i << "\n";
    }

    out << "\nPOINT_DATA " << N << "\n";
    out << "SCALARS speed double 1\n";
    out << "LOOKUP_TABLE default\n";

    for (const auto& p : particles) {
        double speed = std::sqrt(p.velocity[0] * p.velocity[0] + p.velocity[1] * p.velocity[1] + p.velocity[2] * p.velocity[2]);
        out << speed << "\n";
    }
 
    out << "\nSCALARS mass double 1\n";
    out << "LOOKUP_TABLE default\n";

    for (const auto& p : particles) {
        out << p.mass << "\n";
    }
}
};