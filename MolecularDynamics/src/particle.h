#pragma once 
#include <vector>

class Particle{
    public:
        double p_mass; //in grams 
        double p_radius; // in mm
        std::vector<double> p_position; // in mm
        std::vector<double> p_velocity; // in mm/s
        std::vector<double> p_acceleration; // in mm/s^2

        Particle(const double& mass, const double& radius,
                 const std::vector<double>& position,     
                 const std::vector<double>& velocity,
                 const std::vector<double>& acceleration) : 
                 p_mass{mass},p_radius{radius},p_position{position},p_velocity{velocity}, p_acceleration{acceleration}{}
        
};

inline void updatePositionVelocityFirstHalf(std::vector<Particle>& particles, double deltaTime){
    int n = particles.size();
    for (int i=0; i<n; ++i){
        particles[i].p_position[0] += particles[i].p_velocity[0]*deltaTime 
                                + particles[i].p_acceleration[0]*deltaTime*deltaTime*0.5;

        particles[i].p_position[1] += particles[i].p_velocity[1]*deltaTime
                                + particles[i].p_acceleration[1]*deltaTime*deltaTime*0.5;

        particles[i].p_position[2] += particles[i].p_velocity[2]*deltaTime
                                + particles[i].p_acceleration[2]*deltaTime*deltaTime*0.5;

        // position[i] = particles[i].p_position;

        particles[i].p_velocity[0] += particles[i].p_acceleration[0]*deltaTime*0.5;
        particles[i].p_velocity[1] += particles[i].p_acceleration[1]*deltaTime*0.5; 
        particles[i].p_velocity[2] += particles[i].p_acceleration[2]*deltaTime*0.5; 
    }
}

inline void updateVelocitySecondHalf(std::vector<Particle>& particles, double deltaTime){
    int n = particles.size();
    for (int i=0; i<n; ++i){
        particles[i].p_velocity[0] += particles[i].p_acceleration[0]*deltaTime*0.5;
        particles[i].p_velocity[1] += particles[i].p_acceleration[1]*deltaTime*0.5; 
        particles[i].p_velocity[2] += particles[i].p_acceleration[2]*deltaTime*0.5; 
    }  
}