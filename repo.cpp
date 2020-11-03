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


// clfft.repo.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "repo.h"

using std::map;
using std::string;

//	Static initialization of the plan count variable
size_t FFTRepo::planCount	= 1;

//	Handle/Address of the dynamic module that contains the timer, that we discover and load during runtime
void* FFTRepo::timerHandle	= NULL;
//GpuStatTimer* FFTRepo::pStatTimer	= NULL;

metalfftStatus FFTRepo::releaseResources( )
{
	scopedLock sLock( lockRepo(), _T( "releaseResources" ) );
	//	Free all memory allocated in the repoPlans; represents cached plans that were not destroyed by the client
	//
	for( repoPlansType::iterator iter = repoPlans.begin( ); iter != repoPlans.end( ); ++iter )
	{
		FFTPlan* plan	= iter->second.first;
		lockRAII* lock	= iter->second.second;
		if( plan != NULL )
		{
			delete plan;
		}
		if( lock != NULL )
		{
			delete lock;
		}
	}

	//	Reset the plan count to zero because we are guaranteed to have destroyed all plans
	planCount	= 1;

	return	METALFFT_SUCCESS;
}

metalfftStatus FFTRepo::createPlan( metalfftPlanHandle* plHandle, FFTPlan*& fftPlan )
{
	scopedLock sLock( lockRepo(), _T( "insertPlan" ) );

	//	We keep track of this memory in our own collection class, to make sure it's freed in releaseResources
	//	The lifetime of a plan is tracked by the client and is freed when the client calls ::clfftDestroyPlan()
	fftPlan	= new FFTPlan;

	//	We allocate a new lock here, and expect it to be freed in ::clfftDestroyPlan();
	//	The lifetime of the lock is the same as the lifetime of the plan
	lockRAII* lockPlan	= new lockRAII;

	//	Add and remember the fftPlan in our map
	repoPlans[ planCount ] = std::make_pair( fftPlan, lockPlan );

	//	Assign the user handle the plan count (unique identifier), and bump the count for the next plan
	*plHandle	= planCount++;

	return	METALFFT_SUCCESS;
}

metalfftStatus FFTRepo::getPlan( metalfftPlanHandle plHandle, FFTPlan*& fftPlan, lockRAII*& planLock )
{
	scopedLock sLock( lockRepo(), _T( "getPlan" ) );

	//	First, check if we have already created a plan with this exact same FFTPlan
	repoPlansType::iterator iter	= repoPlans.find( plHandle );
	if( iter == repoPlans.end( ) )
		return	METALFFT_INVALID_PLAN;

	//	If plan is valid, return fill out the output pointers
	fftPlan		= iter->second.first;
	planLock	= iter->second.second;

	return	METALFFT_SUCCESS;
}

metalfftStatus FFTRepo::deletePlan( metalfftPlanHandle* plHandle )
{
	scopedLock sLock( lockRepo(), _T( "deletePlan" ) );

	//	First, check if we have already created a plan with this exact same FFTPlan
	repoPlansType::iterator iter	= repoPlans.find( *plHandle );
	if( iter == repoPlans.end( ) )
		return	METALFFT_INVALID_PLAN;

	//	We lock the plan object while we are in the process of deleting it
	{
		scopedLock sLock( *iter->second.second, _T( "clfftDestroyPlan" ) );

		//	Delete the FFTPlan
		delete iter->second.first;
	}

		//	Delete the lockRAII
	delete iter->second.second;

	//	Remove entry from our map object
	repoPlans.erase( iter );

	//	Clear the client's handle to signify that the plan is gone
	*plHandle = 0;

	return	METALFFT_SUCCESS;
}
