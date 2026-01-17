#include<iostream>
#include"kernel.hpp"
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdint>

struct Config{
    float dt;
    float T;
    float trajectories_count;
    float Force_Amplitude;
};

void perform_compatibility_check(){
    return;
}
void parse_arguments(const int argc, const char** argv, float& dt, float& T, int& n, float& A) {

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--dt" && i + 1 < argc) {
            dt = std::stof(argv[++i]);
        } else if (arg == "--T" && i + 1 < argc) {
            T = std::stof(argv[++i]);
        } else if (arg == "--n" && i + 1 < argc) {
            n = std::stoi(argv[++i]);
        } else if (arg == "--A" && i + 1 < argc) {
            A = std::stof(argv[++i]);
        }
    }
}

#define DEFAULT_DT 0.01f
#define DEFAULT_T 10.0f
#define DEFAULT_N 1000
#define DEFAULT_A 1.0f

int main(int argc, char** argv) {

    float dt = DEFAULT_DT, T = DEFAULT_T, A = DEFAULT_A;
    int n = DEFAULT_N;
    parse_arguments(argc, argv, dt, T, n, A);
    perform_compatibility_check();

    const int nt = static_cast<int>(T / dt);
    float* results = (float*)malloc(n * nt * sizeof(float));

    kernel_launcher(results, dt, T, n, A);

    free(results);
    return 0;
}