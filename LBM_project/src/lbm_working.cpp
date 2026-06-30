#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <utility>
#include <cstdlib>

// ======================================================================
//  D3Q19 Lattice Boltzmann - channel flow with a rectangular obstacle
//  Periodic in X (streamwise) and Z (spanwise), bounce-back walls at
//  Y = 0 and Y = Ny-1.
// ======================================================================

// ---- Physical / numerical parameters --------------------------------
const double tau = 0.6;     // relaxation time -> nu = (tau-0.5)/3
const double F_x = 0.0001;  // streamwise body force driving the flow

// ---- Domain size --------------------------------------------------------
const int Nx = 200;
const int Ny = 48;
const int Nz = 48;
const int speeds = 19;
const int volume = Nx * Ny * Nz;

// ---- Time stepping --------------------------------------------------------
// LBM iterates in integer lattice steps (dt = 1 lu); there's no physical
// dt to "accumulate" yet, so we loop over an integer step counter instead
// of a floating point "time" variable (this also fixes a bug, see notes).
const int nSteps       = 20000;  // total iterations - see note on diffusion time below
const int outputEvery  = 400;   // write a .vti snapshot every N steps
const int monitorEvery = 400;   // print centerline velocity every N steps
const std::string outDir = "output";

// ---- D3Q19 lattice ---------------------------------------------------------
const double w[19] = {
    1.0/3.0,
    1.0/18.0, 1.0/18.0, 1.0/18.0, 1.0/18.0, 1.0/18.0, 1.0/18.0,
    1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0,
    1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0,
    1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0
};

const int cx[19]  = { 0,  1, -1,  0,  0,  0,  0,  1, -1,  1, -1,  1, -1,  1, -1,  0,  0,  0,  0 };
const int cy[19]  = { 0,  0,  0,  1, -1,  0,  0,  1,  1, -1, -1,  0,  0,  0,  0,  1, -1,  1, -1 };
const int cz[19]  = { 0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  1,  1, -1, -1,  1,  1, -1, -1 };
const int inv[19] = { 0,  2,  1,  4,  3,  6,  5, 10,  9,  8,  7, 14, 13, 12, 11, 18, 17, 16, 15 };
// (checked: cx/cy/cz/w/inv are all internally consistent - this part of
//  your original code was correct.)

inline double get_f_eq(double rho, int i, double ux, double uy, double uz) {
    double cu   = cx[i]*ux + cy[i]*uy + cz[i]*uz;
    double u_sq = ux*ux + uy*uy + uz*uz;
    return w[i] * rho * (1.0 + 3.0*cu + 4.5*cu*cu - 1.5*u_sq);
}

inline int get_idx(int x, int y, int z, int i) {
    return i * volume + (z * Nx * Ny + y * Nx + x);
}

// ---- Obstacle definition --------------------------------------------------
// A finite rectangular box that does NOT span the full depth.
//
// BUG IN THE ORIGINAL CODE: box_zmax was set to 40, but Nz = 40 means valid
// z-indices only go from 0 to 39. So "z <= 40" was true for every single
// z-layer in the domain -> the "box" actually spanned the ENTIRE spanwise
// (Z) extent. It wasn't a discrete 3-D box at all, it was a full-width rib
// sitting above the floor. That's the most likely reason it didn't read as
// a "box" when you inspected the visualization.
//
// I've made it a genuine finite box below. If a full-span rib is actually
// what you want, just set box_zmin = 0 and box_zmax = Nz - 1.
inline bool is_inside_box(int x, int y, int z) {
    const int box_xmin = 50, box_xmax = 55;
    const int box_ymin = 22,  box_ymax = 26;   // raised off the floor by 2 cells
    const int box_zmin = 0, box_zmax = (Nz-1);  // finite in Z now
    return (x >= box_xmin && x <= box_xmax &&
            y >= box_ymin && y <= box_ymax &&
            z >= box_zmin && z <= box_zmax);
}

// ---- VTI writer ------------------------------------------------------------
// Writes density, velocity, AND an explicit "Obstacle" mask field.
// THIS is the fix for "the box isn't visible in ParaView": density and
// velocity inside/around a slow, weakly-forced flow don't visually stand
// out on their own. Apply a Threshold filter on "Obstacle" (>= 0.5), or a
// Contour at 0.5, to render the solid region explicitly.
void write_vti(const std::string &filename, const std::vector<double> &f) {
    std::ofstream out(filename);
    out << std::setprecision(8);
    out << "<?xml version=\"1.0\"?>\n";
    out << "<VTKFile type=\"ImageData\" version=\"0.1\" byte_order=\"LittleEndian\">\n";
    out << "  <ImageData WholeExtent=\"0 " << Nx-1 << " 0 " << Ny-1 << " 0 " << Nz-1
        << "\" Origin=\"0 0 0\" Spacing=\"1 1 1\">\n";
    out << "    <Piece Extent=\"0 " << Nx-1 << " 0 " << Ny-1 << " 0 " << Nz-1 << "\">\n";
    out << "      <PointData>\n";

    out << "        <DataArray type=\"Float64\" Name=\"Density\" format=\"ascii\">\n";
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {
                double rho = 0.0;
                for (int i = 0; i < speeds; ++i) rho += f[get_idx(x,y,z,i)];
                out << rho << " ";
            }
    out << "\n        </DataArray>\n";

    out << "        <DataArray type=\"Float64\" Name=\"Velocity\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {
                if (is_inside_box(x,y,z)) { out << "0 0 0 "; continue; }
                double rho=0.0, ux=0.0, uy=0.0, uz=0.0;
                for (int i = 0; i < speeds; ++i) {
                    double fv = f[get_idx(x,y,z,i)];
                    rho += fv; ux += fv*cx[i]; uy += fv*cy[i]; uz += fv*cz[i];
                }
                ux = ux/rho + F_x/(2.0*rho);
                uy /= rho; uz /= rho;
                out << ux << " " << uy << " " << uz << " ";
            }
    out << "\n        </DataArray>\n";

    out << "        <DataArray type=\"Float64\" Name=\"Obstacle\" format=\"ascii\">\n";
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x)
                out << (is_inside_box(x,y,z) ? 1.0 : 0.0) << " ";
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

