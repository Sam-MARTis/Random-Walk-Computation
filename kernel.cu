#include<curand.h>
#include<math.h>


void compute_trajectories(float* trajectories, const float sqrt_dt, const float A, const int n, const int nt){
    const int idx = (blockIdx.x * blockDim.x + threadIdx.x)*nt;
    if(idx >= n*nt) return;
    float x = 0.0f;
    const float Asqrtdt = A * sqrt_dt;
    for(int i = 0; i < nt; i++) {
        x += Asqrtdt*trajectories[idx + i];
        trajectories[idx + i] = x;
    }
};


#define TX 256

void kernel_launcher(float* results, const float dt, const float T, const int n, const float A) {
    curandGenerator_t rangen;
    const int nt = int(T/dt);
    float* trajectories;
    const size_t N = (size_t)n * (size_t)nt;
    cudaMalloc(&trajectories, N * sizeof(float));

    curandCreateGenerator(&rangen, CURAND_RNG_PSEUDO_PHILOX4_32_10);
    curandSetPseudoRandomGeneratorSeed(rangen, 1234ULL);

    curandGenerateNormal(rangen, trajectories, N, 0.0f, 1.0f);

    const float sqrt_dt = sqrtf(dt);
    const dim3 blockSize(TX);
    const dim3 gridSize((N + TX - 1) / TX);
    compute_trajectories<<<gridSize, blockSize>>>(trajectories, sqrt_dt, A, n, nt);
    cudaDeviceSynchronize();

}