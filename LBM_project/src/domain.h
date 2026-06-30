#pragma once
#include <string>

struct Domain{
    // ---- Physical / numerical parameters --------------------------------
    // const double tau = 0.53;      // relaxation time -> nu = (tau-0.5)/3
    const int target_Re = 48;
    // double F_x;            // streamwise body force driving the flow

    const double tau = 0.6;     // relaxation time -> nu = (tau-0.5)/3
    const double F_x = 0.0001;  // streamwise body force driving the flow

    // ---- Domain size --------------------------------------------------------
    const int Nx = 100;
    const int Ny = 24;
    const int Nz = 24;
    const int Q = 19;
    const int volume = Nx * Ny * Nz;

    // ---- Obstacle details (needed for Re calculation) -----------------------
    const int box_ymin = 6;
    const int box_ymax = 8;
    const int block_height = box_ymax - box_ymin;

    const int box_zmin = 0;
    const int box_zmax = (Nz-1);

    const int box_xmin = 25;
    const int box_xmax = 30;

    // ---- Time stepping --------------------------------------------------------
    // LBM iterates in integer lattice steps (dt = 1 lu); there's no physical
    // dt to "accumulate" yet, so we loop over an integer step counter instead
    // of a floating point "time" variable (this also fixes a bug, see notes).
    const int nSteps       = 20000;  // total iterations - see note on diffusion time below
    const int outputEvery  = 2000;   // write a .vti snapshot every N steps
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
};
