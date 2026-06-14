#include <cstdio>
#include <string>
#include <iostream>

#include "../include/ConfigReader.hpp"
#include "../include/ParticleIO.hpp"

struct Vec3 {
    double x, y, z;
};

#define checkCudaError(ans) check((ans),__FILE__,__LINE__)
inline void check(cudaError_t err, const char *file, int line){
    if (err != cudaSuccess){
        std::cerr<<"CUDA Runtime error at: "<<file<<":"<<line<<std::endl;
        std::cerr<<cudaGetErrorString(err)<<" "<<file<<std::endl;
    }
}

__device__ int clampCellCoord(int value, int numberOfCells) {
    if (value < 0) {
        return 0;
    }

    if (value >= numberOfCells) {
        return numberOfCells - 1;
    }

    return value;
}

__device__ Vec3 computeParticleToParticleForce(Vec3 disp, Vec3 velI, Vec3 velJ, double overlap, double distance, SimulationConfig config) {
    
        double unitx = disp.x / distance;
        double unity = disp.y / distance;
        double unitz = disp.z / distance;

        double dvx = velI.x - velJ.x;
        double dvy = velI.y - velJ.y;
        double dvz = velI.z - velJ.z;

        double vn = dvx*unitx + dvy*unity + dvz*unitz;

        double forceMagnitude = config.springConstant*overlap - config.dissipationConstant*vn;
        
        double fx = forceMagnitude*unitx;
        double fy = forceMagnitude*unity;
        double fz = forceMagnitude*unitz;

        return {fx, fy, fz};
}

__device__ double computeKineticEnergy(Vec3 v, double mass) {
    return (mass*(v.x*v.x + v.y*v.y + v.z*v.z))/2;
}

__device__ void computeWallToParticleForce(
    Vec3 pos,
    Vec3 velocity,
    double radius,
    double distanceToWall,
    Vec3 normalVector,
    SimulationConfig config,
    double& fx, double& fy, double& fz
) {
    double overlap = radius - distanceToWall;

    if (overlap > 0.0) {
        double vn = velocity.x * normalVector.x + velocity.y * normalVector.y + velocity.z * normalVector.z;

        double forceMagnitude = config.springConstant * overlap - config.dissipationConstant * vn;

        fx += forceMagnitude * normalVector.x;
        fy += forceMagnitude * normalVector.y;
        fz += forceMagnitude * normalVector.z;
    }
}

