#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include "Result.h"

class MetalComputeKernel
{
public:
    MetalComputeKernel( id<MTLComputeCommandEncoder> encoder,
                        id<MTLFunction> processFunction,
                        id<MTLComputePipelineState> processFunctionPSO);

    AMF_RESULT SetArgBuffer(id<MTLBuffer> buffer, int index);

    MTLSize GetCompileWorkgroupSize(NSUInteger maxSize);

    AMF_RESULT Enqueue(MTLSize workgroupSize, MTLSize sizeInWorkgroup);
private:
    id<MTLComputeCommandEncoder> m_encoder;
    id<MTLFunction> m_processFunction;
    id<MTLComputePipelineState> m_processFunctionPSO;
};