#pragma once

#include <stdint.h>
#include <stdio.h>
#include "unicode.compatibility.h"

/*!  @brief metalfft error codes definition(incorporating OpenCL error definitions)
 *
 *   This enumeration is a superset of the OpenCL error codes.  For example, CL_OUT_OF_HOST_MEMORY,
 *   which is defined in cl.h is aliased as METALFFT_OUT_OF_HOST_MEMORY.  The set of basic OpenCL
 *   error codes is extended to add extra values specific to the metalfft package.
 */
// enum metalfftStatus_
// {
// 	METALFFT_INVALID_GLOBAL_WORK_SIZE			= CL_INVALID_GLOBAL_WORK_SIZE,
// 	METALFFT_INVALID_MIP_LEVEL					= CL_INVALID_MIP_LEVEL,
// 	METALFFT_INVALID_BUFFER_SIZE				= CL_INVALID_BUFFER_SIZE,
// 	METALFFT_INVALID_GL_OBJECT					= CL_INVALID_GL_OBJECT,
// 	METALFFT_INVALID_OPERATION					= CL_INVALID_OPERATION,
// 	METALFFT_INVALID_EVENT						= CL_INVALID_EVENT,
// 	METALFFT_INVALID_EVENT_WAIT_LIST			= CL_INVALID_EVENT_WAIT_LIST,
// 	METALFFT_INVALID_GLOBAL_OFFSET				= CL_INVALID_GLOBAL_OFFSET,
// 	METALFFT_INVALID_WORK_ITEM_SIZE			= CL_INVALID_WORK_ITEM_SIZE,
// 	METALFFT_INVALID_WORK_GROUP_SIZE			= CL_INVALID_WORK_GROUP_SIZE,
// 	METALFFT_INVALID_WORK_DIMENSION			= CL_INVALID_WORK_DIMENSION,
// 	METALFFT_INVALID_KERNEL_ARGS				= CL_INVALID_KERNEL_ARGS,
// 	METALFFT_INVALID_ARG_SIZE					= CL_INVALID_ARG_SIZE,
// 	METALFFT_INVALID_ARG_VALUE					= CL_INVALID_ARG_VALUE,
// 	METALFFT_INVALID_ARG_INDEX					= CL_INVALID_ARG_INDEX,
// 	METALFFT_INVALID_KERNEL					= CL_INVALID_KERNEL,
// 	METALFFT_INVALID_KERNEL_DEFINITION			= CL_INVALID_KERNEL_DEFINITION,
// 	METALFFT_INVALID_KERNEL_NAME				= CL_INVALID_KERNEL_NAME,
// 	METALFFT_INVALID_PROGRAM_EXECUTABLE		= CL_INVALID_PROGRAM_EXECUTABLE,
// 	METALFFT_INVALID_PROGRAM					= CL_INVALID_PROGRAM,
// 	METALFFT_INVALID_BUILD_OPTIONS				= CL_INVALID_BUILD_OPTIONS,
// 	METALFFT_INVALID_BINARY					= CL_INVALID_BINARY,
// 	METALFFT_INVALID_SAMPLER					= CL_INVALID_SAMPLER,
// 	METALFFT_INVALID_IMAGE_SIZE				= CL_INVALID_IMAGE_SIZE,
// 	METALFFT_INVALID_IMAGE_FORMAT_DESCRIPTOR	= CL_INVALID_IMAGE_FORMAT_DESCRIPTOR,
// 	METALFFT_INVALID_MEM_OBJECT				= CL_INVALID_MEM_OBJECT,
// 	METALFFT_INVALID_HOST_PTR					= CL_INVALID_HOST_PTR,
// 	METALFFT_INVALID_COMMAND_QUEUE				= CL_INVALID_COMMAND_QUEUE,
// 	METALFFT_INVALID_QUEUE_PROPERTIES			= CL_INVALID_QUEUE_PROPERTIES,
// 	METALFFT_INVALID_CONTEXT					= CL_INVALID_CONTEXT,
// 	METALFFT_INVALID_DEVICE					= CL_INVALID_DEVICE,
// 	METALFFT_INVALID_PLATFORM					= CL_INVALID_PLATFORM,
// 	METALFFT_INVALID_DEVICE_TYPE				= CL_INVALID_DEVICE_TYPE,
// 	METALFFT_INVALID_VALUE						= CL_INVALID_VALUE,
// 	METALFFT_MAP_FAILURE						= CL_MAP_FAILURE,
// 	METALFFT_BUILD_PROGRAM_FAILURE				= CL_BUILD_PROGRAM_FAILURE,
// 	METALFFT_IMAGE_FORMAT_NOT_SUPPORTED		= CL_IMAGE_FORMAT_NOT_SUPPORTED,
// 	METALFFT_IMAGE_FORMAT_MISMATCH				= CL_IMAGE_FORMAT_MISMATCH,
// 	METALFFT_MEM_COPY_OVERLAP					= CL_MEM_COPY_OVERLAP,
// 	METALFFT_PROFILING_INFO_NOT_AVAILABLE		= CL_PROFILING_INFO_NOT_AVAILABLE,
// 	METALFFT_OUT_OF_HOST_MEMORY				= CL_OUT_OF_HOST_MEMORY,
// 	METALFFT_OUT_OF_RESOURCES					= CL_OUT_OF_RESOURCES,
// 	METALFFT_MEM_OBJECT_ALLOCATION_FAILURE		= CL_MEM_OBJECT_ALLOCATION_FAILURE,
// 	METALFFT_COMPILER_NOT_AVAILABLE			= CL_COMPILER_NOT_AVAILABLE,
// 	METALFFT_DEVICE_NOT_AVAILABLE				= CL_DEVICE_NOT_AVAILABLE,
// 	METALFFT_DEVICE_NOT_FOUND					= CL_DEVICE_NOT_FOUND,
// 	METALFFT_SUCCESS							= CL_SUCCESS,
// 	//-------------------------- Extended status codes for metalfft ----------------------------------------
// 	METALFFT_BUGCHECK =  4*1024,	/*!< Bugcheck. */
// 	METALFFT_NOTIMPLEMENTED,		/*!< Functionality is not implemented yet. */
// 	METALFFT_TRANSPOSED_NOTIMPLEMENTED, /*!< Transposed functionality is not implemented for this transformation. */
// 	METALFFT_FILE_NOT_FOUND,		/*!< Tried to open an existing file on the host system, but failed. */
// 	METALFFT_FILE_CREATE_FAILURE,	/*!< Tried to create a file on the host system, but failed. */
// 	METALFFT_VERSION_MISMATCH,		/*!< Version conflict between client and library. */
// 	METALFFT_INVALID_PLAN,			/*!< Requested plan could not be found. */
// 	METALFFT_DEVICE_NO_DOUBLE,		/*!< Double precision not supported on this device. */
// 	METALFFT_DEVICE_MISMATCH,		/*!< Attempt to run on a device using a plan baked for a different device. */
// 	METALFFT_ENDSTATUS				/* The last value of the enum, and marks the length of metalfftStatus. */
// };

