#include "metalFFT_common.h"
#include "repo.h"
#include "action.h"
#pragma once
#define metalfftVersionMajor 0
#define metalfftVersionMinor 3
#define metalfftVersionPatch 0

#define METALFFT_STATIC 1
//#define METALFFT_EXPORTS 1

#define METALFFTAPI

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Initialize a metalfftSetupData struct for the client
*  @details metalfftSetupData is passed to metalfftSetup to control behavior of the FFT runtime.
*  @param[out] setupData Data structure is cleared and initialized with version information and default values
*  @return Enum describes the error condition; superset of OpenCL error codes
*/

METALFFTAPI metalfftStatus metalfftInitSetupData(metalfftSetupData* setupData);

/*! @brief Initialize the internal FFT resources.
*  @details The internal resources include FFT implementation caches kernels, programs, and buffers.
*  @param[in] setupData Data structure that is passed into the setup routine to control FFT generation behavior
* 	and debug functionality
*  @return Enum describing error condition; superset of OpenCL error codes
*/
METALFFTAPI metalfftStatus	metalfftSetup(const metalfftSetupData* setupData, amf::AMFPrograms* pPrograms);


/*! @brief Create a plan object initialized entirely with default values.
*  @details A plan is a repository of state for calculating FFT's.  Allows the runtime to pre-calculate kernels, programs
* 	and buffers and associate them with buffers of specified dimensions.
*  @param[out] plHandle Handle to the newly created plan
*  @param[in] context Client is responsible for providing an OpenCL context for the plan
*  @param[in] dim Dimensionality of the FFT transform; describes how many elements are in the array
*  @param[in] clLengths An array of length of size 'dim';  each array value describes the length of each dimension
*  @return Enum describing error condition; superset of OpenCL error codes
*/
METALFFTAPI metalfftStatus	metalfftCreateDefaultPlan(metalfftPlanHandle* plHandle, const metalfftDim dim,
	const size_t* clLengths);

/*! @brief Set the floating point precision of the FFT data
*  @details Sets the floating point precision of the FFT complex data in the plan.
*  @param[in] plHandle Handle to a previously created plan
*  @param[in] precision Reference to the user clfftPrecision enum
*  @return Enum describing error condition; superset of OpenCL error codes
*/
METALFFTAPI metalfftStatus	metalfftSetPlanPrecision(metalfftPlanHandle plHandle, metalfftPrecision precision);

METALFFTAPI metalfftStatus	metalfftSetPlanFFTmethod(metalfftPlanHandle plHandle, metalfftMethod  method);

METALFFTAPI metalfftStatus	metalfftSetPlanDevice(metalfftPlanHandle plHandle, amf::AMFComputeDevicePtr pComputeDevice);

METALFFTAPI metalfftStatus	metalfftSetPlanContext(metalfftPlanHandle plHandle, amf::AMFContextPtr context);

/*! @brief Set the expected layout of the input and output buffers
*  @details Input and output buffers can be filled with either Hermitian, complex, or real numbers.  Complex numbers can be stored
*  in various layouts; this function informs the library what layouts to use for input and output
*  @param[in] plHandle Handle to a previously created plan
*  @param[in] iLayout Indicates how the input buffers are laid out in memory
*  @param[in] oLayout Indicates how the output buffers are laid out in memory
*/
METALFFTAPI metalfftStatus	metalfftSetLayout(metalfftPlanHandle plHandle, metalfftLayout iLayout, metalfftLayout oLayout);
/*! @brief Set whether the input buffers are to be overwritten with results
*  @details If the setting performs an in-place transform, the input buffers are overwritten with the results of the
*  transform.  If the setting performs an out-of-place transforms, the library looks for separate output buffers
*  on the Enqueue call.
*  @param[in] plHandle Handle to a previously created plan
*  @param[in] placeness Informs the library to either overwrite the input buffers with results or to write them in separate output buffers
*/
METALFFTAPI metalfftStatus	metalfftSetResultLocation(metalfftPlanHandle plHandle, metalfftResultLocation placeness);
/*! @brief Prepare the plan for execution.
*  @details After all plan parameters are set, the client has the option of 'baking' the plan, which informs the runtime that
*  no more change to the parameters of the plan is expected, and the OpenCL kernels can be compiled.  This optional function
*  allows the client application to perform the OpenCL kernel compilation when the application is initialized instead of during the first
*  execution.
*  At this point, the clfft runtime applies all implimented optimizations, including
*  running kernel experiments on the devices in the plan context.
*  <p>  This function takes a long time to execute. If a plan is not baked before being executed,
*  the first call to clfftEnqueueTransform takes a long time to execute.
*  <p>  If any significant parameter of a plan is changed after the plan is baked (by a subsequent call to any one of
*  the functions that has the prefix "clfftSetPlan"), it is not considered an error.  Instead, the plan reverts back to
*  the unbaked state, discarding the benefits of the baking operation.
*  @param[in] plHandle Handle to a previously created plan
*  @param[in] numQueues Number of command queues in commQueueFFT; 0 is a valid value, in which case the client does not want
* 	the runtime to run load experiments and only pre-calculate state information
*  @param[in] commQueueFFT An array of cl_command_queues created by the client; the command queues must be a proper subset of
* 	the devices included in the plan context
*  @param[in] pfn_notify A function pointer to a notification routine. The notification routine is a callback function that
*  an application can register and is called when the program executable is built (successfully or unsuccessfully).
*  Currently, this parameter MUST be NULL or nullptr.
*  @param[in] user_data Passed as an argument when pfn_notify is called.
*  Currently, this parameter MUST be NULL or nullptr.
*  @return Enum describing error condition; superset of OpenCL error codes
*/

