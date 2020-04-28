#include "metalFFT.h"

metalfftStatus clfftInitSetupData( metalfftSetupData* setupData )
{
	//setupData->major	= clfftVersionMajor;
	//setupData->minor	= clfftVersionMinor;
	//setupData->patch	= clfftVersionPatch;
	setupData->debugFlags	= 0;

	return	METALFFT_SUCCESS;
}

//	Allow AMD's implementation of FFT's to allocate internal resources
metalfftStatus	metalfftSetup( const metalfftSetupData* sData )
{
	//	Static data is not thread safe (to create), so we implement a lock to protect instantiation for the first call
	//	Implemented outside of FFTRepo::getInstance to minimize lock overhead; this is only necessary on first creation
	//scopedLock sLock( FFTRepo::lockRepo(), _T( "FFTRepo::getInstance" ) );

	//	First invocation of this function will allocate the FFTRepo singleton; thereafter the object always exists
	//FFTRepo& fftRepo	= FFTRepo::getInstance( );

	//clfftInitRequestLibNoMemAlloc();
	//clfftInitBinaryCache();

	//	Discover and load the timer module if present
	// fftRepo.timerHandle = LoadSharedLibrary( "lib", "StatTimer", true );
	// if( fftRepo.timerHandle )
	// {
	// 	//	Timer module discovered and loaded successfully
	// 	//	Initialize function pointers to call into the shared module
	// 	PFGETSTATTIMER pfGetStatTimer = reinterpret_cast< PFGETSTATTIMER > ( LoadFunctionAddr( fftRepo.timerHandle, "getStatTimer" ) );

	// 	//	Create and initialize our timer class, if the external timer shared library loaded
	// 	if( pfGetStatTimer )
	// 	{
	// 		fftRepo.pStatTimer = reinterpret_cast< GpuStatTimer* > ( pfGetStatTimer( CLFFT_GPU ) );
	// 	}
	// }

	// If the client has no setupData, we are done
	if( sData == NULL )
		return METALFFT_SUCCESS;

	//	Versioning checks commented out until necessary
	////	If the major version number between the client and library do not match, return mismatch
	//if( sData->major > clfftVersionMajor )
	//	return CLFFT_VERSION_MISMATCH;

	////	If the minor version number between the client and library do not match, return mismatch
	//if( sData->minor > clfftVersionMinor )
	//	return CLFFT_VERSION_MISMATCH;

	////	We ignore patch version number for version validation

	//fftRepo.setupData	= *sData;

	return	METALFFT_NOTIMPLEMENTED;
}

metalfftStatus	metalfftTeardown( )
{
	// FFTRepo& fftRepo	= FFTRepo::getInstance( );
	// fftRepo.releaseResources( );

	// FreeSharedLibrary( fftRepo.timerHandle );

	return	METALFFT_NOTIMPLEMENTED;
}