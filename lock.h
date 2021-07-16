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
#if !defined( CLFFT_lock_H )
#define CLFFT_lock_H

#include "unicode.compatibility.h"

#include <pthread.h>
#include <vector>
#include <string>
#include <locale>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cassert>

//	lockRAII provides an abstraction for the concept of a mutex; it wraps all  mutex functions in generic methods
//	Linux implementation not done yet
//	The template argument 'debugPrint' activates debugging information, but if not active the compiler optimizes
//	the print statements out
template< bool debugPrint >
class lockRAII
{
	pthread_mutex_t	mutex;
	pthread_mutexattr_t mAttr;
	tstring			mutexName;
	tstringstream	tstream;

	//	Does not make sense to create a copy of a lock object; private method
	lockRAII( const lockRAII& rhs ): mutexName( rhs.mutexName )
	{
		tstream << std::hex << std::showbase;
	}

	public:
		lockRAII( )
		{
			tstream << std::hex << std::showbase;
			pthread_mutexattr_init( &mAttr );
			pthread_mutexattr_settype( &mAttr, PTHREAD_MUTEX_RECURSIVE );
			pthread_mutex_init( &mutex, &mAttr );
		}

		lockRAII( const tstring& name ): mutexName( name )
		{
			tstream << std::hex << std::showbase;
			pthread_mutexattr_init( &mAttr );
			pthread_mutexattr_settype( &mAttr, PTHREAD_MUTEX_RECURSIVE );
			pthread_mutex_init( &mutex, &mAttr );
		}

		~lockRAII( )
		{
			pthread_mutex_destroy( &mutex );
			pthread_mutexattr_destroy( &mAttr );
		}

		tstring& getName( )
		{
			return mutexName;
		}

		void setName( const tstring& name )
		{
			mutexName	= name;
		}

		void enter( )
		{
			if( debugPrint )
			{
				tstream.str( _T( "" ) );
				tstream << _T( "Attempting pthread_mutex_t( " ) << mutexName << _T( " )" ) << std::endl;
				tout << tstream.str( );
			}

			::pthread_mutex_lock( &mutex );

			if( debugPrint )
			{
				tstream.str( _T( "" ) );
				tstream << _T( "Acquired pthread_mutex_t( " ) << mutexName << _T( " )" ) << std::endl;
				//tstream << _T( "\tOwningThread( " ) << mutex.OwningThread << _T( " )" ) << std::endl;
				//tstream << _T( "\tLockcount( " ) << mutex.LockCount << _T( " )" ) << std::endl;
				//tstream << _T( "\tRecursionCount( " ) << mutex.RecursionCount << _T( " )" ) << std::endl;
				tout << tstream.str( );
			}
		}

		void leave( )
		{
			if( debugPrint )
			{
				tstream.str( _T( "" ) );
				tstream << _T( "Releasing pthread_mutex_t( " ) << mutexName << _T( " )" ) << std::endl;
				//tstream << _T( "\tOwningThread( " ) << mutex.OwningThread << _T( " )" ) << std::endl;
				//tstream << _T( "\tLockcount( " ) << mutex.LockCount << _T( " )" ) << std::endl;
				//tstream << _T( "\tRecursionCount( " ) << mutex.RecursionCount << _T( " )" ) << std::endl << std::endl;
				tout << tstream.str( );
			}

			::pthread_mutex_unlock( &mutex );
		}
};

//	Class used to make sure that we enter and leave critical sections in pairs
//	The template logic logs our CRITICAL_SECTION actions; if the template parameter is false,
//	the branch is constant and the compiler will optimize the branch out
template< bool debugPrint >
class scopedLock
{
	lockRAII< debugPrint >* sLock;
	tstring			sLockName;
	tstringstream	tstream;

	public:
		scopedLock( lockRAII< debugPrint >& lock, const tstring& name ): sLock( &lock ), sLockName( name )
		{
			if( debugPrint )
			{
				tstream.str( _T( "" ) );
				tstream << _T( "Entering scopedLock( " ) << sLockName << _T( " )" ) << std::endl << std::endl;
				tout << tstream.str( );
			}

			sLock->enter( );
		}

		~scopedLock( )
		{
			sLock->leave( );

			if( debugPrint )
			{
				tstream.str( _T( "" ) );
				tstream << _T( "Left scopedLock( " ) << sLockName << _T( " )" ) << std::endl << std::endl;
				tout << tstream.str( );
			}
		}
};

//	Convenience macro to enable/disable debugging print statements
#define lockRAII lockRAII< false >
#define scopedLock scopedLock< false >

#endif	// CLFFT_lock_H