__global__ void computeForce(Particles particles, int* cellsCount, int* cellParticles, SimulationConfig config, size_t maxParticles) {
    size_t start = blockIdx.x * blockDim.x + threadIdx.x;
    size_t stride = gridDim.x * blockDim.x;

    for (size_t i = start; i < config.numberOfParticles; i+=stride) {
        int numberOfCells = static_cast<int>(config.numberOfCells);

        int x_coord = static_cast<int>(particles.x[i] / config.cellSize);
        int y_coord = static_cast<int>(particles.y[i] / config.cellSize);
        int z_coord = static_cast<int>(particles.z[i] / config.cellSize);

        x_coord = clampCellCoord(x_coord, numberOfCells);
        y_coord = clampCellCoord(y_coord, numberOfCells);
        z_coord = clampCellCoord(z_coord, numberOfCells);
    


        double fx = 0;
        double fy = particles.mass[i] * config.gravity;
        double fz = 0;

        for (int dz = -1; dz <= 1; ++dz)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    int nx = x_coord + dx;
                    int ny = y_coord + dy;
                    int nz = z_coord + dz;

                    if (nx < 0 || nx >= config.numberOfCells ||
                        ny < 0 || ny >= config.numberOfCells ||
                        nz < 0 || nz >= config.numberOfCells) {
                            continue;
                    }

                    size_t neighborIdx = config.numberOfCells*config.numberOfCells*nz + config.numberOfCells*ny + nx;
                    int start = maxParticles*neighborIdx;
                    int count = cellsCount[neighborIdx];
                    for (auto j = 0; j < count; ++j) {
                        int particleIndex = cellParticles[start + j];
                        if (i != particleIndex) {
                            double dx = particles.x[i] - particles.x[particleIndex];
                            double dy = particles.y[i] - particles.y[particleIndex];
                            double dz = particles.z[i] - particles.z[particleIndex];

                            double distance = sqrt(dx*dx + dy*dy + dz*dz);
                            double overlap = particles.radius[i] + particles.radius[particleIndex] - distance;
                            if (distance > 1e-12 && overlap > 0.0) {
                                auto force = computeParticleToParticleForce({dx, dy, dz},
                                                        {particles.vx[i], particles.vy[i], particles.vz[i]}, 
                                                        {particles.vx[particleIndex], particles.vy[particleIndex], particles.vz[particleIndex]}, 
                                                        overlap, distance, config);
                                fx += force.x;
                                fy += force.y;
                                fz += force.z;
                            }
                        }
                    }
                }
            }
        }
        
        computeWallToParticleForce({particles.x[i], particles.y[i],  particles.z[i]}, 
                                   {particles.vx[i], particles.vy[i], particles.vz[i]}, particles.radius[i],
                                    particles.x[i], {1.0, 0.0, 0.0}, config,
                                    fx, fy, fz);
        computeWallToParticleForce({particles.x[i], particles.y[i],  particles.z[i]}, 
                                   {particles.vx[i], particles.vy[i], particles.vz[i]}, particles.radius[i],
                                    config.cubeLength - particles.x[i], {-1.0, 0.0, 0.0}, config,
                                    fx, fy, fz);
        computeWallToParticleForce({particles.x[i], particles.y[i],  particles.z[i]}, 
                                   {particles.vx[i], particles.vy[i], particles.vz[i]}, particles.radius[i],
                                    particles.y[i], {0.0, 1.0, 0.0}, config,
                                    fx, fy, fz);
        computeWallToParticleForce({particles.x[i], particles.y[i],  particles.z[i]}, 
                                   {particles.vx[i], particles.vy[i], particles.vz[i]}, particles.radius[i],
                                    config.cubeLength - particles.y[i], {0.0, -1.0, 0.0}, config,
                                    fx, fy, fz);
        computeWallToParticleForce({particles.x[i], particles.y[i],  particles.z[i]}, 
                                   {particles.vx[i], particles.vy[i], particles.vz[i]}, particles.radius[i],
                                    particles.z[i], {0.0, 0.0, 1.0}, config,
                                    fx, fy, fz);
        computeWallToParticleForce({particles.x[i], particles.y[i],  particles.z[i]}, 
                                   {particles.vx[i], particles.vy[i], particles.vz[i]}, particles.radius[i],
                                    config.cubeLength - particles.z[i], {0.0, 0.0, -1.0}, config,
                                    fx, fy, fz);

        particles.ax[i] = fx / particles.mass[i];
        particles.ay[i] = fy / particles.mass[i];
        particles.az[i] = fz / particles.mass[i];

    }
}

__global__ void buildCellList(Particles particles, int* cellsCount, int* cellParticles, SimulationConfig config, size_t maxParticles) {
    size_t start = blockIdx.x * blockDim.x + threadIdx.x;
    size_t stride = gridDim.x * blockDim.x;

    for (size_t i = start; i < config.numberOfParticles; i+=stride) {
        int numberOfCells = static_cast<int>(config.numberOfCells);

        int x_coord = static_cast<int>(particles.x[i] / config.cellSize);
        int y_coord = static_cast<int>(particles.y[i] / config.cellSize);
        int z_coord = static_cast<int>(particles.z[i] / config.cellSize);

        x_coord = clampCellCoord(x_coord, numberOfCells);
        y_coord = clampCellCoord(y_coord, numberOfCells);
        z_coord = clampCellCoord(z_coord, numberOfCells);
    

        size_t index = config.numberOfCells*config.numberOfCells*z_coord + config.numberOfCells*y_coord + x_coord;

        int count = atomicAdd(&cellsCount[index], 1);
        if (count < maxParticles) {
            cellParticles[index*maxParticles + count] = i;
        } else {
            printf("Maximum number of Particles exceeded");
        }
    }

}

