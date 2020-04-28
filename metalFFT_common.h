#include <stdint.h>
#include <stdio.h>
#pragma once

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

enum metalfftStatus_
{
	METALFFT_SUCCESS							= 0,
	METALFFT_INVALID_ARG_VALUE,
	METALFFT_NOTIMPLEMENTED,
	METALFFT_ENDSTATUS				/* The last value of the enum, and marks the length of metalfftStatus. */
};
typedef enum metalfftStatus_ metalfftStatus;

/*!  @brief Specify the expected layouts of the buffers */
typedef enum metalfftLayout_
{
	METALFFT_COMPLEX_INTERLEAVED	= 1,	/*!< An array of complex numbers, with real and imaginary components together (default). */
	METALFFT_COMPLEX_PLANAR,				/*!< Separate arrays of real components and imaginary components. */
	METALFFT_HERMITIAN_INTERLEAVED,		/*!< Compressed form of complex numbers; complex-conjugates are not stored, real and imaginary components are stored in the same array. */
	METALFFT_HERMITIAN_PLANAR,				/*!< Compressed form of complex numbers; complex-conjugates are not stored, real and imaginary components are stored in separate arrays. */
	METALFFT_REAL,							/*!< An array of real numbers, with no corresponding imaginary components. */
	ENDLAYOUT			/*!< The last value of the enum, and marks the length of clfftLayout. */
} metalfftLayout;

typedef enum metalfftPrecision_
{
	METALFFT_SINGLE	= 1,	/*!< An array of complex numbers, with real and imaginary components saved as floats (default). */
	METALFFT_DOUBLE,			/*!< An array of complex numbers, with real and imaginary components saved as doubles. */
	METALFFT_SINGLE_FAST,		/*!< Faster implementation preferred. */
	METALFFT_DOUBLE_FAST,		/*!< Faster implementation preferred. */
	ENDPRECISION	/*!< The last value of the enum, and marks the length of clfftPrecision. */
} metalfftPrecision;



/*!  @brief Specify the expected direction of each FFT, time or the frequency domains */
typedef enum metalfftDirection_
{
	METALFFT_FORWARD	= -1,		/*!< FFT transform from time to frequency domain. */
	METALFFT_BACKWARD	= 1,		/*!< FFT transform from frequency to time domain. */
	METALFFT_MINUS		= -1,		/*!< Alias for the forward transform. */
	METALFFT_PLUS		= 1,		/*!< Alias for the backward transform. */
	ENDDIRECTION			/*!< The last value of the enum, and marks the length of metalfftDirection. */
} metalfftDirection;

/*!  @brief Specify wheter the input buffers are overwritten with results */
typedef enum metalfftResultLocation_
{
	METALFFT_INPLACE		= 1,		/*!< Input and output buffers are the same (default). */
	METALFFT_OUTOFPLACE,				/*!< Input and output buffers are separate. */
	ENDPLACE				/*!< The last value of the enum, and marks the length of metalfftPlaceness. */
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
	ENDDIMENSION			/*!< The last value of the enum, and marks the length of clfftDim. */
} metalfftDim;
