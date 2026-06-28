#pragma once
#include <fstream>
#include <string>
#include "domain.h"
#include "functions.h"

// ---- VTI writer ------------------------------------------------------------
// Writes density, velocity, AND an explicit "Obstacle" mask field.
// THIS is the fix for "the box isn't visible in ParaView": density and
// velocity inside/around a slow, weakly-forced flow don't visually stand
// out on their own. Apply a Threshold filter on "Obstacle" (>= 0.5), or a
// Contour at 0.5, to render the solid region explicitly.

void write_vti(const std::string &filename, const std::vector<double> &f, Domain& domain) {
    std::ofstream out(filename);
    out << std::setprecision(8);
    out << "<?xml version=\"1.0\"?>\n";
    out << "<VTKFile type=\"ImageData\" version=\"0.1\" byte_order=\"LittleEndian\">\n";
    out << "  <ImageData WholeExtent=\"0 " << domain.Nx-1 << " 0 " << domain.Ny-1 << " 0 " << domain.Nz-1
        << "\" Origin=\"0 0 0\" Spacing=\"1 1 1\">\n";
    out << "    <Piece Extent=\"0 " << domain.Nx-1 << " 0 " << domain.Ny-1 << " 0 " << domain.Nz-1 << "\">\n";
    out << "      <PointData>\n";

    out << "        <DataArray type=\"Float64\" Name=\"Density\" format=\"ascii\">\n";
    for (int z = 0; z < domain.Nz; ++z)
        for (int y = 0; y < domain.Ny; ++y)
            for (int x = 0; x < domain.Nx; ++x) {
                double rho = 0.0;
                for (int i = 0; i < domain.Q; ++i) rho += f[get_idx(domain, x,y,z,i)];
                out << rho << " ";
            }
    out << "\n        </DataArray>\n";

    out << "        <DataArray type=\"Float64\" Name=\"Velocity\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (int z = 0; z < domain.Nz; ++z)
        for (int y = 0; y < domain.Ny; ++y)
            for (int x = 0; x < domain.Nx; ++x) {
                if (is_inside_box(domain,x,y,z)) { out << "0 0 0 "; continue; }
                double rho=0.0, ux=0.0, uy=0.0, uz=0.0;
                for (int i = 0; i < domain.Q; ++i) {
                    double fv = f[get_idx(domain, x,y,z,i)];
                    rho += fv; ux += fv*domain.cx[i]; uy += fv*domain.cy[i]; uz += fv*domain.cz[i];
                }
                ux = ux/rho + domain.F_x/(2.0*rho);
                uy /= rho; uz /= rho;
                out << ux << " " << uy << " " << uz << " ";
            }
    out << "\n        </DataArray>\n";

    out << "        <DataArray type=\"Float64\" Name=\"Obstacle\" format=\"ascii\">\n";
    for (int z = 0; z < domain.Nz; ++z)
        for (int y = 0; y < domain.Ny; ++y)
            for (int x = 0; x < domain.Nx; ++x)
                out << (is_inside_box(domain,x,y,z) ? 1.0 : 0.0) << " ";
    out << "\n        </DataArray>\n";

    out << "      </PointData>\n";
    out << "    </Piece>\n";
    out << "  </ImageData>\n";
    out << "</VTKFile>\n";
}

// A .pvd collection lets ParaView load the entire transient series with
// correct time stamps in one go: File -> Open -> channel.pvd
void write_pvd(const std::string &filename, const std::vector<std::pair<int,std::string>> &entries) {
    std::ofstream out(filename);
    out << "<?xml version=\"1.0\"?>\n<VTKFile type=\"Collection\" version=\"0.1\">\n  <Collection>\n";
    for (auto &e : entries)
        out << "    <DataSet timestep=\"" << e.first << "\" file=\"" << e.second << "\"/>\n";
    out << "  </Collection>\n</VTKFile>\n";
}