#include <iostream>

#include "utils/tools.h"
#include "kernel.h"

#include <cuda_runtime.h>

#define ITERATIONS 20

int main()
{
    const size_t row = 8192;
    const size_t column = 8192;
    const size_t matrixSize = row * column;
    const size_t byteSize = matrixSize * sizeof(float);

    getGPUInfo();

    std::cout << "Matrix size (float): " << row << "x" << column << std::endl;
    std::cout << "Iterations: " << ITERATIONS << std::endl << std::endl;

    std::cout << "Allocatin RAM... ";
    float *hostInputMatrix = new float[matrixSize];
    float *hostOutputMatrix = new float[matrixSize];
    std::cout << "OK!" << std::endl;

    std::cout << "Filling matrix... ";
    fillMatrix(hostInputMatrix, row, column);
    std::cout << "OK!" << std::endl;
    //printMatrix(hostInputMatrix, row, column);

    float *deviceInputMatrix = nullptr;
    float *deviceOutputMatrix = nullptr;

    std::cout << "Allocate RAM (device)... ";
    cudaMalloc((void**)&deviceInputMatrix, byteSize);
    cudaMalloc((void**)&deviceOutputMatrix, byteSize);
    std::cout << "OK!" << std::endl;

    std::cout << "Copying data to device... ";
    cudaMemcpy(deviceInputMatrix, hostInputMatrix, byteSize, cudaMemcpyKind::cudaMemcpyHostToDevice);
    std::cout << "OK!" << std::endl;

    cudaEvent_t start, stop;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    std::cout << "Computing... ";
    cudaEventRecord(start, 0);
    for (size_t i = 0; i < ITERATIONS; i++)
        transpose(deviceInputMatrix, deviceOutputMatrix, row ,column);
    cudaEventRecord(stop, 0);
    std::cout << "OK!" << std::endl;

    cudaEventSynchronize(stop);

    std::cout << "Copying data to host... ";
    cudaMemcpy(hostOutputMatrix, deviceOutputMatrix, byteSize, cudaMemcpyKind::cudaMemcpyDeviceToHost);
    std::cout << "OK!" << std::endl << std::endl;

    // std::cout << "\n";
    // printMatrix(hostOutputMatrix, column, row);

    float ms = 0;
    cudaEventElapsedTime(&ms, start, stop);
    std::cout << "Time: " << ms << " ms" << std::endl;

    delete[] hostInputMatrix;
    delete[] hostOutputMatrix;

    cudaFree(deviceInputMatrix);
    cudaFree(deviceOutputMatrix);

    return 0;
}