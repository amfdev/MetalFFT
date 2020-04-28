/* ************************************************************************
 * Copyright 2013 Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "metalFFT.h"
#include "Metal/MetalDeviceWrapper.h"

int main( void )
{
    metalfftStatus err;

    metalfftSetupData fftSetup;
    err = metalfftInitSetupData(&fftSetup);
    err = metalfftSetup(&fftSetup);

    size_t N = 16;
    /* FFT library realted declarations */
    metalfftPlanHandle planHandle;
    metalfftDim dim = METALFFT_1D;
    size_t clLengths[1] = {N};
    /* Create a default plan for a complex FFT. */
    err = metalfftCreateDefaultPlan(&planHandle, dim, clLengths);

    /* Set plan parameters. */
    err = metalfftSetPlanPrecision(planHandle, METALFFT_SINGLE);
    err = metalfftSetLayout(planHandle, METALFFT_COMPLEX_INTERLEAVED, METALFFT_COMPLEX_INTERLEAVED);
    err = metalfftSetResultLocation(planHandle, METALFFT_INPLACE);

    /* Bake the plan. */
    err = metalfftBakePlan(planHandle, 1, NULL);

    MetalDeviceWrapper * device = new MetalDeviceWrapper(NULL);
	AMF_RESULT res;

    const char * source;
    MetalComputeWrapper * compute = NULL;
    res = device->CreateComputeFromSource(&compute, source);

    MetalComputeKernelWrapper * kernel = NULL;
    res = compute->GetKernel("process_array", &kernel);

    /* Execute the plan. */
    err = metalfftEnqueueTransform(planHandle,
                                    METALFFT_FORWARD,
                                    1,
                                    //&queue,
                                    0);//, NULL, NULL, &bufX, NULL, NULL);
    //kernel->Enqueue
    /* Wait for calculations to be finished. */
    compute->FinishQueue();

    /* Release the plan. */
    err = metalfftDestroyPlan( &planHandle );

    /* Release clFFT library. */
    metalfftTeardown();

    return 0;
}
