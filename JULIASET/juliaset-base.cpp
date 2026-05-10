#include <complex>
#include <string>
#include <iostream>
#include <cstdio>
#include "./lodepng/lodepng.h"

struct Parameters{
    //struct to store the parameters
    int pixel_range_min {-2};
    int pixel_range_max {2};
    bool rgb {false};
    int nx {256};
    int ny {256};
    int total_iter {128};
    int threshold {2};
    std::complex<double> c;
};

std::vector<unsigned char> main_greyscale(const Parameters &params){
    //function to generate the greyscale image of the juliaset
    std::vector<unsigned char> image;
    image.resize(params.nx*params.ny);
    for (int j=0; j<params.ny; ++j){
        double imag = params.pixel_range_max - (double)j*(params.pixel_range_max - params.pixel_range_min) / params.ny;
        for (int i=0; i<params.nx; ++i){
            double real = params.pixel_range_min + (double)i*(params.pixel_range_max - params.pixel_range_min) / params.nx;
            std::complex<double> p (real,imag);
            int iter {0};
            while (std::abs(p) < params.threshold && iter < params.total_iter) {
                p = p*p + params.c; 
                ++iter;
            }
            image[j*params.nx+i] = (iter==params.total_iter)? 0:255*(double)iter/params.total_iter;
            
        }
    }
    return image;
}

std::vector<unsigned char> main_rgb(const Parameters &params){
    //function to generate the rgb image of the juliaset
    std::vector<unsigned char> image;
    image.resize(params.nx*params.ny*3); //for three color channels
    std::vector<std::vector<int>> palette = {
        {0, 7, 100},
        {32, 107, 203},
        {237, 255, 255},
        {255, 170, 0},
        {0, 2, 0}
    };
    for (int j=0; j<params.ny; ++j){
        double imag = params.pixel_range_max - (double)j*(params.pixel_range_max - params.pixel_range_min) / params.ny;
        for (int i=0; i<params.nx; ++i){
            double real = params.pixel_range_min + (double)i*(params.pixel_range_max - params.pixel_range_min) / params.nx;
            std::complex<double> p (real,imag);
            int iter {0};
            while (std::abs(p) < params.threshold && iter < params.total_iter) {
                p = p * p + params.c; 
                ++iter;
            }
            int base = (j*params.nx+i)*3;
            std::vector<int> color = palette[iter%palette.size()];
            double t = (double)iter / params.total_iter;  // 0.0 to 1.0
            double scaled = t * (palette.size() - 1);
            int idx = (int)scaled;
            double blend = scaled - idx;  // fractional part

            int nextIdx = std::min(idx + 1, (int)palette.size() - 1);
            image[base+0] = (iter==params.total_iter) ? 0:(unsigned char)(palette[idx][0]*(1-blend) + palette[nextIdx][0] * blend);
            image[base+1] = (iter==params.total_iter) ? 0:(unsigned char)(palette[idx][1]*(1-blend) + palette[nextIdx][1] * blend);
            image[base+2] = (iter==params.total_iter) ? 0:(unsigned char)(palette[idx][2]*(1-blend) + palette[nextIdx][2] * blend);
        }
    }
    return image;
}


int main(int argc, char *argv[]) {
    Parameters params; 
    std::vector<unsigned char> image;
    if (argc>1){
        params.nx = std::stoi(argv[1]);
        params.ny = std::stoi(argv[2]);
        double c_real = std::stod(argv[3]); //
        double c_imag = std::stod(argv[4]); // 
        params.c = std::complex<double> (c_real,c_imag);
        if (argc>5 && std::string(argv[5])=="rgb") params.rgb = true;
    }
    else{
        params.c = std::complex<double> (-0.8, 0.2);
    } 
    printf("Image Size: %d X %d\n",params.nx, params.ny);
    printf("c: %f + %fi\n",params.c.real(), params.c.imag());
    printf("Total Iterations: %d\n",params.total_iter);
    printf("Threshold: %d\n",params.threshold);
    printf("RGB: %s\n",params.rgb?"True":"False");
    
    unsigned error;
    if (params.rgb){
        image = main_rgb(params);
        error = lodepng::encode("output.png", image, params.nx, params.ny, LCT_RGB, 8);
    } 
    else{
        image = main_greyscale(params);
        error = lodepng::encode("output.png", image, params.nx, params.ny, LCT_GREY, 8);
    } 
    if (error)
        printf("PNG save failed: %s\n", lodepng_error_text(error));
    else
        printf("Saved output.png\n");
    return 0;
}
