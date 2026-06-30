#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <utility>
#include <cstdlib>
#include "write_vti.h"
#include "domain.h"
#include "functions.h"

// ======================================================================
//  D3Q19 Lattice Boltzmann - channel flow with a rectangular obstacle
//  Periodic in X (streamwise) and Z (spanwise), bounce-back walls at
//  Y = 0 and Y = domain.Ny-1.
// ======================================================================

int main() {
    Domain domain;
    // force_cal(domain);
    std::cout << "Allocating memory...\n";
    std::vector<double> f_old(domain.Q * domain.volume, 0.0);
    std::vector<double> f_new(domain.Q * domain.volume, 0.0);

    std::cout << "Initializing flow field (rho = 1, u = 0)...\n";
    for (int z = 0; z < domain.Nz; ++z)
        for (int y = 0; y < domain.Ny; ++y)
            for (int x = 0; x < domain.Nx; ++x)
                for (int i = 0; i < domain.Q; ++i) {
                    double feq = get_f_eq(domain, 1.0, i, 0.0, 0.0, 0.0);
                    f_old[get_idx(domain,x,y,z,i)] = feq;
                    f_new[get_idx(domain,x,y,z,i)] = feq;
                }

    system(("mkdir -p " + domain.outDir).c_str());
    std::vector<std::pair<int,std::string>> pvdEntries;

    std::cout << "Starting time loop (" << domain.nSteps << " steps)...\n";

    for (int step = 0; step <= domain.nSteps; ++step) {

        for (int z = 0; z < domain.Nz; ++z) {
            for (int y = 0; y < domain.Ny; ++y) {
                for (int x = 0; x < domain.Nx; ++x) {

                    double f_local[domain.Q];

                    for (int i = 0; i < domain.Q; ++i) {
                        int src_x = (x - domain.cx[i] + domain.Nx) % domain.Nx;   // periodic in X
                        int src_y = y - domain.cy[i];
                        int src_z = (z - domain.cz[i] + domain.Nz) % domain.Nz;   // periodic in Z

                        bool insideBox     = is_inside_box(domain, src_x, src_y, src_z);
                        bool outsideDomain = (src_y < 0 || src_y >= domain.Ny);

                        if (insideBox || outsideDomain) {
                            // full-way bounce-back: f_old here holds the
                            // post-collision (pre-stream) state from last
                            // step, so this correctly reflects momentum.
                            f_local[i] = f_old[get_idx(domain, x, y, z, domain.inv[i])];
                        } else {
                            f_local[i] = f_old[get_idx(domain, src_x, src_y, src_z, i)];
                        }
                    }

                    if (is_inside_box(domain, x, y, z)) {
                        for (int i = 0; i < domain.Q; ++i)
                            f_new[get_idx(domain, x, y, z, i)] = get_f_eq(domain, 1.0, i, 0.0, 0.0, 0.0);
                        continue;
                    }

                    double rho=0.0, ux=0.0, uy=0.0, uz=0.0;
                    for (int i = 0; i < domain.Q; ++i) {
                        rho += f_local[i];
                        ux  += f_local[i]*domain.cx[i];
                        uy  += f_local[i]*domain.cy[i];
                        uz  += f_local[i]*domain.cz[i];
                    }
                    ux /= rho; uy /= rho; uz /= rho;
                    ux += domain.F_x / (2.0 * rho);   // Guo-style half-force velocity shift

                    for (int i = 0; i < domain.Q; ++i) {
                        double feq = get_f_eq(domain, rho, i, ux, uy, uz);
                        f_new[get_idx(domain, x, y, z, i)] = f_local[i] - (f_local[i] - feq) / domain.tau;
                    }
                }
            }
        }

        std::swap(f_old, f_new);

        if (step % domain.monitorEvery == 0) {
            int mx = domain.Nx/2, my = domain.Ny/2, mz = domain.Nz/2;
            double rho=0.0, ux=0.0;
            for (int i = 0; i < domain.Q; ++i) {
                double fv = f_old[get_idx(domain, mx,my,mz,i)];
                rho += fv; ux += fv*domain.cx[i];
            }
            ux = ux/rho + domain.F_x/(2.0*rho);
            std::cout << "step " << step << "  rho_mid=" << rho << "  ux_mid=" << ux << "\n";
        }

        if (step % domain.outputEvery == 0) {
            std::ostringstream fn;
            fn << domain.outDir << "/channel_" << std::setw(5) << std::setfill('0') << step << ".vti";
            write_vti(fn.str(), f_old, domain);
            
            std::ostringstream pvdname;
            pvdname << "channel_" << std::setw(5) << std::setfill('0') << step << ".vti";
            pvdEntries.push_back({step, pvdname.str()});
        }
    }

    write_pvd(domain.outDir + "/channel.pvd", pvdEntries);

    std::cout << "\nFinal velocity profile (Ux) across Y at mid X,Z:\n";
    std::cout << "Y\tUx\n--------------------\n";
    int sx = domain.Nx/2, sz = domain.Nz/2;
    for (int y = 0; y < domain.Ny; ++y) {
        if (is_inside_box(domain, sx, y, sz)) {
            std::cout << y << "\t(solid)\n";
            continue;
        }
        double rho=0.0, ux=0.0;
        for (int i = 0; i < domain.Q; ++i) {
            double fv = f_old[get_idx(domain,sx,y,sz,i)];
            rho += fv; ux += fv*domain.cx[i];
        }
        ux = ux/rho + domain.F_x/(2.0*rho);
        std::cout << y << "\t" << ux << "\n";
    }

    std::cout << "\nDone. Open " << domain.outDir << "/channel.pvd in ParaView.\n";
    return 0;
}