typedef enum metalfftStatus_
{
	METALFFT_SUCCESS							= 0,
	METALFFT_INVALID_ARG_VALUE,
	METALFFT_NOTIMPLEMENTED,
	METALFFT_INVALID_PLAN,
	METALFFT_ENDSTATUS				/* The last value of the enum, and marks the length of metalfftStatus. */
} metalfftStatus;

/*!  @brief Specify the expected layouts of the buffers */
typedef enum metalfftLayout_
{
	METALFFT_COMPLEX_INTERLEAVED	= 1,	/*!< An array of complex numbers, with real and imaginary components together (default). */
	METALFFT_COMPLEX_PLANAR,				/*!< Separate arrays of real components and imaginary components. */
	METALFFT_HERMITIAN_INTERLEAVED,		/*!< Compressed form of complex numbers; complex-conjugates are not stored, real and imaginary components are stored in the same array. */
	METALFFT_HERMITIAN_PLANAR,				/*!< Compressed form of complex numbers; complex-conjugates are not stored, real and imaginary components are stored in separate arrays. */
	METALFFT_REAL,							/*!< An array of real numbers, with no corresponding imaginary components. */
	METALFFT_ENDLAYOUT			/*!< The last value of the enum, and marks the length of clfftLayout. */
} metalfftLayout;

