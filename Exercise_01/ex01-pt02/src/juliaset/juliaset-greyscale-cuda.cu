#include <thrust/complex.h>
#include <thrust/device_vector.h>
#include "../util.h"
#include "juliaset-util.h"
#include "../lodepng/lodepng.h"
#include <stdio.h>

#define checkCudaError(ans) check((ans),__FILE__,__LINE__)
inline void check(cudaError_t err, const char *file, int line){
    if (err != cudaSuccess){
        std::cerr<<"CUDA Runtime error at: "<<file<<":"<<line<<std::endl;
        std::cerr<<cudaGetErrorString(err)<<" "<<file<<std::endl;
    }
}

__device__ void scaleToRange(thrust::complex<double> &z,const int i, const int j,const int min,
                                            const int max,const int nx,const int ny){
    z = thrust::complex<double>(min + (double)i * (max - min) / nx,
                                min + (double)j * (max - min) / ny);
}

__global__ void polynomialIteration(unsigned char *d_image,const int nx,const int ny, 
                                    const int min, const int max, const int threshold, 
                                    const int max_iter, thrust::complex<double> c)
{
    int global_thread_id = blockDim.x*blockIdx.x+threadIdx.x;
    int global_pixel_id = nx*blockIdx.x+threadIdx.x;

    if (global_thread_id==global_pixel_id){
        int iter=0;
        thrust::complex<double> z (0.0,0.0);
        scaleToRange(z, threadIdx.x, blockIdx.x, min, max, nx, ny);
        while(thrust::abs(z)<threshold && iter < max_iter){
            z = z*z + c; 
            iter++;
        }

        if (iter==max_iter) d_image[global_pixel_id] = 0;
        else d_image[global_pixel_id] = (unsigned char)iter;
    }
}

int main(int argc, char *argv[]) {
    size_t nx, ny;
    parseCLA_1d(argc, argv, nx, ny);

    int min{-2};
    int max{2};
    int max_iter {128};
    int threshold {10};
    thrust::complex<double> c (-0.8, 0.2);
    std::vector<unsigned char> h_image;
    h_image.resize(nx*ny);
    
    unsigned char *d_image;
    size_t bytes = nx*ny*sizeof(unsigned char);
    checkCudaError(cudaMalloc(&d_image, bytes));
    checkCudaError(cudaMemcpy(d_image, h_image.data(), bytes, cudaMemcpyHostToDevice));

    // int numBlocks {ny};
    int numBlocks = 512;
    // int numThreads {(nx>1024)?1024:nx}; //max 1024 threads is allowed per block
    int numThreads = 512;

    //the idea is to give each x-layer to one block, each pixel iteration to one thread
    polynomialIteration<<<numBlocks,numThreads>>>(d_image,nx,ny,min,max,threshold,max_iter,c);
    checkCudaError(cudaDeviceSynchronize());

    cudaMemcpy(h_image.data(),d_image,bytes,cudaMemcpyDeviceToHost);
    // checkSolution(h_image.data(),nx*ny);
    checkCudaError(cudaFree(d_image));
    
    unsigned error = lodepng::encode("output.png", h_image, nx, ny, LCT_GREY, 8);

    if (error)
        std::cout << "PNG save failed: "
                  << lodepng_error_text(error) << std::endl;
    else
        std::cout << "Saved output.png\n";

    return 0;
}