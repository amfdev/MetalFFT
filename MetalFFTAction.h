#pragma once
#include "action.h"

#define PI       3.1415926535897932384626433832795    // PI for sine/cos calculations
#define TWOPI    6.283185307179586476925286766559     // 2*PI for sine/cos calculations

#define USE_HOST_IMPL 1

class MetalFFTAction :
	public FFTAction
{
public:
	MetalFFTAction(metalfftStatus & err) : FFTAction(err) {}
	virtual  metalfftStatus enqueue(metalfftPlanHandle plHandle,
		metalfftDirection dir,
		amf::AMFBuffer **input,
		int inputCount,
		amf::AMFBuffer **output,
		int outputCount)  override
	{
		FFTRepo& fftRepo = FFTRepo::getInstance();
		FFTPlan* fftPlan = NULL;
		lockRAII* planLock = NULL;
        metalfftStatus err;

		OPENCL_V(fftRepo.getPlan(plHandle, fftPlan, planLock), _T("fftRepo.getPlan failed"));
		scopedLock sLock(*planLock, _T("MetalFFTAction"));


		amf::AMFBuffer *in = input[0];

		//TODO: condition check
		amf::AMFBuffer* realOut;
		if (fftPlan->placeness == METALFFT_OUTOFPLACE)
		{
            amf::AMFBuffer* out = output[0];
            int bufferSize = in->GetSize();
			if (fftPlan->outputLayout == METALFFT_COMPLEX_INTERLEAVED)
			{
				realOut = out;
			}
			else
			{
				if (fftPlan->inputLayout == METALFFT_REAL)
					bufferSize *= 2;
				fftPlan->pContext->AllocBuffer(amf::AMF_MEMORY_HOST, bufferSize, &realOut);
			}
			err = prepareOut(in, realOut, fftPlan->inputLayout, fftPlan->outputLayout);
            if (err != METALFFT_SUCCESS)
                return err;
            err = process(realOut, bufferSize, fftPlan);
            if (err != METALFFT_SUCCESS)
                return err;
		}
        else
        {
            int bufferSize = in->GetSize();
            if (fftPlan->inputLayout == METALFFT_REAL)
                    bufferSize *= 2;
            fftPlan->pContext->AllocBuffer(amf::AMF_MEMORY_HOST, bufferSize, &realOut);
            err = prepareOut(in, realOut, fftPlan->inputLayout, METALFFT_COMPLEX_INTERLEAVED);
            if (err != METALFFT_SUCCESS)
                return err;
#ifndef USE_HOST_IMPL
            realOut->Convert(memoryType);
#endif // !USE_HOST_IMPL

            err = process(realOut, bufferSize, fftPlan);
            if (err != METALFFT_SUCCESS)
                return err;
#ifndef USE_HOST_IMPL
            realOut->Convert(amf::AMF_MEMORY_HOST);
#endif // !USE_HOST_IMPL

            int elementsCount = bufferSize / sizeof(float) / 2;
            float* inputData = static_cast<float*>(in->GetNative());
            float* outputData = static_cast<float*>(realOut->GetNative());
            if (fftPlan->inputLayout == METALFFT_REAL)
            {
                for (int i = 0; i < elementsCount; ++i)
                {
                    inputData[i] = outputData[i * 2];
                }
            }
            else if (fftPlan->inputLayout == METALFFT_COMPLEX_INTERLEAVED)
            {
                for (int i = 0; i < elementsCount; ++i)
                {
                    inputData[i] = outputData[i];
                }
            }
        }

		return METALFFT_SUCCESS;
	}

    metalfftStatus process(amf::AMFBuffer* output, int bufferSize, FFTPlan* fftPlan)
	{
        AMF_RESULT res = AMF_FAIL;
        int powerOf2 = 0;
        amf_size elementsCount = bufferSize / sizeof(float) / 2;
        if (!isPwrTwo(elementsCount, &powerOf2))
            return METALFFT_NOTIMPLEMENTED;
        double TwoPi_N = TWOPI / (double)elementsCount;
        float Wn_Re, Wn_Im, tmp_Re, tmp_Im;
        float* outputData = static_cast<float*>(output->GetNative());
#ifndef USE_HOST_IMPL
        amf::AMFComputePtr pCompute;
        fftPlan->pComputeDevice->CreateCompute(nullptr, &pCompute);

        amf::AMFComputeKernelPtr pKernel1;
        res = pCompute->GetKernel(kernel_Butterfly_j_0, &pKernel1);

        amf::AMFComputeKernelPtr pKernel2;
        res = pCompute->GetKernel(kernel_Butterfly, &pKernel2);

        res = pKernel2->SetArgBuffer(0, output, amf::AMF_ARGUMENT_ACCESS_WRITE);
#endif // !USE_HOST_IMPL
        for (int stage = 1; stage <= powerOf2; stage++) // Loop for M stages, where 2^M = N
        {
            int BSep = (int)(pow(2, stage)); // Separation between butterflies = 2^stage
            int P = bufferSize / BSep;             // Similar Wn's in this stage = N/Bsep
            int BWidth = BSep / 2;     // Butterfly width (spacing between opposite points) = Separation / 2.
            int butterflyCount = elementsCount / BSep;

            double TwoPi_NP = TwoPi_N * P;

#ifdef USE_HOST_IMPL
            butterfly_j_0(outputData, elementsCount, BSep, BWidth);
#else
            butterfly_j_0_metal(output, elementsCount, BSep, BWidth, pKernel1);
            res = pCompute->FlushQueue();
            res = pCompute->FinishQueue();

            res = pKernel2->SetArgInt32(1, BSep);
            res = pKernel2->SetArgInt32(2, BWidth);
#endif // USE_HOST_IMPL
            //


            for (int j = 1; j < BWidth; j++) // Loop for j calculations per butterfly
            {
                Wn_Re = cos(TwoPi_N * P * j);
                Wn_Im = -sin(TwoPi_N * P * j);

#ifdef USE_HOST_IMPL
                butterfly(outputData, elementsCount, BSep, BWidth, j, Wn_Re, Wn_Im);
#else
                res = pKernel2->SetArgInt32(3, j);
                res = pKernel2->SetArgFloat(4, Wn_Re);
                res = pKernel2->SetArgFloat(5, Wn_Im);
                butterflyCount -= j / BSep;
                amf_size sizeLocal[3] = { butterflyCount, 1, 1 };
                amf_size sizeGlobal[3] = { butterflyCount, 1, 1 };

                pKernel2->GetCompileWorkgroupSize(sizeLocal);
                res = pKernel2->Enqueue(1, 0, sizeGlobal, sizeLocal);
                res = pCompute->FlushQueue();
                res = pCompute->FinishQueue();
#endif // USE_HOST_IMPL
            }
        }
        return METALFFT_SUCCESS;
	}

    void butterfly_j_0_metal(amf::AMFBuffer* outputData, amf_size elementsCount, int BSep, int BWidth, amf::AMFComputeKernelPtr pKernel1)
    {
        AMF_RESULT res = AMF_FAIL;

        res = pKernel1->SetArgBuffer(0, outputData, amf::AMF_ARGUMENT_ACCESS_WRITE);
        res = pKernel1->SetArgInt32(1, BSep);
        res = pKernel1->SetArgInt32(2, BWidth);

        elementsCount /= BSep;
        amf_size sizeLocal[3] = { elementsCount, 1, 1 };
        amf_size sizeGlobal[3] = { elementsCount, 1, 1 };

        pKernel1->GetCompileWorkgroupSize(sizeLocal);
        res = pKernel1->Enqueue(1, 0, sizeGlobal, sizeLocal);
    }

    void butterfly_j_0(float* outputData, int elementsCount, int BSep, int BWidth)
    {
        float tmp_Re, tmp_Im;
        //if R = 0, as WN^0 = (1 + j0)
        for (int HiIndex = 0; HiIndex < elementsCount; HiIndex += BSep) // Loop for HiIndex Step BSep butterflies per stage
        {
            tmp_Re = outputData[(HiIndex + BWidth) * 2];
            tmp_Im = outputData[(HiIndex + BWidth) * 2 + 1];

            outputData[(HiIndex + BWidth) * 2] = outputData[HiIndex * 2] - tmp_Re;
            outputData[(HiIndex + BWidth) * 2 + 1] = outputData[HiIndex * 2 + 1] - tmp_Im;

            outputData[HiIndex * 2] = (outputData[HiIndex * 2] + tmp_Re);
            outputData[HiIndex * 2 + 1] = (outputData[HiIndex * 2 + 1] + tmp_Im);
        }
    }

    void butterfly(float* outputData, int elementsCount, int BSep, int BWidth, int j, float Wn_Re, float Wn_Im)
    {
        float tmp_Re, tmp_Im;
        for (int HiIndex = j; HiIndex < elementsCount; HiIndex += BSep) // Loop for HiIndex Step BSep butterflies per stage
        {
            tmp_Re = (outputData[(HiIndex + BWidth) * 2] * Wn_Re) - (outputData[(HiIndex + BWidth) * 2 + 1] * Wn_Im);
            tmp_Im = (outputData[(HiIndex + BWidth) * 2] * Wn_Im) + (outputData[(HiIndex + BWidth) * 2 + 1] * Wn_Re);

            outputData[(HiIndex + BWidth) * 2] = outputData[HiIndex * 2] - tmp_Re;
            outputData[(HiIndex + BWidth) * 2 + 1] = outputData[HiIndex * 2 + 1] - tmp_Im;

            outputData[HiIndex * 2] = (outputData[HiIndex * 2] + tmp_Re);
            outputData[HiIndex * 2 + 1] = (outputData[HiIndex * 2 + 1] + tmp_Im);
        }
    }


	virtual const char* generateProgramSource(FFTPlan * plan)
	{
		return "generateProgramSource";
	}

    static const char* generateButterfly_j_0_source()
    {
        return R"(
                #include <metal_stdlib>

                using namespace metal;

                kernel void butterfly_j_0(
                    device float*           result,
                    constant int32_t &      step,
                    constant int32_t &      bwidth,

                    uint2 global_id [[thread_position_in_grid]]
                    )
                {
                    const int32_t index = global_id.x * step;
                    const float re = result[(index + bwidth) * 2];
                    const float im = result[(index + bwidth) * 2 + 1];

                    result[(index + bwidth) * 2] = result[index * 2] - re;
                    result[(index + bwidth) * 2 + 1] = result[index * 2 + 1] - im;

                    result[index * 2] = result[index * 2] + re;
                    result[index * 2 + 1] = result[index * 2 + 1] + im;
                })";
    }
    static const char* generateButterflySource()
    {
        return R"(
                #include <metal_stdlib>

                using namespace metal;

                kernel void butterfly(
                    device float*           result,
                    constant int32_t &      step,
                    constant int32_t &      bwidth,
                    constant int32_t &      j,
                    constant float &        wn_re,
                    constant float &        wn_im,

                    uint2 global_id [[thread_position_in_grid]]
                    )
                {
                    const int32_t index = j + (global_id.x) * step;

                    const float re = result[(index + bwidth) * 2] * wn_re - result[(index + bwidth) * 2 + 1] * wn_im;
                    const float im = result[(index + bwidth) * 2] * wn_im + result[(index + bwidth) * 2 + 1] * wn_re;

                    result[(index + bwidth) * 2] = result[index * 2] - re;
                    result[(index + bwidth) * 2 + 1] = result[index * 2 + 1] - im;

                    result[index * 2] = result[index * 2] + re;
                    result[index * 2 + 1] = result[index * 2 + 1] + im;
                })";
    }


    static const char* generateButterfly_j_0_sourceCL()
    {
        return R"(

                __kernel void butterfly_j_0(
                    __global float*        result,
                    const int      step,
                    const int      bwidth
                    )
                {
                    uint id, index;
                    float re, im;
                    id = get_global_id(0);
                    index = id * step;
                    re = result[(index + bwidth) * 2];
                    im = result[(index + bwidth) * 2 + 1];

                    result[(index + bwidth) * 2] = result[index * 2] - re;
                    result[(index + bwidth) * 2 + 1] = result[index * 2 + 1] - im;

                    result[index * 2] = result[index * 2] + re;
                    result[index * 2 + 1] = result[index * 2 + 1] + im;
                })";
    }
    static const char* generateButterflySourceCL()
    {
        return R"(
                __kernel void butterfly(
                    __global float*         result,
                    const int      step,
                    const int      bwidth,
                    const int      j,
                    const float        wn_re,
                    const float       wn_im
                    )
                {
                    uint id, index;
                    float re, im;
                    id = get_global_id(0);
                    index = j + (id) * step;

                    re = result[(index + bwidth) * 2] * wn_re - result[(index + bwidth) * 2 + 1] * wn_im;
                    im = result[(index + bwidth) * 2] * wn_im + result[(index + bwidth) * 2 + 1] * wn_re;

                    result[(index + bwidth) * 2] = result[index * 2] - re;
                    result[(index + bwidth) * 2 + 1] = result[index * 2 + 1] - im;

                    result[index * 2] = result[index * 2] + re;
                    result[index * 2 + 1] = result[index * 2 + 1] + im;
                })";
    }

    static void Register(amf::AMFPrograms* pPrograms)
    {
        const char* source1 = generateButterfly_j_0_source();
        pPrograms->RegisterKernelSource(&kernel_Butterfly_j_0, L"butterfly_j_0", "butterfly_j_0", strlen(source1), (amf_uint8*)source1, "");
        const char* source2 = generateButterflySource();
        pPrograms->RegisterKernelSource(&kernel_Butterfly, L"butterfly", "butterfly", strlen(source2), (amf_uint8*)source2, "");
        memoryType = amf::AMF_MEMORY_METAL;
    }

    static void RegisterCL(amf::AMFPrograms* pPrograms)
    {
        const char* source1 = generateButterfly_j_0_sourceCL();
        pPrograms->RegisterKernelSource(&kernel_Butterfly_j_0, L"butterfly_j_0", "butterfly_j_0", strlen(source1), (amf_uint8*)source1, "");
        const char* source2 = generateButterflySourceCL();
        pPrograms->RegisterKernelSource(&kernel_Butterfly, L"butterfly", "butterfly", strlen(source2), (amf_uint8*)source2, "");
        memoryType = amf::AMF_MEMORY_OPENCL;
    }
    static amf::AMF_KERNEL_ID kernel_Butterfly_j_0;
    static amf::AMF_KERNEL_ID kernel_Butterfly;
    static amf::AMF_MEMORY_TYPE memoryType;
};

