#include "metalFFT.h"
#include "plan.h"

metalfftStatus clfftSetPlanPrecision( metalfftPlanHandle plHandle, metalfftPrecision precision )
{
	//FFTRepo& fftRepo	= FFTRepo::getInstance( );
	FFTPlan* fftPlan	= NULL;
	//lockRAII* planLock	= NULL;

	//OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
	//scopedLock sLock( *planLock, _T( "clfftSetPlanPrecision" ) );

	if( precision >= ENDPRECISION )
		return METALFFT_INVALID_ARG_VALUE;

	//	We do not support METALFFT_*_FAST currently
	if( precision == METALFFT_SINGLE_FAST || precision == METALFFT_DOUBLE_FAST )
		return METALFFT_NOTIMPLEMENTED;



	//	If we modify the state of the plan, we assume that we can't trust any pre-calculated contents anymore
	fftPlan->baked		= false;
	fftPlan->precision	= precision;

	return	METALFFT_SUCCESS;
}

metalfftStatus clfftSetLayout( metalfftPlanHandle plHandle, metalfftLayout iLayout, metalfftLayout oLayout )
{
    //FFTRepo& fftRepo	= FFTRepo::getInstance( );
	FFTPlan* fftPlan	= NULL;
	//lockRAII* planLock	= NULL;

	//OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
	//scopedLock sLock( *planLock, _T( "clfftSetLayout" ) );

	//	Basic error checking on parameter
	if( ( iLayout >= ENDLAYOUT ) || ( oLayout >= ENDLAYOUT ) )
		return METALFFT_INVALID_ARG_VALUE;

	//	We currently only support a subset of formats
	switch( iLayout )
	{
		case METALFFT_COMPLEX_INTERLEAVED:
			{
				if( (oLayout == METALFFT_HERMITIAN_INTERLEAVED) || (oLayout == METALFFT_HERMITIAN_PLANAR) || (oLayout == METALFFT_REAL))
					return METALFFT_NOTIMPLEMENTED;
			}
			break;
		case METALFFT_COMPLEX_PLANAR:
			{
				if( (oLayout == METALFFT_HERMITIAN_INTERLEAVED) || (oLayout == METALFFT_HERMITIAN_PLANAR) || (oLayout == METALFFT_REAL))
					return METALFFT_NOTIMPLEMENTED;
			}
			break;
		case METALFFT_HERMITIAN_INTERLEAVED:
			{
				if(oLayout != METALFFT_REAL) return METALFFT_NOTIMPLEMENTED;
			}
			break;
		case METALFFT_HERMITIAN_PLANAR:
			{
				if(oLayout != METALFFT_REAL) return METALFFT_NOTIMPLEMENTED;
			}
			break;
		case METALFFT_REAL:
			{
				if((oLayout == METALFFT_REAL) || (oLayout == METALFFT_COMPLEX_INTERLEAVED) || (oLayout == METALFFT_COMPLEX_PLANAR))
					return METALFFT_NOTIMPLEMENTED;
			}
			break;
		default:
			return METALFFT_NOTIMPLEMENTED;
			break;
	}

	//	We currently only support a subset of formats
	switch( oLayout )
	{
		case METALFFT_COMPLEX_PLANAR:
		case METALFFT_COMPLEX_INTERLEAVED:
		case METALFFT_HERMITIAN_INTERLEAVED:
		case METALFFT_HERMITIAN_PLANAR:
		case METALFFT_REAL:
			break;
		default:
			return METALFFT_NOTIMPLEMENTED;
			break;
	}

	//	If we modify the state of the plan, we assume that we can't trust any pre-calculated contents anymore
	fftPlan->baked	= false;
	fftPlan->inputLayout	= iLayout;
	fftPlan->outputLayout	= oLayout;

    return	METALFFT_SUCCESS;
}

metalfftStatus metalfftSetResultLocation( metalfftPlanHandle plHandle, metalfftResultLocation placeness)
{
	//FFTRepo& fftRepo	= FFTRepo::getInstance( );
	FFTPlan* fftPlan	= NULL;
	//lockRAII* planLock	= NULL;

	//OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
	//scopedLock sLock( *planLock, _T( "clfftSetResultLocation" ) );

	//	Basic error checking on parameter
	if( placeness >= ENDPLACE )
		return METALFFT_INVALID_ARG_VALUE;

	//	If we modify the state of the plan, we assume that we can't trust any pre-calculated contents anymore
	fftPlan->baked		= false;
	fftPlan->placeness	= placeness;

	return	METALFFT_SUCCESS;
}