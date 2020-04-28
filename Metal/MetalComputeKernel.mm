#include "MetalComputeKernel.h"

MetalComputeKernel::MetalComputeKernel( id<MTLComputeCommandEncoder> encoder,
                        id<MTLFunction> processFunction,
                        id<MTLComputePipelineState> processFunctionPSO)
    :   m_encoder(encoder),
        m_processFunction(processFunction),
        m_processFunctionPSO(processFunctionPSO)
{
    [m_encoder setComputePipelineState:m_processFunctionPSO];
}

AMF_RESULT MetalComputeKernel::SetArgBuffer(id<MTLBuffer> buffer, int index)
{
    [m_encoder setBuffer:buffer offset:0 atIndex:index];
    return AMF_OK;
}

MTLSize MetalComputeKernel::GetCompileWorkgroupSize(NSUInteger maxSize)
{
    NSUInteger threadGroupSize = m_processFunctionPSO.maxTotalThreadsPerThreadgroup;
    if (threadGroupSize > maxSize)
        return MTLSizeMake(maxSize, 1, 1);

    return MTLSizeMake(threadGroupSize, 1, 1);
}

AMF_RESULT MetalComputeKernel::Enqueue(MTLSize workgroupSize, MTLSize sizeInWorkgroup)
{
    [m_encoder dispatchThreads:workgroupSize
            threadsPerThreadgroup:sizeInWorkgroup];
    [m_encoder endEncoding];
    return AMF_OK;
}