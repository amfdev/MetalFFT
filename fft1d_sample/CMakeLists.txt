cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

if(NOT WIN32)
  #message(SEND_ERROR "ERROR: Test are prepared for Windows ONLY!")
endif()

if(AMF_CORE_STATIC)
  message("static AMF build will be used")
  ADD_DEFINITIONS(-DAMF_CORE_STATIC)
else()
  message("shared AMF build will be used")
endif()

if(NOT AMF_NAME)
  set(AMF_NAME openAmf)
  set_property(GLOBAL PROPERTY AMF_NAME ${AMF_NAME})
  get_property(AMF_NAME GLOBAL PROPERTY AMF_NAME)

  message(WARNING "OpenAMF will be built with the default library name: " ${AMF_NAME})
endif()

if(NOT AMF_CORE_STATIC AND NOT AMF_LOADER_NAME)
  set(AMF_LOADER_NAME openAmfLoader)
  set_property(GLOBAL PROPERTY AMF_LOADER_NAME ${AMF_LOADER_NAME})
  get_property(AMF_LOADER_NAME GLOBAL PROPERTY AMF_LOADER_NAME)

  message(WARNING "OpenAMF will be built with the default loader helper name: " ${AMF_LOADER_NAME})
endif()

if(OpenCL_INCLUDE_DIR AND OpenCL_LIBRARIES)
  get_filename_component(Path_To_OpenCL ${OpenCL_INCLUDE_DIR} ABSOLUTE)
  message(WARNING "OpenCL location was set manually(sample)")
  include_directories(${Path_To_OpenCL})
  message("OpenCL_INCLUDE_DIRS: " ${Path_To_OpenCL})
  message("OpenCL_LIBRARIES: " ${OpenCL_LIBRARIES})
  ADD_DEFINITIONS(-DCL_TARGET_OPENCL_VERSION=120)


else()
  find_package(OpenCL REQUIRED)

  if(NOT OpenCL_FOUND)
    message("")
    message(SEND_ERROR "Error: Failed to find OpenCL!")
    return()
  else()
    message("OpenCL found: " ${OpenCL_FOUND})
    message("OpenCL_INCLUDE_DIRS: " ${OpenCL_INCLUDE_DIRS})
    message("OpenCL_LIBRARIES: " ${OpenCL_LIBRARIES})
    message("OpenCL_VERSION_STRING: " ${OpenCL_VERSION_STRING})
    message("OpenCL_VERSION_MAJOR: " ${OpenCL_VERSION_MAJOR})
    message("OpenCL_VERSION_MINOR: " ${OpenCL_VERSION_MINOR})

    message("OpenCL_INCLUDE_DIR: ${OpenCL_INCLUDE_DIR}")
    message("OpenCL_LIBRARY: ${OpenCL_LIBRARY}")

    include_directories(${OpenCL_INCLUDE_DIR})
    ADD_DEFINITIONS(-DCL_TARGET_OPENCL_VERSION=120)
  endif()
endif()

project(fftSample)

# Local header files here ONLY
SET(HEADERS

   )

# Local source files here
SET(SOURCES
    fft1d.cpp
    )

add_subdirectory(.. amffft)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
    ${HEADER_FILES}
)

if(AMF_CORE_STATIC)
  target_link_libraries(${PROJECT_NAME} ${AMF_NAME} ${OpenCL_LIBRARIES})
else()
  target_link_libraries(${PROJECT_NAME} amffft)
endif()
