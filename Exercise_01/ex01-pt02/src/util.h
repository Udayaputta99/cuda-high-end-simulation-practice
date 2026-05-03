#pragma once

#include <cstdlib>

void parseCLA_1d(int argc, char *const *argv, size_t &nx, size_t &ny) {
    // default values
    nx = 512;
    ny = 512;
    // override with command line arguments
    int i = 1;
    if (argc > i) nx = atoi(argv[i]);
    ++i;
    if (argc > i) ny = atoi(argv[i]);
    ++i;
}
