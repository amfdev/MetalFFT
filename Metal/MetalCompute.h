#import <Metal/Metal.h>
#include "MetalComputeKernel.h"
#include "Result.h"

class MetalCompute
{
public:
    MetalCompute(id<MTLDevice> device, id<MTLCommandQueue> commandQueue);
    MetalCompute(id<MTLDevice> device, id<MTLCommandQueue> commandQueue, NSString * source);

    AMF_RESULT GetKernel(NSString * name, MetalComputeKernel ** kernel);

    AMF_RESULT FlushQueue();
    AMF_RESULT FinishQueue();

private:
    id<MTLDevice> m_device;
    id<MTLCommandQueue> m_commandQueue;
    id<MTLLibrary> m_library;
    id<MTLCommandBuffer> m_commandBuffer;
};