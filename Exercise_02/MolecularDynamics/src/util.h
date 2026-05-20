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