typedef enum metalfftPrecision_
{
	METALFFT_SINGLE	= 1,	/*!< An array of complex numbers, with real and imaginary components saved as floats (default). */
	METALFFT_DOUBLE,			/*!< An array of complex numbers, with real and imaginary components saved as doubles. */
	METALFFT_SINGLE_FAST,		/*!< Faster implementation preferred. */
	METALFFT_DOUBLE_FAST,		/*!< Faster implementation preferred. */
	METALFFT_ENDPRECISION	/*!< The last value of the enum, and marks the length of clfftPrecision. */
} metalfftPrecision;



/*!  @brief Specify the expected direction of each FFT, time or the frequency domains */
typedef enum metalfftDirection_
{
	METALFFT_FORWARD	= -1,		/*!< FFT transform from time to frequency domain. */
	METALFFT_BACKWARD	= 1,		/*!< FFT transform from frequency to time domain. */
	METALFFT_MINUS		= -1,		/*!< Alias for the forward transform. */
	METALFFT_PLUS		= 1,		/*!< Alias for the backward transform. */
	METALFFT_ENDDIRECTION			/*!< The last value of the enum, and marks the length of metalfftDirection. */
} metalfftDirection;

/*!  @brief Specify wheter the input buffers are overwritten with results */
typedef enum metalfftResultLocation_
{
	METALFFT_INPLACE		= 1,		/*!< Input and output buffers are the same (default). */
	METALFFT_OUTOFPLACE,				/*!< Input and output buffers are separate. */
	METALFFT_ENDPLACE				/*!< The last value of the enum, and marks the length of metalfftPlaceness. */
} metalfftResultLocation;


struct metalfftSetupData_
{
	unsigned int major;		/*!< Major version number of the project; signifies possible major API changes. */
	unsigned int minor;		/*!< Minor version number of the project; minor API changes that can break backward compatibility. */
	unsigned int patch;		/*!< Patch version number of the project; always incrementing number, signifies change over time. */

	/*! 	Bitwise flags that control the behavior of library debug logic. */
	unsigned long int debugFlags;  /*! This must be set to zero, except when debugging the metalfft library.
	                       *  <p> debugFlags can be set to METALFFT_DUMP_PROGRAMS, in which case the dynamically generated OpenCL kernels are
	                       *  written to text files in the current working directory.  These files have a *.cl suffix.
	                       */
};

typedef struct metalfftSetupData_ metalfftSetupData;

/*!  @brief An abstract handle to the object that represents the state of the FFT(s) */
typedef size_t metalfftPlanHandle;

/*!  @brief The dimension of the input and output buffers that is fed into all FFT transforms */
typedef enum metalfftDim_
{
	METALFFT_1D		= 1,		/*!< 1 Dimensional FFT transform (default). */
	METALFFT_2D,					/*!< 2 Dimensional FFT transform. */
	METALFFT_3D,					/*!< 3 Dimensional FFT transform. */
	METALFFT_ENDDIMENSION			/*!< The last value of the enum, and marks the length of clfftDim. */
} metalfftDim;

typedef enum metalfftImpl_
{
	METALFFT_AUTO	= 1,
	METALFFT_HOST,
	METALFFT_METAL,
	METALFFT_ENDMETHOD
} metalfftMethod;

