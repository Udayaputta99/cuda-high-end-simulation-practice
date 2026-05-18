#pragma once
#include <fstream>
#include <string>
#include <vector>

void writeVTU(const std::string& filename, const double* position, int n, double radius)
{
    std::ofstream f(filename);
    f << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\">\n";
    f << "  <UnstructuredGrid>\n";
    f << "  <Piece NumberOfPoints=\"" << n << "\" NumberOfCells=\"0\">\n";
    f << "    <PointData Scalars=\"Radius\">\n";
    f << "      <DataArray type=\"Float64\" Name=\"Radius\" format=\"ascii\">\n";
    for (int i = 0; i < n; i++)
        f << "        " << radius << "\n";
    f << "      </DataArray>\n";
    f << "    </PointData>\n";
    f << "    <Points>\n";
    f << "      <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (int i = 0; i < n; i++)
        f << "        " << position[3*i+0] << " " << position[3*i+1] << " " << position[3*i+2] << "\n";
        // f << "        " << pos[3*i+0] << " " << pos[3*i+1] << " " << 0 << "\n";
    f << "      </DataArray>\n";
    f << "    </Points>\n";
    f << "    <Cells>\n";
    f << "      <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\"/>\n";
    f << "      <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\"/>\n";
    f << "      <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\"/>\n";
    f << "    </Cells>\n";
    f << "  </Piece>\n";
    f << "  </UnstructuredGrid>\n";
    f << "</VTKFile>\n";
}
void writePlane(const std::string& filename)
{
    std::ofstream f(filename);
    f << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\">\n";
    f << "  <UnstructuredGrid>\n";
    f << "  <Piece NumberOfPoints=\"4\" NumberOfCells=\"1\">\n";
    f << "    <Points>\n";
    f << "      <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    // define your 4 corners — here a 100x100 plane at z=0
    f << "        0   0   0\n";
    f << "        30 0   0\n";
    f << "        30 30 0\n";
    f << "        0   30 0\n";
    f << "      </DataArray>\n";
    f << "    </Points>\n";
    f << "    <Cells>\n";
    // connectivity: indices of the 4 corners
    f << "      <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";
    f << "        0 1 2 3\n";
    f << "      </DataArray>\n";
    // offsets: cumulative count of connectivity entries (1 cell with 4 points = 4)
    f << "      <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
    f << "        4\n";
    f << "      </DataArray>\n";
    // cell type 9 = VTK_QUAD
    f << "      <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";
    f << "        9\n";
    f << "      </DataArray>\n";
    f << "    </Cells>\n";
    f << "  </Piece>\n";
    f << "  </UnstructuredGrid>\n";
    f << "</VTKFile>\n";
}