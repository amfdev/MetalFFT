cmake_minimum_required(VERSION 3.0)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

if(NOT AMF_HOME)
  message(SEND_ERROR "Error: MetalFFT needs AMF!")
else()
  message("MetalFFT will use AMF at folder ${AMF_HOME}")

  include_directories(${AMF_HOME}/amf)
endif()

if(METALFFT_DIR)
  message("")
  message("METALFFT_DIR: " ${METALFFT_DIR})

  #test the ${METALFFT_DIR} is not an absolute path
  if(NOT IS_ABSOLUTE ${METALFFT_DIR})
    #test this cmake is included from other project
    if(NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
      get_filename_component(Path_To_MetalFFT "${CMAKE_SOURCE_DIR}/${METALFFT_DIR}" ABSOLUTE)
    #assume that the project is built from itself
    else()
      get_filename_component(Path_To_MetalFFT "${CMAKE_CURRENT_SOURCE_DIR}/${METALFFT_DIR}" ABSOLUTE)
    endif()
  else()
    get_filename_component(Path_To_MetalFFT ${METALFFT_DIR} ABSOLUTE)
  endif()

  set_property(GLOBAL PROPERTY METALFFT_INCLUDES_DIR ${Path_To_MetalFFT})
  message("METALFFT_DIR expanded: " ${Path_To_MetalFFT})

  ADD_DEFINITIONS(-DUSE_METALFFT)
  include_directories(${Path_To_MetalFFT})
endif()
############################################################################

project(MetalFFT)
set(NAME MetalFFT)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -x objective-c")

set(HEADER_FILES
  plan.h
  metalFFT.h
  action.h
  lock.h
  metalFFT_common.h
  generator.h
  thirdparty/FFT_3.h
  )
set(SOURCE_FILES
  action.cpp
	generator.cpp
	metalFFT.cpp
	plan.cpp
	repo.cpp
  )

add_library(
  ${NAME}
  STATIC
  #SHARED
  ${SOURCE_FILES}
  ${HEADER_FILES}
  )
#include_directories(../../../include)
#add_subdirectory(../../cmake ${AMF_NAME})

if(AMF_CORE_STATIC)
  target_link_libraries(${PROJECT_NAME} ${AMF_NAME})
else()
  target_link_libraries(${PROJECT_NAME} ${AMF_LOADER_NAME})
endif()