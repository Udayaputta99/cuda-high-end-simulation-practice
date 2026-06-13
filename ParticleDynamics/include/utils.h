#pragma once 
#include <string>
#include <iostream>

void pvdInit(std::ofstream& pvd);
void pvdWriteVTU(std::ofstream& pvd, int count, std::string filename);
void pvdFinalize(std::ofstream& pvd);
void writeVTU(const std::string& filename, const double* pos, int N, double radius);
void printSolverDetails(const Domain& domain);

inline void outputFile(int count, const double* pos, const int N, const double radius){
    std::string fname = "particle_"+std::to_string(count)+".vtu";
    writeVTU(fname,pos,N,radius);    
}

