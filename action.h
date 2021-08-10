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
#include <cmath>
#define log10_2_INV 3.3219280948873623478703194294948 // 1/Log10(2)


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
        FFTRepo& fftRepo    = FFTRepo::getInstance( );
        FFTPlan* fftPlan    = NULL;
        lockRAII* planLock    = NULL;

        OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
        scopedLock sLock( *planLock, _T( "HostFFTAction" ) );
        amf::AMFBuffer *in = input[0];
        //TODO: condition check
        if (fftPlan->inputLayout == METALFFT_REAL)
        {
            int size = in->GetSize() / sizeof(float);
            float * inputArray = new float[size * 2];
            float * base = static_cast<float*>(in->GetNative());
            for (int i = 0; i < size; i++)
            {
                inputArray[i * 2] = base[i];
                inputArray[i * 2 + 1] = 0;
            }
            universal_fft(static_cast<ShortComplex*>((void*)inputArray), size, dir == METALFFT_BACKWARD);
            if (fftPlan->placeness == METALFFT_INPLACE)
            {
                for (int i = 0; i < size; i++)
                {
                    base[i] = inputArray[i * 2];
                }
            }
            else
            {
                amf::AMFBuffer *out = output[0];
                float * baseOut = static_cast<float*>(out->GetNative());
                if (fftPlan->outputLayout == METALFFT_REAL)
                {
                    for (int i = 0; i < size; i++)
                    {
                        baseOut[i] = inputArray[i * 2];
                    }
                }
                else if (fftPlan->outputLayout == METALFFT_COMPLEX_INTERLEAVED)
                {
                    for (int i = 0; i < size * 2; i++)
                    {
                        baseOut[i] = inputArray[i];
                    }
                }
            }
            return METALFFT_SUCCESS;
        }
        if (fftPlan->placeness != METALFFT_INPLACE || fftPlan->inputLayout != METALFFT_COMPLEX_INTERLEAVED)
            return METALFFT_NOTIMPLEMENTED;

        universal_fft(static_cast<ShortComplex*>(in->GetNative()), in->GetSize() / sizeof(float) / 2, dir == METALFFT_BACKWARD);
        return METALFFT_SUCCESS;
    }
};

static bool isPwrTwo(int N, int* M)
{
    *M = (int)ceil(log10((double)N) * log10_2_INV);// M is number of stages to perform. 2^M = N
    int NN = (int)pow(2.0, *M);
    if ((NN != N) || (NN == 0)) // Check N is a power of 2.
        return false;

    return true;
}

static metalfftStatus prepareOut(amf::AMFBufferPtr input, amf::AMFBufferPtr output, metalfftLayout inLayout, metalfftLayout outLayout)
{
    //place elements in reverse bit order
    int powerOf2 = 0;
    int incIn = 1;
    int incOut = 1;
    if (inLayout == METALFFT_COMPLEX_INTERLEAVED)
        incIn = 2;
    if (outLayout == METALFFT_COMPLEX_INTERLEAVED)
        incOut = 2;
    const int dataSize = input->GetSize() / incIn /sizeof(float);
    if (!isPwrTwo(dataSize, &powerOf2))
        return METALFFT_NOTIMPLEMENTED;
    
    int MM1 = powerOf2 - 1;
    float* inputData = static_cast<float*>(input->GetNative());
    float* outputData = static_cast<float*>(output->GetNative());
    for (int i = 0; i < dataSize; i++)
    {
        const int inAddr = i * incIn;
        int outAddr = 0;
        int iaddr = i;
        for (int l = 0; l < powerOf2; l++) // Bit reverse i
        {
            if (iaddr & 0x01)     // Detemine least significant bit
                outAddr += (1 << (MM1 - l));    // Increment ii by 2^(powerOf2-1-l) if lsb was 1
            iaddr >>= 1;                // right shift iaddr to test next bit. Use logical operations for speed increase
            if (!iaddr)
                break;
        }
        outAddr *= incOut;
        if (incIn == incOut == 1)
        {
            outputData[outAddr] = inputData[inAddr];
            continue;
        }
        if (incIn == 2 && incOut == 2)
        {
            outputData[outAddr] = inputData[inAddr];
            outputData[outAddr + 1] = inputData[inAddr + 1];
            continue;
        }
        if (incIn == 1  && incOut == 2)
        {
            outputData[outAddr] = inputData[inAddr];
            outputData[outAddr + 1] = 0;
            continue;
        }
        // complex to real convertion
        return METALFFT_INVALID_ARG_VALUE;
    }
    return METALFFT_SUCCESS;
}

#endif // AMD_CLFFT_action_H
