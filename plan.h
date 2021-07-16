#pragma once
#include "metalFFT_common.h"
#include "public/include/core/Factory.h"
#include "public/include/core/Buffer.h"
#include "public/common/AMFFactory.h"

typedef     uint32_t     metal_uint;

//todo - add constructor!
class	FFTPlan
{

public:
	bool baked;
    metalfftPrecision           precision;
    metalfftLayout              inputLayout;
	metalfftLayout              outputLayout;
    metalfftResultLocation      placeness;
    metalfftMethod              method;

    // User created plan
	bool userPlan;
	metalfftPlanHandle plHandle;

    amf::AMFComputeDevicePtr pComputeDevice;
};