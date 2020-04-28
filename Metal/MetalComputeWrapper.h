#pragma once
#include "Result.h"
#include "MetalComputeKernelWrapper.h"

class MetalCompute;

class MetalComputeWrapper
{
public:
    MetalComputeWrapper(void * compute);

    AMF_RESULT GetKernel(const char * name, MetalComputeKernelWrapper ** kernel);
    AMF_RESULT GetKernelWithSource(const char * name, MetalComputeKernelWrapper ** kernel, const char* source);


    AMF_RESULT FlushQueue();
    AMF_RESULT FinishQueue();

private:
    MetalCompute * m_compute;
};