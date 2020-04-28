cmake_minimum_required(VERSION 3.10)
project(AMF_Metal)
set(NAME AMF_Metal)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -x objective-c")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

set(HEADER_FILES
    #MetalDevice.h
    #MetalCompute.h
    #MetalComputeKernel.h
    MetalDeviceWrapper.h
    MetalComputeWrapper.h
    MetalComputeKernelWrapper.h
)
set(SOURCE_FILES
    MetalDevice.mm
    MetalCompute.mm
    MetalComputeKernel.mm
    MetalDeviceWrapper.mm
    MetalComputeWrapper.mm
    MetalComputeKernelWrapper.mm
)

add_library(
    ${NAME}
    STATIC
    #SHARED
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

# set_target_properties(
#     ${NAME}
#     PROPERTIES
#     MACOSX_BUNDLE_INFO_PLIST
#     ${CMAKE_CURRENT_LIST_DIR}/plist.in
# )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -framework AppKit \
    -framework Metal \
    -framework MetalKit"
    )