#include "metalFFT.h"
#include "plan.h"
#include "MetalFFTAction.h"

metalfftStatus clfftSetPlanPrecision( metalfftPlanHandle plHandle, metalfftPrecision precision )
{
	FFTRepo& fftRepo	= FFTRepo::getInstance( );
	FFTPlan* fftPlan	= NULL;
	lockRAII* planLock	= NULL;

	OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
	scopedLock sLock( *planLock, _T( "clfftSetPlanPrecision" ) );

	if( precision >= METALFFT_ENDPRECISION )
		return METALFFT_INVALID_ARG_VALUE;

	//	We do not support METALFFT_*_FAST currently
	if( precision == METALFFT_SINGLE_FAST || precision == METALFFT_DOUBLE_FAST )
		return METALFFT_NOTIMPLEMENTED;



	//	If we modify the state of the plan, we assume that we can't trust any pre-calculated contents anymore
	fftPlan->baked		= false;
	fftPlan->precision	= precision;

	return	METALFFT_SUCCESS;
}

metalfftStatus	metalfftBakePlan(metalfftPlanHandle plHandle, unsigned int numQueues, void *user_data)
{
	return METALFFT_SUCCESS;
}


metalfftStatus metalfftInitSetupData(metalfftSetupData* setupData)
{
	setupData->major = metalfftVersionMajor;
	setupData->minor = metalfftVersionMinor;
	setupData->patch = metalfftVersionPatch;
	setupData->debugFlags = 0;

	return	METALFFT_SUCCESS;
}

metalfftStatus	metalfftTeardown()
{
	FFTRepo& fftRepo = FFTRepo::getInstance();
	fftRepo.releaseResources();
	return	METALFFT_SUCCESS;
}

metalfftStatus	metalfftSetPlanPrecision(metalfftPlanHandle plHandle, metalfftPrecision precision)
{
	FFTRepo& fftRepo = FFTRepo::getInstance();
	FFTPlan* fftPlan = NULL;
	lockRAII* planLock = NULL;

	OPENCL_V(fftRepo.getPlan(plHandle, fftPlan, planLock), _T("fftRepo.getPlan failed"));
	scopedLock sLock(*planLock, _T("metalfftSetPlanPrecision"));

	if (precision >= METALFFT_ENDPRECISION)
		return METALFFT_INVALID_ARG_VALUE;

	//	If we modify the state of the plan, we assume that we can't trust any pre-calculated contents anymore
	fftPlan->baked = false;
	fftPlan->precision = precision;

	return	METALFFT_SUCCESS;
}

metalfftStatus	metalfftSetPlanFFTmethod(metalfftPlanHandle plHandle, metalfftMethod  method)
{
	FFTRepo& fftRepo = FFTRepo::getInstance();
	FFTPlan* fftPlan = NULL;
	lockRAII* planLock = NULL;

	OPENCL_V(fftRepo.getPlan(plHandle, fftPlan, planLock), _T("fftRepo.getPlan failed"));
	scopedLock sLock(*planLock, _T("metalfftSetPlanPrecision"));

	if (method >= METALFFT_ENDMETHOD)
		return METALFFT_INVALID_ARG_VALUE;

	fftPlan->baked = false;
	fftPlan->method = method;

	return	METALFFT_SUCCESS;
}

metalfftStatus	metalfftSetPlanDevice(metalfftPlanHandle plHandle, const amf::AMFComputeDevicePtr & computeDevice)
{
	FFTRepo& fftRepo = FFTRepo::getInstance();
	FFTPlan* fftPlan = NULL;
	lockRAII* planLock = NULL;

	OPENCL_V(fftRepo.getPlan(plHandle, fftPlan, planLock), _T("fftRepo.getPlan failed"));
	scopedLock sLock(*planLock, _T("metalfftSetPlanPrecision"));

	fftPlan->baked = false;
	fftPlan->pComputeDevice = computeDevice;

	return	METALFFT_SUCCESS;
}

METALFFTAPI metalfftStatus	metalfftSetPlanContext(metalfftPlanHandle plHandle, const amf::AMFContextPtr & context)
{
	FFTRepo& fftRepo = FFTRepo::getInstance();
	FFTPlan* fftPlan = NULL;
	lockRAII* planLock = NULL;

	OPENCL_V(fftRepo.getPlan(plHandle, fftPlan, planLock), _T("fftRepo.getPlan failed"));
	scopedLock sLock(*planLock, _T("metalfftSetPlanPrecision"));

	fftPlan->baked = false;
	fftPlan->pContext = context;

	return	METALFFT_SUCCESS;
}

