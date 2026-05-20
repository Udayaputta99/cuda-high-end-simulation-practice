#pragma once
#include <vector>
#include <map>

// std::map<std::string,double> readConfig(const std::string& filename){
//     std::ifstream iFile(filename);
//     std::map<std::string,double> config;
//     if (!iFile.is_open()){
//         throw std::runtime_error("Error: could not open config file - "+filename);
        
//     }
//     std::string line;
//     while (std::getline(iFile,line)){
//         std::stringstream ss(line);
//         std::string key;
//         std::string value;
//         if (std::getline(ss,key,'=') && std::getline(ss,value)){
//             config[key] = std::stod(value);
//         } 
//     }
//     return config;
// }

bool checkOverlap(const std::vector<Particle>& p_arr, 
                    const std::vector<double>& pos, const double radius){
    for (int i=0; i<p_arr.size(); ++i){
        double dist = pow(p_arr[i].p_position[0]-pos[0],2) +
                        pow(p_arr[i].p_position[1]-pos[1],2) +
                        pow(p_arr[i].p_position[2]-pos[2],2);
        if (dist < 4*radius*radius) return false;
    }
    return true;
}

std::vector<double> randVec(const double min, const double max){
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define the range
    //std::cout<<"Random num is: "<<distr(gen)<<"\n";
    std::vector<double> vec;
    for (int i=0; i<3; ++i){
        double randNum = (double)distr(gen)/1.0;
        vec.push_back(randNum);
        // std::cout<<vec[i]<<",";
    }
    return vec;
}