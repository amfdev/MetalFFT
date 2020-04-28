cmake_minimum_required(VERSION 3.0)
project(MetalFFT1D)
set(NAME MetalFFT1D)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -x objective-c")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

set(HEADER_FILES
  #  plan.h
    metalFFT.h
    metalFFT_common.h
)
set(SOURCE_FILES
    fft1d.cpp
   # lifetime.cpp
   # plan.cpp
    #transform.cpp
 #   accessors.cpp
)
add_subdirectory(Metal AMF_Metal)

add_executable(
    ${NAME}
    MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
)


target_link_libraries(${NAME} AMF_Metal)

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