__global__ void updatePositionsAndHalfVelocities(Particles particles, SimulationConfig config) {

    size_t start = blockIdx.x * blockDim.x + threadIdx.x;
    size_t stride = gridDim.x * blockDim.x;

    for (size_t i = start; i < config.numberOfParticles; i+=stride) {
        particles.x[i] = particles.x[i] + particles.vx[i]*config.timeStep + (particles.ax[i]*config.timeStep*config.timeStep)/2;
        particles.y[i] = particles.y[i] + particles.vy[i]*config.timeStep + (particles.ay[i]*config.timeStep*config.timeStep)/2;
        particles.z[i] = particles.z[i] + particles.vz[i]*config.timeStep + (particles.az[i]*config.timeStep*config.timeStep)/2;


        particles.vx[i] = particles.vx[i] + (particles.ax[i]*config.timeStep)/2;
        particles.vy[i] = particles.vy[i] + (particles.ay[i]*config.timeStep)/2;
        particles.vz[i] = particles.vz[i] + (particles.az[i]*config.timeStep)/2;
    }
    
}

__global__ void completeVelocityUpdate(Particles particles, SimulationConfig config) {
    size_t start = blockIdx.x * blockDim.x + threadIdx.x;
    size_t stride = gridDim.x * blockDim.x;

    for (size_t i = start; i < config.numberOfParticles; i+=stride) {
        particles.vx[i] = particles.vx[i] + (particles.ax[i]*config.timeStep)/2;
        particles.vy[i] = particles.vy[i] + (particles.ay[i]*config.timeStep)/2;
        particles.vz[i] = particles.vz[i] + (particles.az[i]*config.timeStep)/2;
    }
}

void allocateMemory(Particles& host, Particles& device, size_t size) {
    checkCudaError(cudaMallocHost(&host.x, size));
    checkCudaError(cudaMalloc(&device.x, size));
    checkCudaError(cudaMallocHost(&host.y, size));
    checkCudaError(cudaMalloc(&device.y, size));
    checkCudaError(cudaMallocHost(&host.z, size));
    checkCudaError(cudaMalloc(&device.z, size));
    checkCudaError(cudaMallocHost(&host.vx, size));
    checkCudaError(cudaMalloc(&device.vx, size));
    checkCudaError(cudaMallocHost(&host.vy, size));
    checkCudaError(cudaMalloc(&device.vy, size));
    checkCudaError(cudaMallocHost(&host.vz, size));
    checkCudaError(cudaMalloc(&device.vz, size));
    checkCudaError(cudaMallocHost(&host.mass, size));
    checkCudaError(cudaMalloc(&device.mass, size));
    checkCudaError(cudaMallocHost(&host.radius, size));
    checkCudaError(cudaMalloc(&device.radius, size));
    // checkCudaError(cudaMallocHost(&host.ax, size));
    checkCudaError(cudaMalloc(&device.ax, size));
    // checkCudaError(cudaMallocHost(&host.ay, size));
    checkCudaError(cudaMalloc(&device.ay, size));
    // checkCudaError(cudaMallocHost(&host.az, size));
    checkCudaError(cudaMalloc(&device.az, size));
}

void freeMemory(Particles &host, Particles &device, size_t size) {
    checkCudaError(cudaFreeHost(host.x));
    checkCudaError(cudaFree(device.x));
    checkCudaError(cudaFreeHost(host.y));
    checkCudaError(cudaFree(device.y));
    checkCudaError(cudaFreeHost(host.z));
    checkCudaError(cudaFree(device.z));
    checkCudaError(cudaFreeHost(host.vx));
    checkCudaError(cudaFree(device.vx));
    checkCudaError(cudaFreeHost(host.vy));
    checkCudaError(cudaFree(device.vy));
    checkCudaError(cudaFreeHost(host.vz));
    checkCudaError(cudaFree(device.vz));
    checkCudaError(cudaFreeHost(host.mass));
    checkCudaError(cudaFree(device.mass));
    checkCudaError(cudaFreeHost(host.radius));
    checkCudaError(cudaFree(device.radius));
    // checkCudaError(cudaFreeHost(host.ax));
    checkCudaError(cudaFree(device.ax));
    // checkCudaError(cudaFreeHost(host.ay));
    checkCudaError(cudaFree(device.ay));
    // checkCudaError(cudaFreeHost(host.az));
    checkCudaError(cudaFree(device.az));
}