metalfftStatus	metalfftEnqueueTransform(
	metalfftPlanHandle plHandle,
	metalfftDirection dir,
	amf::AMFBuffer **input,
	int inputCount,
	amf::AMFBuffer **output,
	int outputCount
)
{
	FFTRepo& fftRepo = FFTRepo::getInstance();
	FFTPlan* fftPlan = NULL;
	lockRAII* planLock = NULL;

	OPENCL_V(fftRepo.getPlan(plHandle, fftPlan, planLock), _T("fftRepo.getPlan failed"));
	scopedLock sLock(*planLock, _T("clfftSetResultLocation"));

	if (fftPlan->method == METALFFT_HOST || fftPlan->method == METALFFT_AUTO)
	{
		metalfftStatus err;
		HostFFTAction action(err);
		if (err != METALFFT_SUCCESS)
			return err;
		return action.enqueue(plHandle, dir, input, 1, output, 0);
	} else if (fftPlan->method == METALFFT_METAL)
	{
		metalfftStatus err;
		MetalFFTAction action(err);
		if (err != METALFFT_SUCCESS)
			return err;
		return action.enqueue(plHandle, dir, input, 1, output, 0);
	}

	return METALFFT_NOTIMPLEMENTED;
}

metalfftStatus	metalfftDestroyPlan(metalfftPlanHandle* plHandle)
{
	FFTRepo& fftRepo = FFTRepo::getInstance();
	FFTPlan* fftPlan = NULL;
	lockRAII* planLock = NULL;

	OPENCL_V(fftRepo.getPlan(*plHandle, fftPlan, planLock), _T("fftRepo.getPlan failed"));

	fftRepo.deletePlan(plHandle);

	return	METALFFT_SUCCESS;
}

METALFFTAPI metalfftStatus	metalfftSetup(const metalfftSetupData* setupData, amf::AMFPrograms* pPrograms)
{
	scopedLock sLock(FFTRepo::lockRepo(), _T("FFTRepo::getInstance"));

	FFTRepo& fftRepo = FFTRepo::getInstance();
	MetalFFTAction::Register(pPrograms);
	//MetalFFTAction::RegisterCL(pPrograms);

	// If the client has no setupData, we are done
	if (setupData == NULL)
		return METALFFT_SUCCESS;
	fftRepo.setupData = *setupData;

	return	METALFFT_SUCCESS;
}

metalfftStatus	metalfftCreateDefaultPlan(metalfftPlanHandle* plHandle, const metalfftDim dim,
	const size_t* clLengths)
{
	FFTPlan *fftPlan = NULL;
	FFTRepo& fftRepo = FFTRepo::getInstance();
	OPENCL_V(fftRepo.createPlan(plHandle, fftPlan), _T("fftRepo.insertPlan failed"));

	fftPlan->baked = false;
	fftPlan->precision = METALFFT_SINGLE;
	fftPlan->inputLayout = METALFFT_COMPLEX_INTERLEAVED;
	fftPlan->outputLayout = METALFFT_COMPLEX_INTERLEAVED;
	fftPlan->dim = dim;
	fftPlan->placeness = METALFFT_INPLACE;
	fftPlan->method = METALFFT_AUTO; //default setting

	fftPlan->userPlan = true;
	fftPlan->plHandle = *plHandle;

	return METALFFT_SUCCESS;
}

metalfftStatus metalfftSetLayout( metalfftPlanHandle plHandle, metalfftLayout iLayout, metalfftLayout oLayout )
{
    FFTRepo& fftRepo	= FFTRepo::getInstance( );
	FFTPlan* fftPlan	= NULL;
	lockRAII* planLock	= NULL;

	OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
	scopedLock sLock( *planLock, _T( "clfftSetLayout" ) );

	//	Basic error checking on parameter
	if( ( iLayout >= METALFFT_ENDLAYOUT ) || ( oLayout >= METALFFT_ENDLAYOUT ) )
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
	FFTRepo& fftRepo	= FFTRepo::getInstance( );
	FFTPlan* fftPlan	= NULL;
	lockRAII* planLock	= NULL;

	OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
	scopedLock sLock( *planLock, _T( "clfftSetResultLocation" ) );

	//	Basic error checking on parameter
	if( placeness >= METALFFT_ENDPLACE )
		return METALFFT_INVALID_ARG_VALUE;

	//	If we modify the state of the plan, we assume that we can't trust any pre-calculated contents anymore
	fftPlan->baked		= false;
	fftPlan->placeness	= placeness;

	return	METALFFT_SUCCESS;
}
