#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>

// --- 1. SIMULATION PARAMETERS ---
const int Nx = 64;   // Channel Length
const int Ny = 32;   // Channel Height (Walls at y=0 and y=Ny-1)
const int Nz = 16;   // Channel Width
const int Q = 19;
const int Volume = Nx * Ny * Nz;

const float tau = 0.6f;          // Relaxation time (controls viscosity)
const float F_x = 0.0001f;       // Driving body force in X direction
const int max_steps = 5000;      // Number of time steps

// --- 2. D3Q19 LATTICE CONSTANTS ---
const float w[19] = {
    1.0f/3.0f,                                                              // i=0
    1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, // i=1..6
    1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f,                         // i=7..10
    1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f,                         // i=11..14
    1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f                          // i=15..18
};

const int cx[19] = { 0,  1, -1,  0,  0,  0,  0,  1, -1,  1, -1,  1, -1,  1, -1,  0,  0,  0,  0 };
const int cy[19] = { 0,  0,  0,  1, -1,  0,  0,  1,  1, -1, -1,  0,  0,  0,  0,  1, -1,  1, -1 };
const int cz[19] = { 0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  1,  1, -1, -1,  1,  1, -1, -1 };

// Opposite direction mapping for bounce-back wall BCs
const int nos[19] = { 0,  2,  1,  4,  3,  6,  5, 10,  9,  8,  7, 14, 13, 12, 11, 18, 17, 16, 15 };

// --- 3. HELPER FUNCTIONS ---
// Structure of Arrays (SoA) Indexing Helper
inline int get_idx(int x, int y, int z, int dir) {
    return dir * Volume + (z * Nx * Ny + y * Nx + x);
}

// Equilibrium function utility
inline float get_feq(int i, float rho, float ux, float uy, float uz) {
    float cu = cx[i]*ux + cy[i]*uy + cz[i]*uz;
    float u_sq = ux*ux + uy*uy + uz*uz;
    return w[i] * rho * (1.0f + 3.0f*cu + 4.5f*cu*cu - 1.5f*u_sq);
}

int main() {
    std::cout << "Allocating memory for " << Nx << "x" << Ny << "x" << Nz << " grid..." << std::endl;
    
    // Allocate data populations
    std::vector<float> f_old(Q * Volume, 0.0f);
    std::vector<float> f_new(Q * Volume, 0.0f);

    // --- STEP 3: INITIALIZATION ---
    std::cout << "Initializing simulation fields to equilibrium (Fluid at rest)..." << std::endl;
    for (int z = 0; z < Nz; ++z) {
        for (int y = 0; y < Ny; ++y) {
            for (int x = 0; x < Nx; ++x) {
                float rho_init = 1.0f;
                float ux_init = 0.0f;
                float uy_init = 0.0f;
                float uz_init = 0.0f;

                for (int i = 0; i < Q; ++i) {
                    float feq_val = get_feq(i, rho_init, ux_init, uy_init, uz_init);
                    f_old[get_idx(x, y, z, i)] = feq_val;
                    f_new[get_idx(x, y, z, i)] = feq_val;
                }
            }
        }
    }

    // --- PHASE 2: MAIN TIME LOOP ---
    std::cout << "Starting time iteration loop..." << std::endl;
    for (int step = 0; step < max_steps; ++step) {
        
        // This set of loops maps 1:1 to your future CUDA threads!
        for (int z = 0; z < Nz; ++z) {
            for (int y = 0; y < Ny; ++y) {
                for (int x = 0; x < Nx; ++x) {
                    
                    float f_local[19];

                    // STEP 4: THE PULL (Streaming + Boundary Conditions)
                    for (int i = 0; i < Q; ++i) {
                        // Subtract vector direction to look backward to source
                        int src_x = (x - cx[i] + Nx) % Nx; // Periodic boundary on X
                        int src_y = y - cy[i];
                        int src_z = z - cz[i];

                        // Wall boundary check (No-slip bounce-back condition on Y and Z boundaries)
                        if (src_y < 0 || src_y >= Ny || src_z < 0 || src_z >= Nz) {
                            f_local[i] = f_old[get_idx(x, y, z, nos[i])]; // Bounce-back from self
                        } else {
                            f_local[i] = f_old[get_idx(src_x, src_y, src_z, i)]; // Pull from neighbor
                        }
                    }

                    // STEP 5: CALCULATE MACROSCOPIC VARIABLES
                    float rho = 0.0f;
                    float ux = 0.0f, uy = 0.0f, uz = 0.0f;
                    for (int i = 0; i < Q; ++i) {
                        rho += f_local[i];
                        ux  += f_local[i] * cx[i];
                        uy  += f_local[i] * cy[i];
                        uz  += f_local[i] * cz[i];
                    }
                    ux /= rho; uy /= rho; uz /= rho;

                    // STEP 6: INJECT DRIVING FORCE (Forcing term)
                    ux += (F_x / (2.0f * rho));

                    // STEP 7: COLLISION AND WRITE OUT
                    for (int i = 0; i < Q; ++i) {
                        float feq_val = get_feq(i, rho, ux, uy, uz);
                        // BGK relaxation calculation written directly to f_new
                        f_new[get_idx(x, y, z, i)] = f_local[i] - (1.0f / tau) * (f_local[i] - feq_val);
                    }
                }
            }
        }

        // STEP 8: POINTER SWAP
        std::swap(f_old, f_new);

        // Progress Print tracking centerline velocity evolution
        if (step % 500 == 0) {
            // Sample a node right in the middle of the channel
            int mid_x = Nx / 2; int mid_y = Ny / 2; int mid_z = Nz / 2;
            float rho_mid = 0; float ux_mid = 0;
            for (int i = 0; i < Q; ++i) {
                float f_val = f_old[get_idx(mid_x, mid_y, mid_z, i)];
                rho_mid += f_val;
                ux_mid  += f_val * cx[i];
            }
            ux_mid = (ux_mid / rho_mid) + (F_x / (2.0f * rho_mid));
            std::cout << "Step: " << step << " | Centerline Velocity Ux: " << ux_mid << std::endl;
        }
    }

    // --- PHASE 3: POST-PROCESSING & VERIFICATION ---
    std::cout << "\nSimulation Finished. Verifying Velocity Profile across Y-axis...\n";
    std::cout << "Y-Coord\tVelocity (Ux)\n";
    std::cout << "-----------------------\n";
    
    int sample_x = Nx / 2;
    int sample_z = Nz / 2;

    for (int y = 0; y < Ny; ++y) {
        float rho = 0.0f;
        float ux = 0.0f;
        for (int i = 0; i < Q; ++i) {
            float f_val = f_old[get_idx(sample_x, y, sample_z, i)];
            rho += f_val;
            ux  += f_val * cx[i];
        }
        ux = (ux / rho) + (F_x / (2.0f * rho));
        
        std::cout << y << "\t" << ux << std::endl;
    }

    return 0;
}