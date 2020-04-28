#include "MetalComputeWrapper.h"
#include "MetalCompute.h"

MetalComputeWrapper::MetalComputeWrapper(void * compute)
{
    m_compute = static_cast<MetalCompute*>(compute);
    assert(m_compute != nil);
}

AMF_RESULT MetalComputeWrapper::GetKernel(const char * name, MetalComputeKernelWrapper ** kernel)
{
    MetalComputeKernel *pKernel = NULL;
    AMF_RESULT res = m_compute->GetKernel([NSString stringWithUTF8String:name], &pKernel);
    if (res != AMF_OK)
        return res;

    (*kernel) = new MetalComputeKernelWrapper(pKernel);
    return AMF_OK;
}

AMF_RESULT MetalComputeWrapper::GetKernelWithSource(const char * name, MetalComputeKernelWrapper ** kernel, const char* source)
{
    MetalComputeKernel *pKernel = NULL;
    AMF_RESULT res = m_compute->GetKernel([NSString stringWithUTF8String:name], &pKernel);
    if (res != AMF_OK)
        return res;

    (*kernel) = new MetalComputeKernelWrapper(pKernel);
    return AMF_OK;
}


AMF_RESULT MetalComputeWrapper::FlushQueue()
{
    return m_compute->FlushQueue();
}

AMF_RESULT MetalComputeWrapper::FinishQueue()
{
    return m_compute->FinishQueue();
}