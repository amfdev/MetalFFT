#pragma once
#include "Result.h"
class MetalComputeKernel;

class MetalComputeKernelWrapper
{
public:
    MetalComputeKernelWrapper(void * computeKernel);

    AMF_RESULT SetArgBuffer(void * buffer, int index);

    AMF_RESULT GetCompileWorkgroupSize(size_t workgroupSize[3]);

    AMF_RESULT Enqueue(size_t globalSize[3], size_t localSize[3]);
private:
    MetalComputeKernel * m_kernel;
};