METALFFTAPI metalfftStatus	metalfftBakePlan(metalfftPlanHandle plHandle, unsigned int numQueues, void *user_data);


/*! @brief Enqueue an FFT transform operation, and return immediately (non-blocking)
	 *  @details This transform API function computes the FFT transform. It is non-blocking as it
	 *  only enqueues the OpenCL kernels for execution. The synchronization step must be managed by the user.
	 *  @param[in] plHandle Handle to a previously created plan
	 *  @param[in] dir Forward or backward transform
	 *  @param[in] numQueuesAndEvents Number of command queues in commQueues; number of expected events to be returned in outEvents
	 *  @param[in] commQueues An array of cl_command_queues created by the client; the command queues must be a proper subset of
	 * 	the devices included in the OpenCL context associated with the plan
	 *  @param[in] numWaitEvents Specify the number of elements in the eventWaitList array
	 *  @param[in] waitEvents Events for which the transform waits to complete before executing on the device
	 *  @param[out] outEvents The runtime fills this array with events corresponding one to one with the input command queues passed
	 *	in commQueues.  This parameter can have the value NULL or nullptr. When the value is NULL, the client is not interested in receiving notifications
	 *	when transforms are finished, otherwise, (if not NULL) the client is responsible for allocating this array with at least
	 *	as many elements as specified in numQueuesAndEvents.
	 *  @param[in] inputBuffers An array of cl_mem objects that contain data for processing by the FFT runtime. If the transform
	 *  is in-place, the FFT results overwrite the input buffers
	 *  @param[out] outputBuffers An array of cl_mem objects that store the results of out-of-place transforms. If the transform
	 *  is in-place, this parameter may be NULL or nullptr and is completely ignored
	 *  @param[in] tmpBuffer A cl_mem object that is reserved as a temporary buffer for FFT processing. If clTmpBuffers is NULL or nullptr,
	 *  and the library needs temporary storage, an internal temporary buffer is created on the fly managed by the library.
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
METALFFTAPI metalfftStatus	metalfftEnqueueTransform(
	metalfftPlanHandle plHandle,
	metalfftDirection dir,
	amf::AMFBuffer **input,
	int inputCount,
	amf::AMFBuffer **output,
	int outputCount
);

/*! @brief Release the resources of a plan.
*  @details A plan may include resources, such as kernels, programs, and buffers that consume memory.  When a plan
*  is no more needed, the client must release the plan.
*  @param[in,out] plHandle Handle to a previously created plan
*  @return Enum describing error condition; superset of OpenCL error codes
*/
METALFFTAPI metalfftStatus	metalfftDestroyPlan(metalfftPlanHandle* plHandle);

/*! @brief Release all internal resources.
*  @details Called when client is done with the FFT library, allowing the library to destroy all resources it has cached
*  @return Enum describing error condition; superset of OpenCL error codes
*/
METALFFTAPI metalfftStatus	metalfftTeardown();

#ifdef __cplusplus
}
#endif
