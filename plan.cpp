#include "plan.h"
#include "metalFFT.h"

metalfftStatus	metalfftCreateDefaultPlan( metalfftPlanHandle* plHandle, const metalfftDim dim,
						const size_t* clLengths )
{
	metalfftStatus ret;// = clfftCreateDefaultPlanInternal(plHandle, context, dim, clLengths);

	if(ret == METALFFT_SUCCESS)
	{
		//FFTRepo& fftRepo	= FFTRepo::getInstance( );
		FFTPlan *fftPlan = nullptr;
		//lockRAII* planLock	= NULL;
		//OPENCL_V( fftRepo.getPlan( *plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );

		fftPlan->userPlan = true;
	}

	return ret;
}

metalfftStatus	metalfftBakePlan( metalfftPlanHandle plHandle, unsigned int numQueues,  void *user_data )
{
    return METALFFT_NOTIMPLEMENTED;
}

metalfftStatus	clfftDestroyPlan( metalfftPlanHandle* plHandle )
{
	// FFTRepo& fftRepo	= FFTRepo::getInstance( );
	// FFTPlan* fftPlan	= NULL;
	// lockRAII* planLock	= NULL;

	// OPENCL_V( fftRepo.getPlan( *plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );

	// //	Recursively destroy subplans, that are used for higher dimensional FFT's
	// if( fftPlan->planX )
	// 	clfftDestroyPlan( &fftPlan->planX );
	// if( fftPlan->planY )
	// 	clfftDestroyPlan( &fftPlan->planY );
	// if( fftPlan->planZ )
	// 	clfftDestroyPlan( &fftPlan->planZ );
	// if( fftPlan->planTX )
	// 	clfftDestroyPlan( &fftPlan->planTX );
	// if( fftPlan->planTY )
	// 	clfftDestroyPlan( &fftPlan->planTY );
	// if( fftPlan->planTZ )
	// 	clfftDestroyPlan( &fftPlan->planTZ );
	// if( fftPlan->planRCcopy )
	// 	clfftDestroyPlan( &fftPlan->planRCcopy );
	// if( fftPlan->planCopy )
	// 	clfftDestroyPlan( &fftPlan->planCopy );

	// fftRepo.deletePlan( plHandle );

	return	METALFFT_NOTIMPLEMENTED;
}