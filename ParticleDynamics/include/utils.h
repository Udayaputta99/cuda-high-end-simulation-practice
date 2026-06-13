#pragma once 
#include <string>

void writeVTU(const std::string& filename, const double* pos, int N, double radius);
void printSolverDetails(const Domain& domain);

inline void outputFile(int count, const double* pos, const int N, const double radius){
    std::string fname = "particle_"+std::to_string(count)+".vtu";
    writeVTU(fname,pos,N,radius);    
}