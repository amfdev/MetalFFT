#include "MetalComputeKernelWrapper.h"
#include "MetalComputeKernel.h"

MetalComputeKernelWrapper::MetalComputeKernelWrapper(void * computeKernel)
{
    m_kernel = static_cast<MetalComputeKernel*>(computeKernel);
    assert(m_kernel != nil);
}

AMF_RESULT MetalComputeKernelWrapper::SetArgBuffer(void * buffer, int index)
{
    return m_kernel->SetArgBuffer((id<MTLBuffer>)buffer, index);
}

AMF_RESULT MetalComputeKernelWrapper::GetCompileWorkgroupSize(size_t workgroupSize[3])
{
    MTLSize size = m_kernel->GetCompileWorkgroupSize(workgroupSize[0]);
    workgroupSize[0] = size.width;
    workgroupSize[1] = size.height;
    workgroupSize[2] = size.depth;

    return AMF_OK;
}

AMF_RESULT MetalComputeKernelWrapper::Enqueue(size_t globalSize[3], size_t localSize[3])
{
    MTLSize workgroupSize = MTLSizeMake(globalSize[0], globalSize[1], globalSize[2]);
    MTLSize sizeInWorkgroup = MTLSizeMake(localSize[0], localSize[1], localSize[2]);

    return m_kernel->Enqueue(workgroupSize, sizeInWorkgroup);
}