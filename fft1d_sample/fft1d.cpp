#include <stdio.h>
#include <stdlib.h>
#include "../../include/core/Factory.h"
#include "../../common/AMFFactory.h"
#include "../../include/core/Buffer.h"
#include "AMF_FFT/metalFFT.h"
#include <iostream>

void print(float *pSignal)
{
	for (size_t i = 0; i < 5; i++)
	{
		std::cout << pSignal[i * 2] << " + " << pSignal[i * 2 + 1] << "i" << std::endl;
	}
}

int main( void )
{
	//OpenAMF part
	////////////////
	AMFFactoryHelper helper;
	helper.Init();
	amf::AMFFactory* factory = helper.GetFactory();

	amf::AMFBuffer *input = NULL;

	amf::AMFContextPtr context;
	factory->CreateContext(&context);
	AMF_RESULT res;
	res = context->AllocBuffer(amf::AMF_MEMORY_HOST, 1024 * sizeof(float), &input);

	float  *inputData = static_cast<float*>(input->GetNative());
	for (int i = 0; i < 512; i++)
	{
		inputData[i * 2] = 0;
		inputData[i * 2 + 1] = 1;
	}
	//MetalFFT part
	//////////////
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
    err = metalfftCreateDefaultPlan(&planHandle, METALFFT_1D, clLengths);

    /* Set plan parameters. */
    err = metalfftSetPlanPrecision(planHandle, METALFFT_SINGLE);
    err = metalfftSetLayout(planHandle, METALFFT_COMPLEX_INTERLEAVED, METALFFT_COMPLEX_INTERLEAVED);
    err = metalfftSetResultLocation(planHandle, METALFFT_INPLACE);
	err = metalfftSetPlanFFTmethod(planHandle, METALFFT_HOST);
	//err = metalfftSetPlanDevice(planHandle, device);

    /* Bake the plan. */
    err = metalfftBakePlan(planHandle, 1, NULL);


	std::cout << "Initial!\n";
	print(inputData);
	err = metalfftEnqueueTransform(planHandle, METALFFT_FORWARD, &input, 1, NULL, 0);
	std::cout << "Forwarded!\n";
	print(inputData);
	err = metalfftEnqueueTransform(planHandle, METALFFT_BACKWARD, &input, 1, NULL, 0);
	std::cout << "Inversed!\n";
	print(inputData);
    /* Release the plan. */
    err = metalfftDestroyPlan(&planHandle);

    /* Release clFFT library. */
    metalfftTeardown();

    return 0;
}