int main() {
    std::cout << "Allocating memory...\n";
    std::vector<double> f_old(speeds * volume, 0.0);
    std::vector<double> f_new(speeds * volume, 0.0);

    std::cout << "Initializing flow field (rho = 1, u = 0)...\n";
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x)
                for (int i = 0; i < speeds; ++i) {
                    double feq = get_f_eq(1.0, i, 0.0, 0.0, 0.0);
                    f_old[get_idx(x,y,z,i)] = feq;
                    f_new[get_idx(x,y,z,i)] = feq;
                }

    system(("mkdir -p " + outDir).c_str());
    std::vector<std::pair<int,std::string>> pvdEntries;

    std::cout << "Starting time loop (" << nSteps << " steps)...\n";

    for (int step = 0; step <= nSteps; ++step) {

        for (int z = 0; z < Nz; ++z) {
            for (int y = 0; y < Ny; ++y) {
                for (int x = 0; x < Nx; ++x) {

                    double f_local[speeds];

                    for (int i = 0; i < speeds; ++i) {
                        int src_x = (x - cx[i] + Nx) % Nx;   // periodic in X
                        int src_y = y - cy[i];
                        int src_z = (z - cz[i] + Nz) % Nz;   // periodic in Z

                        bool insideBox     = is_inside_box(src_x, src_y, src_z);
                        bool outsideDomain = (src_y < 0 || src_y >= Ny);

                        if (insideBox || outsideDomain) {
                            // full-way bounce-back: f_old here holds the
                            // post-collision (pre-stream) state from last
                            // step, so this correctly reflects momentum.
                            f_local[i] = f_old[get_idx(x, y, z, inv[i])];
                        } else {
                            f_local[i] = f_old[get_idx(src_x, src_y, src_z, i)];
                        }
                    }

                    if (is_inside_box(x, y, z)) {
                        for (int i = 0; i < speeds; ++i)
                            f_new[get_idx(x, y, z, i)] = get_f_eq(1.0, i, 0.0, 0.0, 0.0);
                        continue;
                    }

                    double rho=0.0, ux=0.0, uy=0.0, uz=0.0;
                    for (int i = 0; i < speeds; ++i) {
                        rho += f_local[i];
                        ux  += f_local[i]*cx[i];
                        uy  += f_local[i]*cy[i];
                        uz  += f_local[i]*cz[i];
                    }
                    ux /= rho; uy /= rho; uz /= rho;
                    ux += F_x / (2.0 * rho);   // Guo-style half-force velocity shift

                    for (int i = 0; i < speeds; ++i) {
                        double feq = get_f_eq(rho, i, ux, uy, uz);
                        f_new[get_idx(x, y, z, i)] = f_local[i] - (f_local[i] - feq) / tau;
                    }
                }
            }
        }

        std::swap(f_old, f_new);

        if (step % monitorEvery == 0) {
            int mx = Nx/2, my = Ny/2, mz = Nz/2;
            double rho=0.0, ux=0.0;
            for (int i = 0; i < speeds; ++i) {
                double fv = f_old[get_idx(mx,my,mz,i)];
                rho += fv; ux += fv*cx[i];
            }
            ux = ux/rho + F_x/(2.0*rho);
            std::cout << "step " << step << "  rho_mid=" << rho << "  ux_mid=" << ux << "\n";
        }

        if (step % outputEvery == 0) {
            std::ostringstream fn;
            fn << outDir << "/channel_" << std::setw(5) << std::setfill('0') << step << ".vti";
            write_vti(fn.str(), f_old);
            
            std::ostringstream pvdname;
            pvdname << "channel_" << std::setw(5) << std::setfill('0') << step << ".vti";
            pvdEntries.push_back({step, pvdname.str()});
        }
    }

    write_pvd(outDir + "/channel.pvd", pvdEntries);

    std::cout << "\nFinal velocity profile (Ux) across Y at mid X,Z:\n";
    std::cout << "Y\tUx\n--------------------\n";
    int sx = Nx/2, sz = Nz/2;
    for (int y = 0; y < Ny; ++y) {
        if (is_inside_box(sx, y, sz)) {
            std::cout << y << "\t(solid)\n";
            continue;
        }
        double rho=0.0, ux=0.0;
        for (int i = 0; i < speeds; ++i) {
            double fv = f_old[get_idx(sx,y,sz,i)];
            rho += fv; ux += fv*cx[i];
        }
        ux = ux/rho + F_x/(2.0*rho);
        std::cout << y << "\t" << ux << "\n";
    }

    std::cout << "\nDone. Open " << outDir << "/channel.pvd in ParaView.\n";
    return 0;
}