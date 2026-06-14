#pragma once

#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>

#include "rapidcsv.h"

struct Particles {
    double* x = nullptr;
    double* y = nullptr;
    double* z = nullptr;

    double* vx = nullptr;
    double* vy = nullptr;
    double* vz = nullptr;

    double* ax = nullptr;
    double* ay = nullptr;
    double* az = nullptr;

    double* mass = nullptr;
    double* radius = nullptr;
};

class ParticleIO {
    public:
    static void readParticlesFromCSV(Particles host, size_t size) {

        rapidcsv::Document doc("data.csv", rapidcsv::LabelParams(0, -1));
        auto x_col = doc.GetColumn<double>("x");
        auto y_col = doc.GetColumn<double>("y");
        auto z_col = doc.GetColumn<double>("z");
        auto vx_col = doc.GetColumn<double>("vx");
        auto vy_col = doc.GetColumn<double>("vy");
        auto vz_col = doc.GetColumn<double>("vz");
        auto mass_col = doc.GetColumn<double>("mass");
        auto radius_col = doc.GetColumn<double>("radius");

        for (auto i = 0; i < size; ++i) {
            host.x[i] = x_col[i];
            host.y[i] = y_col[i];
            host.z[i] = z_col[i];
            host.vx[i] = vx_col[i];
            host.vy[i] = vy_col[i];
            host.vz[i] = vz_col[i];
            host.mass[i] = mass_col[i];
            host.radius[i] = radius_col[i];
        }
    }

    static void writeParticlesVTK(
    const std::string& output_dir,
    int step,
    const Particles& host,
    size_t size
) {
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

    const std::size_t N = size;

    out << "# vtk DataFile Version 3.0\n";
    out << "DEM particles\n";
    out << "ASCII\n";
    out << "DATASET POLYDATA\n";

    out << "POINTS " << N << " double\n";
    for (std::size_t i = 0; i < N; ++i) {
        out << host.x[i] << " " << host.y[i] << " " << host.z[i] << "\n";
    }

    out << "\nVERTICES " << N << " " << 2 * N << "\n";
    for (std::size_t i = 0; i < N; ++i) {
        out << "1 " << i << "\n";
    }

    out << "\nPOINT_DATA " << N << "\n";

    // Speed scalar
    out << "SCALARS speed double 1\n";
    out << "LOOKUP_TABLE default\n";
    for (std::size_t i = 0; i < N; ++i) {
        double speed = std::sqrt(
            host.vx[i] * host.vx[i] +
            host.vy[i] * host.vy[i] +
            host.vz[i] * host.vz[i]
        );
        out << speed << "\n";
    }

    // Mass scalar
    out << "\nSCALARS mass double 1\n";
    out << "LOOKUP_TABLE default\n";
    for (std::size_t i = 0; i < N; ++i) {
        out << host.mass[i] << "\n";
    }

    // Radius scalar
    out << "\nSCALARS radius double 1\n";
    out << "LOOKUP_TABLE default\n";
    for (std::size_t i = 0; i < N; ++i) {
        out << host.radius[i] << "\n";
    }
}
};