void copyToDevice(Particles &host, Particles &device, size_t size) {
    checkCudaError(cudaMemcpy(device.x, host.x, size, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(device.y, host.y, size, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(device.z, host.z, size, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(device.vx, host.vx, size, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(device.vy, host.vy, size, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(device.vz, host.vz, size, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(device.mass, host.mass, size, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(device.radius, host.radius, size, cudaMemcpyHostToDevice));
}

void copyFromDevice(Particles &host, Particles &device, size_t size) {
    checkCudaError(cudaMemcpy(host.x, device.x, size, cudaMemcpyDeviceToHost));
    checkCudaError(cudaMemcpy(host.y, device.y, size, cudaMemcpyDeviceToHost));
    checkCudaError(cudaMemcpy(host.z, device.z, size, cudaMemcpyDeviceToHost));
    checkCudaError(cudaMemcpy(host.vx, device.vx, size, cudaMemcpyDeviceToHost));
    checkCudaError(cudaMemcpy(host.vy, device.vy, size, cudaMemcpyDeviceToHost));
    checkCudaError(cudaMemcpy(host.vz, device.vz, size, cudaMemcpyDeviceToHost));
    checkCudaError(cudaMemcpy(host.radius, device.radius, size, cudaMemcpyDeviceToHost));
}


int main(int argc, char *argv[]) {
    
    SimulationConfig config = ConfigReader::readConfig();

    size_t size = sizeof(double) * config.numberOfParticles;
    size_t maxParticlesInCell = 256;
    size_t totalCells = config.numberOfCells * config.numberOfCells * config.numberOfCells;
    size_t cellsSize = totalCells * sizeof(int);


    Particles host, device;
    allocateMemory(host, device, size);
    ParticleIO::readParticlesFromCSV(host, config.numberOfParticles);
    copyToDevice(host, device, size);

    
    int *cellsCount, *cellParticles;
    double *h_energy, *d_energy;
    checkCudaError(cudaMalloc(&cellsCount, cellsSize));
    checkCudaError(cudaMalloc(&cellParticles, totalCells * maxParticlesInCell * sizeof(int)));
    checkCudaError(cudaMemset(cellsCount, 0, cellsSize));

    checkCudaError(cudaMallocHost(&h_energy, size));
    checkCudaError(cudaMalloc(&d_energy, size));
    checkCudaError(cudaMemset(d_energy, 0, size));
    
    auto numThreadsPerBlock = 256;
    auto numBlocks = 108*32;

    cudaMemset(cellsCount, 0, cellsSize);
    buildCellList<<<numBlocks, numThreadsPerBlock>>>(device, cellsCount, cellParticles, config, maxParticlesInCell);
    checkCudaError(cudaGetLastError());
    checkCudaError(cudaDeviceSynchronize());

    cudaMemset(device.ax, 0, size);
    cudaMemset(device.ay, 0, size);
    cudaMemset(device.az, 0, size);
    computeForce<<<numBlocks, numThreadsPerBlock>>>(device, cellsCount, cellParticles, config, maxParticlesInCell);
    checkCudaError(cudaGetLastError());
    checkCudaError(cudaDeviceSynchronize());

    ParticleIO::writeParticlesVTK("vtk_output", 0, host, config.numberOfParticles);
    for (auto i = 0; i < config.numberOfSteps; ++i) {

        updatePositionsAndHalfVelocities<<<numBlocks, numThreadsPerBlock>>>(device, config);
        checkCudaError(cudaGetLastError());
        checkCudaError(cudaDeviceSynchronize());

        cudaMemset(cellsCount, 0, cellsSize);
        buildCellList<<<numBlocks, numThreadsPerBlock>>>(device, cellsCount, cellParticles, config, maxParticlesInCell);
        checkCudaError(cudaGetLastError());
        checkCudaError(cudaDeviceSynchronize());

        cudaMemset(device.ax, 0, size);
        cudaMemset(device.ay, 0, size);
        cudaMemset(device.az, 0, size);
        computeForce<<<numBlocks, numThreadsPerBlock>>>(device, cellsCount, cellParticles, config, maxParticlesInCell);
        checkCudaError(cudaGetLastError());
        checkCudaError(cudaDeviceSynchronize());        

        completeVelocityUpdate<<<numBlocks, numThreadsPerBlock>>>(device, config);
        checkCudaError(cudaGetLastError());
        checkCudaError(cudaDeviceSynchronize());

        if ((i + 1) % config.writeInterval == 0) {
            copyFromDevice(host, device, size);
            ParticleIO::writeParticlesVTK("vtk_output", i + 1, host, config.numberOfParticles);
        }
    }

    freeMemory(host, device, size);

    return 0;
}