#pragma once 
#include <vector>
#include <random>
#include <cmath>
class Particle{
    public:
        double p_mass; 
        // double p_radius;
        std::vector<double> p_position; 
        std::vector<double> p_velocity; 
        std::vector<double> p_acceleration; 

        Particle(const double& mass,
                 const std::vector<double>& position,     
                 const std::vector<double>& velocity,
                 const std::vector<double>& acceleration) : 
                 p_mass{mass},p_position{position},p_velocity{velocity}, p_acceleration{acceleration}{}
        
};

void gaussianVelocityDistribution(std::vector<double>& velocity, const double T,std::mt19937& generator){
   
    const double mean = 0.0; //mean velocity is zero 
    const double stddev = std::sqrt(T); //since we are using reduced units, only T is necessary
    std::normal_distribution<double> gaussian(mean,stddev);
    velocity[0] = gaussian(generator);
    velocity[1] = gaussian(generator);
    velocity[2] = gaussian(generator);
    
}

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