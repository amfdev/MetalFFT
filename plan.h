#include "metalFFT_common.h"
#include "../../../include/core/Factory.h"
#include "../../../common/AMFFactory.h"
#include "../../../include/core/Buffer.h"
#pragma once

typedef     uint32_t     metal_uint;

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