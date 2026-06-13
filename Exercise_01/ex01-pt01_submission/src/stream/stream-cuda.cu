#include <chrono>
#include <cstdlib>
#include "../util.h"
#include "stream-util.h"

#define checkCudaError(ans) check((ans),__FILE__,__LINE__)
inline void check(cudaError_t err, const char *file, int line){
    if (err != cudaSuccess){
        std::cerr<<"CUDA Runtime error at: "<<file<<":"<<line<<std::endl;
        std::cerr<<cudaGetErrorString(err)<<" "<<file<<std::endl;
    }
}

__global__ void stream(size_t nx, const double *__restrict__ src, double *__restrict__ dest){
    auto tid = blockIdx.x*blockDim.x+threadIdx.x;
    if (tid < nx){
        dest[tid] = src[tid] + 1;
        //printf("Hello from thread: %d\n",tid);
    }

}

int main(int argc, char* argv[]){
    size_t nx, nItWarmup, nIt;
    parseCLA_1d(argc, argv, nx, nItWarmup, nIt);
    double* src;
    double* dest;
    double* d_src;
    double* d_dest;
    checkCudaError(cudaMallocHost((void**)&src, sizeof(double)*nx));
    checkCudaError(cudaMallocHost((void**)&dest, sizeof(double)*nx));
    checkCudaError(cudaMalloc((void**)&d_src, sizeof(double)*nx));
    checkCudaError(cudaMalloc((void**)&d_dest, sizeof(double)*nx));
    initStream(src, dest, nx);
    checkCudaError(cudaMemcpy(d_src, src, sizeof(double)*nx, cudaMemcpyHostToDevice));
    checkCudaError(cudaMemcpy(d_dest, dest, sizeof(double)*nx, cudaMemcpyHostToDevice));
    int numThreads {256};
    int numBlocks = (nx + numThreads - 1)/numThreads;
    //warmup iterations
    for (auto i=0; i<nItWarmup; ++i){
        stream<<<numBlocks,numThreads>>>(nx, d_src, d_dest);
        std::swap(d_src, d_dest);
    }

    checkCudaError(cudaDeviceSynchronize());
    auto start{std::chrono::steady_clock::now()};
    for (auto i=0; i<nIt; ++i){
        stream<<<numBlocks,numThreads>>>(nx, d_src, d_dest);
        std::swap(d_src, d_dest);
    }
    checkCudaError(cudaDeviceSynchronize());
    auto end{std::chrono::steady_clock::now()};

    checkCudaError(cudaMemcpy(src, d_src, sizeof(double)*nx, cudaMemcpyDeviceToHost));
    checkCudaError(cudaMemcpy(dest, d_dest, sizeof(double)*nx, cudaMemcpyDeviceToHost));

    checkSolutionStream(src, nx, nIt+nItWarmup);//check if the solution is correct
    printStats(end - start, nx, nIt, streamNumReads, streamNumWrites);

    checkCudaError(cudaFree(d_src));
    checkCudaError(cudaFree(d_dest));
    checkCudaError(cudaFreeHost(src));
    checkCudaError(cudaFreeHost(dest));
    // std::cout<<"Program exited without error\n";
    return 0;
}
