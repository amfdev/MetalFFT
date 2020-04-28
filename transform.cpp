#include "metalFFT.h"

metalfftStatus	metalfftEnqueueTransform(
                                            metalfftPlanHandle plHandle,
                                            metalfftDirection dir,
                                            unsigned int numQueuesAndEvents,
                                            //cl_command_queue* commQueues,
                                            unsigned int numWaitEvents//,
                                            //const cl_event* waitEvents,
                                            //cl_event* outEvents,
                                            //cl_mem* inputBuffers,
                                            //cl_mem* outputBuffers,
                                            //cl_mem tmpBuffer
                                            )
{
    return METALFFT_NOTIMPLEMENTED;
}