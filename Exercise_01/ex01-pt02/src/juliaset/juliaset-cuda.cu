#define cimg_use_png

#include <cuComplex.h>
#include <string>
#include "../util.h"
#include "../lodepng/lodepng.h"
#include "../CImg/CImg.h"


struct Parameters {
    unsigned char* image;
    size_t nx;
    size_t ny;
    size_t threshold;
    size_t total_iter;
    int max;
    int min; 
    cuDoubleComplex c;
    double dx;
    double dy;
};

__global__ void juliaset(Parameters param) {
    size_t i = blockIdx.x * blockDim.x + threadIdx.x;
    size_t j = blockIdx.y * blockDim.y + threadIdx.y;
    if (i < param.nx && j < param.ny) {
        double real = param.min + i * param.dx;
        double imag = param.max - j * param.dy;
        cuDoubleComplex p = make_cuDoubleComplex(real, imag);
        size_t iter{0};
        while (cuCabs(p) < param.threshold && iter < param.total_iter) {
            p = cuCadd(cuCmul(p, p), param.c);
            ++iter;
        }

        param.image[j * param.nx + i] = (iter == param.total_iter) ? 0 : 255 * (double)iter / param.total_iter;
    }
}


int main(int argc, char *argv[]) {
    size_t nx, ny;
    double cr, ci;
    parseCLA_1d(argc, argv, nx, ny, cr, ci);

    size_t size = sizeof(unsigned char) * nx * ny;

    unsigned char *dest, *d_dest;
    cudaMallocHost(&dest, size);
    cudaMalloc(&d_dest, size);

    int max = 2;
    int min = -2;
    double dx = static_cast<double>(max - min) / nx;
    double dy = static_cast<double>(max - min) / ny;
    Parameters params {
        .image = d_dest,
        .nx = nx,
        .ny = ny,
        .threshold = 2,
        .total_iter = 128,
        .max = max,
        .min = min,
        .c = make_cuDoubleComplex(cr, ci),
        .dx = dx,
        .dy = dy
    };
    
    size_t bDim{16};
    dim3 numThreadsPerBlock(bDim, bDim);
    dim3 numBlocks((nx + bDim - 1)/ bDim, (ny + bDim - 1)/ bDim );

    juliaset<<<numBlocks, numThreadsPerBlock>>>(params);

    cudaMemcpy(dest, d_dest, size, cudaMemcpyDeviceToHost);

    std::string filename = "images/juliaset-c=(" + std::to_string(cr) + ", " + std::to_string(ci) + ").png";
    unsigned error = lodepng::encode(filename, dest, nx, ny, LCT_GREY, 8);

    if (error) {
        std::cout << "PNG save failed: "
                  << lodepng_error_text(error) << std::endl;
                  return 1;
    }
    else
        std::cout << "Saved PNG file\n";

    cimg_library::CImg<unsigned char> img(dest, nx, ny, 1, 1, true);
    cimg_library::CImgDisplay display(img);

    while (!display.is_closed()) {
        display.wait();
    }

    cudaFree(d_dest);
    cudaFreeHost(d_dest);

    return 0;
}