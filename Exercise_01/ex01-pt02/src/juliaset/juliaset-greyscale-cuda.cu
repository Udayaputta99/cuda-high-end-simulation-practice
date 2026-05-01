#include <thrust/complex.h>
#include <thrust/device_vector.h>
#include "../util.h"
#include "juliaset-util.h"
//#include "../lodepng/lodepng.h"

#define checkCudaError(ans) check((ans),__FILE__,__LINE__)
inline void check(cudaError_t err, const char *file, int line){
    if (err != cudaSuccess){
        std::cerr<<"CUDA Runtime error at: "<<file<<":"<<line<<std::endl;
        std::cerr<<cudaGetErrorString(err)<<" "<<file<<std::endl;
    }
}

__global__ void polynomialIteration(unsigned char *d_image,const auto nx,const auto ny, 
                                    const auto min, const auto max, const auto threshold, 
                                    const auto max_iter, thrust::complex<double> c)
{
    int global_thread_id = blockDim.x*blockIdx.x+threadIdx.x;
    int global_pixel_id = nx*blockIdx.x+threadIdx.x;
    if (global_thread_id==global_pixel_id){
        // int iter=0;
        // thrust::complex<double> z (0.0,0.0);
        // scaleToRange(z, threadIdx.x, blockIdx.x, min, max, nx, ny);
        // while(thrust::abs(z)<threshold && iter < max_iter){
        //     z = z*z + c; 
        //     iter++;
        // }
        // if (iter==max_iter) d_image[global_pixel_id] = 0;
        // else d_image[global_pixel_id] = (unsigned char)iter;
        d_image[global_pixel_id] = 0;
    }
}

__device__ void scaleToRange(thrust::complex<double> &z,const auto i, const auto j,const auto min,
                                            const auto max,const auto nx,const auto ny){
    z = thrust::complex<double>(min + (double)i * (max - min) / nx,
                                min + (double)i * (max - min) / nx);
}

int main(int argc, char *argv[]) {
    size_t nx, ny;
    parseCLA_1d(argc, argv, nx, ny);

    auto min{-2};
    auto max{2};
    auto max_iter {128};
    auto threshold {10};
    thrust::complex<double> c (-0.8, 0.2);
    std::vector<unsigned char> h_image;
    h_image.resize(nx*ny);
    
    unsigned char *d_image = nullptr;
    size_t bytes = nx*ny*sizeof(unsigned char);
    checkCudaError(cudaMalloc(&d_image, bytes));
    checkCudaError(cudaMemcpy(d_image, h_image.data(), bytes, cudaMemcpyHostToDevice));

    auto numBlocks {ny};
    auto numThreads {nx};
    
    polynomialIteration<<<numBlocks,numThreads>>>(d_image,nx,ny,min,max,threshold,max_iter,c);
    checkCudaError(cudaDeviceSynchronize());

    cudaMemcpy(h_image.data(),d_image,bytes,cudaMemcpyDeviceToHost);
    checkSolution(h_image.data(),nx*ny);
    checkCudaError(cudaFree(d_image));
    
    // unsigned error = lodepng::encode("output.png", image, nx, ny, LCT_GREY, 8);

    // if (error)
    //     std::cout << "PNG save failed: "
    //               << lodepng_error_text(error) << std::endl;
    // else
    //     std::cout << "Saved output.png\n";

    return 0;
}