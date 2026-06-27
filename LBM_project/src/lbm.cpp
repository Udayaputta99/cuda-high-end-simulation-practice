#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "writeVTU.h"


double tau = 0.6f;          // Relaxation time (controls viscosity)
double F_x = 0.0001f;      // Driving body force in X direction

// const int dimension = 3;

const int Nx = 100;
const int Ny = 10;
const int Nz = 40;
const int speeds = 19;
const int volume = Nx * Ny * Nz;
double startTime = 0.0;
double endTime = 10.0;
double deltaT = 0.2;

const double w[19] = {
    1.0f/3.0f,                                                              // i=0
    1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, 1.0f/18.0f, // i=1..6
    1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f,                         // i=7..10
    1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f,                         // i=11..14
    1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f                          // i=15..18
};

const int cx[19] = { 0,  1, -1,  0,  0,  0,  0,  1, -1,  1, -1,  1, -1,  1, -1,  0,  0,  0,  0 };
const int cy[19] = { 0,  0,  0,  1, -1,  0,  0,  1,  1, -1, -1,  0,  0,  0,  0,  1, -1,  1, -1 };
const int cz[19] = { 0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  1,  1, -1, -1,  1,  1, -1, -1 };

const int inv[19] = {0, 2, 1, 4, 3, 6, 5, 10, 9, 8, 7, 14, 13, 12, 11, 18, 17, 16, 15};

double get_f_eq(const double rho, const int i, double ux, double uy, double uz){
    double cu = cx[i]*ux + cy[i]*uy + cz[i]*uz;
    double u_sq = ux*ux + uy*uy + uz*uz;
    double f_eq = w[i] * rho * (1.0f + 3.0f*cu + 4.5f*cu*cu - 1.5f*u_sq);
    return f_eq;
}

// SoA indexing:
inline int get_idx(int x, int y, int z, const int i) {
    return i * volume + (z * Nx * Ny + y * Nx + x);
}

bool is_inside_box(int x, int y, int z){
    int box_xmin = (int)Nx*0.06;
    int box_ymin = (int)Ny*0.06;
    int box_zmin = (int)Nz*0.06;
    int box_xmax = (int)Nx*0.07;
    int box_ymax = (int)Ny*0.07;
    int box_zmax = (int)Nz*0.07;
    if (x >= box_xmin && x <= box_xmax &&
        y >= box_ymin && y <= box_ymax &&
        z >= box_zmin && z <= box_zmax){
            return true;
        }
    else return false;
}

int main() {
    std::cout<<"Allocating the memory\n"<< std::endl;

    std::vector<float> f_old(speeds * volume, 0.0f);
    std::vector<float> f_new(speeds * volume, 0.0f);

    std::cout<<"Initialize the flow\n"<< std::endl;

    for(int z = 0; z<Nz; ++z){
        for(int y = 0; y<Ny; ++y){
            for(int x = 0; x<Nx; ++x){

                float rho_init = 1.0f;
                double ux_init = 0;
                double uy_init = 0;
                double uz_init = 0;
                for(int i = 0; i<speeds; ++i){
                    double feq_val = get_f_eq(rho_init, i, ux_init, uy_init, uz_init);
                    f_old[get_idx(x, y, z, i)] = feq_val;
                    f_new[get_idx(x, y, z, i)] = feq_val;
                }
            }
        }
    }

    std::cout<<"Start the time iteration loop\n"<< std::endl;

    int step =0;

    for(double time = startTime; time<=endTime; time += deltaT, step++){

        std::cout << "[DEBUG] Entering time step: " << time << std::endl;
        
        for (int z = 0; z < Nz; ++z) {
            for (int y = 0; y < Ny; ++y) {
                for (int x = 0; x < Nx; ++x) {

                    double f_local[speeds];

                    for (int i = 0; i < speeds; ++i) {
                        // Subtract vector direction to look backward to source
                        int src_x = (x - cx[i] + Nx) % Nx; // Periodic boundary on X
                        int src_y = y - cy[i];
                        int src_z = (z - cz[i] + Nz) % Nz; // Periodic boundary on Z
                        
                        //check for box
                        bool isInsideBox = is_inside_box(src_x,src_y,src_z);
                        //check for domain
                        bool isOutsideDomain = (src_y<0 || src_y >= Ny)?true:false;
                        //if (not true for box and domain)Pull from neighbor
                        //else
                        
                        if (isInsideBox || isOutsideDomain) {
                            f_local[i] = f_old[get_idx(x, y, z, inv[i])]; // Bounce-back from self
                        } else {
                            f_local[i] = f_old[get_idx(src_x, src_y, src_z, i)]; // Pull from neighbor
                        }

                    }

                    // Collision step
                    float rho = 0.0f;
                    float ux = 0.0f, uy = 0.0f, uz = 0.0f;
                    for (int i = 0; i < speeds; ++i) {
                        rho += f_local[i];                      // 𝜌 = Σ f_i
                        ux  += f_local[i] * cx[i];       //𝜌*𝑢𝛼  = Σ f_i cx[i]--> this is momentum, not velocity
                        uy  += f_local[i] * cy[i];
                        uz  += f_local[i] * cz[i];
                    }
                    ux /= rho; uy /= rho; uz /= rho;            //𝑢𝛼  = (Σ f_i cx[i])/𝜌

                    // inject driving force
                    ux += (F_x / (2.0f * rho));

                    for (int i = 0; i < speeds; ++i) {
                        float feq_val = get_f_eq(rho, i, ux, uy, uz);
                        // BGK relaxation calculation written directly to f_new
                        f_new[get_idx(x, y, z, i)] = f_local[i] - (1.0f / tau) * (f_local[i] - feq_val);
                    }
                }
            }
        }
        std::swap(f_old, f_new);
        // Inside main time loop, or right after the loop finishes:

        if (step % 1 == 0){
            std::string filename = "output_" + std::to_string(step) + ".vtu";
            write_vtu(filename, f_old, Nx, Ny, Nz);
        }

        // Progress Print tracking centerline velocity evolution
        if (fmod(time, 500.0) < 1e-12) {
            // Sampling a node in the middle of the channel
            int mid_x = Nx / 2; int mid_y = Ny / 2; int mid_z = Nz / 2;
            double rho_mid = 0; float ux_mid = 0;
            for (int i = 0; i < speeds; ++i) {
                double f_val = f_old[get_idx(mid_x, mid_y, mid_z, i)];
                rho_mid += f_val;
                ux_mid  += f_val * cx[i];
            }
            std::cout<<"rho mid: "<<rho_mid<<std::endl;
            ux_mid = (ux_mid / rho_mid) + (F_x / (2.0f * rho_mid));
            std::cout << "Step: " << time << " | Centerline Velocity Ux: " << ux_mid << std::endl;
        }
    }

    // POST-PROCESSING & VERIFICATION
    std::cout << "\nSimulation Finished. Verifying Velocity Profile across Y-axis...\n";
    std::cout << "Y-Coord\tVelocity (Ux)\n";
    std::cout << "-----------------------\n";
    
    int sample_x = Nx / 2;
    int sample_z = Nz / 2;

    for (int y = 0; y < Ny; ++y) {
        float rho = 0.0f;
        float ux = 0.0f;
        for (int i = 0; i < speeds; ++i) {
            float f_val = f_old[get_idx(sample_x, y, sample_z, i)];
            rho += f_val;
            ux  += f_val * cx[i];
        }
        ux = (ux / rho) + (F_x / (2.0f * rho));
        
        std::cout << y << "\t" << ux << std::endl;
    }

    return 0;

}
