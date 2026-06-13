#include <complex>

#include "../util.h"
#include "../lodepng/lodepng.h"


int main(int argc, char *argv[]) {
    size_t nx, ny;
    parseCLA_1d(argc, argv, nx, ny);

    int min{-2};
    int max{2};
    
    std::complex<double> c(-0.8, 0.2);
    std::vector<unsigned char> image;
    image.resize(nx*ny*3);

    size_t total_iter{128};
    size_t threshold{10};    
    std::vector<std::vector<int>> palette = {
        {0, 7, 100},
        {32, 107, 203},
        {237, 255, 255},
        {255, 170, 0},
        {0, 2, 0}
    };
    for (auto j = 0; j < ny; ++j) {
        for (auto i = 0; i < nx; ++i) {
            double real = min + (double)i * (max - min) / nx;
            double imag = max - (double)j * (max - min) / ny;
            std::complex<double> p(real, imag);
            size_t iter{0};
            while (std::abs(p) < threshold && iter < total_iter) {
                p = p * p + c; 
                ++iter;
            }
            auto base = (j*nx+i)*3;
            iter = iter - log(log(std::abs(p))); //for contour smoothing
            if (iter == total_iter){
                image[base+0] = 0;
                image[base+1] = 0;
                image[base+2] = 0;
            }
            else{
                auto color = palette[iter%palette.size()];
                image[base+0] = color[0];
                image[base+1] = color[1];
                image[base+2] = color[2];
            }
            
        }
    }

    unsigned error = lodepng::encode("output.png", image, nx, ny, LCT_RGB, 8);

    if (error)
        std::cout << "PNG save failed: "<< lodepng_error_text(error) << std::endl;
    else
        std::cout << "Saved output.png\n";
    return 0;
}
