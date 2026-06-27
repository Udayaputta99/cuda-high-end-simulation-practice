void write_vtu(const std::string& filename, const std::vector<float>& f, int Nx, int Ny, int Nz) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    int volume = Nx * Ny * Nz;

    // Local arrays to unpack your SoA distribution functions into macroscopic properties
    std::vector<float> rho_arr(volume);
    std::vector<float> ux_arr(volume);
    std::vector<float> uy_arr(volume);
    std::vector<float> uz_arr(volume);

    // Lattice velocities mirroring your D3Q19 constants
    const int cx[19] = { 0,  1, -1,  0,  0,  0,  0,  1, -1,  1, -1,  1, -1,  1, -1,  0,  0,  0,  0 };
    const int cy[19] = { 0,  0,  0,  1, -1,  0,  0,  1,  1, -1, -1,  0,  0,  0,  0,  1, -1,  1, -1 };
    const int cz[19] = { 0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  1,  1, -1, -1,  1,  1, -1, -1 };
    const float F_x = 0.0001f; // Matching your body force

    // 1. Calculate Macroscopic fields for the snapshot
    for (int z = 0; z < Nz; ++z) {
        for (int y = 0; y < Ny; ++y) {
            for (int x = 0; x < Nx; ++x) {
                int v_idx = z * Nx * Ny + y * Nx + x;
                
                float rho = 0.0f;
                float ux = 0.0f, uy = 0.0f, uz = 0.0f;

                for (int i = 0; i < 19; ++i) {
                    // SoA lookup index matching your get_idx logic
                    int f_idx = i * volume + v_idx;
                    float f_val = f[f_idx];
                    
                    rho += f_val;
                    ux  += f_val * cx[i];
                    uy  += f_val * cy[i];
                    uz  += f_val * cz[i];
                }
                ux = (ux / rho) + (F_x / (2.0f * rho));
                uy /= rho;
                uz /= rho;

                rho_arr[v_idx] = rho;
                ux_arr[v_idx] = ux;
                uy_arr[v_idx] = uy;
                uz_arr[v_idx] = uz;
            }
        }
    }

    // 2. Write VTU XML Header
    out << "<?xml version=\"1.0\"?>\n";
    out << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n";
    out << "  <UnstructuredGrid>\n";
    out << "    <Piece NumberOfPoints=\"" << volume << "\" NumberOfCells=\"" << volume << "\">\n";

    // 3. Write Point Coordinates
    out << "      <Points>\n";
    out << "        <DataArray type=\"Float32\" Name=\"Points\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (int z = 0; z < Nz; ++z) {
        for (int y = 0; y < Ny; ++y) {
            for (int x = 0; x < Nx; ++x) {
                out << x << " " << y << " " << z << "\n";
            }
        }
    }
    out << "        </DataArray>\n";
    out << "      </Points>\n";

    // 4. Write Cell Metadata (Treating every point as a single vertex cell)
    out << "      <Cells>\n";
    out << "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";
    for (int i = 0; i < volume; ++i) {
        out << i << "\n";
    }
    out << "        </DataArray>\n";
    
    out << "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
    for (int i = 1; i <= volume; ++i) {
        out << i << "\n";
    }
    out << "        </DataArray>\n";
    
    out << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";
    for (int i = 0; i < volume; ++i) {
        out << "1\n"; // VTK_VERTEX type is 1
    }
    out << "        </DataArray>\n";
    out << "      </Cells>\n";

    // 5. Write Point Data Fields (Density, Velocity Vector)
    out << "      <PointData>\n";
    
    // Density
    out << "        <DataArray type=\"Float32\" Name=\"Density\" format=\"ascii\">\n";
    for (int i = 0; i < volume; ++i) out << rho_arr[i] << "\n";
    out << "        </DataArray>\n";
    
    // Velocity Vector
    out << "        <DataArray type=\"Float32\" Name=\"Velocity\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (int i = 0; i < volume; ++i) {
        out << ux_arr[i] << " " << uy_arr[i] << " " << uz_arr[i] << "\n";
    }
    out << "        </DataArray>\n";
    
    out << "      </PointData>\n";
    out << "    </Piece>\n";
    out << "  </UnstructuredGrid>\n";
    out << "</VTKFile>\n";

    out.close();
    std::cout << "[INFO] Saved simulation snapshot to: " << filename << std::endl;
}