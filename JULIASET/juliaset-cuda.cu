#include <string>
#include <cstdio>
#include <thrust/complex.h>
#include "cudautils.h"
#include "./lodepng/lodepng.h"

struct Parameters{
    //struct to store the parameters
    double pixel_range_min {-2};
    double pixel_range_max {2};
    bool rgb {false};
    int nx {800};
    int ny {800};
    int total_iter {128};
    double threshold {2};
    double c_real{-0.8};
    double c_imag{0.2};
    };

__global__ void main_greyscale(const Parameters params, unsigned char* d_image){ //DO NOT PASS params as ref!
    //function to generate the greyscale image of the juliaset
    int col = blockIdx.x*blockDim.x + threadIdx.x;
    int row = blockIdx.y*blockDim.y + threadIdx.y;
    thrust::complex<double> c (params.c_real,params.c_imag);
    if (col<params.nx && row<params.ny){
        int iter = 0;
        double imag = params.pixel_range_max - (double)row*(params.pixel_range_max - params.pixel_range_min) / params.ny;
        double real = params.pixel_range_min + (double)col*(params.pixel_range_max - params.pixel_range_min) / params.nx;
        thrust::complex<double> p (real,imag);
        while (thrust::abs(p)<params.threshold && iter < params.total_iter){
            p = p*p + c;
            ++iter;
        }
        d_image[row*params.nx+col] = (iter==params.total_iter)? 0:255*(double)iter/params.total_iter; 
    }
    }

__global__ void main_rgb(const Parameters params, unsigned char* d_image){
    // function to generate the rgb image of the juliaset
    const int palette[5][3] = {
        {0, 7, 100},
        {32, 107, 203},
        {237, 255, 255},
        {255, 170, 0},
        {0, 2, 0}
    };
    const int palette_size = 5;

    int col = blockIdx.x*blockDim.x+threadIdx.x;
    int row = blockIdx.y*blockDim.y+threadIdx.y;
    thrust::complex<double> c (params.c_real, params.c_imag);
    if (col < params.nx && row < params.ny){
        double imag = params.pixel_range_max - (double)row*(params.pixel_range_max - params.pixel_range_min) / params.ny;
        double real = params.pixel_range_min + (double)col*(params.pixel_range_max - params.pixel_range_min) / params.nx;
        thrust::complex<double> p (real, imag);
        int iter = 0;
        while (thrust::abs(p)<params.threshold && iter < params.total_iter){
            p = p*p + c;
            ++iter;
        }
        int base = (row*params.nx+col)*3;
        double t = (double)iter/params.total_iter;
        double scaled = t*(palette_size-1);
        int idx = (int)scaled;     
        double blend = scaled - idx;
        int nextIdx = min(idx + 1, palette_size - 1);
        d_image[base+0] = (iter == params.total_iter) ? 0:(unsigned char)(palette[idx][0] * (1-blend) + palette[nextIdx][0] * blend);
        d_image[base+1] = (iter == params.total_iter) ? 0:(unsigned char)(palette[idx][1] * (1-blend) + palette[nextIdx][1] * blend);
        d_image[base+2] = (iter == params.total_iter) ? 0:(unsigned char)(palette[idx][2] * (1-blend) + palette[nextIdx][2] * blend);
    }
}


int main(int argc, char *argv[]) {

    Parameters params; 

    std::vector<unsigned char> h_image; //host image vector
    unsigned char* d_image; //device image vector
    
    if (argc>1){
        params.nx = std::stoi(argv[1]);
        params.ny = std::stoi(argv[2]);
        params.c_real = std::stod(argv[3]);
        params.c_imag = std::stod(argv[4]); 
        if (argc>5 && std::string(argv[5])=="rgb") params.rgb = true;
    }
    size_t bytes = params.nx*params.ny*sizeof(unsigned char);
    std::string fname = std::to_string(params.c_real)+"+"+std::to_string(params.c_imag)+"i.png";
    printf("Image Size: %d X %d\n",params.nx, params.ny);
    printf("c: %f + %fi\n",params.c_real, params.c_imag);
    printf("Total Iterations: %d\n",params.total_iter);
    printf("Threshold: %d\n",params.threshold);
    printf("RGB: %s\n",params.rgb?"True":"False");
    int base = 16;
    dim3 threads(base,base);
    dim3 blocks((params.nx+threads.x-1)/threads.x,
                (params.ny+threads.y-1)/threads.x);
    
    unsigned error;
    printf("nx=%d ny=%d bytes=%zu\n", params.nx, params.ny, bytes);
    printf("blocks=(%d,%d) threads=(%d,%d)\n", blocks.x, blocks.y, threads.x, threads.y);
    if (params.rgb){
        h_image.resize(params.nx*params.ny*3);
        checkCudaError(cudaMalloc(&d_image,bytes*3)); // Multiplier for three channels 
        checkCudaError(cudaMemcpy(d_image,h_image.data(),bytes,cudaMemcpyHostToDevice));
        main_rgb<<<blocks,threads>>>(params,d_image);
        cudaDeviceSynchronize();
        checkCudaError(cudaMemcpy(h_image.data(),d_image,bytes*3,cudaMemcpyDeviceToHost));
        error = lodepng::encode(fname, h_image, params.nx, params.ny, LCT_RGB, 8);
    } 
    else{
        h_image.resize(params.nx*params.ny);
        checkCudaError(cudaMalloc(&d_image,bytes));
        checkCudaError(cudaMemcpy(d_image,h_image.data(),bytes,cudaMemcpyHostToDevice)); 
        
        main_greyscale<<<blocks,threads>>>(params,d_image);
        cudaDeviceSynchronize();
        checkCudaError(cudaMemcpy(h_image.data(),d_image,bytes,cudaMemcpyDeviceToHost));
        error = lodepng::encode(fname, h_image, params.nx, params.ny, LCT_GREY, 8);
    } 
    checkCudaError(cudaFree(d_image));
    if (error)
        printf("PNG save failed: %s\n", lodepng_error_text(error));
    else
        printf("%s saved\n",fname.c_str());
    return 0;
}