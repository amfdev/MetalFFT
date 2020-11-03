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


#pragma once
#if !defined( AMD_CLFFT_action_H )
#define AMD_CLFFT_action_H

#include "plan.h"
#include "repo.h"
#include "generator.h"
#include "metalFFT_common.h"
#include "thirdparty/FFT_3.h"

class FFTAction
{
public:
    FFTAction(metalfftStatus & err){err = METALFFT_SUCCESS;}
    virtual ~FFTAction(){}

    virtual metalfftStatus enqueue(metalfftPlanHandle plHandle,
												metalfftDirection dir,
												amf::AMFBuffer **input,
												int inputCount,
        										amf::AMFBuffer **output,
												int outputCount) 
    {
        // const char* kernel_src = generateProgramSource();
        // AMFFactoryHelper helper;
        // helper.Init();
        // amf::AMFFactory* factory = helper.GetFactory();
        // amf::AMF_KERNEL_ID kernel = 0;
        // pPrograms->RegisterKernelSource(&kernel, L"kernelIDName", "square2", strlen(kernel_src), (amf_uint8*)kernel_src, NULL);
        // amf::AMFComputePtr pCompute;
        // plan->pComputeDevice->CreateCompute(nullptr, &pCompute);
        // amf::AMFComputeKernelPtr pKernel;
        // res = pCompute->GetKernel(kernel, &pKernel);

        // /////////
        // res = pKernel->SetArgBuffer(1, output, amf::AMF_ARGUMENT_ACCESS_WRITE);
        // amf_size sizeLocal[3] = {1024, 0, 0};
        // amf_size sizeGlobal[3] = {1024, 0, 0};
        // amf_size offset[3] = {0, 0, 0};

        // pKernel->GetCompileWorkgroupSize(sizeLocal);

        // pKernel->Enqueue(1, offset, sizeGlobal, sizeLocal);
        // pCompute->FlushQueue();
		return METALFFT_SUCCESS;
    }

protected:
	virtual const char* generateProgramSource(FFTPlan * plan) { return "generateProgramSource"; }
};

class HostFFTAction: public FFTAction
{
public:
    HostFFTAction(metalfftStatus & err) : FFTAction(err) {}
    virtual  metalfftStatus enqueue(metalfftPlanHandle plHandle,
		metalfftDirection dir,
		amf::AMFBuffer **input,
		int inputCount,
		amf::AMFBuffer **output,
		int outputCount)  override
    {
        FFTRepo& fftRepo	= FFTRepo::getInstance( );
        FFTPlan* fftPlan	= NULL;
        lockRAII* planLock	= NULL;

        OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
        scopedLock sLock( *planLock, _T( "HostFFTAction" ) );

        if (fftPlan->placeness != METALFFT_INPLACE || fftPlan->inputLayout != METALFFT_COMPLEX_INTERLEAVED)
            return METALFFT_NOTIMPLEMENTED;

        amf::AMFBuffer *in = input[0];
        universal_fft(static_cast<ShortComplex*>(in->GetNative()), in->GetSize() / sizeof(float) / 2, dir == METALFFT_BACKWARD);
        return METALFFT_SUCCESS;
    }
};
#endif // AMD_CLFFT_action_H
