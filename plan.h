#include "metalFFT_common.h"

class	FFTPlan
{

public:
	bool baked;
    metalfftPrecision           precision;
    metalfftLayout              inputLayout;
	metalfftLayout              outputLayout;
    metalfftResultLocation      placeness;
    // User created plan
	bool userPlan;
};