#pragma once
#include "domain.h"

inline double get_f_eq(Domain& domain, double rho, int i, double ux, double uy, double uz) {
    double cu   = domain.cx[i]*ux + domain.cy[i]*uy + domain.cz[i]*uz;
    double u_sq = ux*ux + uy*uy + uz*uz;
    return domain.w[i] * rho * (1.0 + 3.0*cu + 4.5*cu*cu - 1.5*u_sq);
}

inline int get_idx(Domain& domain, int x, int y, int z, int i) {
    return i * domain.volume + (z * domain.Nx * domain.Ny + y * domain.Nx + x);
}

// ---- Obstacle definition --------------------------------------------------
// box_zmax was set to 40, but Nz = 40 means valid
// z-indices only go from 0 to 39. So "z <= 40" was true for every single
// z-layer in the domain -> the "box" actually spanned the ENTIRE spanwise
// (Z) extent
inline bool is_inside_box(Domain& domain, int x, int y, int z) {
    return (x >= domain.box_xmin && x <= domain.box_xmax &&
            y >= domain.box_ymin && y <= domain.box_ymax &&
            z >= domain.box_zmin && z <= domain.box_zmax);
}

inline void force_cal(Domain& domain) {
    // 1. Calculate lattice viscosity
    double nu = (2.0 * domain.tau - 1.0) / 6.0;

    // 3. Find target velocity at the block to hit that Re
    double U_target = (domain.target_Re * nu) / static_cast<double>(domain.block_height);

    // 4. Calculate exact F_x based on analytical Poiseuille channel flow profile
    domain.F_x = (8.0 * nu * U_target) / std::pow(static_cast<double>(domain.Ny), 2);

    // Print diagnostic info to the terminal so you can verify it on launch
    std::cout << "=========================================\n";
    std::cout << " LBM PHYSICS AUTO-CONFIG:\n";
    std::cout << " -> Nu_lu:     " << nu << "\n";
    std::cout << " -> Target Re: " << domain.target_Re << "\n";
    std::cout << " -> Target U:  " << U_target << " (Max safe limit is 0.15)\n";
    std::cout << " -> Calculated F_x: " << domain.F_x << "\n";
    std::cout << "=========================================\n\n";
}