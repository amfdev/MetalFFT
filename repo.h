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


#pragma once
#if !defined( CLFFT_repo_H )
#define CLFFT_repo_H
#include <map>
#include "plan.h"
#include "lock.h"

//	This class contains objects that we wish to retain between individual calls into the FFT interface.
//	These objects will be shared across different individual FFT plans, and we wish to keep only one
//	copy of these programs, objects and events.  When the client decides that they either want to reset
//	the library or release all resources, this Repo will release all acquired resources and clean itself
//	up as much as it can.  It is implemented as a Singleton object.
class	FFTRepo
{
	//	All plans that the user creates over the course of using the library are stored here.
	//	Plans can be arbitrarily created and destroyed at anytime by the user, in arbitrary order, so vector
	//	does not seem appropriate, so a map was chosen because of the O(log N) search properties
	//	A lock object is created for each plan, such that any getter/setter can lock the 'plan' object before
	//	reading/writing its values.  The lock object is kept seperate from the plan object so that the lock
	//	object can be held the entire time a plan is getting destroyed in clfftDestroyPlan.
	typedef std::pair< FFTPlan*, lockRAII* > repoPlansValue;
	typedef std::map< metalfftPlanHandle, repoPlansValue > repoPlansType;
	repoPlansType repoPlans;

	//	Static count of how many plans we have generated; always incrementing during the life of the library
	//	This is used as a unique identifier for plans
	static size_t planCount;

	// Private constructor to stop explicit instantiation
	FFTRepo( )
	{}

	// Private copy constructor to stop implicit instantiation
	FFTRepo( const FFTRepo& );

	// Private operator= to assure only 1 copy of singleton
	FFTRepo& operator=( const FFTRepo& );

	~FFTRepo( )
	{
		//	NOTE:  We can't release resources in our destructor because as a static object, the order of destruction of static objects
		//	is not guaranteed, and openCL might already have cleaned itself up.  When clFFT tries to free its resources, an access
		//	violation could occur.
		//releaseResources( );
	};

public:
	//	Used to make the FFTRepo struct thread safe; STL is not thread safe by default
	//	Optimally, we could use a lock object per STL struct, as two different STL structures
	//	can be modified at the same time, but a single lock object is easier and performance should
	//	still be good. This is implemented as a function returning a static local reference to
	//	assert that the lock must be instantiated before the result can be used.
	static lockRAII& lockRepo()
	{
		//	Static initialization of the repo lock variable
		static lockRAII lock(_T("FFTRepo"));
		return lock;
	}

	//	Our runtime library can instrument kernel timings with a GPU timer available in a shared module
	//	Handle/Address of the dynamic module that contains timers
	static void* timerHandle;

	//	Pointer to the timer class queried from the timer shared library
	//static GpuStatTimer* pStatTimer;

	//	Global debug flags that the FFT engine can check to control various debug logic
	metalfftSetupData setupData;

	//	Everybody who wants to access the Repo calls this function to get a repo reference
	static FFTRepo& getInstance( )
	{
		static FFTRepo fftRepo;
		return	fftRepo;
	};

	metalfftStatus releaseResources( );

	metalfftStatus createPlan( metalfftPlanHandle* plHandle, FFTPlan*& fftPlan );
	metalfftStatus getPlan( metalfftPlanHandle plHandle, FFTPlan*& fftPlan, lockRAII*& planLock );
	metalfftStatus deletePlan( metalfftPlanHandle* plHandle );
};

#endif

