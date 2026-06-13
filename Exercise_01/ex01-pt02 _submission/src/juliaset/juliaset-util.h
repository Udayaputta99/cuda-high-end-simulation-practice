#pragma once

#include <iostream>

void checkSolution(const unsigned char *const vec, size_t vecLen) {
    for (size_t i = 0; i < vecLen; ++i)
        if (0 != vec[i]) {
            std::cerr << "Stream check failed for element " << i << " (expected " << 0 << " but got " << vec[i] << ")" << std::endl;
            return;
        }

    std::cout << "  Passed result check" << std::endl;
}