inline tstring metalfftErrorStatusAsString(const int& status)
{
	return _T("Error code not defined");
	//switch (status)
	//{
	/*case METALFFT_INVALID_GLOBAL_WORK_SIZE:
		return _T("CLFFT_INVALID_GLOBAL_WORK_SIZE");
	case CLFFT_INVALID_MIP_LEVEL:
		return _T("CLFFT_INVALID_MIP_LEVEL");
	case CLFFT_INVALID_BUFFER_SIZE:
		return _T("CLFFT_INVALID_BUFFER_SIZE");
	case CLFFT_INVALID_GL_OBJECT:
		return _T("CLFFT_INVALID_GL_OBJECT");
	case CLFFT_INVALID_OPERATION:
		return _T("CLFFT_INVALID_OPERATION");
	case CLFFT_INVALID_EVENT:
		return _T("CLFFT_INVALID_EVENT");
	case CLFFT_INVALID_EVENT_WAIT_LIST:
		return _T("CLFFT_INVALID_EVENT_WAIT_LIST");
	case CLFFT_INVALID_GLOBAL_OFFSET:
		return _T("CLFFT_INVALID_GLOBAL_OFFSET");
	case CLFFT_INVALID_WORK_ITEM_SIZE:
		return _T("CLFFT_INVALID_WORK_ITEM_SIZE");
	case CLFFT_INVALID_WORK_GROUP_SIZE:
		return _T("CLFFT_INVALID_WORK_GROUP_SIZE");
	case CLFFT_INVALID_WORK_DIMENSION:
		return _T("CLFFT_INVALID_WORK_DIMENSION");
	case CLFFT_INVALID_KERNEL_ARGS:
		return _T("CLFFT_INVALID_KERNEL_ARGS");
	case CLFFT_INVALID_ARG_SIZE:
		return _T("CLFFT_INVALID_ARG_SIZE");
	case CLFFT_INVALID_ARG_VALUE:
		return _T("CLFFT_INVALID_ARG_VALUE");
	case CLFFT_INVALID_ARG_INDEX:
		return _T("CLFFT_INVALID_ARG_INDEX");
	case CLFFT_INVALID_KERNEL:
		return _T("CLFFT_INVALID_KERNEL");
	case CLFFT_INVALID_KERNEL_DEFINITION:
		return _T("CLFFT_INVALID_KERNEL_DEFINITION");
	case CLFFT_INVALID_KERNEL_NAME:
		return _T("CLFFT_INVALID_KERNEL_NAME");
	case CLFFT_INVALID_PROGRAM_EXECUTABLE:
		return _T("CLFFT_INVALID_PROGRAM_EXECUTABLE");
	case CLFFT_INVALID_PROGRAM:
		return _T("CLFFT_INVALID_PROGRAM");
	case CLFFT_INVALID_BUILD_OPTIONS:
		return _T("CLFFT_INVALID_BUILD_OPTIONS");
	case CLFFT_INVALID_BINARY:
		return _T("CLFFT_INVALID_BINARY");
	case CLFFT_INVALID_SAMPLER:
		return _T("CLFFT_INVALID_SAMPLER");
	case CLFFT_INVALID_IMAGE_SIZE:
		return _T("CLFFT_INVALID_IMAGE_SIZE");
	case CLFFT_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return _T("CLFFT_INVALID_IMAGE_FORMAT_DESCRIPTOR");
	case CLFFT_INVALID_MEM_OBJECT:
		return _T("CLFFT_INVALID_MEM_OBJECT");
	case CLFFT_INVALID_HOST_PTR:
		return _T("CLFFT_INVALID_HOST_PTR");
	case CLFFT_INVALID_COMMAND_QUEUE:
		return _T("CLFFT_INVALID_COMMAND_QUEUE");
	case CLFFT_INVALID_QUEUE_PROPERTIES:
		return _T("CLFFT_INVALID_QUEUE_PROPERTIES");
	case CLFFT_INVALID_CONTEXT:
		return _T("CLFFT_INVALID_CONTEXT");
	case CLFFT_INVALID_DEVICE:
		return _T("CLFFT_INVALID_DEVICE");
	case CLFFT_INVALID_PLATFORM:
		return _T("CLFFT_INVALID_PLATFORM");
	case CLFFT_INVALID_DEVICE_TYPE:
		return _T("CLFFT_INVALID_DEVICE_TYPE");
	case CLFFT_INVALID_VALUE:
		return _T("CLFFT_INVALID_VALUE");
	case CLFFT_MAP_FAILURE:
		return _T("CLFFT_MAP_FAILURE");
	case CLFFT_BUILD_PROGRAM_FAILURE:
		return _T("CLFFT_BUILD_PROGRAM_FAILURE");
	case CLFFT_IMAGE_FORMAT_NOT_SUPPORTED:
		return _T("CLFFT_IMAGE_FORMAT_NOT_SUPPORTED");
	case CLFFT_IMAGE_FORMAT_MISMATCH:
		return _T("CLFFT_IMAGE_FORMAT_MISMATCH");
	case CLFFT_MEM_COPY_OVERLAP:
		return _T("CLFFT_MEM_COPY_OVERLAP");
	case CLFFT_PROFILING_INFO_NOT_AVAILABLE:
		return _T("CLFFT_PROFILING_INFO_NOT_AVAILABLE");
	case CLFFT_OUT_OF_HOST_MEMORY:
		return _T("CLFFT_OUT_OF_HOST_MEMORY");
	case CLFFT_OUT_OF_RESOURCES:
		return _T("CLFFT_OUT_OF_RESOURCES");
	case CLFFT_MEM_OBJECT_ALLOCATION_FAILURE:
		return _T("CLFFT_MEM_OBJECT_ALLOCATION_FAILURE");
	case CLFFT_COMPILER_NOT_AVAILABLE:
		return _T("CLFFT_COMPILER_NOT_AVAILABLE");
	case CLFFT_DEVICE_NOT_AVAILABLE:
		return _T("CLFFT_DEVICE_NOT_AVAILABLE");
	case CLFFT_DEVICE_NOT_FOUND:
		return _T("CLFFT_DEVICE_NOT_FOUND");
	case CLFFT_SUCCESS:
		return _T("CLFFT_SUCCESS");
	case CLFFT_NOTIMPLEMENTED:
		return _T("CLFFT_NOTIMPLEMENTED");
	case CLFFT_FILE_NOT_FOUND:
		return _T("CLFFT_FILE_NOT_FOUND");
	case CLFFT_FILE_CREATE_FAILURE:
		return _T("CLFFT_FILE_CREATE_FAILURE");
	case CLFFT_VERSION_MISMATCH:
		return _T("CLFFT_VERSION_MISMATCH");
	case CLFFT_INVALID_PLAN:
		return _T("CLFFT_INVALID_PLAN");*/
	//default:
	//	return _T("Error code not defined");
	//	break;
	//}
}

#if defined( _DEBUG )

#define OPENCL_V( fn, msg ) \
{ \
	metalfftStatus vclStatus = static_cast< metalfftStatus >( fn ); \
	switch( vclStatus ) \
	{ \
		case	METALFFT_SUCCESS:		/**< No error */ \
			break; \
		default: \
		{ \
			terr << _T( "OPENCL_V< " ); \
			terr << metalfftErrorStatusAsString( vclStatus ); \
			terr << _T( " > (" )<< static_cast<unsigned>( __LINE__ ) << _T( "): " ); \
			terr << msg << std::endl; \
			return	vclStatus; \
		} \
	} \
}

#else

#define OPENCL_V( fn, msg ) \
{ \
	metalfftStatus vclStatus = static_cast< metalfftStatus >( fn ); \
	switch( vclStatus ) \
	{ \
		case	METALFFT_SUCCESS:		/**< No error */ \
			break; \
		default: \
		{ \
			return	vclStatus; \
		} \
	} \
}
#endif
