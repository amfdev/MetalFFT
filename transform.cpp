#include "metalFFT.h"

metalfftStatus	metalfftEnqueueTransform(
                                            metalfftPlanHandle plHandle,
                                            metalfftDirection dir,
                                            unsigned int numQueuesAndEvents,
                                            //cl_command_queue* commQueues,
                                            unsigned int numWaitEvents//,
                                            //const cl_event* waitEvents,
                                            //cl_event* outEvents,
                                            //cl_mem* inputBuffers,
                                            //cl_mem* outputBuffers,
                                            //cl_mem tmpBuffer
                                            )
{
    cl_int status = CLFFT_SUCCESS;

	FFTRepo& fftRepo	= FFTRepo::getInstance( );
	FFTPlan* fftPlan	= NULL;
	lockRAII* planLock	= NULL;

	//	At this point, the user wants to enqueue a plan to execute.  We lock the plan down now, such that
	//	after we finish baking the plan (if the user did not do that explicitely before), the plan cannot
	//	change again through the action of other thread before we enqueue this plan for execution.
	OPENCL_V( fftRepo.getPlan( plHandle, fftPlan, planLock ), _T( "fftRepo.getPlan failed" ) );
	scopedLock sLock( *planLock, _T( "clfftGetPlanBatchSize" ) );

	if( fftPlan->baked == false )
	{
		OPENCL_V( metalfftBakePlan( plHandle, numQueuesAndEvents, commQueues, NULL, NULL ), _T( "Failed to bake plan" ) );
	}

	if		(fftPlan->inputLayout == METALFFT_REAL)	dir = METALFFT_FORWARD;
	else if	(fftPlan->outputLayout == METALFFT_REAL)	dir = METALFFT_BACKWARD;


	// we do not check the user provided buffer at this release
	cl_mem localIntBuffer = clTmpBuffers;

	if( clTmpBuffers == NULL && fftPlan->tmpBufSize > 0 && fftPlan->intBuffer == NULL)
	{
		// create the intermediate buffers
		// The intermediate buffer is always interleave and packed
		// For outofplace operation, we have the choice not to create intermediate buffer
		// input ->(col+Transpose) output ->(col) output
		fftPlan->intBuffer = clCreateBuffer( fftPlan->context, CL_MEM_READ_WRITE,
			fftPlan->tmpBufSize, 0, &status);
		OPENCL_V( status, _T("Creating the intermediate buffer for large1D Failed") );
		fftPlan->libCreatedIntBuffer = true;

#if defined(DEBUGGING)
		std::cout << "One intermediate buffer is created" << std::endl;
#endif
	}

	if( localIntBuffer == NULL && fftPlan->intBuffer != NULL )
		localIntBuffer = fftPlan->intBuffer;

	if( fftPlan->intBufferRC == NULL && fftPlan->tmpBufSizeRC > 0 )
	{
		fftPlan->intBufferRC = clCreateBuffer( fftPlan->context, CL_MEM_READ_WRITE, fftPlan->tmpBufSizeRC, 0, &status);
		OPENCL_V( status, _T("Creating the intermediate buffer for large1D RC Failed") );
	}

	if( fftPlan->intBufferC2R == NULL && fftPlan->tmpBufSizeC2R > 0 )
	{
		fftPlan->intBufferC2R = clCreateBuffer( fftPlan->context, CL_MEM_READ_WRITE, fftPlan->tmpBufSizeC2R, 0, &status);
		OPENCL_V( status, _T("Creating the intermediate buffer for large1D YZ C2R Failed") );
	}

	//	The largest vector we can transform in a single pass
	//	depends on the GPU caps -- especially the amount of LDS
	//	available
	//
	size_t Large1DThreshold = 0;
	OPENCL_V(fftPlan->GetMax1DLength (&Large1DThreshold), _T("GetMax1DLength failed"));
	BUG_CHECK (Large1DThreshold > 1);

	//Large1DThreshold = 128;

	if(fftPlan->gen != Copy)
	switch( fftPlan->dim )
	{
		case CLFFT_1D:
		{
			if ( Is1DPossible(fftPlan->length[0], Large1DThreshold) )
				break;

			if( ( fftPlan->inputLayout == CLFFT_REAL ) && ( fftPlan->planTZ != 0) )
			{
					//First transpose
					// Input->tmp
					cl_event transTXOutEvents = NULL;
					OPENCL_V( clfftEnqueueTransform( fftPlan->planTX, dir, numQueuesAndEvents, commQueues, numWaitEvents,
						waitEvents, &transTXOutEvents, clInputBuffers, &localIntBuffer, NULL ),
						_T("clfftEnqueueTransform for large1D transTX failed"));

					cl_mem *mybuffers;
					if (fftPlan->placeness==CLFFT_INPLACE)
						mybuffers = clInputBuffers;
					else
						mybuffers = clOutputBuffers;
					//First Row
					//tmp->output
					cl_event rowXOutEvents = NULL;
					OPENCL_V( clfftEnqueueTransform( fftPlan->planX, dir, numQueuesAndEvents, commQueues, 1,
						&transTXOutEvents, &rowXOutEvents, &localIntBuffer, &(fftPlan->intBufferRC), NULL ),
						_T("clfftEnqueueTransform for large1D rowX failed"));
					clReleaseEvent(transTXOutEvents);

					//Second Transpose
					// output->tmp
					cl_event transTYOutEvents = NULL;
					OPENCL_V( clfftEnqueueTransform( fftPlan->planTY, dir, numQueuesAndEvents, commQueues, 1,
						&rowXOutEvents, &transTYOutEvents, &(fftPlan->intBufferRC), &localIntBuffer, NULL ),
						_T("clfftEnqueueTransform for large1D transTY failed"));
					clReleaseEvent(rowXOutEvents);

					//Second Row
					//tmp->tmp, inplace
					cl_event rowYOutEvents = NULL;
					OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1,
						&transTYOutEvents, &rowYOutEvents, &localIntBuffer, &(fftPlan->intBufferRC), NULL ),
						_T("clfftEnqueueTransform for large1D rowY failed"));
					clReleaseEvent(transTYOutEvents);
					//Third Transpose
					// tmp->output
					OPENCL_V( clfftEnqueueTransform( fftPlan->planTZ, dir, numQueuesAndEvents, commQueues, 1,
						&rowYOutEvents, outEvents, &(fftPlan->intBufferRC), mybuffers, NULL ),
						_T("clfftEnqueueTransform for large1D transTZ failed"));
					clReleaseEvent(rowYOutEvents);
			}
			else if ( fftPlan->inputLayout == CLFFT_REAL )
			{
				cl_event colOutEvents = NULL;
				cl_event copyInEvents = NULL;

				// First pass
				// column with twiddle first, OUTOFPLACE, + transpose
				OPENCL_V( clfftEnqueueTransform( fftPlan->planX, CLFFT_FORWARD, numQueuesAndEvents, commQueues, numWaitEvents,
					waitEvents, &colOutEvents, clInputBuffers, &(fftPlan->intBufferRC), localIntBuffer),
					_T("clfftEnqueueTransform large1D col pass failed"));


				cl_mem *out_local;
				out_local = (fftPlan->placeness==CLFFT_INPLACE) ? clInputBuffers : clOutputBuffers;


				// another column FFT output, INPLACE
				OPENCL_V(clfftEnqueueTransform(fftPlan->planY, CLFFT_FORWARD, numQueuesAndEvents, commQueues, 1, &colOutEvents,
					&copyInEvents, &(fftPlan->intBufferRC), &(fftPlan->intBufferRC), localIntBuffer),
					_T("clfftEnqueueTransform large1D second column failed"));
				clReleaseEvent(colOutEvents);

				// copy from full complex to hermitian
				OPENCL_V(clfftEnqueueTransform(fftPlan->planRCcopy, CLFFT_FORWARD, numQueuesAndEvents, commQueues, 1, &copyInEvents,
					outEvents, &(fftPlan->intBufferRC), out_local, localIntBuffer),
					_T("clfftEnqueueTransform large1D RC copy failed"));
				clReleaseEvent(copyInEvents);
			}
			else if( fftPlan->outputLayout == CLFFT_REAL )
			{
				cl_event colOutEvents = NULL;
				cl_event copyOutEvents = NULL;

				if (fftPlan->planRCcopy)
				{
					// copy from hermitian to full complex
					OPENCL_V(clfftEnqueueTransform(fftPlan->planRCcopy, CLFFT_BACKWARD, numQueuesAndEvents, commQueues, numWaitEvents,
						waitEvents, &copyOutEvents, clInputBuffers, &(fftPlan->intBufferRC), localIntBuffer),
						_T("clfftEnqueueTransform large1D RC copy failed"));

					// First pass
					// column with twiddle first, INPLACE,
					OPENCL_V(clfftEnqueueTransform(fftPlan->planX, CLFFT_BACKWARD, numQueuesAndEvents, commQueues, 1,
						&copyOutEvents, &colOutEvents, &(fftPlan->intBufferRC), &(fftPlan->intBufferRC), localIntBuffer),
						_T("clfftEnqueueTransform large1D col pass failed"));
					clReleaseEvent(copyOutEvents);
				}
				else
				{
					// First pass
					// column with twiddle first, INPLACE,
					OPENCL_V(clfftEnqueueTransform(fftPlan->planX, CLFFT_BACKWARD, numQueuesAndEvents, commQueues, numWaitEvents,
						waitEvents, &colOutEvents, clInputBuffers, &(fftPlan->intBufferRC), localIntBuffer),
						_T("clfftEnqueueTransform large1D col pass failed"));
					clReleaseEvent(copyOutEvents);
				}

				cl_mem *out_local;
				out_local = (fftPlan->placeness==CLFFT_INPLACE) ? clInputBuffers : clOutputBuffers;

				// another column FFT output, OUTOFPLACE + transpose
				OPENCL_V( clfftEnqueueTransform( fftPlan->planY, CLFFT_BACKWARD, numQueuesAndEvents, commQueues, 1, &colOutEvents,
					outEvents, &(fftPlan->intBufferRC), out_local, localIntBuffer ),
					_T("clfftEnqueueTransform large1D second column failed"));
				clReleaseEvent(colOutEvents);
			}
			else
			{
				if (fftPlan->transflag)
				{
					//First transpose
					// Input->tmp
					cl_event transTXOutEvents = NULL;
					if(fftPlan->allOpsInplace)
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planTX, dir, numQueuesAndEvents, commQueues, numWaitEvents,
							waitEvents, &transTXOutEvents, clInputBuffers, NULL, NULL ),
							_T("clfftEnqueueTransform for large1D transTX failed"));
					}
					else
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planTX, dir, numQueuesAndEvents, commQueues, numWaitEvents,
							waitEvents, &transTXOutEvents, clInputBuffers, &localIntBuffer, NULL ),
							_T("clfftEnqueueTransform for large1D transTX failed"));
					}

					cl_mem *mybuffers;
					if (fftPlan->placeness==CLFFT_INPLACE)
						mybuffers = clInputBuffers;
					else
						mybuffers = clOutputBuffers;

					//First Row
					//tmp->output
					cl_event rowXOutEvents = NULL;
					if(fftPlan->allOpsInplace)
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planX, dir, numQueuesAndEvents, commQueues, 1,
							&transTXOutEvents, &rowXOutEvents, clInputBuffers, NULL, NULL ),
							_T("clfftEnqueueTransform for large1D rowX failed"));
					}
					else
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planX, dir, numQueuesAndEvents, commQueues, 1,
							&transTXOutEvents, &rowXOutEvents, &localIntBuffer, mybuffers, NULL ),
							_T("clfftEnqueueTransform for large1D rowX failed"));
					}
					clReleaseEvent(transTXOutEvents);

					//Second Transpose
					// output->tmp
					cl_event transTYOutEvents = NULL;
					if(fftPlan->allOpsInplace)
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planTY, dir, numQueuesAndEvents, commQueues, 1,
							&rowXOutEvents, &transTYOutEvents, clInputBuffers, NULL, NULL ),
							_T("clfftEnqueueTransform for large1D transTY failed"));
					}
					else
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planTY, dir, numQueuesAndEvents, commQueues, 1,
							&rowXOutEvents, &transTYOutEvents, mybuffers, &localIntBuffer, NULL ),
							_T("clfftEnqueueTransform for large1D transTY failed"));
					}
					clReleaseEvent(rowXOutEvents);

					//Second Row
					//tmp->tmp, inplace
					cl_event rowYOutEvents = NULL;
					if(fftPlan->allOpsInplace)
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1,
							&transTYOutEvents, &rowYOutEvents, clInputBuffers, NULL, NULL ),
							_T("clfftEnqueueTransform for large1D rowY failed"));
					}
					else
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1,
							&transTYOutEvents, &rowYOutEvents, &localIntBuffer, NULL, NULL ),
							_T("clfftEnqueueTransform for large1D rowY failed"));
					}
					clReleaseEvent(transTYOutEvents);

					//Third Transpose
					// tmp->output
					if(fftPlan->allOpsInplace)
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planTZ, dir, numQueuesAndEvents, commQueues, 1,
							&rowYOutEvents, outEvents, clInputBuffers, NULL, NULL ),
							_T("clfftEnqueueTransform for large1D transTZ failed"));
					}
					else
					{
						OPENCL_V( clfftEnqueueTransform( fftPlan->planTZ, dir, numQueuesAndEvents, commQueues, 1,
							&rowYOutEvents, outEvents, &localIntBuffer, mybuffers, NULL ),
							_T("clfftEnqueueTransform for large1D transTZ failed"));
					}
					clReleaseEvent(rowYOutEvents);

				}
				else
				{
					if (fftPlan->large1D == 0)
					{
						if(fftPlan->planCopy)
						{
							// Transpose OUTOFPLACE
							cl_event transTXOutEvents = NULL;
							OPENCL_V( clfftEnqueueTransform( fftPlan->planTX, dir, numQueuesAndEvents, commQueues, numWaitEvents,
								waitEvents, &transTXOutEvents, clInputBuffers, &localIntBuffer, NULL ),
								_T("clfftEnqueueTransform for large1D transTX failed"));

							// FFT INPLACE
							cl_event rowXOutEvents = NULL;
							OPENCL_V( clfftEnqueueTransform( fftPlan->planX, dir, numQueuesAndEvents, commQueues, 1,
								&transTXOutEvents, &rowXOutEvents, &localIntBuffer, NULL, NULL),
								_T("clfftEnqueueTransform large1D first row pass failed"));
							clReleaseEvent(transTXOutEvents);

							// FFT INPLACE
							cl_event colYOutEvents = NULL;
							OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1, &rowXOutEvents,
								&colYOutEvents, &localIntBuffer, NULL, NULL ),
								_T("clfftEnqueueTransform large1D second column failed"));
							clReleaseEvent(rowXOutEvents);
									
							cl_mem *mybuffers;
							if (fftPlan->placeness==CLFFT_INPLACE)
								mybuffers = clInputBuffers;
							else
								mybuffers = clOutputBuffers;
						
							// Copy kernel
							OPENCL_V( clfftEnqueueTransform( fftPlan->planCopy, dir, numQueuesAndEvents, commQueues, 1, &colYOutEvents,
								outEvents, &localIntBuffer, mybuffers, NULL ),
								_T("clfftEnqueueTransform large1D copy failed"));
							clReleaseEvent(colYOutEvents);
						}
						else
						{
							cl_event colOutEvents = NULL;
							// First pass
							// column with twiddle first, OUTOFPLACE
							OPENCL_V( clfftEnqueueTransform( fftPlan->planX, dir, numQueuesAndEvents, commQueues, numWaitEvents,
								waitEvents, &colOutEvents, clInputBuffers, &localIntBuffer, NULL),
								_T("clfftEnqueueTransform large1D col pass failed"));

							if(fftPlan->planTZ)
							{
								cl_event rowYOutEvents = NULL;
								OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1, &colOutEvents,
									&rowYOutEvents, &localIntBuffer, NULL, NULL ),
									_T("clfftEnqueueTransform large1D second row failed"));

								if (fftPlan->placeness == CLFFT_INPLACE)
								{
									OPENCL_V( clfftEnqueueTransform( fftPlan->planTZ, dir, numQueuesAndEvents, commQueues, 1, &rowYOutEvents,
										outEvents, &localIntBuffer, clInputBuffers, NULL ),
										_T("clfftEnqueueTransform large1D trans3 failed"));
								}
								else
								{
									OPENCL_V( clfftEnqueueTransform( fftPlan->planTZ, dir, numQueuesAndEvents, commQueues, 1, &rowYOutEvents,
										outEvents, &localIntBuffer, clOutputBuffers, NULL ),
										_T("clfftEnqueueTransform large1D trans3 failed"));
								}
						
								clReleaseEvent(rowYOutEvents);

							}
							else
							{
								//another column FFT output, OUTOFPLACE + transpose
								if (fftPlan->placeness == CLFFT_INPLACE)
								{
									OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1, &colOutEvents,
										outEvents, &localIntBuffer, clInputBuffers, NULL ),
										_T("clfftEnqueueTransform large1D second column failed"));
								}
								else
								{
									OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1, &colOutEvents,
										outEvents, &localIntBuffer, clOutputBuffers, NULL ),
										_T("clfftEnqueueTransform large1D second column failed"));
								}
							}

							clReleaseEvent(colOutEvents);
						}
					}
					else
					{
						cl_event colOutEvents = NULL;

						// second pass for huge 1D
						// column with twiddle first, OUTOFPLACE, + transpose
						OPENCL_V( clfftEnqueueTransform( fftPlan->planX, dir, numQueuesAndEvents, commQueues, numWaitEvents,
							waitEvents, &colOutEvents, &localIntBuffer, clOutputBuffers, localIntBuffer),
							_T("clfftEnqueueTransform Huge1D col pass failed"));

						OPENCL_V( clfftEnqueueTransform( fftPlan->planY, dir, numQueuesAndEvents, commQueues, 1, &colOutEvents,
							outEvents, clOutputBuffers, clOutputBuffers, localIntBuffer ),
							_T("clfftEnqueueTransform large1D second column failed"));

						clReleaseEvent(colOutEvents);
					}
				}
			}

			return	CLFFT_SUCCESS;

		}
		case CLFFT_2D:
        case CLFFT_3D:
            return METALFFT_NOTIMPLEMENTED;
    }

	return fftPlan->action->enqueue(plHandle,
                                        dir,
                                        numQueuesAndEvents,
                                        commQueues,
                                        numWaitEvents,
                                        waitEvents,
                                        outEvents,
                                        clInputBuffers,
                                        clOutputBuffers);
}